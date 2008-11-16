#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstring>
#include <Atom.h>
using namespace std;

int main (int argc, char** argv)
{
	//clog << "Size Of Cell " << sizeof(Cell) << " bytes." << endl;
	Runtime runtime;
	
	runtime.parse_and_evaluate("(load \"atom.lsp\")");
	runtime.parse_and_evaluate("(load \"lisp.lsp\")");

	while(true)
	{
		const int buffer_size = 1024;
		char buffer[buffer_size];
		cout << "> " << flush;
		cin.getline (buffer, buffer_size);
		if ( strlen ( buffer ) == 0 ) break;
		runtime.parse_and_evaluate(buffer);

		cout << "Cells before gc: " << Cell::s_count << endl;
		runtime.collect_garbage();
		cout << "Cells after  gc: " << Cell::s_count << endl;
	}

	//cout << "Press any key to quit." << endl;
	char end = cin.get();
	return EXIT_SUCCESS;
}