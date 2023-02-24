
/* (C) Evgeny Sobolev 09.02.1984 y.b., VRN, Russia, 	*/
/* at 09.02.2023 					*/

#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Register {
	typedef unsigned int AddressType;
	typedef unsigned int ValueType;

	inline void preRead(void) {
		asm("dsb ld");
	}

	inline void postWrite(void){
		asm("dsb st");
	}

	template<size_t msb = 0, size_t lsb = 0, typename FieldValueTypeArg = ValueType, typename RegisterValueTypeArg = ValueType >
	struct Field {
		typedef RegisterValueTypeArg 	RegisterValueType;
		typedef FieldValueTypeArg 		FieldValueType;
	private:	
		static constexpr const size_t MSB = msb;
		static constexpr const size_t LSB = lsb;
		static_assert( ( MSB >= LSB ), "Please check register bitfield configuration" );
		static constexpr const size_t FieldBitCount = ( MSB - LSB ) + 1;
		static_assert( ( FieldBitCount <= ( sizeof( FieldValueType ) * 8 )), "Please check Field configuration. Filed size is grather then filed type size" );
		static constexpr const size_t RegisterBitCount = ( sizeof( RegisterValueType ) * 8 );
		static_assert( ( FieldBitCount <= RegisterBitCount ), "Please check Field configuration. Field size is grather then register size" );
		static_assert( ( MSB < RegisterBitCount ), "Bitfield is outside of register" );
		static constexpr const RegisterValueType FieldLsbBitMask =  ( ( RegisterValueType(1) << (FieldBitCount - 1) ) - 1 ) | ( RegisterValueType(1) << (FieldBitCount - 1) );
		static constexpr const RegisterValueType FieldBitMask = ( FieldLsbBitMask << LSB );
	public:
		static constexpr const RegisterValueType getLsbMask() { return FieldLsbBitMask; }
		static constexpr const RegisterValueType getBitMask() { return FieldBitMask; };
		static constexpr const size_t getLsb() { return LSB; };
		static constexpr const size_t getBitCount() { return FieldBitCount; };
	};

	enum class AccessMode {
		ReadOnly,
		WriteOnly,
		ReadWrite,
		Reserved
	};

	template<size_t bitNo = 0, typename FieldValueTypeArg = ValueType, typename RegisterValueTypeArg = ValueType >
	struct Bit : public Field< bitNo, bitNo, FieldValueTypeArg, RegisterValueTypeArg > {};

	template< AddressType address, typename Descr >
	struct RW {
		static constexpr const AccessMode Policy = AccessMode::ReadWrite;
		typedef Descr Description;
		typedef typename Descr::FieldValueType Type;

		static inline constexpr const AddressType getAddress() {
			return address;
		}

		static inline const void set(const typename Descr::FieldValueType value) {
			const typename Descr::RegisterValueType valueToWrite = (static_cast<const typename Descr::RegisterValueType>(value) & Descr::getLsbMask() ) << Descr::getLsb();
			/* Read - Modify - Write */
			if constexpr ( Descr::getBitCount() == ( sizeof( typename Descr::RegisterValueType ) * 8) ) {
				*reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() ) = valueToWrite;
				postWrite();
			} else {
				preRead();
				typename Descr::RegisterValueType regValue = *reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() );
				regValue &= ~( Descr::getBitMask() );
				regValue |= valueToWrite;
				*reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() ) = regValue;
				postWrite();
			}
		}

		static inline const typename Descr::FieldValueType get() {
			preRead();
			const typename Descr::RegisterValueType regValue = *reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() );
			return static_cast<const typename Descr::FieldValueType>( ( regValue >> Descr::getLsb() ) & Descr::getLsbMask() );
		}

	};

	template< AddressType address, typename Descr >
	struct WO {
		static constexpr const AccessMode Policy = AccessMode::WriteOnly;
		typedef Descr Description;
		typedef typename Descr::FieldValueType Type;

		static inline constexpr const AddressType getAddress() {
			return address;
		}

		static inline const void set(const typename Descr::FieldValueType value) {
			static_assert(true, "Don't read register, write register as single field");
			const typename Descr::RegisterValueType valueToWrite = (static_cast<const typename Descr::RegisterValueType>(value) & Descr::getLsbMask() ) << Descr::getLsb();
			*reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() ) = valueToWrite;
			postWrite();
		}

	};

	template<AddressType address, typename Descr>
	struct RO {
		static constexpr const AccessMode Policy = AccessMode::ReadOnly;
		typedef Descr Description;
		typedef typename Descr::FieldValueType Type;

		static inline constexpr const AddressType getAddress() {
			return address;
		}

		static inline const typename Descr::FieldValueType get() {
			preRead();
			const typename Descr::RegisterValueType regValue = *reinterpret_cast<volatile typename Descr::RegisterValueType* const>( getAddress() );
			return static_cast<const typename Descr::FieldValueType>( ( regValue >> Descr::getLsb() ) & Descr::getLsbMask() );
		}
	};

	struct RS_Null {};

	template<AddressType address, typename Descr, typename Next = RS_Null>
	struct RS {
		typedef Next NextRS;
		static constexpr const AccessMode Policy = AccessMode::Reserved;
		static constexpr const typename Descr::FieldValueType DefaultValue = 0;
		typedef Descr Description;
		typedef typename Descr::FieldValueType Type;
		static inline constexpr const AddressType getAddress() {
			return address;
		}
	};

	template<AddressType address>
	struct Description {
		static constexpr const AddressType getAddress() {
			return _address;
		}
		typedef RW< getAddress(), Field<31,0> > Value; 
	private:	
		static constexpr const AddressType _address { address }; 
	};

	template <typename Reg, typename Field >
	constexpr const typename Reg::Value::Type getRegReservedMaskInt() {
		if constexpr ( std::is_same< Field, RS_Null >::value == true ) {
			return 0;
		} else {
			static_assert( ( Reg::Value::getAddress() == Field::getAddress() ), "Please check filed parameter and register" );
			if constexpr ( std::is_same<typename Field::NextRS, RS_Null>::value == true ) { 
				return Field::Description::getBitMask();
			} else {
				return getRegReservedMaskInt<Reg, typename Field::NextRS >() | Field::Description::getBitMask();
			}
		}
		return 0;
	}

	template <typename Reg>
	constexpr const typename Reg::Value::Type getRegReservedMaskInt() {
		return getRegReservedMaskInt<Reg, typename Reg::Reserved>();
	};

	template <typename Reg, typename Field, typename... Fields>
	constexpr typename Reg::Value::Type getRegMaskInt() {
		static_assert( ( Reg::Value::getAddress() == Field::getAddress() ), "Please check filed parameter and register" );
		if constexpr ( sizeof...(Fields) == 0 ) {
			if constexpr ( AccessMode::Reserved == Field::Policy ) {
				return getRegReservedMaskInt<Reg, Field>();
			} else {
				return Field::Description::getBitMask();
			}
		} else {
			if constexpr ( AccessMode::Reserved == Field::Policy ) {
				return getRegReservedMaskInt<Reg, Field>() | getRegMaskInt<Reg, Fields...>();
			} else {
				return Field::Description::getBitMask() | getRegMaskInt<Reg, Fields...>();
			}
		}
	}

	template <typename Reg, typename Field, typename... Fields>
	const typename Reg::Value::Type getRegValueInt( const typename Field::Type val, const typename Fields::Type... args ) {
		static_assert( ( Reg::Value::getAddress() == Field::getAddress() ), "Please check field parameter and register" );
		if constexpr ( sizeof...(Fields) == 0 ) {
			if constexpr ( Field::Policy == AccessMode::Reserved ) {
				return Field::DefaultValue;
			} else {
				const typename Reg::Value::Type regValue = (static_cast<typename Reg::Value::Type>(val) & Field::Description::getLsbMask() ) << Field::Description::getLsb();
				return regValue;
			}
		} else {
			if constexpr ( Field::Policy == AccessMode::Reserved ) {
				return Field::DefaultValue | getRegValueInt<Reg, Fields...>( args... );
			} else {
				const typename Reg::Value::Type regValue = (static_cast<typename Reg::Value::Type>(val) & Field::Description::getLsbMask() ) << Field::Description::getLsb();
				return regValue | getRegValueInt<Reg, Fields...>( args... );
			}
		}
	}

	template< typename Reg, typename... Fields >
	void Write( typename Fields::Type... args )  {
		constexpr const typename Reg::Value::Type ReservedMask = getRegReservedMaskInt< Reg >();
		constexpr const typename Reg::Value::Type ConcatMask = getRegMaskInt< Reg, Fields...>() | ReservedMask;
		if constexpr ( ConcatMask == Reg::Value::Description::getBitMask() ) {
			typename Reg::Value::Type regValue = getRegValueInt<Reg, Fields...>( args... );
			Reg::Value::set(regValue);
		} else {
			typename Reg::Value::Type regValue = Reg::Value::get();
			regValue &= ~( ConcatMask );
			regValue |= getRegValueInt<Reg, Fields...>( args... );
			Reg::Value::set(regValue);
		}
	};

}

