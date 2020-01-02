#pragma once
#include <vector>
#include "components.h"

class vertex;

class edge {
	public:
		edge(const clause& antecedent, vertex& derived_from) : derived_from_(derived_from), antecedent_(antecedent) {}
		const vertex& get_vertex_from() const { return derived_from_; }
		const clause& get_antecedent() const { return antecedent_;}
	private:
		vertex& derived_from_;
		const clause& antecedent_;
};

class vertex {
	public:
		vertex(const literal_observer& literal) : literal_(literal) {}
		const literal_observer& get_literal() const {return literal_;}
	private:
		const literal_observer& literal_;
};

class decision_vertex: public vertex {
	public:
		decision_vertex(const literal_observer& literal):vertex(literal){}
};

class derive_vertex : public vertex {
	public:
		derive_vertex(const literal_observer& literal) :vertex(literal) {}
		void add_edge(vertex& to, const clause& antecedent_);
		const clause& get_antecedent() const {return edges.back().get_antecedent();}
	private:
		std::vector<edge> edges; 
};

class conflict_vertex {
	public:
		void add_edge(vertex& to, const clause& antecedent_);
		const std::vector<edge>& get_edges() const {return edges;}
	private:
		std::vector<edge> edges;
};

