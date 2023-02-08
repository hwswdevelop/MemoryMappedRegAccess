
/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 09.02.2023 					*/


#include <stddef.h>
#include <stdint.h>

namespace Register {

	struct Description {
	
		using RegValueType = uint32_t;
		static constexpr const bool ReadSync = false;
		static constexpr const bool WriteSync = true;
	
		template<size_t BitNo = 0, typename BitValueType = bool>
		struct Bit {
			using ValueType = BitValueType;
			using Type = ValueType;
			static constexpr const size_t LSB = BitNo;
			static constexpr const size_t MSB = BitNo;
			static_assert( (MSB >= LSB), "Please check register bitfield configuration");			
			static constexpr const RegValueType  LsbMask = 0x01;
			static constexpr const RegValueType  Mask = ( LsbMask << LSB );
		};
		
		template<size_t msb = 0, size_t lsb = 0, typename FieldValueType = RegValueType >
		struct Field {
			using ValueType = FieldValueType;
			using Type = ValueType;
			static constexpr const size_t LSB = lsb;
			static constexpr const size_t MSB = msb;
			static_assert( (MSB >= LSB), "Please check register bitfield configuration");			
			static constexpr const RegValueType LsbMask = ( static_cast<RegValueType>(1) << (MSB - LSB) ) | 
								      ( ( static_cast<RegValueType>(1)  << (MSB - LSB) ) - 1 ) ;
			static constexpr const RegValueType Mask = ( LsbMask << LSB );
		};
		
		struct Value : Field<( ( sizeof(RegValueType) * 8) - 1 ), 0> {};

	};
	
	template<typename Description, typename Description::AddressType RegAddress = 0>
	struct Class : public Description {
		using AddressType = typename Description::AddressType;
		using ValueType = typename Description::RegValueType;
		using Fields = Description;
		
		
		constexpr Class() : _address(RegAddress) {}
		constexpr Class( AddressType address ) : _address(address) {}
		constexpr Class( AddressType address, AddressType index) : _address( address + static_cast<AddressType>(index * sizeof(ValueType)) ) {}
		
		inline void syncWrite() const {
			if constexpr( Description::WriteSync ) {
				asm volatile ("dsb");
			}
		}
				
		inline void syncRead() const {
			if constexpr( Description::ReadSync ) {
				asm volatile ("dsb");
			}
		}

		void operator = ( const ValueType value) const {
			*reinterpret_cast<volatile ValueType* const>(_address) = value;
			syncWrite();
		}
		
		const ValueType operator * () const {
			syncRead();
			return *reinterpret_cast<volatile ValueType* const>(_address);
		}
		
		void operator |= ( const ValueType value ) const {
			syncRead();
			*reinterpret_cast<volatile ValueType* const>(_address) |= value;
			syncWrite();
		}

		void operator &= ( const ValueType value ) const {
			syncRead();
			*reinterpret_cast<volatile ValueType* const>(_address) &= value;
			syncWrite();
		}
		
		template<typename Field>
		void set(typename Field::ValueType value) const {
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			regValue &= ~(Field::Mask);
			regValue |= ( static_cast<ValueType>(value) & Field::LsbMask );
			*reinterpret_cast<volatile ValueType* const>(_address) = regValue;
			syncWrite();
		}

		template<typename Field>
		typename Field::ValueType get() const {
			syncRead();
			ValueType regValue = *reinterpret_cast<volatile ValueType* const>(_address);
			return static_cast<typename Field::ValueType>( (regValue >> Field::LSB) & Field::LsbMask );
		}
		
	private:		
		const AddressType _address { RegAddress };
	};
	
}


