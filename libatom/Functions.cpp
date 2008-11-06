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

	Cell* f = runtime.evaluate(function);
	rest = runtime.evaluate(rest);
	return runtime.call_function(f, rest);
}

Cell* function_atom	( Runtime& runtime, Cell* params )
{
	jassert(params && car(params));

	Cell* var = runtime.evaluate(car(params));

	if (nil(var))
	{
		return runtime.m_T;
	}

	if (var->is_a(Cell::LIST))
	{
		return runtime.m_nil;
	}

	return runtime.m_T;
}

Cell* function_quote ( Runtime& runtime, Cell* params )
{
	jassert(params);
	return car(params);
}

Cell* function_cond	( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* current	= params;

	while (current)
	{
		Cell* test = car(car(current));

		Cell* result = runtime.evaluate(test);

		if ( ! nil( result ) )
		{
			Cell* body = cdr(car(current));

			if (body)
			{
				result = function_progn(runtime, body);	
			}

			jassert(result);
			return result;
		}

		current = cdr(current);
	}

	return runtime.m_nil; // return nil;
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

Cell* function_defmacro			( Runtime& runtime, Cell* params )
{
	jassert(0);
	return runtime.m_nil;
}

Cell* function_eval				( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* expression = runtime.evaluate( car(params) );
	return runtime.evaluate(expression);
}

Cell* function_defun			( Runtime& runtime, Cell* params )
{
	jassert(params);
	Cell* name = car(params);
	Cell* args = car(cdr(params));
	Cell* body = cdr(cdr(params));

	jassert(name->is_a(Cell::IDENT));

	Cell* expression = new Cell(Cell::LIST);

	car(expression) = new Cell(Cell::IDENT, runtime.m_hash_lambda);
	car(expression)->set_atom_name("LAMDBA");

	cdr(expression) = new Cell(Cell::LIST);
	car(cdr(expression)) = args;
	cdr(cdr(expression)) = body;

	runtime.m_functions[name->ident()] = expression;

	return name;
}

Cell* function_list ( Runtime& runtime, Cell* params )
{
	Cell* current	= params;
	Cell* head		= new Cell(Cell::LIST);
	Cell* last		= head;

	while(current)
	{
		car(last) = runtime.evaluate(car(current));

		current = cdr(current);

		if (current)
		{
			Cell* next = new Cell(Cell::LIST);
			cdr(last) = next;
			last = next;
		}
	}

	return head;
}

Cell* function_and ( Runtime& runtime, Cell* params )
{
	if (!params)
	{
		return runtime.m_T;
	}

	Cell* current	= params;
	Cell* result	= 0;

	while(current)
	{
		result = runtime.evaluate(car(current));

		// return nil;
		if (nil(result)) return runtime.m_nil;

		current = cdr(current);
	}

	return result;
}

Cell* function_or ( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* current	= params;
	
	while (current)
	{
		Cell* result = runtime.evaluate( car(current) );

		if (!nil(result)) return result;

		current = cdr(current);
	}

	return runtime.m_nil;
}

Cell* function_length		( Runtime& runtime, Cell* params )
{
	jassert(params);
	jassert(car(params));
	Cell* list = runtime.evaluate(car(params));
	jassert(list);
	jassert(list->is_a(Cell::LIST));
	Cell* result = new Cell(Cell::NUMBER);
	result->number() = length(list);
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

Cell* function_null	( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* param = car(params);
	Cell* value = runtime.evaluate(param);

	if (nil(value))
	{
		return runtime.m_T;
	}
	
	// return nil
	return runtime.m_nil;
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
	runtime.m_symbols[symbol->ident()] = value;
	return value;
}

Cell* function_car	( Runtime& runtime, Cell* params )
{
	jassert(params && car(params));
	Cell* first = car(params);
	Cell* cell = runtime.evaluate( first );
	jassert(cell);
	Cell* the_car = car(cell);
	return the_car ? the_car : runtime.m_nil;
}

Cell* function_cdr	( Runtime& runtime, Cell* params )
{
	jassert(params && car(params));
	Cell* first = car(params);
	Cell* cell = runtime.evaluate( first );
	jassert(cell);
	Cell* the_cdr = cdr(cell);
	return the_cdr ? the_cdr : runtime.m_nil;
}

Cell* function_eq ( Runtime& runtime, Cell* params )
{
	jassert(params);
	jassert(cdr(params));

	Cell* firstp	= car(params);
	Cell* secondp	= car(cdr(params));

	Cell* first		= runtime.evaluate(firstp);
	Cell* second	= runtime.evaluate(secondp);

	jassert(first);
	jassert(second);

	if ( first->m_type != second->m_type )
	{
		return runtime.m_nil;
	}

	if (first->is_a(Cell::LIST))
	{
		if (first == second)
		{
			return runtime.m_T;
		}

		return runtime.m_nil;
	}
	
	if (first->is_a(Cell::IDENT))
	{
		if ( first->ident() == second->ident())
		{
			return runtime.m_T;
		}
		return runtime.m_nil;
	}

	if (first->is_a(Cell::NUMBER))
	{
		if ( first->m_union.u_int == second->m_union.u_int )
		{
			return runtime.m_T;
		}
		return runtime.m_nil;
	}

	if ( second->is_a(Cell::STRING) )
	{
		if ( first->m_union.u_string == first->m_union.u_string )
		{
			return runtime.m_T;
		}
		return runtime.m_nil;
	}

	jassert(0);
	return runtime.m_nil;
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

Cell* function_princ			( Runtime& runtime, Cell* params )
{
	Cell* current = params;

	while (current)
	{
		Cell* result = runtime.evaluate(car(current));
		runtime.output( result, false );
		current = cdr(current);
	};

	return runtime.m_T;
}

Cell* function_prog1 ( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* first = car(params);
	Cell* result = runtime.evaluate(first);
	Cell* current = cdr(params);

	while (current)
	{
		runtime.evaluate(car(current));
		current = cdr(current);
	}

	return result;
}

Cell* function_progn ( Runtime& runtime, Cell* params )
{
	jassert(params);

	Cell* result = null;

	Cell* statement = params;

	while ( statement )
	{
		result = runtime.evaluate( car(statement) );
		statement = cdr(statement);
	}

	jassert(result);
	return result;
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

	const char* filename_string = runtime.get_string(filename->m_union.u_string);

	FILE* file = fopen( filename_string, "r");

	if (!file)
	{
		//cerr << "Error opening file \"" << filename_string << "\"" << endl;
		return runtime.m_nil;
	}

	fseek (file, 0, SEEK_END);
	size_t  size = ftell (file);
	rewind(file);

	char* buffer = new char [size];
	size_t count = fread(buffer, size, 1, file);

	bool success = false;

	// todo: this needs to be checked for error correctly
	if (count >= 0)
	{
		success = runtime.parse_and_evaluate(buffer);
	}
	else
	{
		cerr << "No data read from \"" << filename_string << "\"" << endl;
	}

	delete [] buffer;
	return success ? runtime.m_T : runtime.m_nil;
}

