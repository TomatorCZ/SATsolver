#pragma once
#include <vector>
#include "graph.h"
#include "components.h"

class cdcl_algorithm
{
public:
	cdcl_algorithm(cnf_formula& formula) : formula_(formula) {
		variables_placement.resize(formula_.count_of_variables());
	}
private:
	class cdcl_level {
	public:
		void erase_all_assignments();
		cdcl_level(cdcl_algorithm& alg,int id, literal_observer& decision) : id_(id), decision_var(NULL,decision), alg_(alg){}
		void add_literal(const literal_observer& literal, const clause& antecedent);
		int get_id() const { return id_; }
		void add_conflict(const clause& antecedent);
		bool is_asserting_clause(const clause& c) const;
		clause resolve() const; // returns asserting clause from conflict vertex
		std::vector<std::pair<std::shared_ptr<literal_observer>, derive_vertex>>& get_derive_vars() {return derive_vars;}
	private:
		conflict_vertex conflict_var;
		std::vector<std::pair<std::shared_ptr<literal_observer>, derive_vertex>> derive_vars;
		std::pair<std::shared_ptr<literal_observer>, decision_vertex> decision_var;
		cdcl_algorithm& alg_;
		int id_;
	};

	// assign all unit clauses and writes assigned clauses into level
	//	return false if conflict was found and stores it into level
	bool unit_propagation(cdcl_algorithm::cdcl_level& current_level);

	// backtrack to level, erases all assignment made after this level 
	void backtrack(int level);

	//returns ... 
	int sec_highest_level_in_clause(const clause& c) const;

	// returns false, if there is no variable to guess.
	bool make_decision();

	const variable& get_most_recently_assigned(clause& c) const;

	std::vector<cdcl_level> levels;
	std::vector<std::pair<int,int>> variables_placement; // first is id of level, where var is and seconf is index in level, where var is  
	cnf_formula& formula_;
};