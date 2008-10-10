#pragma once
#include <Map.h>
#include <JAssert.h>
#include <ostream>

class Cell : public NoCopy, public Counted<Cell>
{
  private:

	typedef struct
	{
		Cell* m_car;
		Cell* m_cdr;

	} Cons;

	typedef struct  
	{
		Cell*	m_value;
		hash	m_name;

	} Ident;

	typedef union
	{
		Cons	u_cons;
		hash	u_string;
		double	u_float;
		Integer u_int;
		Ident	u_ident;
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
		TRUE		= 1<<6,
		IDENT		= 1<<7
	} Type;

	CellUnion	m_union;
	std::string	m_atom_name;

//private:
	const Type	m_type;
	
	public:

	Cell ( Type type, hash h )
		: m_type(type)
	{
		m_union.u_int = 0;
		
		switch (m_type)
		{
			case STRING:
				m_union.u_string = h;
				break;

			case IDENT:
				m_union.u_ident.m_name	= h;
				m_union.u_ident.m_value = null;
				break;
			
			default:
				jassert(0);
				break;
		}
	}

	Cell ( Type type )
		: m_type(type)
	{
		m_union.u_int = 0;
	}

	bool is_a ( Type type ) const
	{
		return (m_type & type);
	}

	~Cell ( void )
	{
	}

	inline hash& name ( void )
	{
		jassert(is_a(IDENT));
		return m_union.u_ident.m_name;
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

	void set_atom_name ( const std::string& name );
	
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
};

std::ostream& operator << ( std::ostream& os, Cell* cell );



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