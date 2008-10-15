#include <cstdlib>
#include <iostream>
#include <cstring>
#include <Tools/Assert.h>
using namespace std;
#include <Atom.h>
#include <Functions.h>

const static Runtime::State no_match = {0,0};

Runtime::Runtime ( void )
{
	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	m_output = new StandardOutput();

	register_function("APPLY",		function_apply);
	register_function("SETF",		function_setf);
	register_function("CAR",		function_car);
	register_function("CDR",		function_cdr);
	register_function("CONS",		function_cons);
	register_function("EQ",			function_eq);
	register_function("ERROR",		function_error);
	register_function("STRINGP",	function_stringp);
	register_function("IF",			function_if);
	register_function("NTH-VALUE",	function_nth_value);
	register_function("+",			function_plus);
	register_function("-",			function_minus);
	register_function("LOAD",		function_load);
	register_function(">",			function_greater_than);
	register_function("<",			function_less_than);
	register_function("QUOTE",		function_quote);
	register_function("LENGTH",		function_length);

}

Runtime::~Runtime ( void )
{
}

void Runtime::set_output ( Output* output )
{
	delete m_output;
	m_output = output;
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

void append ( Array<char>& output, const char* str )
{
	while (*str)
	{
		output.push_back(*str);
		str++;
	}
}

void write_integer ( Array<char>& output, Integer value )
{
	Array<char> stack;
	stack.reserve(10);
	
	do
	{
		stack.push_back( '0' + (value % 10) );
		value /= 10;

	} while (value > 10);

	while (stack.size())
	{
		output.push_back(stack.back());
		stack.pop_back();
	}
}

void Runtime::to_string ( Array<char>& output, Cell* cell )
{
	to_string_recursive(output, cell, true);
	output.push_back('\n'); // null terminate
	output.push_back('\0'); // null terminate
}

void Runtime::to_string_recursive ( Array<char>& output, Cell* cell, bool head_of_list)
{
	if (!cell)
	{
		append(output, "NIL");
		return;
	}

	switch (cell->m_type)
	{
		case Cell::LIST:
		{
			if (head_of_list)
			{
				append(output, "(");
			}

			if (car(cell))
			{
				to_string_recursive(output, car(cell), car(cell)->is_a(Cell::LIST) );
			}
			else
			{
				append(output, "NIL");
			}

			if (cdr(cell))
			{
				append(output, " ");
				to_string_recursive(output, cdr(cell), car(cell)->is_a(Cell::LIST));
			}

			if (head_of_list)
			{
				append(output, ")");
			}
		}
		break;

	case Cell::ATOM:
	case Cell::IDENT:
		append(output, cell->atom_name());
		break;

	case Cell::STRING:
		{
			const char* str = m_strings.get(cell->m_union.u_string).c_str();
			append(output, "\"");
			append(output, str);
			append(output, "\"");
		}
		break;

	case Cell::NUMBER:
		write_integer(output, cell->number());
		break;

	case Cell::TRUE:
		append(output, "T");
		break;

	default:
		append(output, "Default");
		break;
	}

}


Cell* Runtime::evaluate( Cell* cell )
{
	if (nil(cell))
	{
		return null;
	}
	else if (cell->is_a(Cell::LIST))
	{
		Cell* name		= car(cell);
		Cell* params	= cdr(cell);
		return call_function( name, params);
	}
	else if (cell->is_a(Cell::IDENT))
	{
		return cell->m_union.u_ident.m_value;
	}
	else if (cell->is_a(Cell::NUMBER))
	{
		return cell;
	}
	else if (cell->is_a(Cell::STRING))
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

			{
				Array<char> output;
				to_string(output, state.cell);
				m_output->print(output.begin());
			}

			Cell* result = evaluate(state.cell);

			{
				Array<char> output;
				to_string(output, result);
				m_output->print(output.begin());
			}
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
			atom_cell->set_atom_name(start, end);
			m_symbols[h] = atom_cell;
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

	String str(start, end);

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
				cerr << "hash collision between \"" << m_strings[h].c_str()  << "\" and \"" << str.c_str() << endl;
				h = hash_string_internal<force_uppercase>(start, end, h);
			}
		}
		else
		{
			// add the string
			std::swap( m_strings[h], str );
		}

	};

	return h;
}

Runtime::State Runtime::accept_quoted_s_exp ( const char* input )
{
	jassert(input);

	if (*input == 0) return no_match;

	input = skip_whitespace(input);

	if (*input != '`')
	{
		return no_match;
	}

	Cell* quote = new Cell(Cell::LIST);
	Cell* rest  = new Cell(Cell::LIST);

	State state = accept_s_expression(input+1);
	jassert(valid(state));
	
	const char* quote_start = "QUOTE";
	const char* quote_end   = quote_start + strlen("QUOTE");

	car(quote) = new Cell(Cell::IDENT, hash_ident(quote_start, quote_end));
	car(quote)->set_atom_name("QUOTE");
	cdr(quote) = rest;

	car(rest) = state.cell;
	cdr(rest) = null;

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

	//Cell* head = new Cell(Cell::LIST);

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

	//car(head) = state.cell;
	//state.cell = head;
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
