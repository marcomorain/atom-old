#pragma once
#undef null
#define null (0)

typedef __int64 Integer;
typedef unsigned hash;

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


template <typename Type>
class Counted
{
	public:
	static int	s_count;
	static int	s_next;
	int			m_object_number;

	Counted ( void )
	{
		m_object_number = s_next;
		s_count++;
		s_next++;
	}

	~Counted ( void )
	{
		s_count--;
	}
};
