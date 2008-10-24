#pragma once
#include <Tools/Assert.h>
#include <Tools/Array.h>
#include <Tools/Map.h>
#include <Tools/String.h>
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

	enum
	{
		QUOTE_CHARACTER		= '\'',
		BACKQUOTE_CHARACTER = '`',
		COMMA_CHARACTER		= ','
	};

	enum
	{
		HASH_SEED = 5381
	};


	typedef struct
	{
		Cell*		cell;
		const char* input;

	} State;

	typedef struct  
	{
		Cell* params;
		Cell* body;

	} Macro;

	typedef Cell* (*Function)(Runtime&, Cell*);

private:

	static char uppercase ( char c )
	{
		if ( 'a' <= c && c <= 'z' )
		{
			c -= ('a' - 'A');
		}
		return c;
	}

	template <bool force_uppercase>
	static hash hash_string_internal ( const char* start, const char* end, hash seed = HASH_SEED)
	{
		jassert(start);
		jassert(end);
		jassert(start<end);

		const char* current = start;
		hash h = seed;
		while (current < end)
		{
			const char c = force_uppercase ? uppercase(*current) : *current;
			h = h * 33 ^ c;
			current++;
		}

		return h;
	}

	template <bool force_uppercase>
	hash hash_character_string ( const char* start, const char* end );

	const char* name ( Cell* ident ) const;

public:
	
	hash hash_string ( const char* start, const char* end )
	{
		return hash_character_string<false>(start, end);
	}

	hash hash_ident  ( const char* start )
	{
		return hash_ident(start, start + strlen(start));
	}

	hash hash_ident  ( const char* start, const char* end )
	{
		return hash_character_string<true>(start, end);
	}

	Cell* call_function (Cell* function, Cell* params ); 

	void register_function ( const char* name, Function func );

	Cell* read_atom ( const char* start, const char* end );

	const char* get_string ( const hash h ) const
	{
		return m_strings.get(h).c_str();
	}

  public: // functions

	Runtime ( void );
	~Runtime ( void );

	Cell* evaluate	( Cell* cell );
	Cell* funcall	( Cell* func, Cell* params );
	Cell* replace_commas ( Cell* expression );

	void def_macro ( hash h, Cell* params, Cell* body );

	void output ( Cell* cell ) const;
	void output_recursive ( Cell* cell, bool head_of_list) const;

	bool parse_and_evaluate ( const char* input );

	State accept_atom			( const char* input );
	State accept_ident			( const char* input );
	State accept_string			( const char* input );
	State accept_quoted_s_exp	( const char* input );
	State accept_list			( const char* input );
	State accept_series			( const char* input );
	State accept_s_expression	( const char* input );
	State accept_dot			( const char* input );	

	public:

	Cell* m_T;

	Map<hash, Function> m_builtins;
	Map<hash, Cell*>	m_symbols;
	Map<hash, Cell*>	m_functions;
	Map<hash, Macro>	m_macros;

	// There is only ever one copy of a string.
	Map<hash, String> m_strings;

	const hash m_hash_quote;
	const hash m_hash_backquote;
	const hash m_hash_comma;
	const hash m_hash_lambda;
	const hash m_hash_nil;

	FILE* m_output;

};

inline bool valid ( const Runtime::State& state )
{
	return state.cell && state.input;
}

// Validation
bool atom_char ( const char c );
bool white_space_char ( const char c );
const char* skip_whitespace ( const char* input );
bool read_integer (const char* start, const char* end, Integer& value);

