
#pragma once

/* (C) Evgeny Sobolev 09.02.1984 y.b, VRN, Russia,      */
/* At 16.02.2023                                        */

#include <stddef.h>
#include <stdint.h>
#include <RegistersClass.h>

struct RegAccessConfig {
        using Value   = uint32_t;
        using Address = uint32_t;
        static constexpr const bool ReadSync = true;
        static constexpr const bool WriteSync = true;
};

namespace PeriCrg {


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
struct PllConfig0 : public Register::Description<RegAccessConfig, 0x12010000> {
        // [30:28] The second stage of the APLL outputs the frequency division factor.
        struct Postdiv2 : public Field< 30, 28, uint8_t> {};
        // [26:24] APLL first-stage output frequency division coefficient.
        struct Postdiv1 : public Field< 26, 24, uint8_t> {};
        // [23:0]  The fractional part of the APLL multiplier coefficient.
        struct Frac     : public Field< 23,  0, uint32_t> {};
};

// PERI_CRG_PLL1/PERI_CRG_PLL7 is APLL/VPLL configuration register 1/7.
struct PllConfig1 :  public Register::Description<RegAccessConfig, 12010004> {
        // [26]    APLL clock divider bypass (bypass) control system.
        // 0: no bypass; 1: Bypass;
        enum class TBypass {
                NoBypass,
                Bypass
        };
        struct Bypass       : public Bit < 26, TBypass > {};

        // [25] APLL test signal control.
        // 1: power down working state; 0: Normal working state.
        enum class TDacPowerDown {
                PowerDown,
                Normal
        };
        struct DacPowerDown : public Bit < 25, TDacPowerDown> {};

        // [24] PLL fractional frequency control.
        // 0: decimal mode; 1: Integer mode.
        enum class TFracMode {
                DecimalMode,
                IntegerMode
        };
        struct FracMode : public Bit < 24, TFracMode > {};

        // [23] APLL Power Down control.
        // 1: power down working state; 0: Normal working state.
        enum class TPowerDownControl {
                Normal,
                PowerDown
        };
        struct PowerDown : public Bit < 23, TPowerDownControl > {};

        // [22] APLL VCO Output Power Down control.
        // 1: no clock output; 0: Normal output clock.
        enum class TVcoOutputPowerDown {
                Normal,
                PowerDown
        };
        struct VcoOutPowerDown : public Bit < 22, TVcoOutputPowerDown > {};

        // [21] APLL POSTDIV Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TPostdivPoweDown {
                Normal,
                PowerDown
        };
        struct PostdivPoweDown : public Bit< 21, TPostdivPoweDown > {};

        // [2] APLL FOUT Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TFoutPowerDown {
                Normal,
                NoClockOutput
        };
        struct FoutPowerDown : public Bit< 20, TFoutPowerDown > {};

        // [17:12] RW apll_refdiv APLL reference clock division factor.
        struct Refdiv : public Field < 17, 12, uint8_t> {};

        // [11:0] RW apll_fbdiv Integer part of APLL multiplier coefficient.
        struct FBdiv : public Field < 11, 0, uint16_t > {}; 
};

// PERI_CRG_PLL0/PERI_CRG_PLL6 is APLL/VPLL configuration register 0/6.
struct PllConfig6 : public Register::Description<RegAccessConfig, 0x12010018> {
        // [30:28] The second stage of the APLL outputs the frequency division factor.
        struct Postdiv2 : public Field< 30, 28, uint8_t> {};
        // [26:24] APLL first-stage output frequency division coefficient.
        struct Postdiv1 : public Field< 26, 24, uint8_t> {};
        // [23:0]  The fractional part of the APLL multiplier coefficient.
        struct Frac     : public Field< 23,  0, uint32_t> {};
};

// PERI_CRG_PLL1/PERI_CRG_PLL7 is APLL/VPLL configuration register 1/7.
struct PllConfig7 :  public Register::Description<RegAccessConfig, 0x1201001C> {
        // [26]    APLL clock divider bypass (bypass) control system.
        // 0: no bypass; 1: Bypass;
        enum class TBypass {
                NoBypass,
                Bypass
        };
        struct Bypass       : public Bit < 26, TBypass > {};

        // [25] APLL test signal control.
        // 1: power down working state; 0: Normal working state.
        enum class TDacPowerDown {
                PowerDown,
                Normal
        };
        struct DacPowerDown : public Bit < 25, TDacPowerDown> {};

        // [24] PLL fractional frequency control.
        // 0: decimal mode; 1: Integer mode.
        enum class TFracMode {
                DecimalMode,
                IntegerMode
        };
        struct FracMode : public Bit < 24, TFracMode > {};

        // [23] APLL Power Down control.
        // 1: power down working state; 0: Normal working state.
        enum class TPowerDownControl {
                Normal,
                PowerDown
        };
        struct PowerDown : public Bit < 23, TPowerDownControl > {};

        // [22] APLL VCO Output Power Down control.
        // 1: no clock output; 0: Normal output clock.
        enum class TVcoOutputPowerDown {
                Normal,
                PowerDown
        };
        struct VcoOutPowerDown : public Bit < 22, TVcoOutputPowerDown > {};

        // [21] APLL POSTDIV Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TPostdivPoweDown {
                Normal,
                PowerDown
        };
        struct PostdivPoweDown : public Bit< 21, TPostdivPoweDown > {};

        // [2] APLL FOUT Output Power Down control.
        // 1: no clock output; 0: Normal clock output.
        enum class TFoutPowerDown {
                Normal,
                NoClockOutput
        };
        struct FoutPowerDown : public Bit< 20, TFoutPowerDown > {};

        // [17:12] RW apll_refdiv APLL reference clock division factor.
        struct Refdiv : public Field < 17, 12, uint8_t> {};

        // [11:0] RW apll_fbdiv Integer part of APLL multiplier coefficient.
        struct FBdiv : public Field < 11, 0, uint16_t > {}; 
};

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

// PERI_CRG32 is the SOC clock selection register.
struct SocClkSel : public Register::Description<RegAccessConfig, 0x12010078> {
        // [10] SYSAPB clock selection.
        // 0: 24MHz; 1: 50MHz
        enum class TSysApbClock {
                Freq24MHz,
                Freq50MHZ
        };
        struct SysApbClock : public Bit<10, TSysApbClock> {};

        // [8] SYSCFG Clock selection.
        // 0: 24MHz; 1: 100MHz
        enum class TSysCfgClk {
                Freq24MHz,
                Freq100MHz
        };
        struct SysCfgClk : public Bit<8, TSysCfgClk> {};

        // [6] SYSAXI clock selection.
        // 0: 24MHz; 1: 200MHz
        enum class TSysAxiClk {
                Freq24MHz,
                Freq200MHz
        };
        struct SysAxiClk : public Bit<6,TSysAxiClk> {};

        // [5:3] DDR clock selection.
        // 000: 24MHz; 011: 300MHz;
        // Others: No selection is allowed.
        enum class TDdrClkSel {
                Freq24MHz,
                Freq300MHz = 0x03
        };
        struct DdrClkSel : public Field<5,3, TDdrClkSel> {};

        // [1:0] A7 clock selection.
        // 00: 24MHz; 01: 900MHz; 11: 600MHz;
        // Other: reserved.
        enum class TCoreA7ClkSel {
                Freq24MHz,
                Freq900MHz = 0x01,
                Freq600MHz = 0x03            
        };
        struct CoreA7ClkSel : public Field<1,0, TCoreA7ClkSel> {};
};

// PERI_CRG_PLL122 It is the PLL LOCK status register.
struct PllLockStatus : public Register::Description<RegAccessConfig, 0x120101E8 > {
        // [2] VPLL LOCK state.
        // 0: Unlock; 1: Locked.
        enum class TVPll {
                Unlock,
                Locked
        };
        struct VPll : public Bit<2, TVPll> {};
        
        // [0] APLL LOCK state.
        // 0: Unlock; 1: Locked.
        enum class TAPll {
                Unlock,
                Locked
        };
        struct APll : public Bit<0, TAPll> {};
};

} // namespace PeriCrg
