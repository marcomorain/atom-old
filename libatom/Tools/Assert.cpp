#if defined  NDEBUG
// No asserts...
#else
#include <Tools/Assert.h>
#include <iostream>

void Assert::Fail(const char* file, const unsigned line, const char* message)
{
	std::cerr << file << ":" << line << " Assertion failed: " << message << std::endl;
}

#endif //!NDEBUG
