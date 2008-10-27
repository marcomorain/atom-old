#include <cstdlib>
#include <iostream>
#include <cstring>
#include <Tools/Assert.h>
using namespace std;
#include <Atom.h>
#include <Functions.h>

const static Runtime::State no_match = {0,0};

Runtime::Runtime ( void )
: m_hash_quote		(hash_ident("QUOTE"))
, m_hash_backquote	(hash_ident("BACKQUOTE"))
, m_hash_comma		(hash_ident("COMMA"))
, m_hash_lambda		(hash_ident("LAMBDA"))
, m_hash_nil		(hash_ident("NIL"))
, m_hash_t			(hash_ident("T"))
{	
	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	m_nil = new Cell(Cell::LIST);
	m_nil->set_atom_name("NIL");

	m_output = stdout;

	register_function("AND",		function_and);
	register_function("ATOM",		function_atom);
	register_function("APPLY",		function_apply);
	register_function("SETF",		function_setf);
	register_function("CAR",		function_car);
	register_function("CDR",		function_cdr);
	register_function("CONS",		function_cons);
	register_function("COND",		function_cond);
	register_function("DEFUN",		function_defun);
	register_function("EVAL",		function_eval);
	register_function("EQ",			function_eq);
	register_function("ERROR",		function_error);
	register_function("STRINGP",	function_stringp);
	register_function("IF",			function_if);
	register_function("NULL",		function_null);
	register_function("NTH-VALUE",	function_nth_value);
	register_function("OR",			function_or);
	register_function("+",			function_plus);
	register_function("-",			function_minus);
	register_function("LIST",		function_list);
	register_function("LOAD",		function_load);
	register_function(">",			function_greater_than);
	register_function("<",			function_less_than);
	register_function("QUOTE",		function_quote);
	register_function("PROGN",		function_progn);
	register_function("LENGTH",		function_length);
	register_function("BLOCK",		function_block);
	register_function("RETURN-FROM",function_return_from);
	register_function("BACKQUOTE",	function_backquote);
}

Runtime::~Runtime ( void )
{
}

Cell* Runtime::replace_commas ( Cell* expression )
{
	if (!expression)
	{
		return expression;
	}

	if ( ! listp ( expression ) )
	{
		return expression;
	}

	Cell* first = car (expression);
	
	if ( first && first->is_a(Cell::IDENT))
	{
		if ( first->name() == m_hash_comma )
		{
			Cell* rest = car(cdr(expression));
			return evaluate( replace_commas (rest) );
		}
		else if (first->name() == m_hash_backquote)
		{
			Cell* rest = car(cdr(expression));
			return replace_commas(rest);
		}
	}
	
	if (car(expression))
	{
		car(expression) = replace_commas( car(expression) );
	}

	if (cdr(expression))
	{
		cdr(expression) = replace_commas( cdr(expression) );
	}
	
	return expression;
}

Cell* Runtime::funcall ( Cell* func, Cell* params )
{
	jassert(params);

	Cell* lambda = car(func);
	jassert(lambda->name() == m_hash_lambda);

	Cell* arg_list = car(cdr(func));

	Cell* body =  cdr(cdr(func));

	//todo: cache this somewhere
	const int num_args = length(arg_list);

	jassert( num_args == length(params));

	Array<Cell*> old_values;

	// todo: push args
	for (	Cell* arg = arg_list, *param = params;
			arg;
			arg = cdr(arg), param = cdr(param))
	{
		jassert(param);
		const hash name = car(arg)->name();
		old_values.push_back( m_symbols[name] );
		m_symbols[ name ] = evaluate(car(param));
	}

	Cell* result = function_progn( *this, body );

	// todo: pop args
	int from_end = 1;
	for (Cell* arg = arg_list; arg; arg = cdr(arg))
	{
		const hash name = car(arg)->name();
		m_symbols[ name ] = old_values[old_values.size() - from_end];
		from_end++;
	}
	return result;
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

	cerr << "Function \"" << name( function ) << "\" could not be found." << endl;
	return m_nil;
}

const char* Runtime::name ( Cell* cell ) const
{
	jassert(cell->is_a(Cell::IDENT));
	return m_strings.get( cell->name() ).c_str();
}

void Runtime::register_function ( const char* name, Runtime::Function func )
{
	m_builtins[ hash_ident(name, name+strlen(name)) ] = func;
}
void Runtime::def_macro ( hash h, Cell* params, Cell* body )
{
	Macro& m = m_macros[h];
	m.params = params;
	m.body   = body;
}

void Runtime::output( Cell* cell ) const
{
	output_recursive(cell, true);
	fprintf(m_output, "\n");
}

void Runtime::output_recursive ( Cell* cell, bool head_of_list) const
{
	jassert(cell);

	switch (cell->m_type)
	{
		case Cell::LIST:
		{
			if (head_of_list)
			{
				fprintf(m_output, "(");
			}

			if (car(cell))
			{
				output_recursive(car(cell), car(cell)->is_a(Cell::LIST) );
			}
			else
			{
				fprintf(m_output, "NIL");
			}

			if (cdr(cell))
			{
				fprintf(m_output, " ");
				output_recursive(cdr(cell), car(cell)->is_a(Cell::LIST));
			}

			if (head_of_list)
			{
				fprintf(m_output, ")");
			}
		}
		break;

	case Cell::ATOM:
	case Cell::IDENT:
		fprintf(m_output, name(cell));
		break;

	case Cell::STRING:
		fprintf(m_output, "\"%s\"", m_strings.get(cell->m_union.u_string).c_str());
		break;

	case Cell::NUMBER:
		fprintf(m_output, "%d", cell->number());
		break;

	case Cell::TRUE:
		fprintf(m_output, "T");
		break;

	default:
		fprintf(m_output, "Error: Default");
		break;
	}

}


Cell* Runtime::evaluate( Cell* cell )
{
	if (nil(cell))
	{
		return m_nil;
	}

	if (cell->is_a(Cell::LIST))
	{
		Cell* name		= car(cell);
		Cell* params	= cdr(cell);

		if (name->is_a(Cell::IDENT))
		{
			if (name->name() == m_hash_lambda)
			{
				return cell;
			}
			return call_function( name, params);
		}

		return null;
	}

	if (cell->is_a(Cell::IDENT))
	{
		return m_symbols.get(cell->name());
	}

	if (cell->is_a(Cell::NUMBER) || cell->is_a(Cell::STRING))
	{
		return cell;
	}

	if (cell->is_a(Cell::TRUE))
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
			//output(state.cell);
			Cell* result = evaluate(state.cell);
			output(result);
		}
	}

	return success;
}

Cell* Runtime::read_atom ( const char* start, const char* end )
{
	Integer val;

	Cell* atom;

	if (read_integer(start, end, val))
	{
		// cell
		atom = new Cell(Cell::NUMBER);
		atom->number() = val;
	}
	else
	{
		const hash h = hash_ident(start, end);

		if (h == m_hash_nil)
		{
			atom = m_nil;
		}
		else if (h == m_hash_t)
		{
			atom = m_T;
		}
		else
		{
			atom =  new Cell(Cell::IDENT, h);
			atom->set_atom_name( get_string(h) );
		}
	}

	return atom;
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
	if (c == '_') return true;
	return false;
}

const char* skip_whitespace ( const char* input )
{
	while (true)
	{
		char c = *input;

		switch (c)
		{
			case '\n':
			case '\r':
			case '\t':
			case  ' ':
			case '\f':
			case '\v':
			{
				input++;
				break;
			}

			// comment
			case ';':
			{
				while (*input && *input != '\n') input++;
				break;
			}

			default:
			{
				return input;
			}
		}
	}
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

	const char c = *input;

	switch (c)
	{
		default:
		return no_match;

		case QUOTE_CHARACTER:
		case BACKQUOTE_CHARACTER:
		case COMMA_CHARACTER:
		{
			Cell* quote = new Cell(Cell::LIST);
			Cell* rest  = new Cell(Cell::LIST);

			State state = accept_s_expression(input+1);
			jassert(valid(state));

			hash h;
			if		(c == QUOTE_CHARACTER)		h = m_hash_quote;
			else if (c == BACKQUOTE_CHARACTER)	h = m_hash_backquote;
			else if (c == COMMA_CHARACTER)		h = m_hash_comma;
			else
			{
				jassert(0);
			}

			car(quote) = new Cell(Cell::IDENT, h);

			if		(c == QUOTE_CHARACTER)		car(quote)->set_atom_name("QUOTE");
			else if (c == BACKQUOTE_CHARACTER)	car(quote)->set_atom_name("BACKQUOTE");
			else if (c == COMMA_CHARACTER)		car(quote)->set_atom_name("COMMA");

			cdr(quote) = rest;
			car(rest) = state.cell;
			cdr(rest) = null;

			state.cell = quote;

			return state;
		}
	}
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
