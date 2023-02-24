	

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia, 	*/
/* At 24.02.2023 					*/
/* New description testing */

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
				SocClkSel::DdrClkSel::Type::Freq24MHz,
				SocClkSel::CoreA7ClkSel::Type::Freq24MHz,
				SocClkSel::SysApbClock::Type::Freq24MHz,
				SocClkSel::SysAxiClk::Type::Freq24MHz,
				SocClkSel::SysCfgClk::Type::Freq24MHz
			);
/*
;Prepare, load Register(RegisterBase) addresss
14000a88:       e3a03000        mov     r3, #0				; 0x0000
14000a8c:       e3413201        movt    r3, #4609       	; 0x1201, i.e. R3 <= 0x12010000

14000a90:       e3a02000        mov     r2, #0				; R2 <= 0, i.e all clocks is 24MHz
14000a94:       e5832080        str     r2, [r3, #128]  	; [0x12010000] <= 0
14000a98:       f57ff04e        dsb     st					; Write bariler instruction
*/	

	/* Setup PLLA - bypass mode */
	PllConfig1::Bypass::set(PllConfig1::Bypass::Type::Bypass);
/*
14000a9c:       f57ff04d        dsb     ld						; Read barier
14000aa0:       e5932004        ldr     r2, [r3, #4]			; Load register value R2 <= [0x12010004]
14000aa4:       e3822301        orr     r2, r2, #67108864       ; R2 |= 0x4000000, i.e. set Bypass
14000aa8:       e5832004        str     r2, [r3, #4]			; [0x12010004] <= R2, i.e. Write register
14000aac:       f57ff04e        dsb     st						; Write barier
*/

	/* Setup PLLV - bypass mode */
	PllConfig7::Bypass::set(PllConfig7::Bypass::Type::Bypass);
/*
14000ab0:       f57ff04d        dsb     ld						; Read barier
14000ab4:       e593201c        ldr     r2, [r3, #28]			; Load register value R2 <= [0x1201001C]
14000ab8:       e3822301        orr     r2, r2, #67108864       ; R2 |= 0x4000000, i.e. set Bypass
14000abc:       e583201c        str     r2, [r3, #28]			; [0x1201001C] <= R2, i.e. Write register
14000ac0:       f57ff04e        dsb     st						; Write barier
*/

#if 0
	// Reconfigure PLLA
	Register::Write< PllConfig1,
					 PllConfig1::FracMode,
					 PllConfig1::DacPowerDown,
					 PllConfig1::FoutPowerDown,
					 PllConfig1::PostdivPowerDown,
					 PllConfig1::PowerDown,
					 PllConfig1::Bypass,
					 PllConfig1::Refdiv,
					 PllConfig1::FBdiv>(
						PllConfig1::FracMode::Type::IntegerMode,
						PllConfig1::DacPowerDown::Type::Normal,
						PllConfig1::FoutPowerDown::Type::Normal,
						PllConfig1::PostdivPowerDown::Type::Normal,
						PllConfig1::PowerDown::Type::Normal,
						PllConfig1::Bypass::Type::NoBypass,
						PllConfig1::Refdiv::Type(1),	// 24Mhz / 1 = 24MHz
						PllConfig1::FBdiv::Type(75)	// 24Mhz * 75 = 1800MHz
					 );
/*
;  ***** PllConfig1::VcoOutPowerDown, is skipped - need to read register ******
14000ac4:       f57ff04d        dsb     ld						; Read barier
14000ac8:       e5931004        ldr     r1, [r3, #4]			; Read R1 <= [0x12010004]
14000acc:       e2011501        and     r1, r1, #4194304        ; R1 &= 0x00400000, i.e. Clear all bits except of VcoOutPowerDown
14000ad0:       e301204b        movw    r2, #4171              	; R2 = New value to set bits (Lo bits)
14000ad4:       e3402300        movt    r2, #768         		; R2 <= 0x0300104B, i.e. set bits 25-DacPowerDown, 25-FracMode-Integer, and values RefDiv=1 & FBDiv=0x4B (75 dec))
14000ad8:       e1812002        orr     r2, r1, r2				; R1 |= R2 ; Set values 
14000adc:       e5832004        str     r2, [r3, #4]
14000ae0:       f57ff04e        dsb     st
*/
#else

	// Reconfigure PLLA
	Register::Write< PllConfig1,
					 PllConfig1::FracMode,
					 PllConfig1::DacPowerDown,
					 PllConfig1::FoutPowerDown,
					 PllConfig1::PostdivPowerDown,
					 PllConfig1::VcoOutPowerDown,
					 PllConfig1::PowerDown,
					 PllConfig1::Bypass,
					 PllConfig1::Refdiv,
					 PllConfig1::FBdiv>(
						PllConfig1::FracMode::Type::IntegerMode,
						PllConfig1::DacPowerDown::Type::Normal,
						PllConfig1::FoutPowerDown::Type::Normal,
						PllConfig1::PostdivPowerDown::Type::Normal,
						PllConfig1::VcoOutPowerDown::Type::Normal,
						PllConfig1::PowerDown::Type::Normal,
						PllConfig1::Bypass::Type::NoBypass,
						PllConfig1::Refdiv::Type(1),	// 24Mhz / 1 = 24MHz
						PllConfig1::FBdiv::Type(75)	// 24Mhz * 75 = 1800MHz
					 );

/*
;  ***** PllConfig1::VcoOutPowerDown, is not skipped - don't need to read register before write ******
14000ac4:       e301204b        movw    r2, #4171       	; R2 = New register value to write bits (Lo bits)
14000ac8:       e3402300        movt    r2, #768        	; R2 <= 0x0300104B, i.e. set bits 25-DacPowerDown, 25-FracMode-Integer, and values RefDiv=1 & FBDiv=0x4B (75 dec))
14000acc:       e5832004        str     r2, [r3, #4]		; [0x12010004] <= 0x0300104B, i.e. Write all register bits
14000ad0:       f57ff04e        dsb     st					; Write barier
*/

#endif


	Register::Write< PllConfig0,
					 PllConfig0::Frac,
					 PllConfig0::Postdiv1,
					 PllConfig0::Postdiv2 > (
						PllConfig0::Frac::Type(0),		// Frac = 0
						PllConfig0::Postdiv1::Type(2),	// 1800MHz / 2 = 900MHz
						PllConfig0::Postdiv2::Type(1)	// 900MHz / 1 = 900MHz
					 );

	pllWait(100);

	// Wait for PLLA is locked
	while( PllLockStatus::APll::Type::Locked != PllLockStatus::APll::get() ) {};
/*
; So wait for pll ready
14000ae8:       e3a02000        mov     r2, #0			; Load base address, low
14000aec:       e3412201        movt    r2, #4609       ; Load base addres, high. It is PIC-code (compiled with -fPIC)
14000af0:       f57ff04d        dsb     ld				; Memory barier
14000af4:       e59231e8        ldr     r3, [r2, #488]  ; R3 <= [0x120101e8]
14000af8:       e3130001        tst     r3, #1			; check (R3 & APLL-bit)
14000afc:       0afffffb        beq     14000af0 <_Z7pllInitv+0x6c>	;Loop if not set
*/

#if 0	
	while( 
		!Register::IsEqual<PllLockStatus, 
						   PllLockStatus::APll,
						   PllLockStatus::VPll>( 
							PllLockStatus::APll::Type::Locked,
							PllLockStatus::VPll::Type::Locked ) ){};
/*
; So, I can check different flags, or wait on it
; This is an example how to wait on both PLLs are locked (APLL & VPLL)
14000ae8:       e3a02000        mov     r2, #0			; Prepare, load register base address
14000aec:       e3412201        movt    r2, #4609       ; 0x12010000
14000af0:       f57ff04d        dsb     ld				; Read barer 
14000af4:       e59231e8        ldr     r3, [r2, #488]  ; R3 <= [0x120101e8], i.e. load reg value
14000af8:       e1e03003        mvn     r3, r3			; Not R3
14000afc:       e3130005        tst     r3, #5			; Test, bits 2, 0 is set
14000b00:       1afffffa        bne     14000af0 <_Z7pllInitv+0x6c> ;// Loop if not set
*/
#endif


	Register::Write< PllConfig7,
					 PllConfig7::FracMode,
					 PllConfig7::DacPowerDown,
					 PllConfig7::FoutPowerDown,
					 PllConfig7::PostdivPowerDown,
					 PllConfig7::PowerDown,
					 PllConfig7::Bypass,
					 PllConfig7::Refdiv,
					 PllConfig7::FBdiv>(
						PllConfig7::FracMode::Type::IntegerMode,
						PllConfig7::DacPowerDown::Type::Normal,
						PllConfig7::FoutPowerDown::Type::Normal,
						PllConfig7::PostdivPowerDown::Type::Normal,
						PllConfig7::PowerDown::Type::Normal,
						PllConfig7::Bypass::Type::NoBypass,
						PllConfig7::Refdiv::Type(1),	// 24Mhz / 1 = 24MHz
						PllConfig7::FBdiv::Type(99)	// 24Mhz * (99 + 1) = 2400MHz. Frac is in integer mode
					 );

	Register::Write< PllConfig6,
					 PllConfig6::Frac,
					 PllConfig6::Postdiv1,
					 PllConfig6::Postdiv2 > (
						PllConfig6::Frac::Type(1),		// Frac = 1 (Frac is in integer mode))
						PllConfig6::Postdiv1::Type(4),	// 2400MHz / 4 = 600MHz
						PllConfig6::Postdiv2::Type(1)	// 600MHz / 1 = 600MHz
					 );					 

	pllWait(100);
	// Wait for PLLV is locked
	while( PllLockStatus::VPll::Type::Locked != PllLockStatus::VPll::get() ) {};

	// Switch all clocks to there frequences
	Register::Write< SocClkSel,
			SocClkSel::DdrClkSel,
			SocClkSel::CoreA7ClkSel,
			SocClkSel::SysApbClock,
			SocClkSel::SysAxiClk,
			SocClkSel::SysCfgClk
			 > (
				SocClkSel::DdrClkSel::Type::Freq300MHz,
				SocClkSel::CoreA7ClkSel::Type::Freq900MHz,
				SocClkSel::SysApbClock::Type::Freq50MHZ,
				SocClkSel::SysAxiClk::Type::Freq200MHz,
				SocClkSel::SysCfgClk::Type::Freq100MHz
			);

}

