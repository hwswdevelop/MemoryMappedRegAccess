	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 18.02.2023 					*/

#include <RegistersClass.h>
#include <hi3516ev200_pll_regs.h>

using namespace PeriCrg;

void pllWait( const uint32_t waitTime ){
	for(uint32_t i = 0; i < waitTime; i++) {  asm volatile ("nop"); };
}


void pllInit() {

	/*
		Configuring the PLL requires the following steps:
		Step 1. Switch clocks, select non-PLL clocks, or other PLL clocks.
		Step 2. Change the PLL configuration.
		Step 3. Wait 0.1ms and observe the PLL LOCK indicator register PERI_CRG_PLL122 [10:0].
		Step 4. If the PLL LOCK register is in the LOCK state, the PLL configuration is complete, otherwise go back to step 3 until the PLL LOCK register is in the LOCK state.
	*/

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

