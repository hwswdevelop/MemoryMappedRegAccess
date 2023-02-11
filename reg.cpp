	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 09.02.2023 					*/

#include <stddef.h>
#include <stdint.h>
#include <RegistersClass.h>


/* Sample, how to describe registers */
/*
	So, this register, can be described as
	__attributr_((packed)) ....
	typedef ... union {
		struct {
			uint32_t PllEnable:1;
			uint32_t PheriphClockEnable:1;
		}
		struct {
			uint32_t AllClockEnable:2;
		}
	} *pSomething, Something;
	volatile Something* const reg = (Som...)(addr);
	
	What about reserved fields ????? 
	It have to name it Reserved1, Reserved2, etc...
	
	Another way to describe fields is
	#define PLL_ENABLE (1 << 0) ...., but,
	... if I will use another const with this register.
	
	
	What about Big/Little endian, so if I want to fixup edianess?
		
	But I want to add DMB, DSB, etc...
	I want to check type checking and type converison
	I wanna to add value something like bit description, 
	But where will I describe it????
	
	So, "constexpr Сlass" variable, will not take memory inside
	.text (non-volatile code section),
	.ram (volatile sram/sdram section)
	It is only like optimized inline functions.
	It allow you to make single Read-Modify-Write operation, by the each field
	But it can be un-constexpred, if you will use another constructor.
	
	Another class (Register::CachedClass) will help you access to
	the register memory in the single modification more then one filed.
	Constructor: register _tmp_reg <= REG
	
	Modify: _tmp_reg by "&=", "|=", but after modificaton this instructions,
	may be comressed into single "|=", or single "&=" or two instructions "&=" + "|=". 
	It will optimize access on "Assembly level" access.
	
	Destructor: write register REG <= _tmp_reg
	
	You can use single desccription of both classes.

	You can call me: +79003030374, we will discuss description.
	Evgeny Sobolev 02/09/1984 y.b., VRN, Russia,
	May be it is possible to make it better. I tried to find
	another way, but didn't found.
*/

struct SystemControlRegDescr : public Register::Description<> {	
	struct PllEnable 		: public Bit<0> {};
	struct PheriphClockEnable 	: public Bit<1> {};	
	enum class TAllClockEnable {
		Disable = 0,
		Enable = 0x03
	};
	struct AllClock : public Field<1,0,TAllClockEnable> {};
};

struct ClockControlRegDescr : public Register::Description<> {

	enum class TClockEnable {
		Disable,
		Enable
	};

	enum class TCountMode {
		UpCounting,
		DownCounting,
		Stopped,
		Reserved
	};

	struct Clock	  : public Bit<0, TClockEnable> {};
	struct Counter    : public Field<15, 8, uint8_t> {};
	struct Mode       : public Field<2, 1, TCountMode> {};

};

using SystemControlReg = Register::Class<SystemControlRegDescr, 0x12010000>;
using ClockControlReg  = Register::Class<ClockControlRegDescr,  0x12010004>;

constexpr const SystemControlReg	SystemControl;
constexpr const ClockControlReg 	ClockControl;


void pllInit0() {

	/* Disable all clock. All closk is BIT0 and BIT1 */
	SystemControl.set<SystemControlReg::Fields::AllClock>(SystemControlReg::Fields::AllClock::ValueType::Disable);
	/* Enable PLL */
	SystemControl.set<SystemControlReg::Fields::PllEnable>(true);
	/* Enable periph clock */
	SystemControl.set<SystemControlReg::Fields::PheriphClockEnable>(true);
	/* Write 0 into clock control as full register value */
	ClockControl = 0;
	/* Write 0 into clock control as full register value, via value */
	ClockControl.set<ClockControlReg::Fields::Value>(0);	
	/* Set clock control bitfield */
	ClockControl.set<ClockControlReg::Fields::Clock>( ClockControlReg::Fields::Clock::ValueType::Disable );
	/* Set clock mode */
	ClockControl.set<ClockControlReg::Fields::Mode>( ClockControlReg::Fields::Mode::ValueType::UpCounting );
	/* Set counter value */
	ClockControl.set<ClockControlReg::Fields::Counter>( 5 );	
	/* Set clock enable bit */
	ClockControl.set<ClockControlReg::Fields::Clock>( ClockControlReg::Fields::Clock::ValueType::Enable );


	for(uint32_t i = 1; i < 30; i++) {
		/* Now, disable clock */
		const Register::Class<ClockControlRegDescr> cc_off;
		cc_off.set<ClockControlReg::Fields::Clock>( ClockControlReg::Fields::Clock::ValueType::Disable );
		/* Now enable all clocks, with downcouning */
		Register::CachedClass<ClockControlRegDescr> cc_on(0x12010000 + (i * 4) );
		cc_on.set<ClockControlReg::Fields::Counter>( 10 );
		cc_on.set<ClockControlReg::Fields::Mode>( ClockControlReg::Fields::Mode::ValueType::DownCounting );
		cc_on.set<ClockControlReg::Fields::Clock>( ClockControlReg::Fields::Clock::ValueType::Enable );
	}
	
}

