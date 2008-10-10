#include <cstdlib>
#include <iostream>
#include <cstring>
#include <JAssert.h>
using namespace std;
#include <Atom.h>
#include <Functions.h>

const static Runtime::State no_match = {0,0};

Runtime::Runtime ( void )
{
	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	register_function("SETF",		function_setf);
	register_function("CAR",		function_car);
	register_function("CDR",		function_cdr);
	register_function("STRINGP",	function_stringp);
	register_function("IF",			function_if);
	register_function("+",			function_plus);
	register_function("-",			function_minus);
	register_function("LOAD",		function_load);
	register_function("DEFUN",		function_defun);
	register_function(">",			function_greater_than);
	register_function("<",			function_less_than);
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

	if (!function->is_a(Cell::IDENT))
	{
		cerr << "Error: " << function << " is not a function." << endl;
		return null;
	}

	const hash h = function->name();

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
	m_builtins[ hash_ident(name, name+strlen(name)) ] = func;
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

Cell* Runtime::read_atom ( const char* start, const char* end )
{
	Integer val;

	if (read_integer(start, end, val))
	{
		// cell
		Cell* atom_cell = new Cell(Cell::NUMBER);
		atom_cell->number() = val;
		return atom_cell;
	}
	else
	{
		const hash h = hash_ident(start, end);
		Cell* atom_cell = m_symbols.get(h, null);
		if (!atom_cell)
		{
			// cell
			atom_cell = new Cell(Cell::IDENT, h);
			atom_cell->set_atom_name(string(start, end));
			m_symbols[h] = atom_cell;
		}
		else
		{
			atom_cell = atom_cell;
		}
		return atom_cell;
	}
}

bool read_integer (const char* start, const char* end, Integer& value)
{
	jassert(start);
	jassert(end);
	jassert(start < end);

	const Integer base = 10;
	Integer accum = 0;

	Integer sign = 1;

	if (start[0] == '-')
	{
		sign = -1;
		start++;
		if (start == end)
		{
			return false;
		}
	}

	while (start < end)
	{
		const char c = (*start);
		if (c < '0') return false;
		if (c > '9') return false;
		accum *= base;
		accum += (c - '0');
		start++;
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
	if (c == '>') return true;
	if (c == '<') return true;
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

template <bool force_uppercase>
hash Runtime::hash_character_string ( const char* start, const char* end )
{
	jassert(start);
	jassert(end);
	jassert(start<end);

	hash h = hash_string_internal<force_uppercase>(start, end);

	const size_t length = end - start;

	while (true)
	{
		if (m_strings.has_key(h))
		{
			if (0 == _strnicmp(m_strings[h].c_str(), start, length))	
			{
				// String is already in the map, the hash is ok
				return h;
			}
			else
			{
				// collision, re-hash
				String str(start, end);
				cerr << "hash collision between \"" << m_strings[h].c_str()  << "\" and \"" << str.c_str() << endl;
				h = hash_string_internal<force_uppercase>(start, end, h);
			}
		}
		else
		{
			// add the string
			m_strings[h] = String(start, end);
		}

	};

	return h;
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

	hash h = hash_string(start, end);

	State state;
	state.input = end + 1;
	state.cell	= new Cell(Cell::STRING, h);
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

	// todo: make read_atom return a state, rename to accept atom
	// maybe inline here
	State state;
	state.cell	= read_atom(start, end);
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