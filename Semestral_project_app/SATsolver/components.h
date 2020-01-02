#pragma once
#include <memory>
#include <vector>
#include <string>
#include "resizable_array.h"

/**
 * @brief Represents variable and assignment.
 * 
 */
class variable {
	public:
		variable() : id_(0), assignment_(2) {}
		variable(unsigned int id) : id_(id), assignment_(2) {}

		unsigned int get_id() const { return id_; }
		char get_assignment() const { return assignment_; }
		void set_assignment(char assignment) { assignment_ = assignment; }
	private:
		unsigned int id_;
		/**
		 * @brief Values: 0 - false, 1 = true, something else - undefined
		 * 
		 */
		char assignment_;
};

#pragma region Observers

/**
 * @brief Class helper for identifing positive and negative literal observer.
 * 
 */
enum lTYPE {
	positive,
	negative
};

/**
 * @brief Abstract class representants literals.
 * 
 */
class literal_observer {
	public:
		literal_observer(variable& var) : var_(var) {}
		variable& get_variable() const { return var_; }

		virtual ~literal_observer() {}
		virtual lTYPE get_type() const = 0;
		virtual char get_assignment() const = 0;
		virtual std::shared_ptr<literal_observer> clone() const = 0;
		virtual void set_assignment(char assignment) = 0;
	protected:
	/**
	 * @brief Instance of variable, which represents assignment of literal.
	 * 
	 */
		variable& var_;
};

class positive_literal_observer : public literal_observer {
	public:
		positive_literal_observer(variable& var) : literal_observer(var) {}
		char get_assignment() const override { return var_.get_assignment(); }
		virtual void set_assignment(char assignment) override { var_.set_assignment(assignment); }
		virtual std::shared_ptr<literal_observer> clone() const override { return std::make_shared<positive_literal_observer>(*this); };
		virtual lTYPE get_type() const {return lTYPE(positive);};
};

class negative_literal_observer : public literal_observer {
	public:
		negative_literal_observer(variable& var) : literal_observer(var) {}
		/**
		 * @brief Get the assignment object due to negative literal, so it is negated. 
		 * 
		 * @return char as assignment
		 */
		char get_assignment() const override;
		/**
		 * @brief Set the assignment object due to negative literal, so it is negated.
		 * 
		 * @param assignment Method interprets it as negation of asignment.
		 */
		virtual void set_assignment(char assignment) override;
		virtual std::shared_ptr<literal_observer> clone() const override { return std::make_shared<negative_literal_observer>(*this); };
		virtual lTYPE get_type() const { return lTYPE(negative); };
};

#pragma endregion

class clause {
	public:
		literal_observer& operator[](size_t index) const { return *literals_[index]; }
		literal_observer& at(size_t index) const { if (index < literals_.size()) { return (*this)[index]; } else throw; }
		/**
		 * @brief Iterator throw literals in clause.
		 * 
		 */
		class iterator {
		public:
			iterator(const iterator& it) : c_(it.c_), position_(it.position_) {}
			iterator(const clause& c, size_t position) : c_(c), position_(position) {}
			literal_observer& operator*() const { return c_.at(position_); }
			bool operator!=(const iterator& it) const { return position_ != it.position_; }
			iterator operator++() { position_++; return *this; }
		private:
			const clause& c_;
			size_t position_{ 0 };
		};
		iterator begin() const { return iterator(*this, 0); }
		iterator end() const { return iterator(*this, literals_.size()); }
		size_t size() const { return literals_.size(); }
		void print_clause(std::ostream& output) const;
		bool constains(const literal_observer& literal) const; 
		void add_literal(const literal_observer& literal) { literals_.push_back(literal.clone()); }
	private:
		class ftor_is_item {
		public:
			ftor_is_item(const literal_observer& literal) :literal_(literal){}
			bool operator() (const std::shared_ptr<literal_observer>& x);
		private:
			const literal_observer& literal_;
		};

		std::vector<std::shared_ptr<literal_observer>> literals_;
};

class cnf_formula {
	public:
		clause& operator[](size_t index) const { return clauses_[index]; }
		clause& at(size_t index) const { if (index < clauses_.size()) { return (*this)[index]; } else throw; }
		/**
		 * @brief Iterator throw clauses in cnf formula.
		 * 
		 */
		class clause_iterator {
		public:
			clause_iterator(const clause_iterator& it) : f_(it.f_), position_(it.position_) {}
			clause_iterator(const cnf_formula& f, size_t position) : f_(f), position_(position) {}
			clause& operator*() const { return f_.at(position_); }
			bool operator!=(const clause_iterator& it) const { return position_ != it.position_; }
			clause_iterator operator++() { position_++; return *this; }
		private:
			const cnf_formula& f_;
			size_t position_{ 0 };
		};
		clause_iterator begin() const { return clause_iterator(*this, 0); }
		clause_iterator end() const { return clause_iterator(*this, clauses_.size()); }

		/**
		 * @brief Loads formula to intern containers.
		 * 
		 * @param input Dimacs format of cnf formula.
		 */
		void load_formula(std::istream& input);

		void print_formula(std::ostream& output) const;

		/**
		 * @brief Gets count of variables constained in cnf formula.
		 * 
		 * @return size_t 
		 */
		size_t count_of_variables() const {return variables_.size();}

		/**
		 * @brief Checks, if clause is unit clause. 
		 * 
		 * @return literal_observer* returns NULL, if not, else literal, which is unassigned.
		 */
		literal_observer* is_unit_clause(const clause&) const;

		/**
		 * @brief Seeks, if formula contains conflict clause.
		 * 
		 * @return clause* returns NULL, if not, else clause, which is conflicting.
		 */
		clause* seek_conflict() const; 

		/**
		 * @brief Checks, if clause is conflict clause. 
		 * 
		 */
		bool is_conflict_clause(const clause& c) const;

		/**
		 * @brief Finds first unsigned var.
		 * 
		 * @return literal_observer* NULL if no vars is unassigned 
		 */
		literal_observer* find_first_unsigned_var() const;

		const variable& get_variable( int id) const {return  variables_[id-1];}

		/**
		 * @brief Get the observer object.
		 * 
		 * @param variable_id 
		 * @param positive true returns positive literal, else negative
		 * @return const literal_observer& 
		 */
		const literal_observer& get_observer(int variable_id, bool positive) const ;

		void add_clause(const clause& c) {clauses_.push_back(c) ;}
	private:
		/**
		 * @brief Checks of str_to_comp constains prefix. 
		 * 
		 */
		bool starts_with(const std::string& str_to_comp, const std::string& prefix) const;

		/**
		 * @brief Parse clause from line in dimacs format.
		 * 
		 */
		clause read_clause(const std::string& line) const;

		std::vector<variable> variables_;

		/**
		 * @brief Negative literals are on odd position, positives on even positions.
		 * 
		 */
		std::vector<std::shared_ptr<literal_observer>> observers_; 

		/**
		 * @brief iterators, references and pointers safe usage
		 * 
		 */
		resizable_array<clause> clauses_; 

		const char comment_ = 'c';
		const std::string paragrapf_ = "p cnf";
};