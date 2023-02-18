
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

	template<typename RegisterAccessConfig = AccessConfig, typename RegisterAccessConfig::Address RegisterAddress = 0 >
	struct Description {
		using RegisterValueType							= typename RegisterAccessConfig::Value;
		using RegisterAddressType						= typename RegisterAccessConfig::Address;
		static constexpr const bool ReadSync 			= RegisterAccessConfig::ReadSync;
		static constexpr const bool WriteSync			= RegisterAccessConfig::WriteSync;
		static constexpr const RegisterAddressType RegisterDefaultAddress = RegisterAddress;

		static constexpr const RegisterAddressType address() {
			return RegisterDefaultAddress;
		}

		static constexpr const size_t size() {
			return sizeof(RegisterValueType);
		}

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

			static constexpr const size_t BitCount = (MSB - LSB) + 1;

			static constexpr const RegValueType LsbMask = (   static_cast<RegValueType>(1) << (MSB - LSB) ) | 
								      					  ( ( static_cast<RegValueType>(1)  << (MSB - LSB) ) - 1 ) ;

			static constexpr const RegValueType Mask = ( LsbMask << LSB );
			
			inline void set(const ValueType value) const {
				if constexpr ( BitCount != (sizeof(RegValueType) * 8) ) {
					syncReadCommand();
					RegValueType regValue = *reinterpret_cast<volatile RegValueType* const>(RegisterAddress);
					regValue  &= ~(Field::Mask);
					regValue  |= ( static_cast<const RegValueType>(value) & Field::LsbMask ) << LSB;
					*reinterpret_cast<volatile RegValueType* const>(RegisterAddress) = regValue;
				} else {
					*reinterpret_cast<volatile RegValueType* const>(RegisterAddress) = static_cast<const RegValueType>(value);
				}
				syncWriteCommand();
			};

			inline const ValueType get() const {
				syncReadCommand();
				RegValueType regValue = *reinterpret_cast<volatile RegValueType* const>(RegisterAddress);
				return static_cast<const ValueType>( ( regValue >> LSB ) & LsbMask );
			}
		};

		template< size_t BitNo = 0, typename BitValueType = bool, typename RegValueType = RegisterValueType >
		struct Bit :   public Field< BitNo, BitNo, BitValueType, RegValueType > {};
		struct Value : public Field< (sizeof(RegisterValueType) * 8) - 1, 0, RegisterValueType> {};

		static inline void syncReadCommand() {
			if constexpr(ReadSync){
				asm("dsb");
			}
		}

		static inline void syncWriteCommand() {			
			if constexpr(WriteSync){
				asm("dsb st");
			}
		}

	};

	template< typename Description >
	struct Class {
		using AddressType = typename Description::RegisterAddressType;
		using ValueType = typename Description::RegisterValueType;
		using Fields = Description;

		constexpr Class() : _address(Description::address()) {}
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
			// TO DO: Add register filed type check
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			regValue &= ~(Field::Mask);
			regValue |= ( static_cast<ValueType>(value) & Field::LsbMask ) << Field::LSB;
			*reinterpret_cast<volatile ValueType* const>(_address) = regValue;
			syncWrite();
		}

		template<typename Field>
		typename Field::ValueType get() const volatile {
			// TO DO: Add register filed type check
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			return static_cast<typename Field::ValueType>( (regValue >> Field::LSB) & Field::LsbMask );
		}
		
	private:	
		inline void syncWrite() const volatile {
			Description::syncWriteCommand();
		}
				
		inline void syncRead() const volatile {
			Description::syncReadCommand();
		}		
		
	private:
		const AddressType _address { Description::address() };
	};


	template<typename Description>
	struct Cached  {
		using AddressType = typename Description::RegisterAddressType;
		using ValueType = typename Description::RegisterValueType;
		using Fields = Description;
		
		constexpr Cached() : _reg(Description::address()), _value(*_reg) {}
		constexpr Cached( AddressType address ) : _reg(address), _value(*_reg) {}
		
		~Cached(){
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
			// TO DO: Add register filed type check
			_value  &= ~(Field::Mask);
			_value  |= ( static_cast<ValueType>(value) & Field::LsbMask ) << Field::LSB;
		}
		
		template<typename Field>
		typename Field::ValueType get() const {
			// TO DO: Add register filed type check
			return static_cast<typename Field::ValueType>( ( _value >> Field::LSB) & Field::LsbMask );
		}

	private:
		const Class<Description>				_reg {};		
		ValueType 								_value{};		
	};

}

