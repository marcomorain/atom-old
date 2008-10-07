#pragma once
#include <algorithm>

template <typename Type>
class Array
{

public:
	typedef Type		value_type;	// The type of object, T, stored in the vector.
	typedef Type*		pointer;
	typedef Type&		reference;
	typedef const Type& const_reference;
	typedef int			size_type;
	typedef int			difference_type;
	typedef Type*		iterator;
	typedef	const Type* const_iterator;			// Const iterator used to iterate through a vector.
	typedef	Type*		reverse_iterator;		// 	Reversible  	Iterator used to iterate backwards through a vector.
	typedef	const Type* const_reverse_iterator; // 	Reversible  	Const iterator used to iterate backwards through a vector.

private:

	Type*		m_data;
	size_type	m_size;
	size_type	m_capacity;


	// Grow the capacity of the array to the next highest power of two
	void grow ( void )
	{
		reserve(next_highest_power(m_capacity+1));
	}

public:

	bool ok(void) const
	{
		const bool not_negative = (m_capacity > -1) && (m_size > -1);
		const bool size_ok = m_capacity >= m_size;
		const bool pointer_ok = ( m_capacity > 0 ) ? !!m_data : (m_data == 0);
		return not_negative && size_ok && pointer_ok;
	}

	bool valid_index (const size_type index) const
	{
		return (index > -1) && (index < m_size);
	}


	static bool is_power_of_two (const size_type v)
	{
		return !(v & (v - 1)) && v;
	}


	// compute the next highest power of 2 of 32-bit v
	size_type next_highest_power(size_type v)
	{
		jassert(v>-1);
		static_assert(sizeof(size_type) == 4);
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		v += (v==0);
		jassert(is_power_of_two(v));
		return v;
	}

	iterator begin(void)			// Returns an iterator pointing to the beginning of the vector.
	{
		return m_data;
	}

	iterator end()					// Returns an iterator pointing to the end of the vector.
	{
		return (m_data + m_size);
	}

	const_iterator begin() const	// Returns a const_iterator pointing to the beginning of the vector.
	{
		return m_data;
	}

	const_iterator end() const		// Returns a const_iterator pointing to the end of the vector.
	{
		return (m_data + m_size);
	}


	// Reversible  	Returns a reverse_iterator pointing to the beginning of the reversed vector.
	reverse_iterator rbegin() 
	{
		jassert(m_size > 0);
		return m_data[m_size - 1];
	}

	// Reversible  	Returns a reverse_iterator pointing to the end of the reversed vector.
	reverse_iterator rend()
	{
		jassert(m_size > 0);
		return m_data[0];
	}

	// 	Reversible  	Returns a const_reverse_iterator pointing to the beginning of the reversed vector.
	const_reverse_iterator rbegin() const
	{
		jassert(m_size > 0);
		return m_data[m_size - 1];
	}

	// Reversible  	Returns a const_reverse_iterator pointing to the end of the reversed vector.
	const_reverse_iterator rend() const
	{
		jassert(m_size>0);
		return m_data[0];
	}

	// Returns the size of the vector.
	size_type size() const
	{
		return m_size;
	}

	// Returns the largest possible size of the vector.
	size_type max_size() const
	{
		jassert(0);
		return 0;
	}

	// true if the vector's size is 0.
	bool empty() const
	{
		return m_size == 0;
	}

	// Random Access  	Returns the n'th element.
	reference operator[](size_type n)
	{
		jassert(valid_index(n));
		return m_data[n];
	}

	// Random Access  	Returns the n'th element.
	const_reference operator[](size_type n) const
	{
		jassert(valid_index(n));
		return m_data[n];
	}

	// Creates an empty vector.
	Array()
		: m_data(0)
		, m_size(0)
		, m_capacity(0)
	{
	}

	// Sequence 	Creates a Array with n elements.
	Array(size_type n)
		: m_size(n)
		, m_capacity(n)
	{
		m_data = new Type[n];
	}

	// Sequence 	Creates a Array with n copies of t.
	Array(size_type n, const Type& fill)
		: m_size(n)
		, m_capacity(n)
	{
		m_data = new Type[n];

		for (size_type i=0; i<n; n++)
		{
			m_data[i] = fill;
		}

	}

	// private, copy function
	inline void copy_from(const Array& other)
	{
		jassert(m_capacity	== 0);
		jassert(m_size		== 0);
		jassert(m_data		== 0);

		m_capacity = m_size = other.m_size;
		m_data = new Type[m_size];

		for (size_type i=0; i<m_size; i++)
		{
			m_data[i] = other.m_data[i];
		}

		jassert(ok());
	}

	// Erases the nth object from the array
	// The last item is swapped, so the order
	// is not maintained.
	void erase (const size_type n)
	{
		jassert(valid_index(n));
		
		if (m_size > 1)
		{
			m_data[n] = m_data[m_size-1];
		}

		m_size--;
	}

	// The copy constructor
	Array(const Array& other)
	{
		m_size		= 0;
		m_capacity	= 0;
		copy_from(other);
	}

	~Array(void)
	{
		clear();
	}

	void clear ( void )
	{
		jassert(ok());
		delete [] m_data;
		m_data		= 0;
		m_size		= 0;
		m_capacity	= 0;
	}

	// Container 	The assignment operator
	Array& operator=(const Array&);


	// Sequence 	Returns the first element.
	reference front(void)
	{
		jassert(!empty());
		return m_data[0];
	}

	// Returns the first element.
	const_reference front(void) const
	{
		jassert(!empty());
		return m_data[0];
	}

	// Returns the last element.
	reference back()
	{
		jassert(!empty());
		return m_data[m_size-1];
	}

	// Returns the last element.
	const_reference back() const
	{
		jassert(!empty());
		return m_data[m_size-1];
	}

	// Inserts a new element at the end.
	void push_back(const Type& datum)
	{
		const size_type new_size = m_size + 1;

		if ( new_size > m_capacity )
		{
			grow();
		}

		m_data[m_size] = datum;
		m_size++;

		jassert(ok());
	}

	// Removes the last element.
	void pop_back(void)
	{
		jassert(!empty());
		back().~Type();
		m_size--;
	}


	// Swaps the contents of two vectors.
	// Runs in O(1) time
	void swap(Array& other)
	{
		std::swap(m_size,		other.m_size);
		std::swap(m_capacity,	other.m_capacity);
		std::swap(m_data,		other.m_data);
	}

	// Number of elements for which memory has been allocated. capacity() is always greater than or equal to size().
	size_type capacity() const 
	{
		jassert(ok());
		return m_capacity;
	}

	// If n is less than or equal to capacity(), this call has no effect.
	// Otherwise, it is a request for allocation of additional memory. If the request
	// is successful, then capacity() is greater than or equal to n; otherwise,
	// capacity() is unchanged. In either case, size() is unchanged.
	void reserve(size_type n)
	{
		jassert(n>0);

		if (n > m_capacity)
		{
			// Allocate
			Type* new_data = new Type[n];

			// Copy across
			for (size_type i=0; i<m_size; i++)
			{
				new_data[i] = m_data[i];
			}

			// Delete
			delete [] m_data;

			// Fixup
			m_data = new_data;
			m_capacity = n;
		}
	}

	void shrink ( void )
	{
		// If nothing is allocated, return stright away.
		// We now know that something is allcocated.
		if (m_capacity == 0) return;

		jassert(m_capacity);
		jassert(m_data);

		if (m_size == 0)
		{
			delete [] m_data;
			m_data		= 0;
			m_capacity	= 0;
		}
		else
		{
			const size_type smallest_size = next_highest_power(m_size);

			if (smallest_size < m_capacity)
			{
				Type* new_data = new Type[smallest_size];

				for (int i=0; i<m_size; i++)
				{
					new_data[i] = m_data[i];
				}

				delete [] m_data;
				m_data		= new_data;
				m_capacity	= smallest_size;
			}
		}
	}
};
