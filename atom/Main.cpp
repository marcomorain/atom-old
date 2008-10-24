#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cstring>
#include <Atom.h>
using namespace std;

int main (int argc, char** argv)
{
	clog << "Size Of Cell " << sizeof(Cell) << " bytes." << endl;
	Runtime runtime;
	/*
	runtime.parse_and_evaluate(	"(setf y '(1 2 3))\n"
								"(defun mycar (x) ( car x ) )\n"
								"(mycar y)");
	*/

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