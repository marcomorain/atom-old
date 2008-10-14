#pragma once
#include <JAssert.h>

class String
{
public:

	String()
	: m_data(null)
	{
	}

	String ( const char* zero_terminated )
	{
		const char* end = zero_terminated;
		while(*end) end++;
		set(zero_terminated, end);
	}

	String (const char* start, const char* end)
	{
		set(start, end);
	}

	String ( const String& other )
	{
		if (this != &other)
		{
			set_from(other);
		}
	}

	~String ( void )
	{
		delete [] m_data;
	}

	String& operator = ( const String& other )
	{
		if (this != &other)
		{
			delete m_data;
			set_from(other);
		}
		return *this;
	}
	
	const char* c_str ( void ) const
	{
		return m_data;
	}

	size_t length ( void ) const
	{
		if (!m_data) return 0;
		char* end = m_data;
		while(*end) end++;
		return end - m_data;
	}

	static void swap ( String& a, String& b )
	{
		if (&a != &b)
		{
			std::swap(a.m_data, b.m_data);
		}
	}


private:

	void set_from ( const String& other )
	{
		jassert(this != &other);
		if (other.m_data == null)
		{
			m_data = null;
		}
		else
		{
			set(other.m_data, other.m_data + other.length());
		}
	}

	void set(const char* start, const char* end)
	{
		jassert(start<end);
		const size_t length = end - start;
		jassert(length > 0);
		m_data = new char [length+1];

		char* current = m_data;
		while(start<end)
		{
			*current = *start;
			start++;
			current++;
		}
		*current = 0;
	}

	// The immutable string data
	// Zero terminated
	char* m_data;
};

namespace std
{
	template <> inline void swap(String& a, String& b)
	{
		String::swap(a,b);
	}
}