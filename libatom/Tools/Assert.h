#pragma once
#include <Compiler.h>

#undef  jassert
#ifdef  NDEBUG
#	define jassert(cond)				do { (void)sizeof((cond)); } while(0)
#   define static_assert(cond)		do { (void)sizeof((cond)); } while(0)
#else
#	define jassert(cond)		do { if (!(cond)) { Assert::Fail(__FILE__, __LINE__, #cond); BREAKPOINT(); } } while (0)
#   define static_assert(test) typedef char _assertion[(test) ? 1 : 0]

class Assert
{
  public:
	static void Fail (const char* file, const unsigned line, const char* message);
};

#endif
