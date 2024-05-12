#pragma once

#include <limits>

#include <tundra/core/fixed.hpp>
#include <tundra/core/types.hpp>
#include <tundra/core/list.hpp>

constexpr td::uint32 NUM_FRAMES = 100;

static td::Fixed32<12> compute_list_average(const td::List<td::uint32>& list) {
    if( list.get_size() == 0 ) return 0;

    td::uint64 total = 0;
    for( td::uint32 i = 0; i < list.get_size(); i++ ) {
        total += list[i];
    }
    total <<= 12;
    total /= list.get_size();
    TD_ASSERT(total < (td::uint64)std::numeric_limits<td::int32>::max, "Total is too big to fit in 32 bits Fixed32<12>", total);

    return td::Fixed32<12>::from_raw_fixed_value((td::int32)total);
}

static td::Fixed32<12> compute_list_average(const td::List<td::Fixed32<12>>& list) {
    if( list.get_size() == 0 ) return 0;

    td::Fixed32<12> total = 0;
    for( td::uint32 i = 0; i < list.get_size(); i++ ) {
        total += list[i];
    }
    total /= td::Fixed32<12>((int)list.get_size());
    return total;
}