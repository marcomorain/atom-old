#pragma once
#include <JAssert.h>
#include <ostream>

class Cell : public NoCopy
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
		char*	u_string;
		double	u_float;
		__int64	u_int;

	} CellUnion;

  public:

	typedef enum
	{
		ATOM		= 1<<0,
		STRING		= 1<<1,
		NUMBER		= 1<<2,
		FUNCTION	= 1<<3,
		LIST		= 1<<4,
		CONS		= 1<<5,
		TRUE		= 1<<6
	} Type;

	CellUnion	m_union;
	char*		m_atom_name;

//private:
	const Type	m_type;
	static int	s_count;
	int			m_id;
	
	public:

	Cell ( Type type )
		: m_type(type)
		, m_atom_name(null)
	{
		m_union.u_int = 0;
		m_id = s_count++;
	}

	bool is_a ( Type type ) const
	{
		return (m_type & type);
	}

	~Cell ( void )
	{
		delete [] m_atom_name;
	}

	const char* atom_name ( void ) const
	{
		return m_atom_name;
	}

	void set_atom_name ( const char* atom_name );

	inline Cell* car () const
	{
		return m_union.u_cons.m_car;
	}

	inline Cell* cdr () const
	{
		return m_union.u_cons.m_cdr;
	}

	inline Cell*& car ()
	{
		return m_union.u_cons.m_car;
	}

	inline Cell*& cdr ()
	{
		return m_union.u_cons.m_cdr;
	}
};

std::ostream& operator << ( std::ostream& os, Cell& cell );



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