#pragma once

#include "tundra/core/string-util.hpp"
#include "tundra/core/string.hpp"
#include <tundra/core/assert.hpp>
#include <tundra/core/types.hpp>


namespace td {

    namespace internal {
        struct ConstEvalDouble {
            consteval explicit ConstEvalDouble(long double d) : d(d) { }
            long double d;
        };
    }
    
    template<typename TDerived, typename TStoreType, typename TIntermediate, int TNumFractionBits>
    class FixedBase {
    public:

        using Type = TStoreType;

        static_assert(TNumFractionBits > 0, "TNumFractionBits must be larger than 0 (otherwise use a regular integer format)");
        
        static_assert(
            TNumFractionBits < sizeof(TStoreType) * 8,
            "Number of fraction bits must be lower than the number of bits in store type (integer part must at least have 1 bit)");

        constexpr FixedBase() : value(0) { };        

        constexpr FixedBase(const FixedBase& other) : value(other.value) { }

        consteval FixedBase(internal::ConstEvalDouble d) : value((TStoreType)(d.d * ONE)) { }

        // Same type, other number of fraction bits
        template<typename TOtherDerived, int TOtherNumFractionBits>
        constexpr explicit FixedBase(const FixedBase<TOtherDerived, TStoreType, TIntermediate, TOtherNumFractionBits>& other) 
            : value(raw_from_other_fixed_type(other)) { }

        // Different type
        template<typename TOtherDerived, typename TOtherStoreType, typename TOtherIntermediate, int TOtherNumFractionBits>
        constexpr explicit FixedBase(const FixedBase<TOtherDerived, TOtherStoreType, TOtherIntermediate, TOtherNumFractionBits>& other) 
            : value(raw_from_other_fixed_type(other)) { }

        constexpr TDerived& operator=(const FixedBase& other) {
            value = other.value;
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived& operator=(const TDerived& other) {
            value = other.value;
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived operator+(const TDerived& other) const {
            return from_raw_fixed_value(value + other.value);
        }

        constexpr TDerived& operator+=(const TDerived& other) {
            value += other.value;
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived operator-(const TDerived& other) const {
            return from_raw_fixed_value(value - other.value);
        }

        constexpr TDerived& operator-=(const TDerived& other) {
            value -= other.value;
            return static_cast<TDerived&>(*this);
        }

        // Avoids the conversion of the operand to a FixedPoint value (not needed for raw integers)
        constexpr TDerived operator*(TStoreType other) const {
            TStoreType new_value = (TStoreType)(value * other);
            return from_raw_fixed_value(new_value);
        }

        constexpr TDerived& operator*=(TStoreType other) {
            value *= other;
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived operator*(const TDerived& other) const {
            TIntermediate intermediate_value = (TIntermediate)value * (TIntermediate)other.value;
            TStoreType new_value = (TStoreType)(intermediate_value >> TNumFractionBits);
            return from_raw_fixed_value(new_value);
        }

        constexpr TDerived& operator*=(const TDerived& other) {
            value = (TStoreType)(((TIntermediate)value * (TIntermediate)other.value) >> TNumFractionBits);
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived operator/(TStoreType other) const {
            TStoreType new_value = (TStoreType)(value / other);
            return from_raw_fixed_value(new_value);
        }

        constexpr TDerived& operator/=(TStoreType other) {
            value /= other;
            return static_cast<TDerived&>(*this);
        }

        constexpr TDerived operator/(const TDerived& other) const {
            TStoreType new_value = (TStoreType)((((TIntermediate)value) << TNumFractionBits) / other.value);
            return from_raw_fixed_value(new_value);
        }

        constexpr TDerived& operator/=(const TDerived& other) {
            value = (TStoreType)((((TIntermediate)value) << TNumFractionBits) / other.value);
            return static_cast<TDerived&>(*this);
        }

        // Multiplies without storing in intermediate type (expensive for 64-bit intermediate)
        constexpr TDerived fast_multiply(const TDerived& other) const requires(sizeof(TIntermediate) > 4) {
            return from_raw_fixed_value((TStoreType)((value * other.value) >> TNumFractionBits));
        }

        // Divides without storing in intermediate type (expensive for 64-bit intermediate)
        constexpr TDerived fast_divide(const TDerived& other) const requires(sizeof(TIntermediate) > 4) {
            return from_raw_fixed_value((TStoreType)((value << TNumFractionBits) / other.value));
        }

        constexpr TStoreType get_raw_value() const { return value; }

        constexpr TStoreType get_raw_integer() const { return value >> TNumFractionBits; }

        constexpr TStoreType get_raw_fraction() const { return value & FRACTION_MASK; }

        static constexpr TStoreType ONE = 1 << TNumFractionBits;

        static constexpr TStoreType FRACTION_MASK = ONE - 1;

    protected:

        constexpr explicit FixedBase(TStoreType t) : value((TStoreType)(t << TNumFractionBits)) { }

        static constexpr TDerived from_raw_fixed_value(TStoreType value) {
            TDerived f;
            f.value  = value;
            return f; 
        }

        TStoreType value;

    private:

        template<typename TOtherDerived, typename TOtherStoreType, typename TOtherIntermediate, int TOtherNumFractionBits>
        static constexpr TStoreType raw_from_other_fixed_type(const FixedBase<TOtherDerived, TOtherStoreType, TOtherIntermediate, TOtherNumFractionBits>& other) {

            constexpr int num_fraction_bits_difference = TNumFractionBits - TOtherNumFractionBits;
            constexpr bool is_converting_to_smaller_fraction = num_fraction_bits_difference < 0;

            if constexpr ( is_converting_to_smaller_fraction ) {
                return (TStoreType)(other.get_raw_value() >> (TOtherStoreType)(-num_fraction_bits_difference));
            } 
            else {
                return (TStoreType)(((TStoreType)other.get_raw_value()) << (TStoreType)num_fraction_bits_difference);
            }
        }

    };

    consteval internal::ConstEvalDouble to_fixed(double d) {
        return internal::ConstEvalDouble(d);
    };

    template<int TNumFractionBits>
    class Fixed16 : public FixedBase<Fixed16<TNumFractionBits>, td::int16, td::int32, TNumFractionBits> {
    public:

        using Base = FixedBase<Fixed16<TNumFractionBits>, td::int16, td::int32, TNumFractionBits>;

        using Base::Base;

        constexpr Fixed16(int16 t) : Base(t) { }
        
        consteval Fixed16(internal::ConstEvalDouble d) : Base(d) { }

        constexpr bool operator==(const Fixed16& other) const { return this->value == other.value; }
        
    };

    template<int TNumFractionBits>
    class UFixed16 : public FixedBase<UFixed16<TNumFractionBits>, td::uint16, td::uint32, TNumFractionBits> {
    public:

        using Base = FixedBase<UFixed16<TNumFractionBits>, td::uint16, td::uint32, TNumFractionBits>;

        using Base::Base;
        
        constexpr UFixed16(uint16 t) : Base(t) { }

        consteval UFixed16(internal::ConstEvalDouble d) : Base(d) { }

        constexpr bool operator==(const UFixed16& other) const { return this->value == other.value; }

    };

    template<int TNumFractionBits>
    class Fixed32 : public FixedBase<Fixed32<TNumFractionBits>, td::int32, td::int64, TNumFractionBits> {
    public:

        using Base = FixedBase<Fixed32<TNumFractionBits>, td::int32, td::int64, TNumFractionBits>;

        using Base::Base;

        constexpr Fixed32(int32 t) : Base(t) { }

        consteval Fixed32(internal::ConstEvalDouble d) : Base(d) { }

        template<int TSameNumFractionBits>
        constexpr Fixed32(const Fixed16<TSameNumFractionBits>& other) requires(TNumFractionBits == TSameNumFractionBits) : Base(other) { }

        template<int TSameNumFractionBits>
        constexpr Fixed32(const UFixed16<TNumFractionBits>& other) requires(TNumFractionBits == TSameNumFractionBits) : Base(other) { }

        constexpr bool operator==(const Fixed32& other) const { return this->value == other.value; }

    };

    template<int TNumFractionBits>
    class UFixed32 : public FixedBase<UFixed32<TNumFractionBits>, td::uint32, td::uint64, TNumFractionBits> {
    public:

        using Base = FixedBase<UFixed32<TNumFractionBits>, td::uint32, td::uint64, TNumFractionBits>;

        using Base::Base;

        constexpr UFixed32(uint32 t) : Base(t) { }

        consteval UFixed32(internal::ConstEvalDouble d) : Base(d) { }

        template<int TSameNumFractionBits>
        constexpr UFixed32(const Fixed16<TSameNumFractionBits>& other) : Base(other) { static_assert(TSameNumFractionBits == TNumFractionBits); }

        template<int TSameNumFractionBits>
        constexpr UFixed32(const UFixed16<TSameNumFractionBits>& other) : Base(other) { static_assert(TSameNumFractionBits == TNumFractionBits);  }

        constexpr bool operator==(const UFixed32& other) const { return this->value == other.value; }

    };

    namespace internal {

        static constexpr uint32 MAX_PRINT_PRECISION = 8;

        // We cannot print anything bigger than the max int32 (i.e. 9 decimals),
        // as printf does not seem to support llu (uint64). 

        static constexpr uint32 POW_10_RESULTS[MAX_PRINT_PRECISION + 2] { 
            1U,
            10U,
            100U,
            1000U,
            10000U,
            100000U,
            1000000U,
            10000000U,
            100000000U,
            1000000000U,
        };

        template<typename TDerived, typename TStoreType, typename TIntermediate, int TNumFractionBits>
        String fixed_base_to_string(const FixedBase<TDerived, TStoreType, TIntermediate, TNumFractionBits>& fixed_point, uint32 precision) {
            using TFixedBase = FixedBase<TDerived, TStoreType, TIntermediate, TNumFractionBits>;
            
            TD_ASSERT(precision <= MAX_PRINT_PRECISION, "Precision above %d is not support", MAX_PRINT_PRECISION); // Haven't tested if 18 is the magic number

            const TStoreType raw_value = fixed_point.get_raw_value();
            const uint64 positive_value = (uint64)(raw_value < 0 ? -raw_value : raw_value);
            
            // We keep intermediary results in uint64, but we will only print uint32.uint32.
            // TODO: All calculations does not need this high precision so they could be optimized

            uint64 whole_part = (uint64)(positive_value >> TNumFractionBits); 
            uint64 fraction_part = (uint64)(positive_value & TFixedBase::FRACTION_MASK); 

            if( precision == 0 ) {
                uint64 first_decimal = ((fraction_part * 10) >> TNumFractionBits);
                if( first_decimal > 4 ) {
                    whole_part++;
                }
                
                if( raw_value < 0 ) {
                    return td::string_util::create_from_format("-%u", (uint32)whole_part);
                }
                else {
                    return td::string_util::create_from_format("%u", (uint32)whole_part); 
                }
            }
            else if( fraction_part == 0 ) {

                char* zeros = new char[precision + 1];
                for( uint32 i = 0; i < precision; i++ ) {
                    zeros[i] = '0';
                }
                zeros[precision] = '\0';

                td::String s = raw_value < 0 
                    ? td::string_util::create_from_format("-%u.%s",(uint32)whole_part, zeros)
                    : td::string_util::create_from_format("%u.%s", (uint32)whole_part, zeros);

                delete[] zeros;
                return s;
            }
            else {
                fraction_part *= POW_10_RESULTS[precision];

                uint64 fractions_first_decimal = ((fraction_part & (uint64)TFixedBase::FRACTION_MASK) * 10) >> TNumFractionBits;
                fraction_part >>= TNumFractionBits;

                if( fractions_first_decimal > 4 ) {
                    fraction_part++;
                }

                if( raw_value < 0 ) {
                    return td::string_util::create_from_format("-%u.%u", (uint32)whole_part, (uint32)fraction_part);
                }
                else {
                    return td::string_util::create_from_format("%u.%u", (uint32)whole_part, (uint32)fraction_part); 
                }
            }
        }    
    }

    template<int TNumFractionBits>
    String to_string(const Fixed32<TNumFractionBits>& fixed_point, uint32 precision = 3) {
        return internal::fixed_base_to_string(fixed_point, precision);
    }

    template<int TNumFractionBits>
    String to_string(const UFixed32<TNumFractionBits>& fixed_point, uint32 precision = 3) {
        return internal::fixed_base_to_string(fixed_point, precision);
    }

    template<int TNumFractionBits>
    String to_string(const UFixed16<TNumFractionBits>& fixed_point, uint32 precision = 3) {
        return internal::fixed_base_to_string(fixed_point, precision);
    }

    template<int TNumFractionBits>
    String to_string(const Fixed16<TNumFractionBits>& fixed_point, uint32 precision = 3) {
        return internal::fixed_base_to_string(fixed_point, precision);
    }
}