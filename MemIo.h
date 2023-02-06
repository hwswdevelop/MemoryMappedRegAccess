
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Register {

	template<typename IoDescription>
	struct MemIo {
		using IoDescr = IoDescription;
		using Access = typename IoDescr::Access;
		using Modification = typename IoDescr::Modification;
		
		/* Initlize. Set address */
		constexpr MemIo(const typename IoDescr::BusDataType address) : _address(address) {}
			
		/* Return register address */
		inline typename IoDescr::BusDataType address() const {
			return _address;
		}
		
		/* Return register data size */
		inline size_t size() const {
			return sizeof(typename IoDescr::RegDataType);
		}

		/* Read from regiter if possible */
		inline typename IoDescr::RegDataType read() const {
			static_assert(Access::Read::readable, "Register is not readable" );
			readSync();
			typename IoDescr::RegDataType regValue  = *reinterpret_cast<volatile typename IoDescr::RegDataType*>(_address);
			/* Apply "AND Mask", if required */
			if constexpr ( Modification::Read::AndMask::enabled ) {
				regValue &= Modification::Read::AndMask::value;
			}
			/* Apply "OR Mask", if required */
			if constexpr ( Modification::Read::OrMask::enabled ) {
				regValue |= Modification::Read::OrMask::value;
			}
			return regValue;
		};
		
		/* Write into register if possible, mask apply before write */
		inline void write(const typename IoDescr::RegDataType value) const {
			static_assert(Access::Write::writable, "Register is not writable" );
			typename IoDescr::RegDataType regValue = value;
			/* Apply "AND Mask", if required */
			if constexpr ( Modification::Write::AndMask::enabled ) {
				regValue &= Modification::Write::AndMask::value;
			}
			/* Apply "OR Mask", if required */
			if constexpr ( Modification::Write::OrMask::enabled ) {
				regValue |= Modification::Write::OrMask::value;
			}

			*reinterpret_cast<volatile typename IoDescr::RegDataType*>(_address) = regValue;
			writeSync();
		};
		
	private:
		inline void readSync() const {
			/* Cache sync */
			if constexpr( Access::Read::Sync::cache ) {
				static_assert(!Access::Read::Sync::cache, "Please implement functionality");
			}
			/* CPU Sync */
			if constexpr( Access::Read::Sync::cpu ) {
				asm("dmb");
			}
		}

		inline void writeSync() const {
			/* CPU sync */
			if constexpr( Access::Write::Sync::cpu ) {
				asm("dsb");
			}
			/* Cache or/and WriteBuffer sync */
			if constexpr( Access::Write::Sync::cache ) {
				static_assert(!Access::Write::Sync::cache, "Please implement functionality");
			}
		}	
	private:
		const typename IoDescr::BusDataType _address {};					
	};

} // Register



