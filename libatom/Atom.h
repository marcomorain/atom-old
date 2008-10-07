#pragma once
#include <JAssert.h>
#include <Array.h>
#include <Map.h>
#include <Cell.h>

/*
"Name"    = 'LISP'
"Author"  = 'John McCarthy'
"Version" = 'Minimal'
"About"   = 'LISP is an abstract language that organizes ALL data around "lists".'

"Start Symbol" = <s-Expression> 

{Atom Char}   = {Printable} - {Whitespace} - [()"\'']

Atom = ( {Atom Char} | '\'{Printable} )+ 

<s-Expression> ::=	  <Quote> Atom 
					| <Quote> '(' <Series> ')' 
					| <Quote> '(' <s-Expression> '.' <s-Expression> ')'

<Series> ::=	  <s-Expression> <Series> 
				|

<Quote>  ::=	  ''      !Quote = do not evaluate 
				|
*/
/*
	<s-expr>	=>	<quoted-s-expr>
				|	Atom
				|	<list>

	<list>		=>	'(' <series> ')'
				|	'(' <s-expr> . <s-expr> ')'

	<series>	=>	<s-expr> <series> 
				|	-

	<quoted-s-epr>	=>	'`' <s-epr>
*/

class Runtime : public NoCopy
{

	public:

	Map<hash, Cell*> m_symbols;

	Cell* m_nil;
	Cell* m_T;
	
	typedef Cell* (*Function)(Runtime&, Cell*);

	Array<Function>		m_functions;
	Array<const char*>	m_function_names;

	Cell* call_function (const char* function_name, Cell* params ); 

	void register_function ( const char* name, Function func );

	 Runtime ( void );
	~Runtime ( void );

	Cell* execute ( Cell* cell );
	Cell* pushAtom ( char* atom );

};

typedef struct
{
	Cell*		cell;
	const char* input;
} State;


inline bool valid ( const State& state )
{
	return state.cell && state.input;
}

// Validation
bool atom_char ( const char c );
bool white_space_char ( const char c );
const char* skip_whitespace ( const char* input );
bool read_integer (const char* string, Integer& value);

// Parser Functions
State accept_atom			( const char* input, Runtime& parser );
State accept_ident			( const char* input, Runtime& parser );
State accept_string			( const char* input, Runtime& parser );
State accept_quoted_s_exp	( const char* input, Runtime& parser );
State accept_list			( const char* input, Runtime& parser );
State accept_series			( const char* input, Runtime& parser );
State accept_s_expression	( const char* input, Runtime& parser );
State accept_dot ( const char* input, Runtime& parser );
bool parse ( const char* input );
