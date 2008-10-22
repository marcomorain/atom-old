#include <UnitTest++.h>
#include <Atom.h>

TEST(String)
{
	// nulls
	{
		String s;
		CHECK(s.c_str() == null);

		String t = s;
		CHECK(t.c_str() == null);

		String u(t);
		CHECK(u.c_str() == null);
	}

	// strings
	{
		String h("Hello");
		CHECK(h.c_str());
		CHECK(strlen("Hello") == strlen(h.c_str()));

		String x = h;
		CHECK(strcmp(x.c_str(), h.c_str()) == 0);

		String y(x);

		CHECK(strcmp(y.c_str(), "Hello") == 0);

		const char* start = "Start of a string";
		const char* end = start + strlen("Start of a");
		String start_end(start, end);
		CHECK(start_end.c_str());
		CHECK(strcmp(start_end.c_str(), "Start of a") == 0);
	}
}

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

static inline bool read_integer_z(const char* s, Integer& val)
{
	return read_integer(s, s+strlen(s), val);
}

TEST(read_integer)
{
	Integer val;
	
	CHECK(read_integer_z ("0", val));
	CHECK(val == 0);

	CHECK(read_integer_z("1", val));
	CHECK(val == 1);

	CHECK(read_integer_z("10", val));
	CHECK(val == 10);

	CHECK(read_integer_z("9999999", val));
	CHECK(val == 9999999);

	CHECK(read_integer_z("3242", val));
	CHECK(val == 3242);

	CHECK(read_integer_z("-1", val));
	CHECK(val == -1);

	CHECK(read_integer_z("-0", val));
	CHECK(val == 0);
	
	CHECK(!read_integer_z("-", val));
	
	CHECK(!read_integer_z("a", val));
	CHECK(!read_integer_z("123j", val));
	CHECK(!read_integer_z("f887837", val));
	CHECK(!read_integer_z(" 343242", val));
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
	CHECK( number.cell->number() == 7);
}

TEST(skip_whitespace)
{
	const char* comment = "; a comment";
	const char* end = skip_whitespace(comment);
	CHECK(end == comment + strlen(comment));
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

TEST(function_plus)
{
	Runtime r;
	Runtime::State state;
	Cell* result;

	state = r.accept_s_expression("(+ 1 2)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == 3);

	state = r.accept_s_expression("(+ 9)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == 9);

	state = r.accept_s_expression("(+ -34 -1)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == -35);

	state = r.accept_s_expression("(+ 1 2 -13)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == -10);
}

TEST(function_minus)
{
	Runtime r;
	Runtime::State state;
	Cell* result;

	state = r.accept_s_expression("(- 1 2)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == -1);

	state = r.accept_s_expression("(- 9)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == -9);

	state = r.accept_s_expression("(- -34 -1)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == -33);

	state = r.accept_s_expression("(- 1 2 -13)");
	CHECK(valid(state));
	result = r.evaluate(state.cell);
	CHECK(result->is_a(Cell::NUMBER));
	CHECK(result->number() == 12);
}

int main(int argc, char** argv)
{ 
	return UnitTest::RunAllTests();
}