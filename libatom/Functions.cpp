#include <Atom.h>
#include <iostream>
using namespace std;
#include <cstdlib>

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
		return runtime.m_nil;
	}
}

Cell* function_if ( Runtime& runtime, Cell* params )
{
	Cell* result = runtime.evaluate( car (params) );

	bool condition = result && (result != runtime.m_nil);

	if (condition && result->is_a(Cell::NUMBER))
	{
		condition = ( result->m_union.u_int != 0 );
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
		result->m_union.u_int += number->m_union.u_int;
		current = cdr(current);
	}
	return result;
}

Cell* function_load ( Runtime& runtime,	Cell* params )
{
	jassert(params);
	Cell* filename = car(params);
	jassert(filename);

	FILE* file = fopen(filename->m_union.u_string, "r");

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
	return result ? runtime.m_T : runtime.m_nil;
}

Cell* function_defun	( Runtime& runtime, Cell* params )
{
	// m_functions
	Cell* name = car(params);
	Cell* body = cdr(params);
	jassert(name);
	jassert(body);
	runtime.m_functions[name->m_union.u_ident.m_name] = body;
	return name;
}
