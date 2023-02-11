
/* (C) Evgeny Sobolev 09.02.1984 y.b., VRN, Russia, 	*/
/* at 09.02.2023 					*/

#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Register {

	struct AccessConfig {
		using Value   = uint32_t;
		using Address = uint32_t;
		static constexpr const bool ReadSync = false;
		static constexpr const bool WriteSync = false;
	};

	template<typename RegisterAccessConfig = AccessConfig>
	struct Description {
		using RegisterValueType					= typename RegisterAccessConfig::Value;
		using RegisterAddressType				= typename RegisterAccessConfig::Address;
		static constexpr const bool ReadSync 	= RegisterAccessConfig::ReadSync;
		static constexpr const bool WriteSync	= RegisterAccessConfig::WriteSync;

		template< size_t msb = 0, size_t lsb = 0, typename FieldValueType = RegisterValueType, typename RegValueType = RegisterValueType >
		struct Field {
			using ValueType = FieldValueType;
			using Type = ValueType;

			static constexpr const size_t LSB = lsb;
			static constexpr const size_t MSB = msb;

			static_assert( ( MSB >= LSB ), "Please check register bitfield configuration" );
			static_assert( (MSB - LSB + 1) <= (sizeof(RegValueType) * 8), "Please check register value size" );
			static_assert( ( MSB < ( sizeof(RegValueType) * 8) ), "Bitfield is outside of register");
			static_assert( ( LSB >= 0 ), "Please check LSB value");
			static_assert( ( ( MSB - LSB + 1 ) <= ( sizeof(ValueType) * 8 ) ), "Please check type of Field. Bit count > size of type" );

			static constexpr const RegValueType LsbMask = (   static_cast<RegValueType>(1) << (MSB - LSB) ) | 
								      					  ( ( static_cast<RegValueType>(1)  << (MSB - LSB) ) - 1 ) ;
			static constexpr const RegValueType Mask = ( LsbMask << LSB );
		};

		template< size_t BitNo = 0, typename BitValueType = bool, typename RegValueType = RegisterValueType >
		struct Bit : public Field< BitNo, BitNo, BitValueType, RegValueType > {};

		struct Value : Field<( ( sizeof(RegisterValueType) * 8) - 1 ), 0> {};
	};
	

	template<typename Description, typename Description::RegisterAddressType RegAddress = 0>
	struct Class {
		using AddressType = typename Description::RegisterAddressType;
		using ValueType = typename Description::RegisterValueType;
		using Fields = Description;

		constexpr Class() : _address(RegAddress) {}
		constexpr Class( AddressType address ) : _address(address) {}
		
		const AddressType address() const {
			return _address;
		}

		const size_t size() {
			return sizeof(ValueType);
		}
	
		void operator = ( const ValueType value) const volatile {
			*reinterpret_cast<volatile ValueType* const>(_address) = value;
			syncWrite();
		}
		
		const ValueType operator * () const volatile {
			syncRead();
			return *reinterpret_cast<volatile ValueType* const>(_address);
		}
		
		void operator |= ( const ValueType value ) const volatile {
			syncRead();
			*reinterpret_cast<volatile ValueType* const>(_address) |= value;
			syncWrite();
		}

		void operator &= ( const ValueType value ) const volatile {
			syncRead();
			*reinterpret_cast<volatile ValueType* const>(_address) &= value;
			syncWrite();
		}
		
		template<typename Field>
		void set(typename Field::ValueType value) const volatile {
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			regValue &= ~(Field::Mask);
			regValue |= ( static_cast<ValueType>(value) & Field::LsbMask );
			*reinterpret_cast<volatile ValueType* const>(_address) = regValue;
			syncWrite();
		}

		template<typename Field>
		typename Field::ValueType get() const volatile {
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			return static_cast<typename Field::ValueType>( (regValue >> Field::LSB) & Field::LsbMask );
		}
		
	private:	
		inline void syncWrite() const volatile {
			if constexpr( Description::WriteSync ) {
				asm volatile ("dmb st");
			}
		}
				
		inline void syncRead() const volatile {
			if constexpr( Description::ReadSync ) {
				asm volatile ("dmb");
			}
		}		
		
	private:
		const AddressType _address { RegAddress };
	};


	template<typename Description, typename Description::RegisterAddressType RegAddress = 0>
	struct CachedClass  {
		using AddressType = typename Description::RegisterAddressType;
		using ValueType = typename Description::RegisterValueType;
		using Fields = Description;
		
		constexpr CachedClass() : _reg(RegAddress), _value(*_reg) {}
		 
		constexpr CachedClass( AddressType address ) : _reg(RegAddress), _value(*_reg) {}
		
		~CachedClass(){
			_reg = _value;
		}	

		inline void operator = ( const ValueType value) {
			_value = value;
		}
		
		const ValueType operator * () const {
			return _value;
		}
		
		inline void operator |= ( const ValueType value ) {
			_value |= value;
		}

		inline void operator &= ( const ValueType value ) {
			_value &= value;
		}
		
		template<typename Field>
		inline void set(typename Field::ValueType value) {
			_value  &= ~(Field::Mask);
			_value  |= ( static_cast<ValueType>(value) & Field::LsbMask );
		}
		
		template<typename Field>
		typename Field::ValueType get() const {
			return static_cast<typename Field::ValueType>( ( _value >> Field::LSB) & Field::LsbMask );
		}

	private:
		const Class<Description, RegAddress>	_reg {};		
		ValueType 								_value{};		
	};

		
}

