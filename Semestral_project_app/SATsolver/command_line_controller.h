#pragma once
#include <string>
#include <iostream>
#include <vector>

class command_line_controller
{
	public:
		void execute(const std::vector<std::string>& cmd);
	private:
		void help_cmd(std::ostream& output) const;
		void file_cmd(const std::string& input, bool log, bool assignment) const;
		const std::string cmd_help = "help";
		const std::string cmd_file = "file";
		const std::string opt_log = "-l";
		const std::string opt_assignment = "-a";
};

