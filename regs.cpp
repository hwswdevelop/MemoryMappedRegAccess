	

#include <stddef.h>
#include <stdint.h>

#include <MemIo.h>
#include <MemIoDescription.h>



namespace Register {

	using MemIo32 = MemIo<Mem32IoDescription>;
	using MemIo16 = MemIo<Mem16IoDescription>;
	using MemIo8  = MemIo<Mem8IoDescription>;

} // Register

namespace Register {

	struct Description {
	
		using RegDataType = uint32_t;
	
		template<size_t BitNo = 0, typename DataType = bool>
		struct Bit {
			using Type = DataType;
			static constexpr const size_t LSB = BitNo;
			static constexpr const size_t MSB = BitNo;
		};
		
		template<size_t msb = 0, size_t lsb = 0, typename DataType = RegDataType >
		struct Field {
			using Type = DataType;
			static constexpr const size_t LSB = lsb;
			static constexpr const size_t MSB = msb;
		};

	};

} // Register


// Packed structute????? 
// Will not solve problem of endianess....
// Don't work correctly with some types of comilers.


namespace Register {

	template<typename RegisterIo = MemIo32>
	struct SyncRegister {
		using RegDataType = typename RegisterIo::IoDescr::RegDataType;
		using BusDataType = typename RegisterIo::IoDescr::BusDataType;

		constexpr SyncRegister(const BusDataType address) : _regIo(address) {};

		template<typename Field>
		static constexpr const RegDataType getMaskAtLsb() {
			constexpr const size_t bitCount = (Field::MSB - Field::LSB) + 1; // +1 -1 will be optimized as const		
			constexpr const RegDataType mask = ( static_cast<const RegDataType>(1) << (bitCount - 1) ) |
						           ( ( static_cast<const RegDataType>(1) << (bitCount - 1) ) );
			return mask; 
		}
		
		template<typename Field>
		static constexpr const RegDataType getMask() {
			constexpr const size_t bitCount = (Field::MSB - Field::LSB) + 1; // +1 -1 will be optimized as const		
			constexpr const RegDataType zMask = ( static_cast<const RegDataType>(1) << (bitCount - 1) ) |
						           ( ( static_cast<const RegDataType>(1) << (bitCount - 1) ) );				
			constexpr const RegDataType mask =  zMask << Field::LSB;
			return mask;
		}

		template<typename Field>
		inline typename Field::Type get() {
			constexpr const RegDataType mask = getMaskAtLsb<Field>();
			return static_cast<typename Field::Type>( ( _regIo.read()  >> Field::LSB) & mask );
		}	
			
		template<typename Field>
		inline void set(typename Field::Type value) {
			constexpr const RegDataType mask = getMask<Field>();
			constexpr const RegDataType lsbMask = getMaskAtLsb<Field>();
			RegDataType regValue = _regIo.read();
			regValue &= ~mask;
			regValue |= (static_cast<RegDataType>(value) &  lsbMask) << Field::LSB;
			_regIo.write(regValue);
		}
		
	private:
		const RegisterIo 	_regIo {};		
	};
	
	template<typename RegisterIo = MemIo32>
	struct AsyncRegister {
		using RegDataType = typename RegisterIo::IoDescr::RegDataType;
		using BusDataType = typename RegisterIo::IoDescr::BusDataType;

		constexpr AsyncRegister(const BusDataType address) : _regIo(address) {
			_value = _regIo.read();
		};

		~AsyncRegister() {
			_regIo.write(_value);
		}

		template<typename Field>
		static constexpr const RegDataType getMaskAtLsb() {
			constexpr const size_t bitCount = (Field::MSB - Field::LSB) + 1; // +1 -1 will be optimized as const		
			constexpr const RegDataType mask = ( static_cast<const RegDataType>(1) << (bitCount - 1) ) |
						           ( ( static_cast<const RegDataType>(1) << (bitCount - 1) ) );
			return mask; 
		}
		
		template<typename Field>
		static constexpr const RegDataType getMask() {
			constexpr const size_t bitCount = (Field::MSB - Field::LSB) + 1; // +1 -1 will be optimized as const		
			constexpr const RegDataType zMask = ( static_cast<const RegDataType>(1) << (bitCount - 1) ) |
						           ( ( static_cast<const RegDataType>(1) << (bitCount - 1) ) );				
			constexpr const RegDataType mask =  zMask << Field::LSB;
			return mask;
		}
								
		template<typename Field>
		inline typename Field::Type get() {
			constexpr const RegDataType mask = getMaskAtLsb<Field>();
			return static_cast<typename Field::Type>( ( _value >> Field::LSB ) & mask );
		}
	
		template<typename Field>
		inline void set(typename Field::Type value) {
			constexpr RegDataType mask = getMask<Field>();
			constexpr RegDataType lsbMask = getMaskAtLsb<Field>();
			_value &= mask;
			_value |= ( static_cast<RegDataType>(value) &  lsbMask ) << Field::LSB;									
		}
		
	private:
		const RegisterIo 	_regIo {};
		RegDataType		_value {};
		
	};

} // Register


enum class SystemClockMode {
	Mode0 = 0,
	Mode1,
	Mode2	
};

using fSystemClock_Enable = Register::Description::Bit<0>;
using fSystemClock_Mode   = Register::Description::Field<3,1, SystemClockMode>;
using fSystemClock_Minute = Register::Description::Field<10,4, uint8_t>;
using fSystemClock_Hour   = Register::Description::Field<15,11, uint8_t>;
using fSystemClock_Day    = Register::Description::Field<31,16, uint16_t>;

Register::SyncRegister<> rSystemClock(0x12000000);

void pllInit() {
	/* Single line write (read-modify-write) */
	rSystemClock.set<fSystemClock_Enable>(false);

	{	
		Register::AsyncRegister<> SystemClock(0x12000000);
		
		/// ************* +/- ?????? ******************
		// Commit on desctructor
		// Optized access to fields Read-Mdify-Write
		// Parallel write into port
		// Dont'use packed attribute. 
		// What about endian problems ????
		// Possible make field read/write, value control
		// Possible to check regiter and field comparation
		// Possible to check value ranges
		// Possible to check data types 
		// BUT.... RECORD IS NOT SO SIMPLE ....
		// BUT.... RECORD IS NOT SO SIMPLE ....
		// BUT.... RECORD IS NOT SO SIMPLE ....
		// BUT.... RECORD IS NOT SO SIMPLE ....
		// Lines ... will be optimized to single value
		
		SystemClock.set<fSystemClock_Mode>(SystemClockMode::Mode1);
		SystemClock.set<fSystemClock_Minute>(1);
		SystemClock.set<fSystemClock_Hour>(2);
		SystemClock.set<fSystemClock_Day>(128);
	}
	
	/* Single line write (read-modify-write) */
	rSystemClock.set<fSystemClock_Enable>(true);
	
	// So, I think, after optimization, this code will take a few lines.

/*
14000a64 <_Z7pllInitv>:
14000a64:       f57ff05f        dmb     sy
14000a68:       e3003ad4        movw    r3, #2772       ; 0xad4
14000a6c:       e3413400        movt    r3, #5120       ; 0x1400
14000a70:       e5931000        ldr     r1, [r3]
14000a74:       e5912000        ldr     r2, [r1]
14000a78:       e3c22001        bic     r2, r2, #1
14000a7c:       e5812000        str     r2, [r1]
14000a80:       f57ff04f        dsb     sy
14000a84:       f57ff05f        dmb     sy
14000a88:       e3a02412        mov     r2, #301989888  ; 0x12000000
14000a8c:       e5921000        ldr     r1, [r2]
14000a90:       e3a01000        mov     r1, #0
14000a94:       e5821000        str     r1, [r2]
14000a98:       f57ff04f        dsb     sy
14000a9c:       f57ff05f        dmb     sy
14000aa0:       e5932000        ldr     r2, [r3]
14000aa4:       e5923000        ldr     r3, [r2]
14000aa8:       e3833001        orr     r3, r3, #1
14000aac:       e5823000        str     r3, [r2]
14000ab0:       f57ff04f        dsb     sy
14000ab4:       e12fff1e        bx      lr
*/	
	 
		
}



