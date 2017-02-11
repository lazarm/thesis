#include <Optimization.h>

class GraphNode
{
public:
	Point_2 p;
	vector<int> neighbours;
	shared_ptr<GraphNode> parent;
	
	GraphNode(double x, double y) {
		p = Point_2(x, y);
		parent = nullptr;
	}

	GraphNode(Point_2 ps) { 
		p = ps;
		parent = nullptr;
	}
};

template <class Iterator>
void constructG(Iterator begin, Iterator end)
{
	int k = 0;
	for (Iterator it = begin; it != end-1; ++it)
	{
		for (Iterator it2 = it+1; it2 != end; ++it2)
		{
			if (CGAL::squared_distance((**it).p, (**it2).p) <= 1)
			{
				k++;
				(**it).neighbours.push_back(it2- begin);
				(**it2).neighbours.push_back(it - begin);
			}
		}
	}
}

void runBfs(shared_ptr<GraphNode> s, vector<shared_ptr<GraphNode>> nodes)
{
	s->p.dist = 0;
	s->p.visited = true;
	deque <shared_ptr<GraphNode>> Q;
	Q.push_back(s);
	int k = 0;
	while (Q.size() > 0)
	{
		shared_ptr<GraphNode> v = Q.front();
		Q.pop_front();
		for (auto it = v->neighbours.begin(); it != v->neighbours.end(); ++it)
		{
			shared_ptr<GraphNode> u = nodes.at(*it);
			if (!(u->p.visited))
			{
				k++;
				u->p.visited = true;
				u->p.dist = v->p.dist+1;
				u->parent = v;
				Q.push_back(u);
			}
		}
	}
}

void resetGraphNodes(vector<shared_ptr<GraphNode>> nodes) {
	for (auto n : nodes) {
		n->p.dist = (numeric_limits<int>::max)();
		n->p.visited = false;
		n->parent = nullptr;
	}
}

void resetGraph(vector<shared_ptr<GraphNode>> nodes) {
	for (auto n : nodes) {
		n->neighbours.clear();
	}
}
