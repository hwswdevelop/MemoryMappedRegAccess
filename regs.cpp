	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 09.02.2023 					*/

#include <stddef.h>
#include <stdint.h>
#include <RegistersClass.h>


/* Sample, how to describe registers */


struct SystemControlRegDescr : public Register::Description {
	using AddressType = uint32_t;
	using ValueType	  = uint32_t;
	
	struct PllEnable 		: public Bit<0> {};
	struct PheriphClockEnable 	: public Bit<1> {};
	
	enum class TAllClockEnable {
		Disable = 0,
		Enable = 0x03
	};
	struct AllClock : public Field<1,0,TAllClockEnable> {};
};

struct ClockControlRegDescr : public Register::Description {
	using AddressType = uint32_t;
	using ValueType	  = uint32_t;
	
	static constexpr const bool ReadSync = true;
	
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


void pllInit() {

	/* Disable all clock. All closk is BIT0 and BIT1 */
	SystemControl.set<SystemControlReg::AllClock>(SystemControlReg::AllClock::ValueType::Disable);
	/* Enable PLL */
	SystemControl.set<SystemControlReg::PllEnable>(true);
	/* Enable periph clock */
	SystemControl.set<SystemControlReg::PheriphClockEnable>(true);

	/* Write 0 into clock control as full register value */
	ClockControl = 0;
	/* Write 0 into clock control as full register value, via value */
	ClockControl.set<ClockControlReg::Value>(0);	
	/* Set clock control bitfield */
	ClockControl.set<ClockControlReg::Clock>( ClockControlReg::Clock::ValueType::Disable );
	/* Set clock mode */
	ClockControl.set<ClockControlReg::Mode>( ClockControlReg::Mode::ValueType::UpCounting );
	/* Set counter value */
	ClockControl.set<ClockControlReg::Counter>( 5 );	
	/* Set clock enable bit */
	ClockControl.set<ClockControlReg::Clock>( ClockControlReg::Clock::ValueType::Enable );


	for(uint32_t i = 1; i < 30; i++) {
		/* Now, disable clock */
		const Register::Class<ClockControlRegDescr> cc_off;
		cc_off.set<ClockControlReg::Clock>( ClockControlReg::Clock::ValueType::Disable );
		/* Now enable all clocks, with downcouning */
		Register::CachedClass<ClockControlRegDescr> cc_on(0x12010000 + (i * 4) );
		cc_on.set<ClockControlReg::Counter>( 10 );
		cc_on.set<ClockControlReg::Mode>( ClockControlReg::Mode::ValueType::DownCounting );
		cc_on.set<ClockControlReg::Clock>( ClockControlReg::Clock::ValueType::Enable );
	}
	
}

