#pragma once
#include <memory>

// Assignments units for literals
class variable {
public:
	variable(unsigned int id) : id_(id), assignment_(2) {}

	unsigned int get_id() const { return id_; }
	char get_assignment() const { return assignment_; }
	void set_assignment(char assignment) { assignment_ = assignment; }
private:
	unsigned int id_;
	char assignment_; // 0 - false, 1 - true, something else - undefined
};

// Abstract class of representants of variables(literals)
class literal_observer {
public:
	literal_observer(variable& var) : var_(var) {}
	variable& get_variable() const { return var_; }

	virtual ~literal_observer() {}
	virtual char get_assignment() const = 0;
	virtual std::unique_ptr<literal_observer> clone() const = 0;
	virtual void set_assignment(char assignment) = 0;
protected:
	variable& var_;
};

class positive_literal_observer : public literal_observer {
public:
	positive_literal_observer(variable& var) : literal_observer(var) {}
	char get_assignment() const override { return var_.get_assignment(); }
	virtual void set_assignment(char assignment) override { var_.set_assignment(assignment); }
	virtual std::unique_ptr<literal_observer> clone() const override { return std::make_unique<positive_literal_observer>(*this); };
};

class negative_literal_observer : public literal_observer {
public:
	negative_literal_observer(variable& var) : literal_observer(var) {}
	char get_assignment() const override; // Gets negetion of variable assignment 
	virtual void set_assignment(char assignment) override { var_.set_assignment(assignment); }
	virtual std::unique_ptr<literal_observer> clone() const override { return std::make_unique<negative_literal_observer>(*this); };
};