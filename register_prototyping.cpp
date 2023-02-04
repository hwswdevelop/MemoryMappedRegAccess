
#include <stddef.h>
#include <stdint.h>

struct DefaultRegister {

	struct Access {
		using BusType = uint32_t;
		using Type = uint32_t;
		static constexpr const bool checkAlignment = false;
		static constexpr const Type zeroMask = 0;
		static constexpr const Type ffMask = ~(static_cast<Type>(0));
	};

	struct Policy {

		struct Read {
	
			static constexpr const bool readable = true;

			struct AndMask {
				static constexpr const bool enabled  = false;
				static constexpr const Access::Type value  = Access::ffMask;
			};

			struct OrMask {
				static constexpr const bool enabled  = false;
				static constexpr const Access::Type value  = Access::zeroMask;
			};
	
			struct Sync {
				static constexpr const bool cpu = false;
				static constexpr const bool cache = false;
			};
		};

		struct Write {

			static constexpr const bool writable = true;
			static constexpr const bool maskValues = true;

			struct AndMask {
				static constexpr const bool enabled  = false;
				static constexpr const Access::Type value  = Access::ffMask;
			};

			struct OrMask {
				static constexpr const bool enabled  = false;
				static constexpr const Access::Type value  = Access::zeroMask;
			};

			struct Sync {
				static constexpr const bool cpu = true;
				static constexpr const bool cache = false;
			};
		};

	};
	
	struct Description {
	
		template<size_t BitNo>
		struct Bit {
			using Type = bool;
			static constexpr const size_t LSB = BitNo;
			static constexpr const size_t MSB = BitNo;
		};

		template<size_t msb, size_t lsb>
		struct Field {
			using Type = Access::Type;
			static constexpr const size_t LSB = lsb;
			static constexpr const size_t MSB = msb;
		};
		
		struct Value : public Field<31,0> {};
									
	};

};


template<typename Access = DefaultRegister::Access, typename Policy = DefaultRegister::Policy>
struct Register {

	constexpr Register(const typename Access::BusType address) : _address(address) {
		if ( Access::checkAlignment ){	
			if constexpr ( sizeof(typename Access::Type) > 1 ) {
				constexpr const typename Access::Type loBitMask =
					( ( static_cast<const typename Access::Type>(1) ) << ( sizeof(typename Access::Type) - 2 ) ) - 1 ;
				//static_assert( (0 == ( address & loBitMask) ), "Register address is not alligned");
				// TO DO: fix unalligned access
				static_assert( !Access::checkAlignment, "Message not supported, loop only" );
				while( _address & loBitMask );
			}
		}
	};

	inline typename Access::BusType address() const {
		return _address;
	}
	
	inline size_t size() const {
		return sizeof(typename Access::Type);
	}

	inline void write(const typename Access::Type value) const {
		static_assert(Policy::Write::writable, "Register is not writable" );		
		typename Access::Type regValue = value;
		/* Apply "AND Mask", if required */
		if constexpr ( Policy::Write::AndMask::enabled ) {
			regValue &= Policy::Write::AndMask::value;
		}
		/* Apply "OR Mask", if required */
		if constexpr ( Policy::Write::OrMask::enabled ) {
			regValue |= Policy::Write::OrMask::value;
		}

		*reinterpret_cast<volatile typename Access::Type*>(_address) = regValue;
		writeSync();
	};
	
	inline typename Access::Type read()  const {
		static_assert(Policy::Read::readable, "Register is not readable" );
		readSync();
		typename Access::Type regValue  = *reinterpret_cast<volatile typename Access::Type*>(_address);
		/* Apply "AND Mask", if required */
		if constexpr ( Policy::Read::AndMask::enabled ) {
			regValue &= Policy::Read::AndMask::value;
		}
		/* Apply "OR Mask", if required */
		if constexpr ( Policy::Read::OrMask::enabled ) {
			regValue |= Policy::Read::OrMask::value;
		}
		return regValue;
	};
	
	inline void set(const typename Access::Type setBitMask) const {
		typename Access::Type regValue = read();
		regValue |= setBitMask;
		write(regValue);
	}
	
	inline void clear(const typename Access::Type clearBitMask) const {
		typename Access::Type regValue = read();
		regValue &= ~clearBitMask;
		write(regValue);
	}
	
private:

	inline void readSync() const {
		/* Cache sync */
		if constexpr( Policy::Read::Sync::cache ) {
			static_assert(!Policy::Read::Sync::cache, "Please implement functionality");
		}
		/* CPU Sync */
		if constexpr( Policy::Read::Sync::cpu ) {
			asm("dmb");
		}
	}

	inline void writeSync() const {
		/* CPU sync */
		if constexpr( Policy::Write::Sync::cpu ) {
			asm("dsb");
		}
		/* Cache or/and WriteBuffer sync */
		if constexpr( Policy::Write::Sync::cache ) {
			static_assert(!Policy::Write::Sync::cache, "Please implement functionality");
		}
	}
	
private:
	const typename Access::BusType _address {};
	
};

template<typename Description = DefaultRegister::Description, typename Access = DefaultRegister::Access, typename Policy = DefaultRegister::Policy>
struct AsyncRegister {
			
	constexpr AsyncRegister( const typename Access::BusType address ) : _register( address ) {}

	inline void checkout() {
		_value = _register.read();
	}
	
	inline void commit() const {
		_register.write(_value);	
	}

	template<typename FieldId>
	inline void set(const typename FieldId::Type value) {
		constexpr const size_t lsb = FieldId::LSB;
		constexpr const size_t msb = FieldId::MSB;
		static_assert( ( msb >= lsb ), "Lowest bit of fieled should be less or equal then Most signifant bit");
		constexpr const size_t bitCount = msb - lsb + 1;
		constexpr const typename Access::Type zMask = ( (static_cast<const typename Access::Type>(1) << ( bitCount - 1 )) - 1) | 
								(static_cast<const typename Access::Type>(1) << ( bitCount - 1 ) );
		constexpr const typename Access::Type mask = (zMask << lsb);
		_value &= ~(mask);
		if constexpr (Policy::Write::maskValues) {
			_value |= ( (static_cast<const typename Access::Type>(value)) << lsb ) & mask;
		} else {
			_value |= ( (static_cast<const typename Access::Type>(value)) << lsb );
		}
	};

	template<typename FieldId>
	inline typename	FieldId::Type get() const {
		constexpr const size_t lsb = FieldId::LSB;
		constexpr const size_t msb = FieldId::MSB;
		static_assert( ( msb >= lsb ), "Lowest bit of fieled should be less or equal then Most signifant bit");
		constexpr const size_t bitCount = msb - lsb + 1;
		constexpr const typename Access::Type zMask = ( (static_cast<const typename Access::Type>(1) << ( bitCount - 1 )) - 1) | 
								(static_cast<const typename Access::Type>(1) << ( bitCount - 1 ) );
		constexpr const typename Access::Type mask =  (zMask << lsb);
		const auto result = static_cast<const typename FieldId::Type>(( _value >> lsb ) & zMask);
		return result;
	}

private:
	Register<Access, Policy> _register{};		
	typename Access::Type _value {};
	
};

template<typename Description = DefaultRegister::Description, typename Access = DefaultRegister::Access, typename Policy = DefaultRegister::Policy>
struct AsyncAutoRegister {

	constexpr AsyncAutoRegister( const typename Access::BusType address ) : _asyncRegister(address) {
		_asyncRegister.checkout();
	}
	~AsyncAutoRegister() {
		_asyncRegister.commit();
	}
	
	template<typename FieldId>
	inline typename	FieldId::Type get() {
		return _asyncRegister.template get<FieldId>();
	}
	
	template<typename FieldId>
	inline void set(const typename FieldId::Type value) {
		_asyncRegister.template set<FieldId>(value);
	}
	
private:	
	/* Incapsulation, because of some constexpr */
	AsyncRegister<Description, Access, Policy> _asyncRegister {};
};

template<typename Description = DefaultRegister::Description, typename Access = DefaultRegister::Access, typename Policy = DefaultRegister::Policy>
struct SyncRegister {
	
	SyncRegister( const typename Access::BusType address ) : _address(address) {}

	inline typename Access::Type read() {
		return {};
	}
		
	inline void write(typename Access::Type value) {
	}
	
	template <typename FieldId>
	inline typename	FieldId::Type get() {
		AsyncRegister<Description, Access, Policy> reg(_address);
		reg.checkout();
		return ( reg.template get<FieldId>() );
	}

	template <typename FieldId>
	inline void set(const typename FieldId::Type value) {
		AsyncAutoRegister<Description, Access, Policy> reg(_address);
		reg.template set<FieldId>(value);
	}
	
private:
	/* Incapsulation, because of some constexpr */	
	const typename Access::BusType _address {};	
};


void pllInit() {
	AsyncRegister<> reg0(0x10000000);
	SyncRegister<>  reg1(0x10000004);	
	reg1.set<DefaultRegister::Description::Bit<4>>(false);
	reg1.set<DefaultRegister::Description::Field<3,0>>(3);
	reg1.set<DefaultRegister::Description::Value>(0x12345678);
}
