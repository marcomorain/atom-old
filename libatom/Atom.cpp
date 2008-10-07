#include <cstdlib>
#include <iostream>
#include <string>
#include <JAssert.h>
using namespace std;

#include <Atom.h>
#include <Functions.h>
/*
static bool valid_atom_chars [127] =
{
	0,
}
*/

Runtime::Runtime ( void )
{
	m_nil = new Cell(Cell::LIST);
	m_nil->set_atom_name("nil");

	m_T = new Cell(Cell::TRUE);
	m_T->set_atom_name("T");

	register_function("setq", function_setq);
	register_function("car",  function_car);
	register_function("cdr",  function_cdr);

	m_last_list = null;	
	m_last_parse = null;
}

Runtime::~Runtime ( void )
{
}


Cell* Runtime::get_last_parse ( void ) const
{
	return m_last_parse;
}


void Runtime::printCell ( const Cell* c ) const
{
	if (!c) return;

	cout << "(";

	// todo: make get atom name
	if (c->m_atom_name)
	{
		cout << "Atom name: " << c->m_atom_name;
	}
	else if (c->is_a(Cell::STRING))
	{
		jassert(0);
	}
	else
	{
		printCell( car(c) );
	}

	cout << ", ";
	printCell( cdr(c) );
	cout << ")";
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

void Runtime::pushQuote ( void )
{
}

void Runtime::pushString ( char* str )
{
	Cell* c = new Cell(Cell::LIST);
	c->set_atom_name("Points to a string");
	addCell(c);

	Cell* string_cell = new Cell(Cell::STRING);
	string_cell->set_atom_name("A string atom");
	addCell(string_cell);
	car(c) = string_cell;
	setLastCell(c);
}

void Runtime::pushAtom ( char* atom )
{
	Cell* c = new Cell(Cell::LIST);
	c->set_atom_name("Points to an atom");
	addCell(c);

	// cell
	Cell* atom_cell = new Cell(Cell::NUMBER);
	atom_cell->set_atom_name(atom);
	addCell(atom_cell);

	car(c) = atom_cell;

	setLastCell(c);
}

void Runtime::pushLeftParen ( void )
{
	startList();
}

void Runtime::startList ( void )
{
	Cell* new_list = new Cell(Cell::LIST);
	addCell(new_list);
	m_last_list = new_list;
}

void Runtime::pushRightParen ( void )
{
	// cell
	endList();
	m_last_cell.push_back(null);
}

void Runtime::endList ( void )
{
	m_last_cell.pop_back();
}

void Runtime::pushDot ( void )
{
}

Cell* Runtime::addCell ( Cell* cell )
{
	if (!m_last_parse)
	{
		m_last_parse = cell;
	}

	if (m_last_list)
	{
		jassert(m_last_list->is_a(Cell::LIST));
		car(m_last_list) = cell;
		m_last_list = null;
	}
	
	if (getLastCell())
	{
		cdr(getLastCell()) = cell;
	}

	setLastCell(cell);

	return cell;
}

Cell* Runtime::getLastCell ( void ) const
{
	Cell* last = 0;
	if (m_last_cell.size())
	{
		last = m_last_cell.back();
	}
	return last;
}

void Runtime::setLastCell ( Cell* c )
{
	if (m_last_cell.size() > 0)
	{
		m_last_cell.back() = c;
	}
	else
	{
		m_last_cell.push_back(c);
	}
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

const char* accept_quote ( const char* input, Runtime& parser)
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);
	if (*input == '`')
	{
		parser.pushQuote();
		input++;
	}
	return input;
}

const char* accept_string ( const char* input, Runtime& parser )
{
	jassert(input);
	jassert(*input == '\"');

	const char* start = input + 1;
	const char* end = start;

	while (*end != '\"')
	{
		end++;

		if (*end == 0)
		{
			return null;
		}
	}

	const int length = end - start;
	char* s = (char*)malloc(length+1);
	strncpy(s, start, length);
	s[length] = 0;

	parser.pushString(s);

	return end + 1;
}

const char* accept_ident ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);

	const char* start = input;
	const char* end	= input;
	while (atom_char(*end)) end++;
	const int length = end - start;

	if (length < 1)
	{
		return null;
	}

	char* atom = (char*)malloc(length+1);
	strncpy(atom, start, length);
	atom[length] = null; // null terminal

	parser.pushAtom(atom);
	return end;
}

const char* accept_atom ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace (input);

	const char* atom = 0;

	if (*input == '\"')
	{
		atom = accept_string(input, parser);
	}
	else
	{
		atom = accept_ident(input, parser);
	}
	return atom;
}

const char* accept_left_paren ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);
	if ( *input == '(' )
	{
		parser.pushLeftParen();
		return input + 1;
	}
	return null;
}

const char* accept_right_paren ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);
	if ( *input == ')' )
	{
		parser.pushRightParen();
		return input + 1;
	}
	return null;
}


const char* accept_dot ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);
	if ( *input == '.' )
	{
		parser.pushDot();
		return input + 1;
	}
	return null;
}

const char* accept_s_expression ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace ( input );
	input = accept_quote(input, parser);

	if (const char* atom = accept_atom(input, parser))
	{
		return atom;
	}

	const char* s_expression = null;
	const char* dot = null;
	const char* left = accept_left_paren(input, parser);

	if (!left)
	{
		return null;
	}

	const char* series = accept_series(left, parser);
	
	if (!series)
	{
		s_expression = accept_s_expression(left, parser);

		if (!s_expression)
		{
			cout << "Parse error - unexpected token at " << left << endl;
			return null;
		}

		dot = accept_dot(s_expression, parser);
		
		if (!dot)
		{
			cout << "Parse error - expected . at " << s_expression << endl;
			return null;
		}	
	}

	const char* body = series ? series : dot;

	const char* right = accept_right_paren( body, parser);

	if (!right)
	{
		cout << "Parse error - expected ) at " << body << endl;
		return null;
	}

	return right;
}

const char* accept_series ( const char* input, Runtime& parser )
{
	jassert(input);
	if (*input == 0) return null;
	input = skip_whitespace(input);

	const char* s_expression = accept_s_expression(input, parser);

	if (!s_expression)
	{
		return null;
	}

	const char* series = accept_series(s_expression, parser);

	return series ? series : s_expression;
}

bool parse ( const char* input )
{
	Runtime parser;

	const char* last = accept_s_expression (input, parser);

	if (last)
	{
		Cell* parsed_input = parser.get_last_parse();
		Cell* result = parser.execute( parsed_input );
		cout << *result << endl << endl;
	}
	return !!last;
}
