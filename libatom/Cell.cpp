#include <Cell.h>
#include <cstring>
using std::ostream;
int Counted<Cell>::s_count = 0;
int Counted<Cell>::s_next  = 0;

void Cell::set_atom_name ( const std::string& name )
{
	m_atom_name = name;
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
			stream << cell->number();
			break;

		case Cell::TRUE:
			stream << "T";
			break;

		default:
			stream << "Default";
			break;
	}

	return stream;
}

