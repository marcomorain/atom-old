#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <Atom.h>
using namespace std;

int main (int argc, char** argv)
{
	Runtime runtime;
	//runtime.parse("(a b (c d) e)");

	while(true)
	{
		const int buffer_size = 1024;
		char buffer[buffer_size];
		cout << "> " << flush;
		cin.getline (buffer, buffer_size);
		if ( strlen ( buffer ) == 0 ) break;
		runtime.parse_and_evaluate(buffer);
	}

	cout << "Press any key to quit." << endl;
	char end = cin.get();
	return EXIT_SUCCESS;
}