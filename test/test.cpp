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
	
	CHECK(!read_integer("a", val));
	CHECK(!read_integer("123j", val));
	CHECK(!read_integer("f887837", val));
	CHECK(!read_integer(" 343242", val));
}


TEST(accept_ident)
{
	Runtime r;

	State state;

	const char* input1 = "this is a valid input";

	state = accept_atom(input1, r);
	CHECK(valid(state));

	CHECK(!valid(accept_ident("\"invalid\"", r)));

	const char* input3 = "*this-is-a-long-one*";
	state = accept_ident(input3, r);
	CHECK(valid(state));
	CHECK(0 == strcmp(input3, car(state.cell)->atom_name()));

	State number = accept_ident("7", r);
	CHECK(valid(number));
	CHECK( car ( number.cell)->is_a(Cell::NUMBER));
	CHECK( car ( number.cell)->m_union.u_int == 7);
}

TEST(accept_string)
{
	Runtime runtime;

	State state;

	const char* input1 = " \"this is a string\"with a tail";
	state = accept_string(input1, runtime);

	CHECK(state.cell);
	CHECK(*state.input == 'w');

	const char* input2 = "this is not a string";
	state = accept_string(input2, runtime);

	CHECK(!state.cell);
	CHECK(!state.input);
}


TEST(accept_list)
{
	Runtime runtime;

	CHECK(!valid(accept_list(" this is not valid ", runtime)));

	CHECK(valid(accept_list("( ) ", runtime)));

	CHECK(!valid(accept_list("((", runtime)));

	CHECK(valid(accept_list("( this is a valid list )", runtime)));
}

TEST(runtime)
{
}

int main(int argc, char** argv)
{ 
	return UnitTest::RunAllTests();
}