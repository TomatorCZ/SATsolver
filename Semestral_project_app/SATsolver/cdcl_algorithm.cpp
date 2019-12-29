#include "cdcl_algorithm.h"

using namespace std;

clause cdcl_algorithm::cdcl_level::resolve() const{
	//TODO
}

bool cdcl_algorithm::cdcl_level::is_asserting_clause(const clause& c) const {
	
	bool occurence = false;
	for (auto l = c.begin(); l != c.end(); ++l)
	{
		if (alg_.variables_placement[(*l).get_variable().get_id()].first == id_)
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

	return formula_.get_variable(index);
}

int cdcl_algorithm::sec_highest_level_in_clause(const clause& c) const
{
	if (c.size() == 1)
		return 0;

	int sec_highest = 0;

	for (auto l = c.begin(); l != c.end(); ++l)
	{
		int lvl = variables_placement[(*l).get_variable().get_id()].first;
		
		if (lvl > sec_highest && lvl < levels.size())
		{
			sec_highest = lvl;
		}
	}
	return sec_highest;
}

void cdcl_algorithm::backtrack(int level) {
	// delete assignments and level
	while (level > levels.size())
	{
		levels.back().erase_all_assignments();
		levels.pop_back();
	}
}

void cdcl_algorithm::cdcl_level::erase_all_assignments() {
	for (auto vars = derive_vars.begin(); vars != derive_vars.end(); vars++)
		(*vars).first->get_variable().set_assignment(2);
	
	decision_var.first->set_assignment(2);
}

bool cdcl_algorithm::make_decision()
{
	literal_observer* unsigned_var = formula_.find_first_unsigned_var();
	if (unsigned_var != NULL)
	{
		unsigned_var->set_assignment(1);
		levels.emplace_back(*this, levels.size(), *unsigned_var);
		return true;
	}
	return false;
}

bool cdcl_algorithm::unit_propagation(cdcl_algorithm::cdcl_level& current_level) {
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
				

				current_level.add_literal(*unit, *c); // add variable to current level

				 //check conflict
				clause* conflict = formula_.seek_conflict();

				if (conflict != NULL)
				{
					current_level.add_conflict(*conflict);
					return false;
				}
			}
		}
	} while (unit != NULL);

	return true;
}

void cdcl_algorithm::cdcl_level::add_literal(const literal_observer& literal, const clause& antecedent){
	alg_.variables_placement[literal.get_variable().get_id()] = pair<int, int>(id_,derive_vars.size());
	derive_vertex v(literal);

	for (auto l = antecedent.begin(); l != antecedent.end(); ++l)
	{
		if (literal.get_variable().get_id() != (*l).get_variable().get_id())
			v.add_edge(alg_.levels[alg_.variables_placement[(*l).get_variable().get_id() - 1].first].get_derive_vars()[alg_.variables_placement[(*l).get_variable().get_id() - 1].second].second, antecedent);
	}
	derive_vars.emplace_back(literal.clone(),v);
}

void cdcl_algorithm::cdcl_level::add_conflict(const clause& antecedent) {
	for (auto l = antecedent.begin(); l != antecedent.end(); ++l)
	{
		conflict_var.add_edge(alg_.levels[alg_.variables_placement[(*l).get_variable().get_id() - 1].first].get_derive_vars()[alg_.variables_placement[(*l).get_variable().get_id() - 1].second].second, antecedent);
	}
}