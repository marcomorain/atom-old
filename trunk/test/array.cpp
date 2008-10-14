#include <UnitTest++.h>
#include <Atom.h>

TEST(array)
{
	Array<int> a;
	Array<int>::size_type s = a.size();
	CHECK(s == 0);
	a.push_back(1);
	a.push_back(2);
	s = a.size();
	CHECK(s == 2);
	Array<int>::size_type c = a.capacity();
	CHECK(c == 2);
	a.push_back(3);
	c = a.capacity();
	CHECK(c == 4);
	a.push_back(4);
	c = a.capacity();
	CHECK(c == 4);
	s = a.size();
	CHECK(s==4);
	CHECK(a[s-1] == 4);
	for (int i=0; i<4; i++)
	{
		a.pop_back();
		s = a.size();
	}
	a.reserve(4);
	c = a.capacity();
	a.reserve(8);
	CHECK(a.capacity() > c);
	CHECK(a.size() == 0);
	a.shrink();
	CHECK(a.capacity() == 0);

	for (int i=0; i<100; i++)
		a.push_back(i);
	for (int i=0; i<99; i++)
		a.pop_back();

	c = a.capacity();
	CHECK(c >= 100);
	
	a.shrink();

	CHECK(a.size() == 1);
	CHECK(a.capacity() < c);

	a.erase(0);

	a.shrink();

	a.push_back(5);
	a.push_back(6);
	CHECK(a.back() == 6);
	CHECK(a.front() == 5);

	a.erase(0);
	a.erase(0);

	for (int i=0; i<200; i++)
	{
		a.push_back(7);
	}

	Array<int>::value_type total = 0;
	for (Array<int>::const_iterator i = a.begin(); i != a.end(); ++i)
	{
		total += *i;
	}
	CHECK(total == 7 * 200);

	
}