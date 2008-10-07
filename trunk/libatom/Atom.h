#pragma once
#include <JAssert.h>
#include <Array.h>
#include <Map.h>
#include <Cell.h>

/*
"Name"    = 'LISP'
"Author"  = 'John McCarthy'
"Version" = 'Minimal'
"About"   = 'LISP is an abstract language that organizes ALL'
| 'data around "lists".'

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



class Runtime : public NoCopy
{
  private:
  public:

	Array<Cell*> m_last_cell;
	Map<hash, Cell*> m_symbols;

	Cell* m_nil;
	Cell* m_T;
	Cell* m_last_parse;
	Cell* m_last_list;
	
	typedef Cell* (*Function)(Runtime&, Cell*);

	Array<Function>		m_functions;
	Array<const char*>	m_function_names;

	Cell* call_function (const char* function_name, Cell* params ); 

	void register_function ( const char* name, Function func );

	 Runtime ( void );
	~Runtime ( void );

private:

	Cell* getLastCell ( void ) const;
	void setLastCell ( Cell* c );
	Cell* addCell ( Cell* cell );
	void endList ( void );
	void startList ( void );

public:
	
	void printCell ( const Cell* cell ) const;
	void pushQuote ( void );
	void pushString ( char* str );
	void pushAtom ( char* atom );
	void pushLeftParen ( void );
	void pushRightParen ( void );
	void pushDot ( void );
	Cell* execute ( Cell* cell );
	Cell* get_last_parse ( void ) const;
};

bool atom_char ( const char c );
bool white_space_char ( const char c );
const char* skip_whitespace ( const char* input );
const char* accept_quote ( const char* input, Runtime& parser);
const char* accept_atom ( const char* input, Runtime& parser );
const char* accept_left_paren ( const char* input, Runtime& parser );
const char* accept_right_paren ( const char* input, Runtime& parser );
const char* accept_dot ( const char* input, Runtime& parser );
const char* accept_s_expression ( const char* input, Runtime& parser );
const char* accept_series ( const char* input, Runtime& parser );
bool parse ( const char* input );
