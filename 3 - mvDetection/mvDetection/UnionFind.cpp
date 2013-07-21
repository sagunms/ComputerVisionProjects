#include "UnionFind.h"

cUnionFind::cUnionFind() : elements(0), sets(0) {
}

cUnionFind::~cUnionFind() {
	for (int i = 0; i < nodes.size(); i++) delete nodes[i];
	nodes.clear();
}

node* cUnionFind::MakeSet(int i) {
	if (elements + 1 > nodes.size()) nodes.push_back(new node);

	nodes[elements]->parent = nodes[elements];
	nodes[elements]->i = i;
	nodes[elements]->rank = 0;

	elements++;
	sets++;

	return nodes[elements-1];
}

node* cUnionFind::Find(node* a) {			// with path compression
	if (a->parent == a) return a;
	else {
		a->parent = Find(a->parent);
		return a->parent;
	}
}

void cUnionFind::Union(node* a0, node* a1) {		// union by rank
	if (a0 == a1) return;

	node *a2 = Find(a0);
	node *a3 = Find(a1);

	if (a2 == a3) return;

	if      (a2->rank < a3->rank) a2->parent = a3;
	else if (a3->rank < a2->rank) a3->parent = a2;
	else {
		a2->parent = a3;
		a3->rank++;
	}

	sets--;
}

int cUnionFind::ElementCount() {
	return elements;
}

int cUnionFind::SetCount() {
	return sets;
}

int cUnionFind::Reduce() {
	int j = 0;
	for (int i = 0; i < elements; i++)
		if (nodes[i]->parent == nodes[i])
			nodes[i]->i = j++;
	return j;
}

void cUnionFind::Reset() {
	elements = sets = 0;
}