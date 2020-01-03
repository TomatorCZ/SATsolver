#pragma once
#include <vector>
#include <iostream>
#include "graph.h"
#include "components.h"

class cdcl_algorithm
{
public:
	cdcl_algorithm(cnf_formula& formula, std::ostream& log, bool able_log) : formula_(formula), log_(log), able_log_(able_log){
		variables_placement.resize(formula_.count_of_variables());
	}
	/**
	 * @brief Resolve formula satisfiability
	 * 
	 */
	bool make();

	/**
	 * @brief Gets the assignment of formula. It is meaningful if formula is satisfiable
	 * 
	 * @return std::vector<std::pair<int, int>> Where first is id of variable and second is assignment
	 */
	std::vector<std::pair<int, int>> get_assignment() const;
private:
	class cdcl_level {
	public:
		/**
		 * @brief Decesions and propagation made in this level will be erased.
		 * 
		 */
		void erase_all_assignments();
		cdcl_level(cdcl_algorithm& alg,int id, const literal_observer& decision) : id_(id), decision_var(decision.clone(),decision), alg_(alg){}
		void add_literal(const literal_observer& literal, const clause& antecedent);
		int get_id() const { return id_; }
		void add_conflict(const clause& antecedent);
		bool is_asserting_clause(const clause& c) const;
		/**
		 * @brief finds unsatisfiable clause in formula.
		 * 
		 */
		clause find_unsat_clause() const;

		/**
		 * @brief Makes resolvent from clauses c1 and c2
		 * 
		 * @param v is in c2
		 * @return clause resolvent of c2 and c1
		 */
		clause resolve_clauses(const clause& c1, const clause& c2, const literal_observer& v) const; 

		std::vector<std::pair<std::shared_ptr<literal_observer>, derive_vertex>>& get_derive_vars() {return derive_vars;}
	private:
		conflict_vertex conflict_var;
		std::vector<std::pair<std::shared_ptr<literal_observer>, derive_vertex>> derive_vars;
		std::pair<std::shared_ptr<literal_observer>, decision_vertex> decision_var;
		cdcl_algorithm& alg_;
		int id_;
	};

	/**
	 * @brief Assigns all unit clauses and writes assigned clauses into level
	 * 
	 * @return false if conflict was found and stores it into level
	 */
	bool unit_propagation(cdcl_algorithm::cdcl_level& current_level);

	/**
	 * @brief Backtracks to level, erases all assignment made after this level 
	 * 
	 */
	void backtrack(int level);

	/**
	 * @brief Returns variable, which is in second highest level in clause.
	 * 
	 */
	int sec_highest_level_in_clause(const clause& c) const;

	/**
	 * @brief Make new decision and makes new decision level.
	 * 
	 * @return true 
	 * @return false if there is no variable to guess.
	 */
	bool make_decision();

	/**
	 * @brief Learn new clause and find level to backtrack.
	 * 
	 * @return int level to backtrack
	 */
	int analyze_conflict();

	/**
	 * @brief Checks if there is conflict in formula and adds it into current level
	 * 
	 * @return false  if there is conflict.
	 */
	bool check_conflict_help(cdcl_algorithm::cdcl_level& current_level);

	const variable& get_most_recently_assigned(clause& c) const;

	int current_level;
	std::vector<cdcl_level> levels;
	/**
	 * @brief First is id of level, where var is and seconf is index in level, where var is.
	 * 
	 */
	std::vector<std::pair<int,int>> variables_placement;
	cnf_formula& formula_;
	std::ostream& log_;
	bool able_log_;
};
