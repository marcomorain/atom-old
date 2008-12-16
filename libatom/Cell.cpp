#include <Cell.h>
#include <cstring>
using std::ostream;
int Counted<Cell>::s_count = 0;
int Counted<Cell>::s_next  = 0;

Cell* Cell::s_head = null;

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

void Cell::destroy_not_marked(char tag)
{
	while (s_head->m_tag != tag)
	{
		Cell* to_delete = s_head;
		s_head = s_head->m_next;
		to_delete->m_next = (Cell*)0xdeadbeef;
		delete to_delete;
	}

	Cell* current	= s_head;

	while (current)
	{
		if (current->m_next && current->m_next->m_tag != tag)
		{
			Cell* to_delete = current->m_next;
			current->m_next = current->m_next->m_next;
			to_delete->m_next = (Cell*)0xdeadbeef;
			delete to_delete;
		}

		current = current->m_next;
	}
}
