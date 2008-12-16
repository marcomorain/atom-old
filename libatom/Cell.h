#pragma once
#include <Tools/Map.h>
#include <Tools/Assert.h>
#include <Tools/String.h>
#include <ostream>

class Cell : public NoCopy, public Counted<Cell>
{
  private:

	typedef struct
	{
		Cell* m_car;
		Cell* m_cdr;

	} Cons;

	typedef union
	{
		Cons	u_cons;
		hash	u_string;
		double	u_float;
		Integer u_int;
		hash	u_ident;
	} CellUnion;

  public:

	typedef enum
	{
		INVALID		= 0,
		ATOM		= 1<<0,
		STRING		= 1<<1,
		NUMBER		= 1<<2,
		FUNCTION	= 1<<3,
		LIST		= 1<<4,
		CONS		= 1<<5,
		TRUE		= 1<<6,
		IDENT		= 1<<7
	} Type;

	CellUnion	m_union;
	String		m_atom_name;
	char		m_tag;

	Cell*			m_next;
	static Cell*	s_head;

//private:
	const Type	m_type;
	
	public:

	Cell ( Type type, hash h )
		: m_type(type)
		, m_tag(0)
	{
		init_links();
		m_union.u_int = 0;
		
		switch (m_type)
		{
			case STRING:
				m_union.u_string = h;
				break;

			case IDENT:
				m_union.u_ident = h;
				break;
			
			default:
				jassert(0);
				break;
		}
	}

	Cell ( Type type )
		: m_type(type)
		, m_tag(0)
	{
		init_links();
		m_union.u_int = 0;
	}

	bool is_a ( Type type ) const
	{
		return (m_type & type);
	}

//	private:

	~Cell ()
	{
		jassert(m_next == (Cell*)0xdeadbeef);
	}

	public:

	static void destroy_not_marked(char tag);

	inline const hash& ident ( void )
	{
		jassert(is_a(IDENT));
		return m_union.u_ident;
	}

	inline Integer& number ( void )
	{
		jassert(is_a(NUMBER));
		return m_union.u_int;
	}

	const char* atom_name ( void ) const
	{
		return m_atom_name.c_str();
	}

	void set_atom_name ( const char* start, const char* end );
	void set_atom_name ( const char* name );

	inline Cell* car () const
	{
		jassert(is_a(LIST));
		return m_union.u_cons.m_car;
	}

	inline Cell* cdr () const
	{
		jassert(is_a(LIST));
		return m_union.u_cons.m_cdr;
	}

	inline Cell*& car ()
	{
		jassert(is_a(LIST));
		return m_union.u_cons.m_car;
	}

	inline Cell*& cdr ()
	{
		jassert(is_a(LIST));
		return m_union.u_cons.m_cdr;
	}

	inline void init_links ( void )
	{
		m_next = s_head;
		s_head = this;
	}

};


inline Cell* car ( const Cell* cell )
{
	return cell->car();
}

inline Cell* cdr ( const Cell* cell )
{
	return cell->cdr();
}

inline Cell*& car ( Cell* cell )
{
	return cell->car();
}

inline Cell*& cdr ( Cell* cell )
{
	return cell->cdr();
}

/// Returns the length of the given list
inline int length ( const Cell* list )
{
	jassert(list->is_a(Cell::LIST));

	int result = 0;

	while (list)
	{
		result++;
		list = cdr(list);
	}

	return result;
}

inline bool nil ( Cell* cell )
{
	if (!cell) return true;
	if (cell->is_a(Cell::LIST))
	{
		return !(car(cell) || cdr(cell));
	}
	return false;
}

inline bool listp ( Cell* cell )
{
	jassert(cell);
	return cell->is_a(Cell::LIST) || nil(cell);
}
