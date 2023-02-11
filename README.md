# MemoryMappedRegAccess
C++ optimezed safe access to memory mapped register (prototyping)

So, I tried to prototype different register access...
It is possiblet to make it safe.
Assert on wrong values, possible static asserts, and other
compile time or runtime error detection.


////////////////////////////
/* Sample, how to describe registers */
/*
	So, this register, can be described as
	__attributr_((packed)) ....
	typedef ... union {
		struct {
			uint32_t PllEnable:1;
			uint32_t PheriphClockEnable:1;
		}
		struct {
			uint32_t AllClockEnable:2;
		}
	} *pSomething, Something;
	What about reserved fields ????? 
	It will be Reserved1, Reserved2, etc...
	What about Big/Little endian, so if I want to fixup edianess?
		
	But I want to add DMB, DSB, etc...
	I want to check type checking and type converison
	I wanna to add value something like bit description, 
	But where will I describe it????
	
	So, "constexpr class" variable, will not take memory inside
	.text (non-volatile code section),
	.ram (volatile sram/sdram section)
	It is only like optimized inline functions.
	It allow you to make single Read-Modify-Write operation, by the each field
	But it can be unconstexpred, if you will use another constructor.
	
	Another class (Register::CachedClass) will help you access to
	the register memory in the single modification more then one filed.
	Constructor: register _tmp_reg <= REG
	
	Modify: _tmp_reg by "&=", "|=", but after modificaton this instructions,
	may be comressed into single "|=", or single "&=" or two instructions "&=" + "|=". 
	It will optimize access on "Assembly level" access.
	
	Destructor: write register REG <= _tmp_reg
	
	You can use single desccription of both classes.
	You can call me: +79003030374, we will discuss description.
	Evgeny Sobolev 02/09/1984 y.b., VRN, Russia,
	May be it is possible to make it better. I tried to find
	another way, but didn't found.
*/

struct SystemControlRegDescr : public Register::Description<> {	
	struct PllEnable 		: public Bit<0> {};
	struct PheriphClockEnable 	: public Bit<1> {};	
	enum class TAllClockEnable {
		Disable = 0,
		Enable = 0x03
	};
	struct AllClock : public Field<1,0,TAllClockEnable> {};
};

