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

TEST(accept_atom)
{
	//CHECK(accept_atom("*var*"));
	//CHECK(accept_atom("??test"));
}

TEST(cell)
{
}

TEST(runtime)
{
	Runtime runtime;
	CHECK(!runtime.get_last_parse());
}

int main(int argc, char** argv)
{ 
	return UnitTest::RunAllTests();
}