
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace Register {

	/* Defualt memory mapped register description */
	template<typename RegValueType = uint32_t, typename BusAccessType = uint32_t>
	struct MemIoDescription {
		/* Bus address type */
		using BusDataType = BusAccessType;

		/* Register value type ( uint8_t, uint16_t, uint32_t, uint64_t )*/
		using RegDataType = RegValueType;
		
		/* Access type */
		struct Access {	
			/* Read rules */
			struct Read {
				/* Is read operation of this register allowed? */
				static constexpr const bool readable = true;
				/* Synchronization policy */
				struct Sync {
					/* CPU syncronization like memory syncronization barier */
					static constexpr const bool cpu = true;
					/* Cahce syncronization, like cahce line invalidation, or read buffer sync */
					static constexpr const bool cache = false;
				};
			};			
			/* Write rules */
			struct Write {
				/* I write operation allowed? */
				static constexpr const bool writable = true;
				struct Sync {
					/* CPU syncronization like memory synctonization barier */
					static constexpr const bool cpu = true;
					/* Cahce syncronization like cahce write back (line close), or write buffer flush */
					static constexpr const bool cache = false;
				};
			};

		};
		
		/* Modification */		
		struct Modification {	
			/* Accepted when read register from memory */
			struct Read {
				/* Do I need to apply logical "AND" to value, after register is read? */
				struct AndMask {
					static constexpr const bool enabled  = false;
					static constexpr const RegDataType value  = ~(static_cast<RegDataType>(0));
				};
				/* Do I need to apply logical "OR" to value after register is read? */
				struct OrMask {
					static constexpr const bool enabled  = false;
					static constexpr const RegDataType value  = 0;
				};
			};
			/* Accepted when write this register to memory */
			struct Write {
				/* Do I need apply "AND" mask to value, before write into regeriter? */
				struct AndMask {
					static constexpr const bool enabled  = false;
					static constexpr const RegDataType value  = ~(static_cast<RegDataType>(0));
				};
				/* Do I need to apply "OR" maks to value, before write into register? */
				struct OrMask {
					static constexpr const bool enabled  = false;
					static constexpr const RegDataType value  = 0;
				};
			};
		};
				
	};
		
	/* Describe default memory access */
	using Mem32IoDescription = MemIoDescription<uint32_t, uint32_t>;
	using Mem16IoDescription = MemIoDescription<uint16_t, uint32_t>;
	using Mem8IoDescription  = MemIoDescription<uint8_t,  uint32_t>;
		
} // Register



