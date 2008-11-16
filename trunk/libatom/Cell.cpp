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

void Cell::destroy_marked(char tag)
{
	Cell* last		= null;
	Cell* current	= s_head;

	while (current)
	{
		Cell* next = current->m_next;

		jassert(next != 0xFeeeFeee);

		if (current->m_tag == tag)
		{			
			if (last)
			{
				jassert(last->m_tag != tag);
				last->m_next = current->m_next;
			}

			current->m_next = null;
			delete current;
			current = 0;
		}
		else
		{
			jassert(current != 0xFeeeFeee);
			jassert(last != 0xFeeeFeee);
			last = current;
		}

		current = next;
	}
}
