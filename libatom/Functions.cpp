#include <Atom.h>

Cell* function_setq ( Runtime& runtime, Cell* params )
{
	runtime.m_symbols[ hash_string(params->atom_name()) ] = cdr(params);
	return cdr(params);
}

Cell* function_car	( Runtime& runtime, Cell* params )
{
	return car(params);
}

Cell* function_cdr	( Runtime& runtime, Cell* params )
{
	return cdr(params);
}

Cell* function_stringp	( Runtime& runtime, Cell* params )
{
	if ( params->is_a(Cell::STRING))
	{
		return runtime.m_T;
	}
	else
	{
		return runtime.m_nil;
	}
}
