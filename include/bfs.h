#include <Optimization.h>

class GraphNode
{
public:
	Point_2 p;
	vector<int> neighbours;
	int dist;
	bool visited;
	shared_ptr<GraphNode> parent;
	
	GraphNode(double x, double y) {
		p = Point_2(x, y);
		dist = (numeric_limits<int>::max)();
		visited = false;
		parent = nullptr;
	}

	GraphNode(Point_2 ps) { 
		p = ps;
		dist = (numeric_limits<int>::max)();
		visited = false;
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
	//cout << "constructG k: " << k << endl;
}

void runBfs(shared_ptr<GraphNode> s, vector<shared_ptr<GraphNode>> nodes)
{
	s->dist = 0;
	s->visited = true;
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
			if (!(u->visited))
			{
				k++;
				u->visited = true;
				u->dist = v->dist+1;
				u->parent = v;
				Q.push_back(u);
			}
		}
	}
	//cout << "visited points: " << k << endl;
}

void resetGraphNodes(vector<shared_ptr<GraphNode>> nodes) {
	for (auto n : nodes) {
		n->dist = (numeric_limits<int>::max)();
		n->visited = false;
		n->parent = nullptr;
	}
}

void resetGraph(vector<shared_ptr<GraphNode>> nodes) {
	for (auto n : nodes) {
		n->neighbours.clear();
	}
}

void testBfs(vector<Point_2> points)
{
	vector<shared_ptr<GraphNode>> nodes;
	for (auto p: points) {
		nodes.push_back(shared_ptr<GraphNode>(new GraphNode (p)));
	}
	CGAL::Timer cost;
	double totalQueryTime = 0;
	double totalConstructionTime = 0;
	for (int k = 0; k < 5; ++k) {
		//cout << "Starting construction of graph G." << endl;
		cost.reset(); cost.start();
		constructG(nodes.begin(), nodes.end());
		cost.stop();
		totalConstructionTime += cost.time();
		
		//cout << "Construction of graph G finished in " << cost.time() << endl;
		cost.reset();
		size_t nodesLength = nodes.size();
		for (int i = 0; i < 1; ++i) {
			int idx = ceil(rand()*nodesLength / RAND_MAX);
			//cout << "idx: " << idx << endl;
			shared_ptr<GraphNode> n = nodes[idx];
			cost.start();
			runBfs(n, nodes);
			cost.stop();
			//cout << cost.time() << endl;
			//for (auto n : nodes) {
			//	cout << (*n).p << " " << (*n).dist << endl;
			//}
			resetGraphNodes(nodes);
		}
		//cout << "Average time (50 iterations) of bfs algorithm on G: " << cost.time()/50.0 << endl;
		totalQueryTime += cost.time()/50.0;
		resetGraph(nodes);
	}
	cout << "Average time (5 iterations) for construction of graph G: " << totalConstructionTime/5.0 << endl;
	cout << "Average time (5 iterations) for running 50 iterations of bfs algortithm on G: " << totalQueryTime / 2.0 << endl;
}