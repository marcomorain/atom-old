#include <cstdlib>
#include <iostream>
#include <string>
#include <JAssert.h>
using namespace std;
#include <Atom.h>
#include <Functions.h>

const static Runtime::State no_match = {0,0};

Runtime::Runtime ( void )
{
	m_nil = new Cell(Cell::LIST);
	m_nil->set_atom_name("nil");

	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	register_function("SETF",		function_setf);
	register_function("CAR",		function_car);
	register_function("CDR",		function_cdr);
	register_function("STRINGP",	function_stringp);
	register_function("IF",			function_if);
	register_function("+",			function_plus);
	register_function("LOAD",		function_load);
	register_function("DEFUN",		function_defun);
}

Runtime::~Runtime ( void )
{
}

Cell* Runtime::funcall ( Cell* func, Cell* params )
{
	return null;
}


Cell* Runtime::call_function (Cell* function, Cell* params )
{
	jassert(function);
	jassert(function->atom_name());

	const hash h = hash_string(function->atom_name());

	Function built_in = m_builtins.get(h, null);

	if (built_in)
	{
		return built_in(*this, params);
	}

	Cell* func = m_functions.get(h, null);

	if (func)
	{
		return funcall ( func, params );
	}

	cerr << "Function \"" << function->atom_name() << "\" could not be found." << endl;
	return null;
}

void Runtime::register_function ( const char* name, Runtime::Function func )
{
	m_builtins[ hash_string(name) ] = func;
}

Cell* Runtime::evaluate( Cell* cell )
{
	if (nil(cell))
	{
		return null;
	}
	else if (cell->is_a(Cell::LIST))
	{
		Cell* function	= car ( cell );
		return call_function( car(function), cdr (function ) );
	}
	else if (cell->is_a(Cell::IDENT))
	{
		return cell->m_union.u_ident.m_value;
	}
	else if (cell->is_a(Cell::NUMBER))
	{
		return cell;
	}
	jassert(0);
	return null;
}

bool Runtime::parse_and_evaluate ( const char* input )
{
	bool success = false;
	State state;
	state.input = input;

	while (1)
	{
		state = accept_s_expression ( state.input );
		
		if (!state.cell)
		{
			break;
		}
		else
		{
			success = true;
			Cell* result = evaluate(state.cell);
			cout << result << endl;
		}
	}

	return success;
}

Cell* Runtime::pushAtom ( char* atom )
{
	Integer val;

	if (read_integer(atom, val))
	{
		// cell
		Cell* atom_cell = new Cell(Cell::NUMBER);
		atom_cell->m_union.u_int = val;
		return atom_cell;
	}
	else
	{
		const hash h = hash_string(atom);
		Cell* atom_cell = m_symbols.get(h, null);
		if (!atom_cell)
		{
			// cell
			atom_cell = new Cell(Cell::IDENT, atom);
			m_symbols[hash_string(atom)] = atom_cell;
		}
		else
		{
			atom_cell = atom_cell;
		}
		return atom_cell;
	}
}

bool read_integer (const char* string, Integer& value)
{
	jassert(string);
	jassert(strlen(string)>0);

	const Integer base = 10;
	Integer accum = 0;

	Integer sign = 1;

	if (string[0] == '-')
	{
		sign = -1;
		string++;
		if (string[0] == 0)
		{
			return false;
		}
	}

	while (*string)
	{
		accum *= base;
		const char c = (*string);
		if (c < '0') return false;
		if (c > '9') return false;
		accum += (c - '0');
		string++;
	}

	// only update value if the input was valid;
	value = accum * sign;
	return true;
}


bool atom_char ( const char c )
{
	if (c >= 'A' && c <= 'Z') return true;
	if (c >= 'a' && c <=' z') return true;
	if (c >= '0' && c <= '9') return true;
	if (c == '*') return true;
	if (c == '+') return true;
	if (c == '-') return true;
	if (c == '?') return true;
	return false;
}

bool white_space_char ( const char c )
{
	switch (c)
	{
		case '\n':
		case '\r':
		case '\t':
		case ' ': return true;
		default:  return false;
	}
}

const char* skip_whitespace ( const char* input )
{
	while ( white_space_char ( *input ) ) input++;
	return input;
}

Runtime::State Runtime::accept_quoted_s_exp ( const char* input )
{
	// todo test this!
	//jassert(0);

	jassert(input);

	if (*input == 0) return no_match;

	input = skip_whitespace(input);

	if (*input != '`')
	{
		return no_match;
	}

	Cell* quote = new Cell(Cell::LIST);

	State state = accept_s_expression(input+1);
	jassert(valid(state));
	
	car(quote) = state.cell;
	state.cell = quote;

	return state;
}

Runtime::State Runtime::accept_string ( const char* input )
{
	jassert(input);
	input = skip_whitespace(input);

	if (*input != '\"')
	{
		return no_match;
	}

	const char* start	= input + 1;
	const char* end		= start;

	while (*end != '\"')
	{
		end++;
		if (*end == 0) return no_match;
	}

	const int length = end - start;
	char* s = (char*)malloc(length+1);
	strncpy(s, start, length);
	s[length] = 0;

	// todo: make functions take "start, end" char*
	// to avoid this alloction
	
	
	State state;
	state.input = end + 1;
	state.cell	= new Cell(Cell::STRING, s);
	state.cell->set_atom_name("A string atom");
	return state;
}

Runtime::State Runtime::accept_ident ( const char* input )
{
	jassert(input);
	if (*input == 0) return no_match;

	input = skip_whitespace(input);

	const char* start = input;
	const char* end	= input;
	while (atom_char(*end)) end++;
	const int length = end - start;

	if (length < 1)
	{
		return no_match;
	}

	char* atom = (char*)malloc(length+1);
	strncpy(atom, start, length);
	atom[length] = null; // null terminal

	// to upper case
	{
		char* current = atom;
		while (*current)
		{
			if ( (*current >= 'a') && (*current <= 'z'))
			{
				*current -= ('a' - 'A');
			}
			current++;
		}
	}

	// todo: make push atom accept a start,end char* pair
	State state;
	state.cell	= pushAtom(atom);
	state.input = end; 
	return state;
}

Runtime::State Runtime::accept_atom ( const char* input )
{
	jassert(input);
	if (*input == 0) return no_match;

	State state;

	state = accept_string(input);

	if (!valid(state))
	{
		state = accept_ident(input);
	}

	return state;
}

Runtime::State Runtime::accept_dot ( const char* input )
{
	jassert(0);
	return no_match;
}

Runtime::State Runtime::accept_list ( const char* input )
{
	jassert(input);
	if (*input == 0) return no_match;
	input = skip_whitespace ( input );

	if (*input != '(')
	{
		//cerr << __FILE__ << "(" << __LINE__ << ") " << "Parse error. Expected a '(' at " << input << endl;
		return no_match;
	}

	// Skip past the '('
	input++;

	input = skip_whitespace(input);

	if (*input == ')')
	{
		State nil;
		nil.input = input + 1;
		nil.cell = new Cell(Cell::LIST);
		nil.cell->set_atom_name("() nil list");
		return nil;
	}

	Cell* head = new Cell(Cell::LIST);

	State state = accept_series(input);
	
	if (!valid(state))
	{
		//jassert(0);
		// dotted list
		//state = accept_s_expression(input);
	}

	if (!valid(state))
	{
		cerr << __FILE__ << "(" << __LINE__ << ") " << "Parse error reading a list at " << input << endl;
		return no_match;
	}

	input = skip_whitespace(state.input);
	
	if (*input != ')')
	{
		cerr << __FILE__ << "(" << __LINE__ << ") " << "Parse error. Expecting \")\" at " << input << endl;
		return no_match;
	}

	car(head) = state.cell;
	state.cell = head;
	state.input = input+1;
	return state;
}

Runtime::State Runtime::accept_s_expression ( const char* input )
{
	jassert(input);
	if (*input == 0) return no_match;
	input = skip_whitespace ( input );

	State state = accept_quoted_s_exp(input);

	if (valid(state)) 
	{
		return state;
	}

	state = accept_atom(input);

	if (valid(state))
	{
		return state;
	}

	state = accept_list(input);

	if (valid(state))
	{
		return state;
	}

	return no_match;
}

Runtime::State Runtime::accept_series	( const char* input )
{
	jassert(input);
	if (*input == 0) return no_match;
	input = skip_whitespace(input);

	State sexpr = accept_s_expression(input);

	if (!valid(sexpr))
	{
		return no_match;
	}

	Cell* list_cell = new Cell(Cell::LIST);
	car(list_cell) = sexpr.cell;
	sexpr.cell = list_cell;

	State series = accept_series(sexpr.input);

	if (valid(series))
	{
		Cell* start = sexpr.cell;
		cdr(start) = series.cell;
		series.cell = start;
		return series;
	}
	else
	{
		return sexpr;
	}
}