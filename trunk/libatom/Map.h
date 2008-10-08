#pragma once
#include <JAssert.h>
#include <Array.h>

inline hash hash_string (const char* str )
{
	jassert(str);
	hash h = 5381;
	while (*str)
	{
		h = h * 33 ^ *str;
		str++;
	}
	return h;
}

template <typename Key, typename Data>
class Map
{
	typedef struct
	{
		Key			m_key;
		Data		m_data;
	} Pair;

	Array<Pair> m_data;

public:

	bool has_key ( const Key& key ) const
	{
		for (int i=0; i<m_data.size(); i++)
		{
			if (m_data[i].m_key == key) return true;
		}
		return false;
	}

	Data& operator [] ( const Key& key )
	{
		for (int i=0; i<m_data.size(); i++)
		{
			if (m_data[i].m_key == key) return m_data[i].m_data;
		}
		Pair p;
		p.m_key = key;
		m_data.push_back(p);
		return m_data.back().m_data;
	}

	Data get ( const Key& key ) const
	{
		jassert(has_key(key));
		for (int i=0; i<m_data.size(); i++)
		{
			if (m_data[i].m_key == key) return m_data[i].m_data;
		}
		BREAKPOINT();
	}

	Data get (const Key& key, const Data& default ) const
	{
		for (int i=0; i<m_data.size(); i++)
		{
			if (m_data[i].m_key == key) return m_data[i].m_data;
		}
		return default;
	}
};