#include <rangeTree.h>

struct GraphNode
{
	Point_2 p;
	vector<GraphNode*> neighbours;
	int dist;
	bool visited;
	
	GraphNode(Point_2 ps) { 
		p = ps;
		dist = numeric_limits<int>::max();
		visited = false;
	}
};

template <class Iterator>
void constructG(Iterator begin, Iterator end)
{
	for (Iterator it = begin; it != end-1; ++it)
	{
		for (Iterator it2 = it+1; it2 != end; ++it2)
		{
			if (CGAL::squared_distance((*it)->p, (*it2)->p) <= 1)
			{
				(*it)->neighbours.push_back(*it2);
				(*it2)->neighbours.push_back(*it);
			}
		}
	}
}

void runBfs(GraphNode* s)
{
	(*s).dist = 0;
	s->visited = true;
	deque <GraphNode*> Q;
	Q.push_back(s);
	while (Q.size() > 0)
	{
		GraphNode* v = Q.front();
		Q.pop_front();
		for (vector<GraphNode*>::iterator it = v->neighbours.begin(); it != v->neighbours.end(); ++it)
		{
			if (!((*it)->visited))
			{
				(*it)->visited = true;
				(*it)->dist = v->dist+1;
				Q.push_back((*it));
			}
		}
	}
}
