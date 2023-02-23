	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 24.02.2023 					*/
/* New description testing */

#include <RegistersClass.h>
#include <hi3516ev200_pll_regs.h>

using namespace PeriCrg;

void pllWait( const uint32_t waitTime ){
	for(uint32_t i = 0; i < waitTime; i++) {  asm volatile ("nop"); };
}


#include <cstdint>
#include <type_traits>


/******************************************************************************/
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
	void WriteReg( typename Fields::Type... args )  {
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
/******************************************************************************/

using namespace Register;

struct Reg1 {
	enum class TA {
		Zero, One
	};
	typedef RW< 0x12010000, Field< 0, 0, TA> > A;
	typedef RW< 0x12010000, Field< 4, 1> >     B;
	typedef RW< 0x12010000, Field< 13, 5> >    C;
	typedef RW< 0x12010000, Field< 30, 15> >   D;

	typedef RW< 0x12010000, Field< 31, 0>>     Value;
	typedef RS< 0x12010000, Field< 14, 14>, 
			RS< 0x12010000, Field< 31, 31> >> Reserved;

	//typedef RS_Null 						  Reserved;
};

void aaa(void) {

	// Reg1::Value::Type regVal = Reg1::Value::get();
	// Reg1::A::set( Reg1::A::Type::One );
	// Reg1::B::set( Reg1::B::get() + 1 );

	WriteReg< Reg1, 
		Reg1::A,
		Reg1::B,
		Reg1::D,
		Reg1::C > (
			Reg1::A::Type::Zero,
			0,
			0,
			0
		);
/*
14000a84:       e3a03000        mov     r3, #0		; 0x0000 low part of address
14000a88:       e3413201        movt    r3, #4609   	; 0x1201 high part of address
14000a8c:       e3a02000        mov     r2, #0		; Load value as combianation of bits (Template)
14000a90:       e5832000        str     r2, [r3]	; Write into register
14000a94:       f57ff04e        dsb     st		; Barier (Store)
*/		
	WriteReg< Reg1, 
		Reg1::A,
		Reg1::B,
		Reg1::D,
		Reg1::C > (
			Reg1::A::Type::One,
			0,
			0,
			0
		);
/*		
14000a98:       e3a02001        mov     r2, #1		; New value as combination of bits (Template)
14000a9c:       e5832000        str     r2, [r3]	; Write into register
14000aa0:       f57ff04e        dsb     st		; Barier (Store)
*/
	WriteReg< Reg1, 
		Reg1::A,
		//Reg1::B,
		Reg1::D,
		Reg1::C > (
			Reg1::A::Type::One,
			//0,
			0,
			0
		);
/*
14000aa4:       f57ff04d        dsb     ld		; Barier (Load), because of not all bits are write
14000aa8:       e5932000        ldr     r2, [r3]	; Load register value, because of not all bits are write
14000aac:       e202201e        and     r2, r2, #30 	; "And" mask, all bits except of Reg::B is cleared
14000ab0:       e3822001        orr     r2, r2, #1	; "Or" mask, Reg:A = 1
14000ab4:       e5832000        str     r2, [r3]	; Store register value
14000ab8:       f57ff04e        dsb     st		; Barier (Store)
*/

}

void pllInit() {

	/*
		Configuring the PLL requires the following steps:
		Step 1. Switch clocks, select non-PLL clocks, or other PLL clocks.
		Step 2. Change the PLL configuration.
		Step 3. Wait 0.1ms and observe the PLL LOCK indicator register PERI_CRG_PLL122 [10:0].
		Step 4. If the PLL LOCK register is in the LOCK state, the PLL configuration is complete, otherwise go back to step 3 until the PLL LOCK register is in the LOCK state.
	*/
	aaa();

	// Switch all clocks to 24MHz clock
	Register::Write< SocClkSel,
			SocClkSel::DdrClkSel,
			SocClkSel::CoreA7ClkSel,
			SocClkSel::SysApbClock,
			SocClkSel::SysAxiClk,
			SocClkSel::SysCfgClk
			 > (
				SocClkSel::DdrClkSel::ValueType::Freq24MHz,
				SocClkSel::CoreA7ClkSel::ValueType::Freq24MHz,
				SocClkSel::SysApbClock::ValueType::Freq24MHz,
				SocClkSel::SysAxiClk::ValueType::Freq24MHz,
				SocClkSel::SysCfgClk::ValueType::Freq24MHz
			);

	/* Setup PLLA - bypass mode */
	PllConfig1::Bypass::set(PllConfig1::Bypass::ValueType::Bypass);
	/* Setup PLLV - bypass mode */
	PllConfig7::Bypass::set(PllConfig7::Bypass::ValueType::Bypass);

	// Reconfigure PLLA
	Register::Write< PllConfig1,
					 PllConfig1::FracMode,
					 PllConfig1::DacPowerDown,
					 PllConfig1::FoutPowerDown,
					 PllConfig1::PostdivPoweDown,
					 PllConfig1::PowerDown,
					 PllConfig1::Bypass,
					 PllConfig1::Refdiv,
					 PllConfig1::FBdiv>(
						PllConfig1::FracMode::ValueType::IntegerMode,
						PllConfig1::DacPowerDown::ValueType::Normal,
						PllConfig1::FoutPowerDown::ValueType::Normal,
						PllConfig1::PostdivPoweDown::ValueType::Normal,
						PllConfig1::PowerDown::ValueType::Normal,
						PllConfig1::Bypass::ValueType::NoBypass,
						PllConfig1::Refdiv::ValueType(1),	// 24Mhz / 1 = 24MHz
						PllConfig1::FBdiv::ValueType(75)	// 24Mhz * 75 = 1800MHz
					 );

	Register::Write< PllConfig0,
					 PllConfig0::Frac,
					 PllConfig0::Postdiv1,
					 PllConfig0::Postdiv2 > (
						PllConfig0::Frac::ValueType(0),		// Frac = 0
						PllConfig0::Postdiv1::ValueType(2),	// 1800MHz / 2 = 900MHz
						PllConfig0::Postdiv2::ValueType(1)	// 900MHz / 1 = 900MHz
					 );

	pllWait(100);

	// Wait for PLLA is locked
	while( PllLockStatus::APll::ValueType::Locked != PllLockStatus::APll::get() ) {};

	Register::Write< PllConfig7,
					 PllConfig7::FracMode,
					 PllConfig7::DacPowerDown,
					 PllConfig7::FoutPowerDown,
					 PllConfig7::PostdivPoweDown,
					 PllConfig7::PowerDown,
					 PllConfig7::Bypass,
					 PllConfig7::Refdiv,
					 PllConfig7::FBdiv>(
						PllConfig7::FracMode::ValueType::IntegerMode,
						PllConfig7::DacPowerDown::ValueType::Normal,
						PllConfig7::FoutPowerDown::ValueType::Normal,
						PllConfig7::PostdivPoweDown::ValueType::Normal,
						PllConfig7::PowerDown::ValueType::Normal,
						PllConfig7::Bypass::ValueType::NoBypass,
						PllConfig7::Refdiv::ValueType(1),	// 24Mhz / 1 = 24MHz
						PllConfig7::FBdiv::ValueType(99)	// 24Mhz * (99 + 1) = 2400MHz. Frac is in integer mode
					 );

	Register::Write< PllConfig6,
					 PllConfig6::Frac,
					 PllConfig6::Postdiv1,
					 PllConfig6::Postdiv2 > (
						PllConfig6::Frac::ValueType(1),		// Frac = 1 (Frac is in integer mode))
						PllConfig6::Postdiv1::ValueType(4),	// 2400MHz / 4 = 600MHz
						PllConfig6::Postdiv2::ValueType(1)	// 600MHz / 1 = 600MHz
					 );					 

	pllWait(100);
	// Wait for PLLV is locked
	while( PllLockStatus::VPll::ValueType::Locked != PllLockStatus::VPll::get() ) {};

	// Switch all clocks to there frequences
	Register::Write< SocClkSel,
			SocClkSel::DdrClkSel,
			SocClkSel::CoreA7ClkSel,
			SocClkSel::SysApbClock,
			SocClkSel::SysAxiClk,
			SocClkSel::SysCfgClk
			 > (
				SocClkSel::DdrClkSel::ValueType::Freq300MHz,
				SocClkSel::CoreA7ClkSel::ValueType::Freq900MHz,
				SocClkSel::SysApbClock::ValueType::Freq50MHZ,
				SocClkSel::SysAxiClk::ValueType::Freq200MHz,
				SocClkSel::SysCfgClk::ValueType::Freq100MHz
			);

}

