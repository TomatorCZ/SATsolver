#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <typeinfo>
#include <algorithm>
#include "components.h"

using namespace std;

const literal_observer& cnf_formula::get_observer(int variable_id, bool positive) const {
	if (positive)
		return *observers_[(variable_id - 1) * 2];
	else
		return *observers_[(variable_id - 1) * 2 + 1];
}

bool clause::ftor_is_item::operator()(const shared_ptr<literal_observer>& x){
	return (*x).get_type() == (literal_).get_type() && x->get_variable().get_id() == literal_.get_variable().get_id();
}

bool clause::constains(const literal_observer& literal) const{
	return find_if(literals_.begin(),literals_.end(),ftor_is_item(literal)) != literals_.end();
}

char negative_literal_observer::get_assignment() const {
	if (var_.get_assignment() == 0)
		return 1;
	if (var_.get_assignment() == 1)
		return 0;
	else
		return var_.get_assignment();
}

literal_observer* cnf_formula::find_first_unsigned_var() const {
	for (size_t i = 0; i < observers_.size(); i+=2)
	{
		if (observers_[i]->get_assignment() != 0 && observers_[i]->get_assignment() != 1)
		{
			return &*observers_[i];
		}
	}
	return NULL;
}

void negative_literal_observer::set_assignment(char assignment) {
	if (assignment == 0)
		var_.set_assignment(1);
	if (assignment == 1)
		var_.set_assignment(0);
	else
		var_.set_assignment(assignment);
}

bool cnf_formula::is_conflict_clause(const clause& c) const {

	for (auto l = c.begin(); l != c.end(); ++l)
	{
		if ((*l).get_assignment() != 0)
			return false;
	}

	return true;
}

clause* cnf_formula::seek_conflict() const {
	for (auto c = clauses_.begin(); c != clauses_.end(); ++c)
	{
		if (is_conflict_clause(*c))
			return &*c;
	}
	return NULL;
}

literal_observer* cnf_formula::is_unit_clause(const clause& c) const {
	literal_observer* result = NULL;

	for (auto l = c.begin(); l != c.end(); ++l) // iterates all literals and seek unsigned literal
	{
		if ((*l).get_assignment() == 1)
			return NULL;
		if (((*l).get_assignment() != 0))
		{
			if (result == NULL)
				result = &*l;
			else
				return NULL;
		}
	}

	return result;
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