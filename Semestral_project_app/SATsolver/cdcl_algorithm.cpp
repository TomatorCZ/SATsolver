#include "cdcl_algorithm.h"

using namespace std;

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

				// check conflict
				clause* conflict = formula_.seek_conflict();

				if (conflict != NULL)
				{
				//	implication_graph.add_conflict(*conflict);
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