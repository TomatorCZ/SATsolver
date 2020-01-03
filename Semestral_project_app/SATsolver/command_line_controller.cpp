#include <string>
#include <fstream>
#include <algorithm>
#include "command_line_controller.h"
#include "components.h"
#include "cdcl_algorithm.h"

using namespace std;

void command_line_controller::execute(const std::vector<std::string>& cmd) {
	if (cmd.size() == 1)
	{
		cout << "wrong command";
		return;
	}

	if (cmd[1] == cmd_help)
	{
		if (cmd.size() != 2)
		{ 
			cout << "wrong command";
			return;
		}
		help_cmd(cout);
		return;
	}

	if (cmd[1] == cmd_file)
	{
		bool log = false;
		bool assignment = false;

		if (cmd.size() > 5 &&  3 < cmd.size())
		{
			cout << "missing file path";
			return;
		}

		if (cmd.size() > 3)
		{
			if (cmd[3] == opt_log)
				log = true;
			else if (cmd[3] == opt_assignment)
				assignment = true;
			else
			{
				cout << "unknown option";
				return;
			}
		}

		if (cmd.size() > 4)
		{
			if (cmd[4] == opt_log)
				log = true;
			else if (cmd[4] == opt_assignment)
				assignment = true;
			else
			{
				cout << "unknown option";
				return;
			}
		}
		file_cmd(cmd[2],log,assignment);
		return;
	}

}

void command_line_controller::help_cmd(std::ostream& output) const {
	output << "help = gives list of commands" << endl;
	output << "file <path to dimacs format file> = load dimacs format and returns result" << endl;
	output << "options:" << endl;
	output << "-l = writes logs of solver process" << endl;
	output << "-a = writes assignment if formula will be satisfiable" << endl;
}

void command_line_controller::file_cmd(const string& path, bool log, bool assignment) const {
	ifstream input (path);

	cnf_formula formula;
	formula.load_formula(input);

	cdcl_algorithm alg(formula,cout,log);

	bool sat = alg.make();
	if (sat)
		cout << "SATISFIABLE" << endl;
	else
		cout << "UNSATISFIABLE" << endl;

	if (assignment && sat)
	{
		std::cout << "Assignment:" << endl;
		std::cout << "id   assignment" << endl;
		auto assign = alg.get_assignment();
		for (auto var = assign.begin(); var != assign.end(); var++)
		{
			std::cout << var->first << "   " << var->second << endl;
		}
	}
}
