#include <Atom.h>
#include <Functions.h>

#include <iostream>
#include <cstdlib>
using namespace std;

Cell* function_apply		( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* function = car(params);
	Cell* rest = cdr(params);
	rest = runtime.evaluate(rest);
	return runtime.call_function(function, rest);
}

Cell* function_quote ( Runtime& runtime, Cell* params )
{
	return car(params);
}

Cell* function_cons	( Runtime& runtime, Cell* params )
{
	Cell* cons = new Cell(Cell::LIST);

	// requires 2 params
	jassert(params);
	jassert(cdr(params));

	Cell* first		= car(params);
	Cell* second	= car(cdr(params));

	car(cons) = runtime.evaluate( first );
	cdr(cons) = runtime.evaluate( second );

	return cons;
}

Cell* function_length		( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* list = runtime.evaluate(params);
	jassert(list);
	jassert(list->is_a(Cell::LIST));

	int length = 0;

	while (list)
	{
		length++;
		list=cdr(list);
	}

	Cell* result = new Cell(Cell::NUMBER);
	result->number() = length;
	return result;
}

Cell* function_imagpart	( Runtime& runtime, Cell* params )
{
	BREAKPOINT();
}

Cell* function_realpart		( Runtime& runtime, Cell* params )
{
	BREAKPOINT();
}

Cell* function_error ( Runtime& runtime, Cell* params )
{
	BREAKPOINT();
}

Cell* function_nth_value	( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* count   = car(params);
	Cell* current = runtime.evaluate(car(cdr(params)));

	jassert(count->is_a(Cell::NUMBER));

	Integer i = count->number();

	jassert(i > -1);

	while (i--)
	{
		current = cdr(current);
	}

	return car(current);
}

Cell* function_block ( Runtime& runtime, Cell* params )
{
	jassert(params);
	jassert(car(params));
	jassert(cdr(params));

	Cell* block_name = runtime.evaluate( car(params) );

	// todo: start a new block here, for return from
	Cell* block = runtime.evaluate( cdr( params ) );

	return block;
}

Cell* function_backquote	( Runtime& runtime, Cell* params )
{
	jassert(params);
	return runtime.replace_commas(car(params));
}

Cell* function_return_from	( Runtime& runtime, Cell* params )
{
	jassert(0);
	return null;
}

Cell* function_setf ( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* symbol = car(params);
	jassert(symbol && symbol->is_a(Cell::IDENT));
	jassert(cdr(params));
	Cell* value = runtime.evaluate(car(cdr(params)));
	symbol->m_union.u_ident.m_value = value;
	return value;
}

Cell* function_car	( Runtime& runtime, Cell* params )
{
	return car(params);
}

Cell* function_cdr	( Runtime& runtime, Cell* params )
{
	return cdr(params);
}

Cell* function_eq ( Runtime& runtime, Cell* params )
{
	jassert(params);
	jassert(cdr(params));

	Cell* first		= car(params);
	Cell* second	= car(cdr(params));

	jassert(first);
	jassert(second);

	if ( first->m_type != second->m_type )
	{
		return new Cell(Cell::LIST);
	}

	if (first->is_a(Cell::LIST))
	{
		if (first == second)
		{
			return runtime.m_T;
		}

		return new Cell(Cell::LIST);
	}
	
	if (first->is_a(Cell::IDENT))
	{
		if ( first->m_union.u_ident.m_value == second->m_union.u_ident.m_value )
		{
			return runtime.m_T;
		}
		return new Cell(Cell::LIST);
	}

	if (first->is_a(Cell::NUMBER))
	{
		if ( first->m_union.u_int == second->m_union.u_int )
		{
			return runtime.m_T;
		}
		return new Cell(Cell::LIST);
	}

	const bool second_number = second->is_a(Cell::NUMBER);

	if ( second->is_a(Cell::STRING) )
	{
		if ( first->m_union.u_string == first->m_union.u_string )
		{
			return runtime.m_T;
		}
		return new Cell(Cell::LIST);
	}

	jassert(0);
	return new Cell(Cell::LIST);
}

Cell* function_stringp	( Runtime& runtime, Cell* params )
{
	if ( params->is_a(Cell::STRING))
	{
		return runtime.m_T;
	}
	else
	{
		return new Cell(Cell::LIST);
	}
}

Cell* function_if ( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* result = runtime.evaluate( car (params) );

	bool condition = !nil(result);

	if (condition && result->is_a(Cell::NUMBER))
	{
		condition = ( result->number() != 0 );
	}

	if (condition)
	{
		return runtime.evaluate( car (cdr(params)) );
	}
	else
	{
		return runtime.evaluate( car(cdr(cdr(params))) );
	}
}

Cell* function_plus	( Runtime& runtime, Cell* params )
{
	Cell* result = new Cell(Cell::NUMBER);
	Cell* current = params;
	while (current)
	{
		Cell* number = runtime.evaluate(car(current));
		result->number() += number->number();
		current = cdr(current);
	}
	return result;
}

Cell* function_minus ( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* nextp  = cdr(params);

	if (!nextp)
	{
		Cell* result = new Cell(Cell::NUMBER);
		result->number() = -car(params)->number();
		return result;
	}

	jassert(car(params));
	Cell* accum = runtime.evaluate(car(params));
	
	while (nextp)
	{
		Cell* number = runtime.evaluate(car(nextp));
		accum->number() -= number->number();
		nextp = cdr(nextp);
	}
	return accum;
}

// Return T if its arguments are in strictly increasing order, NIL otherwise. 
Cell* function_greater_than ( Runtime& runtime, Cell* params )
{		
	Cell* first  = runtime.evaluate(car(params));
	jassert(first);

	if (!cdr(params))
	{
		return runtime.m_T;
	}

	Cell* second =  runtime.evaluate(car(cdr(params)));
	jassert(second);

	if ( first->number() <= second->number() )
	{
		return null;
	}

	return function_greater_than ( runtime, cdr(params));
}

//  Return T if its arguments are in strictly decreasing order, NIL otherwise. 
Cell* function_less_than ( Runtime& runtime, Cell* params )
{	
	jassert(params);
	Cell* first  = runtime.evaluate(car(params));
	jassert(first);

	if (!cdr(params))
	{
		return runtime.m_T;
	}

	Cell* second =  runtime.evaluate(car(cdr(params)));
	jassert(second);

	if ( first->number() >= second->number() )
	{
		return null;
	}

	return function_less_than ( runtime, cdr(params));
}

Cell* function_load ( Runtime& runtime,	Cell* params )
{
	jassert(params);
	Cell* filename = car(params);
	jassert(filename);

	FILE* file = fopen( runtime.get_string(filename->m_union.u_string), "r");

	if (!file)
	{
		cerr << "Error opening file \"" << filename->m_union.u_string << "\"" << endl;
		return null;
	}

	fseek (file, 0, SEEK_END);
	size_t  size = ftell (file);
	rewind(file);

	char* buffer = new char [size];
	size_t read_error = fread(buffer, size, 1, file);

	bool result = false;

	if (!read_error)
	{
		result = runtime.parse_and_evaluate(buffer);
	}
	delete [] buffer;
	return result ? runtime.m_T : new Cell(Cell::LIST);
}

