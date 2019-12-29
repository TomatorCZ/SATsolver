#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "components.h"

using namespace std;

bool test_load_dimacs_cnf(istream& input)
{
	cnf_formula f;
	f.load_formula(input);
	f.print_formula(cout);
	return true;
}

int main()
{
	ifstream f("1.cnf");
	test_load_dimacs_cnf(f);


	std::cout << "Hello World!\n";
}
