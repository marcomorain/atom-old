#include <cstdlib>
#include <iostream>
#include <string>
#include <JAssert.h>
using namespace std;
#include <Atom.h>
#include <Functions.h>

const static State no_match = {0,0};

Runtime::Runtime ( void )
{
	m_nil = new Cell(Cell::LIST);
	m_nil->set_atom_name("nil");

	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	register_function("setq", function_setq);
	register_function("car",  function_car);
	register_function("cdr",  function_cdr);
}

Runtime::~Runtime ( void )
{
}

Cell* Runtime::call_function (const char* function_name, Cell* params )
{
	jassert(params);
	jassert(function_name);
	jassert(m_function_names.size() == m_functions.size());

	for (int c=0; c<m_function_names.size(); c++)
	{
		if ( 0 == strcmp ( function_name, m_function_names[c] ) )
		{
			return m_functions[c](*this, params);
		}
	}

	jassert(0 && "Function not found");
	return 0;
}

void Runtime::register_function ( const char* name, Runtime::Function func )
{
	jassert(m_function_names.size() == m_functions.size());

	for (int i=0; i<m_function_names.size(); i++)
	{
		jassert(strcmp(m_function_names[i], name));
	}

	unsigned function_index = m_function_names.size();
	m_function_names.push_back(name);
	m_functions.push_back(func);
}

Cell* Runtime::execute ( Cell* cell )
{
	Cell* function =  car ( cell );
	return call_function( function->m_atom_name, cdr (function ) );
}

Cell* Runtime::pushAtom ( char* atom )
{
	Integer val;

	if (read_integer(atom, val))
	{
		cout << "Accepted an integer " << val << endl;

		Cell* c = new Cell(Cell::LIST);
		c->set_atom_name("Points to an integer");

		// cell
		Cell* atom_cell = new Cell(Cell::NUMBER);
		atom_cell->m_union.u_int = val;
		car(c) = atom_cell;
		return c;
	}
	else
	{
		cout << "Accepted an atom " << atom << endl;

		Cell* c = new Cell(Cell::LIST);
		c->set_atom_name("Points to an ident");

		// cell
		Cell* atom_cell = new Cell(Cell::IDENT);
		atom_cell->set_atom_name(atom);
		car(c) = atom_cell;

		return c;
	}
}

bool read_integer (const char* string, Integer& value)
{
	jassert(string);
	jassert(strlen(string)>0);

	const Integer base = 10;
	Integer accum = 0;
	
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
	value = accum;
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

State accept_quoted_s_exp ( const char* input, Runtime& parser)
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

	State state = accept_s_expression(input+1, parser);
	jassert(valid(state));
	
	car(quote) = state.cell;
	state.cell = quote;

	return state;
}

State accept_string ( const char* input, Runtime& parser )
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
	cout << "Accepted a string " << s << endl;
	state.cell = new Cell(Cell::LIST);
	state.cell->set_atom_name("Points to a string");
	Cell* string_cell = new Cell(Cell::STRING, s);
	string_cell->set_atom_name("A string atom");
	car(state.cell) = string_cell;
	state.input = end + 1;

	return state;
}

State accept_ident ( const char* input, Runtime& parser )
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

	// todo: make push atom accept a start,end char* pair
	State state;
	state.cell	= parser.pushAtom(atom);
	state.input = end; 
	return state;
}

State accept_atom ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return no_match;

	State state;

	state = accept_string(input, parser);

	if (!valid(state))
	{
		state = accept_ident(input, parser);
	}

	return state;
}

State accept_dot ( const char* input, Runtime& parser )
{
	jassert(0);
	return no_match;
}

State accept_list ( const char* input, Runtime& parser )
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
		cout << "Accepting the NIL list \"( )\"" << endl;
		State nil;
		nil.input = input + 1;
		nil.cell = parser.m_nil;
		return nil;
	}

	Cell* head = new Cell(Cell::LIST);

	State state = accept_series(input, parser);
	
	if (!valid(state))
	{
		state = accept_s_expression(input, parser);
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
	else
	{
		car(head) = state.cell;
		state.cell = head;
		return state;
	}
}

State accept_s_expression ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return no_match;
	input = skip_whitespace ( input );

	State state = accept_quoted_s_exp(input, parser);

	if (valid(state)) 
	{
		return state;
	}

	state = accept_atom(input, parser);

	if (valid(state))
	{
		return state;
	}

	state = accept_list(input, parser);

	if (valid(state))
	{
		return state;
	}

	return no_match;
}

State accept_series	( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return no_match;
	input = skip_whitespace(input);

	State sexpr = accept_s_expression(input, parser);

	if (!valid(sexpr))
	{
		return no_match;
	}

	State series = accept_series(sexpr.input, parser);

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

bool parse ( const char* input )
{
	Runtime parser;

	const State s = accept_s_expression (input, parser);

	cout << s.cell << endl;

	if (0)
	{
		Cell* result = parser.execute( s.cell );
		cout << result << endl << endl;
	}

	return !!s.cell;
}
