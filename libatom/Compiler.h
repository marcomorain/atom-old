#pragma once
#undef null
#define null (0)

#ifdef WIN32
#	define BREAKPOINT() do { __asm int 3 } while(0)
#	pragma warning ( disable : 4800 ) // Warning when casting to a bool from a pointer.
#elif defined OSX
#   include <signal.h>
#   define BREAKPOINT() raise(SIGTRAP)
#else
# error could not define BREAKPOINT
#endif

class NoCopy
{
// Private copy CTORS with no implementation
private:
	NoCopy(const NoCopy& copy);
	NoCopy& operator=(const NoCopy& copy);
// Public non-copy CTOR
public:
	inline NoCopy () {}; 
};
