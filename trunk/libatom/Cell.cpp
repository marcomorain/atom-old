#include <Cell.h>
#include <cstring>
using std::ostream;
int Cell::s_count = 0;

void Cell::set_atom_name ( const char* atom_name )
{
	if (m_atom_name)
	{
		delete [] m_atom_name;
	}

	if (!atom_name)
	{
		m_atom_name = null;
	}
	else
	{
		const size_t length = strlen ( atom_name );
		m_atom_name = new char [ length + 1];
		strncpy(m_atom_name, atom_name, length);
		m_atom_name[length] = 0;
		jassert(strcmp(m_atom_name, atom_name) == 0);
	}
}

ostream& operator << ( ostream& stream, Cell* cell )
{
	if (!cell)
	{
		stream << "NIL";
		return stream;
	}

	switch (cell->m_type)
	{
		case Cell::LIST:
		{
			stream << "(";
			
			if (car(cell))
			{
				stream << car(cell);
			}
			else
			{
				stream << "NIL";
			}

			stream << " ";

			if (cdr(cell))
			{
				stream << cdr(cell);
			}

			stream << ")";
		}
		break;

		case Cell::ATOM:
		case Cell::IDENT:
			stream << cell->atom_name();
			break;

		case Cell::STRING:
			stream << '\"'<< cell->m_union.u_string << '\"';
			break;

		case Cell::NUMBER:
			stream << cell->m_union.u_int;
			break;

		default:
			stream << "Default";
			break;
	}

	return stream;
}

