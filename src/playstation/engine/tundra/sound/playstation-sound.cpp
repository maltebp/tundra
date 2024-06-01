#include "tundra/core/fixed.hpp"
#include "tundra/core/limits.hpp"
#include "tundra/core/types.hpp"
#include <tundra/sound/playstation-sound.hpp>

#include <cstring>

#include <psxspu.h>
#include <psxetc.h>

#include <tundra/core/log.hpp>
#include <tundra/core/assert.hpp>
#include <tundra/core/time.hpp>
#include <tundra/core/duration.hpp>
#include <tundra/assets/sound/sound-asset-load-info.hpp>
#include <tundra/assets/sound/sound-asset.hpp>

namespace td {

    namespace playstation_sound::internal {

        // From https://github.com/Lameguy64/PSn00bSDK/blob/master/examples/sound/vagsample/main.c
        struct VagHeader {
            uint32_t magic;			// 0x70474156 ("VAGp") for mono files
            uint32_t version;
            uint32_t interleave;	// Unused in mono files
            uint32_t size;			// Big-endian, in bytes
            uint32_t sample_rate;	// Big-endian, in Hertz
            uint16_t _reserved[5];
            uint16_t channels;		// Unused in mono files
            char     name[16];
        };

        struct SoundPlayId {
            uint32 raw_id;

            SoundPlayId(uint32 raw_id) : raw_id(raw_id) { 
                TD_ASSERT(get_channel_index() <= 23, "Channel ID must be between 0 and 23");
                TD_ASSERT(get_channel_play_id() < 134217728, "Channel Play ID must be less than 134217728");
            }

            SoundPlayId(uint8 channel_id, uint32 channel_play_id) 
                :   raw_id((uint32)channel_id | (uint32)(channel_play_id << 5))
            { 
                TD_ASSERT(channel_id <= 23, "Channel ID must be between 0 and 23");
                TD_ASSERT(channel_play_id < 134217728, "Channel Play ID must be less than 134217728");
            }

            uint8 get_channel_index() const {
                return raw_id & 0b01'1111;
            }

            uint32 get_channel_play_id() const {
                return raw_id >> 5;
            }

        };

        struct ChannelPlaybackInfo {
            uint32 sound_play_id = 0;
            UFixed32<12> time_ending = 0; // Seconds
        };
        
        constexpr uint32 SAMPLE_BLOCK_SIZE = 16; // Bytes
        constexpr uint32 NUM_SAMPLES_PER_SAMPLE_BLOCK = 28;
        
        constexpr uint32 SPU_NUM_CHANNELS = 24;
        constexpr uint32 SPU_MEMORY_SIZE = 512 * 1024; // bytes

        // The first 5060 bytes of SPU RAM are reserved for some psxspu internal buffers
        const uint32 SPU_MEMORY_START_ADDRESS = 0x1010;

        // I.e. the first byte after memory, so this is an invalid address
        const uint32 SPU_MEMORY_END_ADDRESS = 0x1010 + SPU_MEMORY_SIZE; 

        const uint32 MUSIC_CHANNEL_INDEX = 23;

        static bool is_initialized = false;

        static uint32 next_sample_address = SPU_MEMORY_START_ADDRESS; 

        static const ITime* time;

        static ChannelPlaybackInfo channel_playback_infos[SPU_NUM_CHANNELS] = {};

        static uint8 next_channel_to_use = 0;

        // psn00bsdk's implementation of SpuIsTransferCompleted(SPU_TRANSFER_WAIT) is bugged
        // so I have implemented a working version here instead
        static void wait_for_spu_transfer_to_complete() {
            const int TIMEOUT = 0x100000;
            const int TRANSFER_STATE_MASK = 1 << 24;
            
            for (int i = TIMEOUT; i; i--) {
                if ((DMA_CHCR(DMA_SPU) & TRANSFER_STATE_MASK) == 0) return;
            }

            TD_ASSERT(false, "Waiting for SPU DMA transfer timed out");
        }

        bool is_sound_data_looping(const byte* data, uint32 size) {
            const byte* end = data + size;
            const byte* block = data;

            bool has_loop_start_block = false;
            bool has_loop_repeat_block = false;

            while(block < end) {
                byte flags = *(block + 1);

                // This is the value that the "loop-repeat" block has (at least when set by psxavenc)
                if( (flags & 0b0011) == 0b0011 ) has_loop_repeat_block = true; 
                if( flags & 0b0100 ) has_loop_start_block = true; 

                if( flags & 0b0001) break; // End flag

                block += SAMPLE_BLOCK_SIZE;
            }

            if( has_loop_start_block && !has_loop_repeat_block ) {
                TD_DEBUG_LOG("Warning: SoundAsset has no block with repeat flag set, but it has a with start flag set");
            }
            else if( !has_loop_start_block && has_loop_repeat_block ) {
                TD_DEBUG_LOG("Warning: SoundAsset has no block with start flag set, but it has a block with repeat flag set");
            }

            return has_loop_start_block && has_loop_repeat_block;

            return false;
        }

        static void play_sound_in_channel(const SoundAsset& sound, uint32 channel_index) {
            TD_ASSERT(channel_index < SPU_NUM_CHANNELS, "Channel index is too high (must be %d was %d)", SPU_NUM_CHANNELS, channel_index);

            internal::ChannelPlaybackInfo& channel_playback_info = internal::channel_playback_infos[channel_index];

            if( sound.is_looping ) {
                channel_playback_info.time_ending = td::limits::numeric_limits<UFixed32<12>>::max;
            }
            else 
            {
                channel_playback_info.time_ending = UFixed32<12>(internal::time->get_duration_since_start().to_seconds()) + sound.length;
            }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"

            // Make sure the channel is stopped.
            SpuSetKey(0, 1 << channel_index);

            SPU_CH_FREQ(channel_index) = getSPUSampleRate(sound.sample_rate);
            SPU_CH_ADDR(channel_index) = getSPUAddr(sound.load_info->spu_address);

            // Set the channel's volume and ADSR parameters (0x80ff and 0x1fee are
            // dummy values that disable the ADSR envelope entirely).
            SPU_CH_VOL_L(channel_index) = 0x3fff;
            SPU_CH_VOL_R(channel_index) = 0x3fff;
            SPU_CH_ADSR1(channel_index) = 0x00ff;
            SPU_CH_ADSR2(channel_index) = 0x0000;

            // Start the channel.
            SpuSetKey(1, 1 << channel_index);

#pragma GCC diagnostic pop
        }

    }

    void playstation_sound::initialize(const ITime* time) {
        TD_ASSERT(!internal::is_initialized, "PlayStation sound is already initizalized");
        TD_ASSERT(time != nullptr, "PlayStation sound is already initizalized");

        SpuInit();
        internal::is_initialized = true;
        internal::time = time;
    }

    const SoundAsset& playstation_sound::load_sound(const td::byte* data) {
        TD_ASSERT(internal::is_initialized, "PlayStation sound is not initialized");
        TD_ASSERT(data != nullptr, "Pointer to sound data to load is nullptr");

        internal::VagHeader vag_header;

        // Copying (potentially) unaligned header into aligned header
        std::memcpy(&vag_header, data, sizeof(internal::VagHeader));
        
        SoundAsset* asset = new SoundAsset();
        asset->load_info = new SoundAssetLoadInfo();

        uint32 data_size = __builtin_bswap32(vag_header.size);
        
        // Round the size up to the nearest multiple of 64, as SPU DMA transfers
        // are done in 64-byte blocks.
        data_size = (data_size + 63) & ~((uint32)63);
        
        TD_ASSERT(
            internal::next_sample_address + data_size < internal::SPU_MEMORY_END_ADDRESS, 
            "Not enough remaining space in SPU RAM for sound data of size %d", data_size
        );
        
        asset->sample_rate = __builtin_bswap32(vag_header.sample_rate);
        TD_ASSERT(asset->sample_rate > 0, ".vag file's sample rate is 0");

        // This is an address in SPU RAM (hence it's not a pointer type)
        asset->load_info->spu_address = internal::next_sample_address;
        internal::next_sample_address = asset->load_info->spu_address + data_size;
        
        uint64 num_samples = (data_size / internal::SAMPLE_BLOCK_SIZE) * internal::NUM_SAMPLES_PER_SAMPLE_BLOCK;
        asset->length = UFixed32<12>::from_raw_fixed_value((uint32)((num_samples << 12) / asset->sample_rate));
        
        SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
        SpuSetTransferStartAddr(asset->load_info->spu_address);

        // This is a "pointer" to SPU ram (hence it's not a pointer type)
        const byte* vag_data = data + sizeof(internal::VagHeader);

        asset->is_looping = internal::is_sound_data_looping(vag_data, data_size);

        SpuWrite((const uint32_t *)vag_data, data_size);
        internal::wait_for_spu_transfer_to_complete();

        // Sound assets live for the rest of the program's life-time
        // so no need to keep track of the pointer
        return *asset;
    }

    uint32 playstation_sound::play_sound(const SoundAsset& sound) {
        TD_ASSERT(internal::is_initialized, "PlayStation sound is not initialized");

        UFixed32<12> seconds_since_start = UFixed32<12>(internal::time->get_duration_since_start().to_seconds());

        bool found_channel = false;
        uint8 channel_index = internal::next_channel_to_use;
        do {
            if( channel_index != internal::MUSIC_CHANNEL_INDEX ) {
                if( internal::channel_playback_infos[channel_index].time_ending < seconds_since_start) {
                    found_channel = true;
                    break;
                }
            }

            channel_index++;
            if( channel_index >= internal::SPU_NUM_CHANNELS ) channel_index = 0;
        } while(channel_index != internal::next_channel_to_use);

        if( !found_channel ) {
            TD_DEBUG_LOG("No channels available when playing sounds");
            return 0;
        }

        internal::ChannelPlaybackInfo& channel_play_back_info = internal::channel_playback_infos[channel_index];
        channel_play_back_info.sound_play_id++;

        internal::play_sound_in_channel(sound, channel_index);
        return internal::SoundPlayId(channel_index, channel_play_back_info.sound_play_id).raw_id;
    }

    void playstation_sound::play_music(const SoundAsset &music) {
        TD_ASSERT(music.is_looping, "SoundAsset can only be played as music if it is looping");
        internal::play_sound_in_channel(music, internal::MUSIC_CHANNEL_INDEX);
    }

    void playstation_sound::stop_sound(uint32 sound_play_id_raw) {
        internal::SoundPlayId sound_play_id = sound_play_id_raw;

        uint8 channel_index = sound_play_id.get_channel_index();
        
        internal::ChannelPlaybackInfo& channel_play_back_info = internal::channel_playback_infos[sound_play_id.get_channel_index()];
        if( channel_play_back_info.sound_play_id == sound_play_id.get_channel_play_id() ) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
            SpuSetKey(0, 1 << channel_index);
#pragma GCC diagnostic pop
            channel_play_back_info.time_ending = 0;
        }
    }

    void playstation_sound::stop_music() {

        internal::ChannelPlaybackInfo& channel_play_back_info = internal::channel_playback_infos[internal::MUSIC_CHANNEL_INDEX];
        if( channel_play_back_info.time_ending != 0 ) {
            #pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
            SpuSetKey(0, 1 << internal::MUSIC_CHANNEL_INDEX);
#pragma GCC diagnostic pop
            channel_play_back_info.time_ending = 0;
        }
    }
    
}