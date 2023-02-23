
/* (C) Evgeny Sobolev 09.02.1984 y.b., VRN, Russia, 	*/
/* at 09.02.2023 					*/

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Register {

	struct AccessConfig {
		typedef uint32_t ValueType;
		typedef uint32_t AddressType;
		static constexpr const bool ReadSync = false;
		static constexpr const bool WriteSync = false;
	};

	template< typename RegisterAccessConfig = AccessConfig, const typename RegisterAccessConfig::AddressType RegisterAddress = 0 >
	struct Description {
		typedef typename RegisterAccessConfig::ValueType RegisterValueType;
	
		static constexpr const typename RegisterAccessConfig::AddressType address() {
			return RegisterAddress;
		}

		static constexpr const size_t size() {
			return sizeof( typename RegisterAccessConfig::AddressType );
		}

		template< size_t msb = 0, size_t lsb = 0, 
				  typename FieldValueType = typename RegisterAccessConfig::ValueType, 
				  typename RegValueType = typename RegisterAccessConfig::ValueType,
				  typename RegAddressType = typename RegisterAccessConfig::AddressType,
				  RegAddressType FieldAddress = RegisterAddress >			  
		struct Field {
			typedef FieldValueType ValueType;

			static constexpr const size_t LSB = lsb;
			static constexpr const size_t MSB = msb;

			static_assert( ( MSB >= LSB ), "Please check register bitfield configuration" );
			static_assert( ( ( MSB - LSB + 1 ) <= ( sizeof(FieldValueType) * 8 ) ), "Please check type of Field. Bit count > size of type" );
			static_assert( ( MSB - LSB + 1 ) <= ( sizeof(RegValueType) * 8 ), "Please check register value size & register value type" );
			static_assert( ( MSB < ( sizeof(RegValueType) * 8) ), "Bitfield is outside of register" );
			static_assert( ( LSB >= 0 ), "Please check LSB value" );

			static constexpr const size_t BitCount = (MSB - LSB) + 1;

			static constexpr const RegValueType LsbMask = (   static_cast<RegValueType>(1) << (MSB - LSB) ) | 
								      					  ( ( static_cast<RegValueType>(1)  << (MSB - LSB) ) - 1 ) ;

			static constexpr const RegValueType Mask = ( LsbMask << LSB );

			static constexpr RegAddressType address() {
				return FieldAddress;
			}
			
			static inline void set(const FieldValueType value) {
				if constexpr ( BitCount != ( sizeof(RegValueType) * 8) ) {
					syncReadCommand();
					RegValueType regValue = *(reinterpret_cast<volatile RegValueType* const>( FieldAddress ));
					regValue  &= ~(Field::Mask);
					regValue  |= ( static_cast<const RegValueType>(value) & Field::LsbMask ) << LSB;
					*(reinterpret_cast<volatile RegValueType* const>( FieldAddress )) = regValue;
				} else {
					RegValueType regValue = ( static_cast<const RegValueType>(value) & Field::LsbMask ) << LSB;
					*(reinterpret_cast<volatile RegValueType* const>( FieldAddress )) = regValue;
				}
				syncWriteCommand();
			};

			static inline const FieldValueType get() {
				syncReadCommand();
				RegValueType regValue = *(reinterpret_cast<volatile RegValueType* const>( FieldAddress ) );
				return static_cast<const FieldValueType>( ( regValue >> LSB ) & LsbMask );
			}

		};

		template< size_t BitNo = 0, typename BitValueType = bool, typename RegValueType = typename RegisterAccessConfig::ValueType >
		struct Bit :   public Field< BitNo, BitNo, BitValueType, RegValueType > {};
		struct Value : public Field< (sizeof(typename RegisterAccessConfig::ValueType) * 8) - 1, 0, typename RegisterAccessConfig::ValueType> {};

		static inline void syncReadCommand() {
			if constexpr( RegisterAccessConfig::ReadSync ) {
				asm("dsb");
			}
		}

		static inline void syncWriteCommand() {			
			if constexpr( RegisterAccessConfig::WriteSync ) {
				asm("dsb st");
			}
		}

	};

	template <typename Reg, typename Field, typename... Fields>
	constexpr typename Reg::RegisterValueType getRegMask() {
		static_assert( ( Reg::address() == Field::address() ), "Please check filed parameter and register" );
		if constexpr ( sizeof...(Fields) == 0 ) {
			return Field::Mask;
		} else {
			return getRegMask<Reg, Fields...>() | Field::Mask;
		}
	}

	template <typename Reg, typename Field, typename... Fields>
	const typename Reg::RegisterValueType getRegValue( const typename Field::ValueType val, const typename Fields::ValueType... args ) {
		static_assert( ( Reg::address() == Field::address() ), "Please check field parameter and register" );
		if constexpr ( sizeof...(Fields) == 0 ) {
			const typename Reg::RegisterValueType regValue = (static_cast<typename Reg::RegisterValueType>(val) & Field::LsbMask) << Field::LSB;
			return regValue;
		} else {
			const typename Reg::RegisterValueType regValue = (static_cast<typename Reg::RegisterValueType>(val) & Field::LsbMask) << Field::LSB;
			return regValue | getRegValue<Reg, Fields...>( args... );
		}
	}

	template<typename Reg, typename... Fields>
	void Write( typename Fields::ValueType... args )  {
		constexpr const typename Reg::RegisterValueType ConcatMask = getRegMask<Reg, Fields...>();
		if constexpr ( ConcatMask == Reg::Value::Mask ) {
			typename Reg::RegisterValueType regValue = getRegValue<Reg, Fields...>( args... );
			Reg::Value::set(regValue);
		} else {
			typename Reg::RegisterValueType regValue = Reg::Value::get();
			regValue &= ~( ConcatMask );
			regValue |= getRegValue<Reg, Fields...>( args... );
			Reg::Value::set(regValue);
		}
	};


}

