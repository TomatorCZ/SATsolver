#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "command_line_controller.h"
#include "components.h"
#include "cdcl_algorithm.h"

using namespace std;

bool test_load_dimacs_cnf(istream& input)
{
	cnf_formula f;
	f.load_formula(input);

	f.print_formula(cout);
	return true;
}

bool test_cnf(istream& input, bool expected)
{
	cnf_formula f;
	f.load_formula(input);

	std::cout << "Testing cnf formula..." << endl;
	f.print_formula(cout);
	
	std::cout << "logs:" << endl;
	cdcl_algorithm alg(f,cout,true);

	bool result = alg.make();

	std::cout << "Correct ? " << (result == expected) << endl;
	std::cout << "Assignment:" << endl;
	std::cout << "id  assignment" << endl;
	auto assign = alg.get_assignment();
	for (auto var = assign.begin(); var != assign.end(); var++)
	{
		std::cout << var->first << "  "<< var->second<< endl;
	}

	return result == expected;
}

int main(int argc, char** argv)
{
	/*string file = "data/aim-100-1_6-no-1.cnf";
	bool expected = false;

	ifstream f(file);

	test_cnf(f, expected);*/

	command_line_controller c;

	vector<string> arg(argv, argv + argc);

	c.execute(arg);
}
