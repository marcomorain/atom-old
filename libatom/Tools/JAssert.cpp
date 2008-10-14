#if defined  NDEBUG
// No asserts...
#else
#include <JAssert.h>
#include <iostream>

void Assert::Fail(const char* file, const unsigned line, const char* message)
{
	// todo cerr
	std::cout << file << ":" << line << " Assertion failed: " << message << std::endl;
}

#endif //!NDEBUG
