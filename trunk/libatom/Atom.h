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
//  private: // members
public:


	Cell* m_nil;
	Cell* m_T;
	
	typedef Cell* (*Function)(Runtime&, Cell*);

	Map<hash, Function> m_builtins;
	Map<hash, Cell*>	m_symbols;
	Map<hash, Cell*>	m_functions;

	Cell* call_function (Cell* function, Cell* params ); 

	void register_function ( const char* name, Function func );

	Cell* pushAtom ( char* atom );

  public: // functions

	Runtime ( void );
	~Runtime ( void );

	Cell* evaluate ( Cell* cell );

	Cell* funcall ( Cell* func, Cell* params );


	bool parse_and_evaluate ( const char* input );

	public: // Parser Functions


	typedef struct
	{
		Cell*		cell;
		const char* input;

	} State;

	State accept_atom			( const char* input );
	State accept_ident			( const char* input );
	State accept_string			( const char* input );
	State accept_quoted_s_exp	( const char* input );
	State accept_list			( const char* input );
	State accept_series			( const char* input );
	State accept_s_expression	( const char* input );
	State accept_dot			( const char* input );	
};

inline bool nil ( Cell* cell )
{
	return ! ( cell && ( car(cell) || cdr(cell) ) );
}

inline bool valid ( const Runtime::State& state )
{
	return state.cell && state.input;
}

// Validation
bool atom_char ( const char c );
bool white_space_char ( const char c );
const char* skip_whitespace ( const char* input );
bool read_integer (const char* string, Integer& value);

