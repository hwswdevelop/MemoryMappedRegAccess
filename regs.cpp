	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 18.02.2023 					*/

#include <RegistersClass.h>
#include <hi3516ev200_pll_regs.h>


using namespace PeriCrg;

void pllWait( const uint32_t waitTime ){
	for(uint32_t i = 0; i < waitTime; i++) {  asm volatile ("nop"); };
}

void pllInit()
{
	/*
		Configuring the PLL requires the following steps:
		Step 1. Switch clocks, select non-PLL clocks, or other PLL clocks.
		Step 2. Change the PLL configuration.
		Step 3. Wait 0.1ms and observe the PLL LOCK indicator register PERI_CRG_PLL122 [10:0].
		Step 4. If the PLL LOCK register is in the LOCK state, the PLL configuration is complete, otherwise go back to step 3 until the PLL LOCK register is in the LOCK state.
	*/

	// Switch all clocks to 24MHz clock
	{
		Register::Cached<SocClkSel> reg;
		reg.set<SocClkSel::CoreA7ClkSel>(SocClkSel::CoreA7ClkSel::ValueType::Freq24MHz);
		reg.set<SocClkSel::SysApbClock>(SocClkSel::SysApbClock::ValueType::Freq24MHz);
		reg.set<SocClkSel::SysAxiClk>(SocClkSel::SysAxiClk::ValueType::Freq24MHz);
		reg.set<SocClkSel::SysCfgClk>(SocClkSel::SysCfgClk::ValueType::Freq24MHz);
		reg.set<SocClkSel::DdrClkSel>(SocClkSel::DdrClkSel::ValueType::Freq24MHz); 	
	}	
	/* Setup PLLA - bypass mode */
	PllConfig1::Bypass().set(PllConfig1::Bypass::ValueType::Bypass);
	/* Setup PLLV - bypass mode */
	PllConfig7::Bypass().set(PllConfig7::Bypass::ValueType::Bypass);

	// Reconfigure PLLA
	{
		Register::Cached<PllConfig1> reg;
		reg.set<PllConfig1::FracMode>(PllConfig1::FracMode::ValueType::IntegerMode);
		reg.set<PllConfig1::DacPowerDown>(PllConfig1::DacPowerDown::ValueType::Normal);
		reg.set<PllConfig1::FoutPowerDown>(PllConfig1::FoutPowerDown::ValueType::Normal);
		reg.set<PllConfig1::PostdivPoweDown>(PllConfig1::PostdivPoweDown::ValueType::Normal);
		reg.set<PllConfig1::PowerDown>(PllConfig1::PowerDown::ValueType::Normal);
		reg.set<PllConfig1::Bypass>(PllConfig1::Bypass::ValueType::NoBypass);
		reg.set<PllConfig1::Refdiv>(1); // 24Mhz / 1 = 24MHz
		reg.set<PllConfig1::FBdiv>(75); // 24Mhz * 75 = 1800MHz
	}
	{
		Register::Cached<PllConfig0> reg;
		reg.set<PllConfig0::Frac>(0);		// Frac = 0
		reg.set<PllConfig0::Postdiv1>(2);	// 1800MHz / 2 = 900MHz
		reg.set<PllConfig0::Postdiv2>(1);
	}
	pllWait(100);
	// Wait for PLLA is locked
	while( PllLockStatus::APll::ValueType::Locked != PllLockStatus::APll().get() ) {};

	// Reconfigure PLLV
	{
		Register::Cached<PllConfig7> reg;
		reg.set<PllConfig7::FracMode>(PllConfig7::FracMode::ValueType::IntegerMode);
		reg.set<PllConfig7::DacPowerDown>(PllConfig7::DacPowerDown::ValueType::Normal);
		reg.set<PllConfig7::FoutPowerDown>(PllConfig7::FoutPowerDown::ValueType::Normal);
		reg.set<PllConfig7::PostdivPoweDown>(PllConfig7::PostdivPoweDown::ValueType::Normal);
		reg.set<PllConfig7::PowerDown>(PllConfig7::PowerDown::ValueType::Normal);
		reg.set<PllConfig7::Bypass>(PllConfig7::Bypass::ValueType::NoBypass);
		reg.set<PllConfig7::Refdiv>(1); // 24Mhz / 1 = 24MHz
		reg.set<PllConfig7::FBdiv>(99); // 24Mhz * (99 + 1) = 2400MHz // Frac is in integer mode
	}
	{
		Register::Cached<PllConfig6> reg;
		reg.set<PllConfig6::Frac>(1);		// Frac = 1 (Frac is in integer mode))
		reg.set<PllConfig6::Postdiv1>(4);	// 2400MHz / 4 = 600MHz
		reg.set<PllConfig6::Postdiv2>(1);	// 600MHz / 1 = 600MHz
	}
	pllWait(100);
	// Wait for PLLV is locked
	while( PllLockStatus::VPll::ValueType::Locked != PllLockStatus::VPll().get() ) {};
	// Siwtch to PLL clock
	SocClkSel::CoreA7ClkSel().set( SocClkSel::CoreA7ClkSel::ValueType::Freq900MHz );
}

