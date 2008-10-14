#include <Cell.h>
#include <cstring>
using std::ostream;
int Counted<Cell>::s_count = 0;
int Counted<Cell>::s_next  = 0;

void Cell::set_atom_name ( const char* start, const char* end )
{
	m_atom_name = String(start, end);
}

void Cell::set_atom_name ( const char* name )
{
	const char* end = name + strlen(name);
	jassert(end > name);
	set_atom_name(name, end);
}

