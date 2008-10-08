#include <UnitTest++.h>
#include <Atom.h>

TEST(atom_char)
{
	CHECK( atom_char('a'));
	CHECK( atom_char('z'));
	CHECK( atom_char('1'));
	CHECK( atom_char('*'));
	CHECK( atom_char('A'));
	CHECK( atom_char('Z'));
	CHECK( atom_char('-'));
	CHECK( atom_char('+'));
	CHECK( atom_char('?'));
	CHECK(!atom_char(0)  );
	CHECK(!atom_char('\n'));
	CHECK(!atom_char('\t'));
	CHECK(!atom_char('\r'));
}

TEST(cell)
{
}

TEST(read_integer)
{
	Integer val;
	
	CHECK(read_integer ("0", val));
	CHECK(val == 0);

	CHECK(read_integer ("1", val));
	CHECK(val == 1);

	CHECK(read_integer ("10", val));
	CHECK(val == 10);

	CHECK(read_integer ("9999999", val));
	CHECK(val == 9999999);

	CHECK(read_integer ("3242", val));
	CHECK(val == 3242);

	CHECK(read_integer("-1", val));
	CHECK(val == -1);

	CHECK(read_integer("-0", val));
	CHECK(val == 0);
	
	CHECK(!read_integer("-", val));
	
	CHECK(!read_integer("a", val));
	CHECK(!read_integer("123j", val));
	CHECK(!read_integer("f887837", val));
	CHECK(!read_integer(" 343242", val));
}


TEST(accept_ident)
{
	Runtime r;

	Runtime::State state;

	const char* input1 = "this is a valid input";

	state = r.accept_atom(input1);
	CHECK(valid(state));

	CHECK(!valid(r.accept_ident("\"invalid\"")));

	const char* input3 = "*this-is-a-long-one*";
	state = r.accept_ident(input3);
	CHECK(valid(state));
	CHECK(0 == _stricmp(input3, state.cell->atom_name()));

	Runtime::State number = r.accept_ident("7");
	CHECK(valid(number));
	CHECK( number.cell->is_a(Cell::NUMBER));
	CHECK( number.cell->m_union.u_int == 7);
}

TEST(accept_string)
{
	Runtime runtime;

	Runtime::State state;

	const char* input1 = " \"this is a string\"with a tail";
	state = runtime.accept_string(input1);

	CHECK(state.cell);
	CHECK(*state.input == 'w');

	const char* input2 = "this is not a string";
	state = runtime.accept_string(input2);

	CHECK(!state.cell);
	CHECK(!state.input);
}


TEST(accept_list)
{
	Runtime runtime;

	CHECK(!valid(runtime.accept_list(" this is not valid ")));

	CHECK(valid(runtime.accept_list("( ) ")));

	CHECK(!valid(runtime.accept_list("((")));

	CHECK(valid(runtime.accept_list("( this is a valid list )")));
}

TEST(runtime)
{
}

int main(int argc, char** argv)
{ 
	return UnitTest::RunAllTests();
}