#include <bfs.h>
#include <unordered_map>
#include <chrono>
#include <thread>

struct pair_hash {
	template <class T1, class T2>
	size_t operator () (const pair<T1, T2> &p) {
		auto h1 = hash<T1>{}(p.first);
		auto h2 = hash<T2>{}(p.second);
		// use cantor pairing function to hash a pair of integers
		return (h1 + h2)*(h1 + h2 + 1) / 2 + h2;
	}
};

using NodePointer = shared_ptr<GraphNode>;
using nodePointerList = list<NodePointer>;
using intPair = pair<int, int>;
using Unordered_map = unordered_map<intPair, nodePointerList, pair_hash>;

class GridGraph
{
private:
	Unordered_map cells;
public:
	GridGraph(){};
	~GridGraph(){};
	template <class Iterator>
	GridGraph(Iterator begin, Iterator end);
	Unordered_map getCells() { return cells; }
};

intPair computeGridPoint(GraphNode v) {
	int xfloor, yfloor;
	// it's safe to cast to double because we're doing floor operation
	// and point coordinates read from input file are rational numbers
	if (v.p.x() < 0) { xfloor = int(CGAL::to_double(-(-v.p.x() + 1))); }
	else { xfloor = int(CGAL::to_double(v.p.x())); }
	if (v.p.y() < 0) { yfloor = int(CGAL::to_double(-(-v.p.y() + 1))); }
	else { yfloor = int(CGAL::to_double(v.p.y())); }
	pair<int, int> gridPoint(xfloor, yfloor);
	return gridPoint;
}

template <class Iterator>
GridGraph::GridGraph(Iterator begin, Iterator end)
{
	for (auto p = begin; p != end; ++p)
	{
		pair<int, int> cellPoint = computeGridPoint(**p);
		if (!cells.count(cellPoint)) {
		    cells.emplace(cellPoint, nodePointerList());
		}
		cells.at(cellPoint).push_back(*p);
	}
}

void checkNeighbours(NodePointer v, deque<NodePointer>* Q, intPair cellOffset, GridGraph g)
{
	intPair gridPoint = computeGridPoint(*v);
	intPair l(gridPoint.first + cellOffset.first, gridPoint.second + cellOffset.second);
	if (g.getCells().count(l)) {
		nodePointerList cands = g.getCells().at(l);
		for (auto u : cands) {
			if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
				u->visited = true;
				u->dist = v->dist + 1;
				u->parent = v;
				Q->push_back(u);
			}
		}
	}
}

double gridQuery(GridGraph g, NodePointer r)
{
	CGAL::Timer cost;
	cost.start();
	r->dist = 0;
	r->visited = true;
	Unordered_map gCells = g.getCells();
	deque <NodePointer> Q;
	vector<intPair> neighbourCellOffsets{ intPair(0, 0), intPair(-1, 0),
		intPair(-1, 1), intPair(0, 1),
		intPair(1, 1), intPair(1, 0),
		intPair(1, -1), intPair(0, -1), intPair(-1, -1)
	};
	Q.push_back(r);
	intPair gridPoint;
	intPair oldGridPoint = intPair(-1, -1);
	intPair l;
	int ka = 0;
	while (Q.size() > 0)
	{
		shared_ptr<GraphNode> v = Q.front();
		Q.pop_front();
		gridPoint = computeGridPoint(*v);
		if (true) {
			for (auto cellOffset : neighbourCellOffsets) {
				l = intPair(gridPoint.first + cellOffset.first, gridPoint.second + cellOffset.second);
				Unordered_map::iterator it = gCells.find(l);
				nodePointerList neighbours{};
				if (it != gCells.end()) {
					for (auto u : it->second) {
						if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
							u->visited = true;
							u->dist = v->dist + 1;
							u->parent = v;
							Q.push_back(u);
							it->second.remove(u);
						}
					}
				}
			}
		}
		oldGridPoint = gridPoint;
	}
	return cost.time();
}

void resetGridGraphNodes(nodePointerList nodes) {
	for (auto n : nodes) {
		n->dist = (numeric_limits<int>::max)();
		n->visited = false;
		n->parent = nullptr;
	}
}

void testBfsGrid(vector<Point_2> points) {
	nodePointerList nodes;
	for (auto p : points) {
		nodes.push_back(NodePointer(new GraphNode(p)));
	}
	CGAL::Timer cost;
	size_t nodesLength = nodes.size();
	double testTime = 0;
	for (int i = 0; i < 50; i++) {
		int idx = ceil(rand()*nodesLength / RAND_MAX);
		nodePointerList::iterator it = nodes.begin();
		advance(it, idx);
		NodePointer n = *it;
		cost.reset(); cost.start();
		GridGraph g(nodes.begin(), nodes.end());
		cost.stop();
		double queryTime = gridQuery(g, n);
		testTime += cost.time() + queryTime;
		resetGridGraphNodes(nodes);
	}
	cout << "Average time (5 iterations) for running 50 iterations of bfs algorithm on grid graph: " << testTime / 50.0 << endl;
}