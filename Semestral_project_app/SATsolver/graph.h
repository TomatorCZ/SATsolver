#pragma once
#include <vector>
#include "components.h"

class vertex;

class edge {
	public:
		edge(const clause& antecedent, vertex& derived_from) : derived_from_(derived_from), antecedent_(antecedent) {}
	private:
		vertex& derived_from_;
		const clause& antecedent_;
};

class vertex {
};

class decision_vertex: public vertex {
	public:
		decision_vertex(const literal_observer& literal):literal_(literal){}
	private:
		const literal_observer& literal_;
};

class derive_vertex : public vertex {
	public:
		derive_vertex(const literal_observer& literal) :literal_(literal) {}
		void add_edge(vertex& to, const clause& antecedent_);
	private:
		std::vector<edge> edges; 
		const literal_observer& literal_;
};

class conflict_vertex : public vertex {
	public:
		void add_edge(vertex& to, const clause& antecedent_);
	private:
		std::vector<edge> edges;
};

