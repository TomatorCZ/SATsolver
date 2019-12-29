#pragma once
#include <memory>
#include <vector>
#include "resizable_array.h"

// Assignments units for literals
class variable {
	public:
		variable() : id_(0), assignment_(2) {}
		variable(unsigned int id) : id_(id), assignment_(2) {}

		unsigned int get_id() const { return id_; }
		char get_assignment() const { return assignment_; }
		void set_assignment(char assignment) { assignment_ = assignment; }
	private:
		unsigned int id_;
		char assignment_; // 0 - false, 1 - true, something else - undefined
};

#pragma region Observers

// Abstract class of representants of variables(literals)
class literal_observer {
	public:
		literal_observer(variable& var) : var_(var) {}
		variable& get_variable() const { return var_; }

		virtual ~literal_observer() {}
		virtual char get_assignment() const = 0;
		virtual std::shared_ptr<literal_observer> clone() const = 0;
		virtual void set_assignment(char assignment) = 0;
	protected:
		variable& var_;
};

class positive_literal_observer : public literal_observer {
	public:
		positive_literal_observer(variable& var) : literal_observer(var) {}
		char get_assignment() const override { return var_.get_assignment(); }
		virtual void set_assignment(char assignment) override { var_.set_assignment(assignment); }
		virtual std::shared_ptr<literal_observer> clone() const override { return std::make_shared<positive_literal_observer>(*this); };
};

class negative_literal_observer : public literal_observer {
	public:
		negative_literal_observer(variable& var) : literal_observer(var) {}
		char get_assignment() const override; // Gets negetion of variable assignment 
		virtual void set_assignment(char assignment) override;
		virtual std::shared_ptr<literal_observer> clone() const override { return std::make_shared<negative_literal_observer>(*this); };
};

#pragma endregion

class clause {
	public:
		literal_observer& operator[](size_t index) const { return *literals_[index]; }
		literal_observer& at(size_t index) const { if (index < literals_.size()) { return (*this)[index]; } else throw; }
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
		void add_literal(literal_observer& literal) { literals_.push_back(literal.clone()); }
	private:
		std::vector<std::shared_ptr<literal_observer>> literals_;
};

class cnf_formula {
	public:
		clause& operator[](size_t index) const { return clauses_[index]; }
		clause& at(size_t index) const { if (index < clauses_.size()) { return (*this)[index]; } else throw; }
		class iterator {
		public:
			iterator(const iterator& it) : f_(it.f_), position_(it.position_) {}
			iterator(const cnf_formula& f, size_t position) : f_(f), position_(position) {}
			clause& operator*() const { return f_.at(position_); }
			bool operator!=(const iterator& it) const { return position_ != it.position_; }
			iterator operator++() { position_++; return *this; }
		private:
			const cnf_formula& f_;
			size_t position_{ 0 };
		};

		iterator begin() const { return iterator(*this, 0); }
		iterator end() const { return iterator(*this, clauses_.size()); }
		void load_formula(std::istream& input);
		void print_formula(std::ostream& output) const;
	private:
		bool starts_with(const std::string& str_to_comp, const std::string& prefix) const;
		clause read_clause(const std::string& line) const;

		std::vector<variable> variables_;
		std::vector<std::shared_ptr<literal_observer>> observers_; // negative are on odd position, positive on even positions
		resizable_array<clause> clauses_; // resizable_array because iterators, references and pointers are same
		// Constants of dimacs cnf loader
		const char comment_ = 'c';
		const std::string paragrapf_ = "p cnf";
};