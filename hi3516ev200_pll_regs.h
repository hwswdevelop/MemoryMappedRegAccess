
#pragma once

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia,      */
/* At 16.02.2023                                        */

#include <stddef.h>
#include <stdint.h>
#include <RegistersClass.h>


namespace PeriCrg {

using namespace Register;

/*
        HiSilicon DataSheet.
        Fbdiv: integer part of multiplier coefficient;
        frac: the fractional part of the multiplication factor;
        refdiv: reference clock frequency division factor;
        pstdiv1: first stage output frequency division coefficient;
        pstdiv2: second stage output frequency division coefficient.
        For the configuration coefficients of each PLL, see the corresponding bits of the corresponding configuration registers in Table 3-3.
*/

// PERI_CRG_PLL0/PERI_CRG_PLL6 is APLL/VPLL configuration register 0/6.
struct PllConfig0 : public Description<0x12010000> {
        // [30:28] The second stage of the APLL outputs the frequency division factor.
        typedef RW< getAddress(), Field< 30, 28, uint8_t>> Postdiv2;
        // [26:24] APLL first-stage output frequency division coefficient.
        typedef RW< getAddress(), Field< 26, 24, uint8_t>> Postdiv1;
        // [23:0]  The fractional part of the APLL multiplier coefficient.
        typedef RW< getAddress(), Field< 23,  0, uint32_t>> Frac;
        // [27], [31] - Reservied fields
        typedef RS< getAddress(), Field< 27, 27 >, 
                RS< getAddress(), Field< 31, 31> > > Reserved;
};

// PERI_CRG_PLL1/PERI_CRG_PLL7 is APLL/VPLL configuration register 1/7.
struct PllConfig1 :  public Description<0x12010004> {
        // [26]    APLL clock divider bypass (bypass) control system.
        // 0: no bypass; 1: Bypass;
        enum class TBypass {
                NoBypass,
                Bypass
        };
        typedef RW< getAddress(), Bit < 26, TBypass > > Bypass;

        // [25] APLL test signal control.
        // 1: power down working state; 0: Normal working state.
        enum class TDacPowerDown {
                PowerDown,
                Normal
        };
        typedef RW< getAddress(), Bit < 25, TDacPowerDown> > DacPowerDown;

        // [24] PLL fractional frequency control.
        // 0: decimal mode; 1: Integer mode.
        enum class TFracMode {
                DecimalMode,
                IntegerMode
        };
        typedef RW< getAddress(), Bit < 24, TFracMode >>  FracMode;

        // [23] APLL Power Down control.
        // 1: power down working state; 0: Normal working state.
        enum class TPowerDownControl {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit < 23, TPowerDownControl >> PowerDown;
 
        // [22] APLL VCO Output Power Down control.
        // 1: no clock output; 0: Normal output clock.
        enum class TVcoOutputPowerDown {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit < 22, TVcoOutputPowerDown >> VcoOutPowerDown;
 
        // [21] APLL POSTDIV Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TPostdivPowerDown {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit< 21, TPostdivPowerDown >> PostdivPowerDown;


        // [20] APLL FOUT Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TFoutPowerDown {
                Normal,
                NoClockOutput
        };
        typedef RW< getAddress(), Bit< 20, TFoutPowerDown >> FoutPowerDown;

        // [17:12] RW apll_refdiv APLL reference clock division factor.
        typedef RW< getAddress(), Field < 17, 12, uint8_t>> Refdiv;

        // [11:0] RW apll_fbdiv Integer part of APLL multiplier coefficient.
        typedef RW< getAddress(), Field < 11, 0, uint16_t>> FBdiv;

        // [31:27], [19:18] Reserved
        typedef RS< getAddress(), Field<19, 18>,
                RS< getAddress(), Field<31,27> > > Reserved;
 
};

// PERI_CRG_PLL0/PERI_CRG_PLL6 is APLL/VPLL configuration register 0/6.
struct PllConfig6 : public Description< 0x12010018 > {
        // [30:28] The second stage of the APLL outputs the frequency division factor.
        typedef RW< getAddress(), Field< 30, 28, uint8_t>> Postdiv2;
        // [26:24] APLL first-stage output frequency division coefficient.
        typedef RW< getAddress(), Field< 26, 24, uint8_t>> Postdiv1;
        // [23:0]  The fractional part of the APLL multiplier coefficient.
        typedef RW< getAddress(), Field< 23,  0, uint32_t>> Frac;
        // [27], [31] - Reservied fields
        typedef RS< getAddress(), Field< 27, 27 >, 
                RS< getAddress(), Field< 31, 31> > > Reserved;
};

// PERI_CRG_PLL1/PERI_CRG_PLL7 is APLL/VPLL configuration register 1/7.
struct PllConfig7 :  public Description< 0x1201001C > {
        // [26]    APLL clock divider bypass (bypass) control system.
        // 0: no bypass; 1: Bypass;
        enum class TBypass {
                NoBypass,
                Bypass
        };
        typedef RW< getAddress(), Bit < 26, TBypass > > Bypass;

        // [25] APLL test signal control.
        // 1: power down working state; 0: Normal working state.
        enum class TDacPowerDown {
                PowerDown,
                Normal
        };
        typedef RW< getAddress(), Bit < 25, TDacPowerDown> > DacPowerDown;

        // [24] PLL fractional frequency control.
        // 0: decimal mode; 1: Integer mode.
        enum class TFracMode {
                DecimalMode,
                IntegerMode
        };
        typedef RW< getAddress(), Bit < 24, TFracMode >>  FracMode;

        // [23] APLL Power Down control.
        // 1: power down working state; 0: Normal working state.
        enum class TPowerDownControl {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit < 23, TPowerDownControl >> PowerDown;
 
        // [22] APLL VCO Output Power Down control.
        // 1: no clock output; 0: Normal output clock.
        enum class TVcoOutputPowerDown {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit < 22, TVcoOutputPowerDown >> VcoOutPowerDown;
 
        // [21] APLL POSTDIV Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TPostdivPowerDown {
                Normal,
                PowerDown
        };
        typedef RW< getAddress(), Bit< 21, TPostdivPowerDown >> PostdivPowerDown;


        // [20] APLL FOUT Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TFoutPowerDown {
                Normal,
                NoClockOutput
        };
        typedef RW< getAddress(), Bit< 20, TFoutPowerDown >> FoutPowerDown;

        // [17:12] RW apll_refdiv APLL reference clock division factor.
        typedef RW< getAddress(), Field < 17, 12, uint8_t>> Refdiv;

        // [11:0] RW apll_fbdiv Integer part of APLL multiplier coefficient.
        typedef RW< getAddress(), Field < 11, 0, uint16_t>> FBdiv;
        
        // [31:27], [19:18] Reserved
        typedef RS< getAddress(), Field<19, 18>,
                RS< getAddress(), Field<31,27> > > Reserved;
 
};


#if 0

// PERI_CRG20 is the APLL spread spectrum configuration register.
struct PllASpectrumSpread : public Register::Description<RegAccessConfig, 0x12000050>  {

        // [12:9]  ssmod divval[12:9]: SSMOD divval control.
        // 0x0: no frequency division;
        // 0x1: no frequency division;
        // 0x2: divide by 2;
        // 0xF: divide by 15.
        struct SsModDivider : public Field< 12, 9 > {};

        // [8:4]  ssmod spread[8:4] SSMOD spread control.
        // 1：0.1%；2：0.2%; 3：0.3%；...; 6：0.6%; ... 31：3.1%
        struct SpectrumSpreadMod : public Field< 8, 4 > {};

        // [3] SSMOD downspread control.
        // 0: Intermediate spread frequency;
        // 1: Spread spectrum downward.
        enum class TSpectrumDownSpread {
                IntermediateSpreadFrequency,
                EnableSpreadDownward       
        };
        struct SpectrumDownSpreadControl : public Bit< 3, TSpectrumDownSpread > {};

        // [2] ssmod_disable[2] SSMOD disable control.
        // 0: enable;
        // 1: disable.
        enum class TSpectrumSpreadMode {
                Enable,
                Disable
        };
        struct SpectrumSpreadMode : public Bit < 2, TSpectrumSpreadMode > {};

        // [1] ssmod_rst_req SSMOD reset control.
        // 0: no reset;
        // 1: Reset.
        enum class TSpectrumSpreadModeReset {
                NoReset,
                Reset
        };
        struct SpectrumSpreadModeReset : public Bit < 1, TSpectrumSpreadModeReset > {};

        // [0] SSMOD clock gating configuration, default off close.
        // 0: off;
        // 1: open.
        enum class TClockGating {
                Off,
                Open
        };
        struct ClockGating : public Bit <0, TClockGating > {};

};

// PERI_CRG23 Exhibition for VPLL
struct VpllExhibition : public Register::Description<RegAccessConfig, 0x1201005C> {

        // [12:9] ssmod divval[12:9]: SSMOD divval control.
        // 0x0: no frequency division;
        // 0x1: no frequency division;
        // 0x2: divide by 2;
        // 0x3: divide by 3;
        struct SsModDivVal : public Field < 12, 9, uint8_t> {};

        // [8:4] ssmod spread[8:4] SSMOD spread control.
        // 0 : 0; 1: 0.1%; 2: 0.2%
        // 3: 0.3%; 4: 0.4%; 5: 0.5%;
        struct SsVModSpreadVal : public Field < 8, 4, uint8_t > {};

        // [3] ssmod downspread[3] SSMOD downspread control.
        // 0: Intermediate spread frequency;
        // 1: Spread spectrum downward.
        enum class TSpreadMode {
                Intermediate,
                SpreadDownward
        };
        struct SpreadMode : public Bit< 3, TSpreadMode > {};

        // [2] ssmod_disable[2] SSMOD disable control.
        // 0: enable;
        // 1: disable.
        enum class TSsModEn {
                Enable,
                Disable
        };
        struct SsModEn : public Bit< 2, TSsModEn > {};

        // [1] ssmod_rst_req[1] SSMOD reset control.
        // 0: no reset;
        // 1: reset.
        enum class TSsModRstReq {
                NoRequest,
                Reset
        };
        struct SsModRstReq : public Bit< 1, TSsModRstReq > {};

        // [0] ssmod_cken[0] SSMOD clock gating configuration, disabled by default.
        // 0: off; 1: open.
        enum class TSsModClkEn {
                Off,
                Open
        };
        struct SsModClkEn : public Bit< 0, TSsModClkEn > {};
 
};

// PERI_CRG30 is the CPU frequency mode and reset configuration register.
struct ResetConfig : public Register::Description<RegAccessConfig, 0x12010078>  {
        // [10] CS soft reset request.
        // 0: cancel reset;
        // 1: Reset
        enum class TCSSoftResetRequest {
                CancelReset,
                Reset
        };
        struct CSSoftResetRequest : public Bit< 10, TCSSoftResetRequest > {};

        // [9] Soft reset request for SOCDBG.
        // 0: cancel reset;
        // 1: Reset.
        enum class TSocDbgResetRequest {
                CancelReset,
                Reset
        };
        struct SocDbgResetRequest : public Bit< 9, TSocDbgResetRequest > {};

        // [8] Soft reset request for L2.
        // 0: cancel reset;
        // 1: Reset.
        enum class TL2SoftResetRequest {
                CancelRequest,
                Reset
        };
        struct L2SoftResetRequest : public Bit < 8, TL2SoftResetRequest > {};

        // [3] A7 Soft reset request for DBG.
        // 0: cancel reset;
        // 1: Reset.
        enum class TArmDbgSoftRequest {
                CancelRequest,
                Request
        };
        struct ArmDbgSoftRequest : public Bit < 3, TArmDbgSoftRequest > {};

        // [1] Soft reset request for A7
        // 0: cancel reset;
        // 1: Reset.
        enum class TArmSoftRequest {
                CancelRequest,
                Request
        };
        struct ArmSoftRequest : public Bit < 1, TArmSoftRequest > {};

        // [0] A7 PCLKDBG Clock Gating
        // 0: clock off;
        // 1: Clock is on.
        enum class TArmPclkClockGating {
                ClockOff,
                ClockOn
        };
        struct ArmPclkClockGating : public Bit <0, TArmPclkClockGating> {};
};

// PERI_CRG31 Configure registers for DDR clock and reset.
struct DdrClkAndRst : public Register::Description<RegAccessConfig, 0x1201007C> {
        // [6] DDR APB Gating Configuration Register
        // 0: turn off the clock;
        // 1: Turn on the clock.
        enum class TDdrApbClk {
                Disabled,
                Enabled
        };
        struct DdrApbClk : public Bit<6,TDdrApbClk> {};

        // [5] DDR HiPACK Gating Configuration Register
        // 0: turn off the clock;
        // 1: Turn on the clock.
        enum class TDdrHiPackClk {
                Disable,
                Enable
        };
        struct DdrHiPackClk : public Bit<5,TDdrHiPackClk> {};

        // [4] DDR CFG Gating Configuration Register
        // 0: turn off the clock;
        // 1: Turn on the clock.
        enum class TDdrCfgClk {
                Disable,
                Enable
        };
        struct DdrCfgClk : public Bit<5,TDdrCfgClk> {};

        // [2] DDR PHY PLL spread spectrum clock bypass configuration register
        // 0: bypass the spread spectrum clock of DDR PHY PLL;
        // 1: Use the spread spectrum clock of DDR PHY PLL.
        enum class TSpreadSpectrum {
                Bypass,
                Enabled
        };
        struct SpreadSpectrum : public Bit<2,TSpreadSpectrum> {};

        // [1] DDR APB soft reset request
        // 0: cancel reset;
        // 1: reset.
        enum class TApbSoftReset {
                Cancel,
                Reset
        };
        struct ApbSoftReset : public Bit<1, TApbSoftReset> {};

        // [0] DDR HiPACK soft reset request
        // 0: cancel reset;
        // 1: reset.
        enum class TDdrHiPackReset {
                Cancel,
                Reset
        };
        struct DdrHiPackReset : public Bit<0, TDdrHiPackReset> {};
        
};

#endif

// PERI_CRG32 is the SOC clock selection register.
struct SocClkSel : public Register::Description< 0x12010080 > {
        // [10] SYSAPB clock selection.
        // 0: 24MHz; 1: 50MHz
        enum class TSysApbClock {
                Freq24MHz,
                Freq50MHZ
        };
        typedef RW< getAddress(), Bit<10, TSysApbClock>> SysApbClock;
 
        // [8] SYSCFG Clock selection.
        // 0: 24MHz; 1: 100MHz
        enum class TSysCfgClk {
                Freq24MHz,
                Freq100MHz
        };
        typedef RW< getAddress(), Bit<8, TSysCfgClk>> SysCfgClk;
 
        // [6] SYSAXI clock selection.
        // 0: 24MHz; 1: 200MHz
        enum class TSysAxiClk {
                Freq24MHz,
                Freq200MHz
        };
        typedef RW< getAddress(), Bit<6,TSysAxiClk>> SysAxiClk;

        // [5:3] DDR clock selection.
        // 000: 24MHz; 011: 300MHz;
        // Others: No selection is allowed.
        enum class TDdrClkSel {
                Freq24MHz,
                Freq300MHz = 0x03
        };
        typedef RW< getAddress(), Field<5,3, TDdrClkSel>> DdrClkSel;
        
        // [1:0] A7 clock selection.
        // 00: 24MHz; 01: 900MHz; 11: 600MHz;
        // Other: reserved.
        enum class TCoreA7ClkSel {
                Freq24MHz,
                Freq900MHz = 0x01,
                Freq600MHz = 0x03            
        };
        typedef RW< getAddress(), Field<1,0, TCoreA7ClkSel>> CoreA7ClkSel;

        // [31:11], [9], [7], [2] - Reserved
        typedef RS< getAddress(), Bit<2>,
                RS< getAddress(), Bit<7>,
                RS< getAddress(), Bit<9>,
                RS< getAddress(), Field<31,11> >>>> Reserved;
};


#if 0
// PERI_CRG40 Configure the register for the frequency of the media function block.
struct MediaBlockFreq : public Register::Description<RegAccessConfig, 0x120100A0> {
        // [22:20] VPSS clock selection.
        // 000: 257MHz;
        // 001: 200MHz;
        // 010: 150MHz;
        // 011: 100MHz;
        // 100: 75MHz;
        enum class TVpssClk {
                Freq257MHz,
                Freq200MHz,
                Freq150MHz,
                Freq100MHz,
                Freq75MHz
        };
        struct VpssClk : public Field< 22, 20, TVpssClk > {};

        // [18] JPGE clock selection.
        // 0: 450MHz; 1: 300MHz.
        enum class TJpgeClk {
                Freq450MHz,
                Freq300MHz
        };
        struct JpgeClk : public Bit< 18, TJpgeClk > {};

        // [10] VDEU clock selection.
        // 0: 450MHz; 1: 300MHz.
        enum class TVeduClk {
                Freq450MHz,
                Freq300MHz
        };
        struct VeduClk : public Bit< 10, TVeduClk > {};

        // [2:1] IVE clock selection.
        // 00: 450MHz;
        // 01: 360MHz;
        // 10: 300MHz;
        // 11: 100MHz.
        enum class TIveClockSel {
                Freq450MHz,
                Freq360MHz,
                Freq300MHz,
                Freq100MHz
        };
        struct IveClockSel : public Field< 2, 1, TIveClockSel > {};

};
#endif

// PERI_CRG_PLL122 It is the PLL LOCK status register.
struct PllLockStatus : public Register::Description< 0x120101E8 > {
        // [2] VPLL LOCK state.
        // 0: Unlock; 1: Locked.
        enum class TVPll {
                Unlock,
                Locked
        };
        typedef RW< getAddress(), Bit<2, TVPll>> VPll;
        
        // [0] APLL LOCK state.
        // 0: Unlock; 1: Locked.
        enum class TAPll {
                Unlock,
                Locked
        };
        typedef RW< getAddress(), Bit<0, TAPll>> APll;

        // [31:3], [1] - Reserved
        typedef RS< getAddress(), Bit<1>,
                RS< getAddress(), Field<31,3>>> Reserved;

};

} // namespace PeriCrg

