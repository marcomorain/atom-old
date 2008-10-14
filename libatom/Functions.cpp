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

	Cell* param1 = runtime.evaluate( car(params) );
	Cell* param2 = runtime.evaluate( car(cdr(params)) );

	car(cons) = param1;
	cdr(cons) = param2;
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
	Cell* result = runtime.evaluate( car (params) );

	bool condition = nil(result);

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

