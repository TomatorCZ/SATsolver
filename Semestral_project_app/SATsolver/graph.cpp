#include "graph.h"
void derive_vertex::add_edge(vertex& to, const clause& antecedent_){
	edges.emplace_back(antecedent_, to);
}

void conflict_vertex::add_edge(vertex& to, const clause& antecedent_) {
	edges.emplace_back(antecedent_, to);
}