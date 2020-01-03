#include <typeinfo>
#include <vector>
#include "components.h"
#include "cdcl_algorithm.h"

using namespace std;

vector<std::pair<int, int>> cdcl_algorithm::get_assignment() const{
	vector<pair<int, int>> assignment;

	for (size_t i = 1; i <= formula_.count_of_variables(); i++)
		assignment.push_back({(int)i,(int)formula_.get_variable(i).get_assignment()});

	return assignment;
}

bool cdcl_algorithm::make() {

	cdcl_level zero_lvl(*this, 0, formula_.get_observer(1,true));
	levels.push_back(zero_lvl);

	if (!unit_propagation(zero_lvl))
		return false;

	while (true)
	{
		if (!make_decision())
			return true;

		if (!unit_propagation(levels.back()))
		{
			bool backtracked = false;
			while (!backtracked)
			{
				int level = analyze_conflict();
				if (level < 0)
					return false;

				backtrack(level);

				backtracked = unit_propagation( levels.back());
			}
		}
	}
}

clause cdcl_algorithm::cdcl_level::find_unsat_clause() const{
	clause result;
	
	// negation of literals
	for (auto edge = conflict_var.get_edges().begin(); edge != conflict_var.get_edges().end(); edge++)
	{
		int id = edge->get_vertex_from().get_literal().get_variable().get_id();
		bool positive = edge->get_vertex_from().get_literal().get_type() != lTYPE(0);
		result.add_literal(alg_.formula_.get_observer(id, positive));
	}
	
	return result;
}

int cdcl_algorithm::analyze_conflict() {
	if(able_log_)
		log_ << "Analyzing conflict in lvl " << current_level << endl;

	if (current_level == 0)
		return -1;

	clause c = levels.back().find_unsat_clause();
	while (!levels.back().is_asserting_clause(c))
	{
		if (able_log_)
			log_ << "Analyzing conflict : making resolution" << current_level << endl;

		const variable& v = get_most_recently_assigned(c);
		int index = variables_placement[v.get_id() - 1].second;
		const clause& antecedent_of_v = levels.back().get_derive_vars()[index].second.get_antecedent();	
		c = levels.back().resolve_clauses(c, antecedent_of_v, *levels.back().get_derive_vars()[variables_placement[v.get_id() - 1].second].first);
	}

	// add learnt clause
	formula_.add_clause(c);
	
	if (able_log_) { 
		log_ << "Learnt clause : ";
		c.print_clause(log_);
	}

	return sec_highest_level_in_clause(c);
}

clause cdcl_algorithm::cdcl_level::resolve_clauses(const clause& c1, const clause& c2, const literal_observer& v) const{
	clause result;
	
	for (auto l = c2.begin(); l != c2.end(); ++l)
	{
		if ((*l).get_type() != (v).get_type() || (*l).get_variable().get_id() != v.get_variable().get_id())
			result.add_literal((*l));
	}

	for (auto l = c1.begin(); l != c1.end(); ++l)
	{
		if ((*l).get_type() == (v).get_type() || (*l).get_variable().get_id() != v.get_variable().get_id())
		{
			if (!result.constains(*l))
				result.add_literal((*l));
		}
	}

	return result;
}

bool cdcl_algorithm::cdcl_level::is_asserting_clause(const clause& c) const {
	
	bool occurence = false;
	for (auto l = c.begin(); l != c.end(); ++l)
	{
		int a = (*l).get_variable().get_id();
		if (alg_.variables_placement[(*l).get_variable().get_id() - 1].first == id_)
		{	
			if (occurence)
				return false;
			occurence = true;
			continue;
		}
	}
	return true;
}

const variable& cdcl_algorithm::get_most_recently_assigned(clause& c) const {
	int index = (*c.begin()).get_variable().get_id() - 1;

	for (auto  l = c.begin(); l != c.end(); ++l)
	{
		int l_index = (*l).get_variable().get_id() - 1;

		if (variables_placement[l_index].first > variables_placement[index].first ||
			(variables_placement[l_index].first == variables_placement[index].first && variables_placement[l_index].second > variables_placement[index].second))
		{
			index = l_index;
		}
	}

	return formula_.get_variable(index + 1);
}

int cdcl_algorithm::sec_highest_level_in_clause(const clause& c) const
{
	if (c.size() == 1)
		return 0;

	int sec_highest = 0;

	for (auto l = c.begin(); l != c.end(); ++l)
	{
		int lvl = variables_placement[(*l).get_variable().get_id() - 1].first;
		
		if (lvl > sec_highest && lvl < levels.size() -1)
			sec_highest = lvl;
	}
	return sec_highest;
}

void cdcl_algorithm::backtrack(int level) {
	
	if (able_log_)
		log_ << "Backtrack to lvl " << level << endl;
	
	// delete assignments and level
	while (level + 1< levels.size())
	{
		levels.back().erase_all_assignments();
		levels.pop_back();		
	}

	current_level = levels.size() - 1;
}

void cdcl_algorithm::cdcl_level::erase_all_assignments() {
	for (auto vars = derive_vars.begin(); vars != derive_vars.end(); vars++)
		(*vars).first->get_variable().set_assignment(2);
	
	decision_var.first->set_assignment(2);
}

bool cdcl_algorithm::make_decision()
{
	if (able_log_)
		log_ << "Making decision"<< endl;

	literal_observer* unsigned_var = formula_.find_first_unsigned_var();
	if (unsigned_var != NULL)
	{
		if (able_log_)
			log_ << "Making decision: in lvl " << levels.size() << endl;

		unsigned_var->set_assignment(1);
		variables_placement[unsigned_var->get_variable().get_id()-1].first = levels.size();
		variables_placement[unsigned_var->get_variable().get_id() - 1].second = -1; // -1 means that variable is decision in decision level.
		levels.emplace_back(*this, levels.size(), *unsigned_var);// + 1 because decision level starts on 1 by convention.
		current_level++;
		return true;
	}

	if (able_log_)
		log_ << "Making decision: no vars found " << endl;

	clause* c = formula_.seek_conflict();
	return false;
}

bool cdcl_algorithm::check_conflict_help(cdcl_algorithm::cdcl_level& current_level) {
	clause* conflict = formula_.seek_conflict();

	if (conflict != NULL)
	{
		if (able_log_){
			log_ << "Unit propagation: conflict detected: ";
			conflict->print_clause(log_);
		}

		if (levels.size() != 0)
			current_level.add_conflict(*conflict);
		return false;
	}
	return true;
}

bool cdcl_algorithm::unit_propagation(cdcl_algorithm::cdcl_level& current_level) {

	if (able_log_)
		log_ << "Unit propagation in decision lvl " << current_level.get_id() << endl;

	literal_observer* unit = NULL;

	do // iterates until there is not any unit clause
	{
		unit = NULL;
		for (auto c = formula_.begin(); c != formula_.end(); ++c)
		{
			unit = formula_.is_unit_clause(*c);

			if (unit != NULL) // Assign variable on true
			{
				if (typeid(*unit) == typeid(negative_literal_observer))
					unit->get_variable().set_assignment(0);
				else
					unit->get_variable().set_assignment(1);
				
				if (able_log_)
					log_ << "Unit propagation: adding variable: " << (*unit).get_variable().get_id() << endl;

				current_level.add_literal(*unit, *c); // add variable to current level

				//check conflict
				if (!check_conflict_help(current_level))
					return false;
			}
		}
	} while (unit != NULL);

	return check_conflict_help(current_level);
}

void cdcl_algorithm::cdcl_level::add_literal(const literal_observer& literal, const clause& antecedent){
	alg_.variables_placement[literal.get_variable().get_id() - 1] = pair<int, int>(id_,derive_vars.size());
	derive_vertex v(literal);

	for (auto l = antecedent.begin(); l != antecedent.end(); ++l)
	{
		if (literal.get_variable().get_id() != (*l).get_variable().get_id())
		{ 
			int level_of_var = alg_.variables_placement[(*l).get_variable().get_id() - 1].first;
			int index_in_level = alg_.variables_placement[(*l).get_variable().get_id() - 1].second;

			if (index_in_level == -1)
				v.add_edge(alg_.levels[level_of_var].decision_var.second, antecedent);
			else
				v.add_edge(alg_.levels[level_of_var].get_derive_vars()[index_in_level].second, antecedent);
		}
	}
	derive_vars.emplace_back(literal.clone(),v);
}

void cdcl_algorithm::cdcl_level::add_conflict(const clause& antecedent) {
	if (alg_.able_log_)
		alg_.log_ << "Unit propagation: adding conflict to lvl " << id_ << endl;
	for (auto l = antecedent.begin(); l != antecedent.end(); ++l)
	{
		int level_of_var = alg_.variables_placement[(*l).get_variable().get_id() - 1].first;
		int index_in_level = alg_.variables_placement[(*l).get_variable().get_id() - 1].second;

		if (index_in_level == -1)
			conflict_var.add_edge(alg_.levels[level_of_var].decision_var.second, antecedent);
		else
			conflict_var.add_edge(alg_.levels[level_of_var].get_derive_vars()[index_in_level].second, antecedent);	
	}
}