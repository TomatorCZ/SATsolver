#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <typeinfo>
#include "components.h"

using namespace std;

char negative_literal_observer::get_assignment() const {
	if (var_.get_assignment() == 0)
		return 1;
	if (var_.get_assignment() == 1)
		return 0;
	else
		return var_.get_assignment();
}

void negative_literal_observer::set_assignment(char assignment) {
	if (assignment == 0)
		var_.set_assignment(1);
	if (assignment == 1)
		var_.set_assignment(0);
	else
		var_.set_assignment(assignment);
}

void cnf_formula::print_formula(std::ostream& output) const {
	for (auto c = clauses_.begin(); c != clauses_.end(); ++c)
	{
		for (auto l = (*c).begin(); l != (*c).end(); ++l)
		{
			if (typeid(*l) == typeid(negative_literal_observer))
				output << '-';
			output << (*l).get_variable().get_id() << ' ';
		}
		output << '0' << endl;
	}
}

void cnf_formula::load_formula(istream& input) {

	string line;
	bool start_read_clauses = false;
	unsigned int variables;
	unsigned int clauses = 0;

	while (clauses > 0 || !start_read_clauses)
	{
		getline(input, line);

		if (input.fail())
			throw;

		if (line[0] == comment_) // comment line
			continue;

		if (starts_with(line, paragrapf_)) // info line
		{
			stringstream ss(line.c_str() + paragrapf_.size()); // skip nonumbers
			ss >> variables;
			ss >> clauses;
			start_read_clauses = true;
			
			// load variables and observers
			variables_.resize(variables);
			for (unsigned int i = 1; i <= variables; i++)
			{ 
				variables_[i-1] = variable(i);
				observers_.push_back(make_shared<positive_literal_observer>(variables_[i - 1]));
				observers_.push_back(make_shared<negative_literal_observer>(variables_[i - 1]));
			}
			continue;
		}

		if (start_read_clauses) // clause line
		{
			clauses_.push_back(read_clause(line));
			clauses--;
		}
		else
			throw exception("bad format");
	}
}

bool cnf_formula::starts_with(const std::string& str_to_comp, const std::string& prefix) const {
	if (str_to_comp.size() < prefix.size())
		return false;

	int i = 0;
	while (str_to_comp[i] == prefix[i] && i < prefix.size())
		i++;

	return i == prefix.size();
}

clause cnf_formula::read_clause(const std::string& line) const {
	clause result;
	int variable;
	stringstream ss(line);

	while (true)
	{
		ss >> variable;
		if (ss.fail() || variable == 0)
			break;

		if (variable > (int)variables_.size() || -variable > (int)variables_.size())
			throw exception("unknown variable");

		if (variable < 0)
			result.add_literal(*observers_[(-variable - 1)*2 +1]);
		else
			result.add_literal(*observers_[(variable - 1) * 2]);
	}

	return result;
}