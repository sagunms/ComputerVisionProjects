#include <vector>

struct node {
	node *parent;
	int i, rank;
};
	
class cUnionFind {
  private:
	std::vector<node *> nodes;
	int elements, sets;

  protected:
    
  public:
	cUnionFind();
	~cUnionFind();

	node* MakeSet(int i);
	node* Find(node* a);
	void Union(node* a0, node* a1);

	int ElementCount();
	int SetCount();

	int Reduce();
	void Reset();
};