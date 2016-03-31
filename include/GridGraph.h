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

using Unordered_map = unordered_map<pair<int, int>, vector<shared_ptr<GraphNode>>, pair_hash>;

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

pair<int, int> computeGridPoint(GraphNode v) {
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
	//tuple<int, int> dim = getGridDimension(begin, end);
	for (auto p = begin; p != end; ++p)
	{
		pair<int, int> cellPoint = computeGridPoint(**p);
		if (!cells.count(cellPoint)) {
		    cells.emplace(cellPoint, vector<shared_ptr<GraphNode>>());
		}
		cells.at(cellPoint).push_back(*p);
	}
}

/*
template <class Iterator>
tuple<int, int> getGridDimension(Iterator begin, Iterator end)
{
	double minx = numeric_limits<int>::max();
	double maxx = numeric_limits<int>::min();
	double miny = numeric_limits<int>::max();
	double maxy = numeric_limits<int>::min();

	for (auto p = begin; p != end; ++p)
	{
		double px = p->x();
		double py = p->y();
		if (px > maxx) { maxx = px; }
		if (px < minx) { minx = px; }
		if (py > maxy) { maxy = py; }
		if (py < miny) { miny = py; }
	}
	int xdim = ceil(maxx - minx);
	int ydim = ceil(maxy - miny);
	return tuple<int, int>(xdim, ydim);
}*/

vector<shared_ptr<GraphNode>> getNeighbouringCells(GridGraph g, GraphNode v)
{
	pair<int, int> gridPoint = computeGridPoint(v);
	int xfloor = gridPoint.first;
	int yfloor = gridPoint.second;
	vector<pair<int, int>> cellPoints{ gridPoint };
	vector<shared_ptr<GraphNode>> neighbourCandidates;
	vector<shared_ptr<GraphNode>> cands;
	pair<int, int> l(xfloor, yfloor);
	Unordered_map cells = g.getCells();
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int,int>(xfloor - 1, yfloor);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor + 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor + 1, yfloor);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor + 1, yfloor - 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor, yfloor - 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor - 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	return neighbourCandidates;
}

void gridQuery(GridGraph g, shared_ptr<GraphNode> r)
{
	r->dist = 0;
	r->visited = true;
	deque <shared_ptr<GraphNode>> Q;
	Q.push_back(r);
	while (Q.size() > 0)
	{
		shared_ptr<GraphNode> v = Q.front();
		Q.pop_front();
		vector<shared_ptr<GraphNode>> neighbours = getNeighbouringCells(g, *v);
		for (auto u : neighbours) {
			if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
				u->visited = true;
				u->dist = v->dist + 1;
				u->parent = v;
				Q.push_back(u);
			}
		}
	}
}

void resetGridGraphNodes(vector<shared_ptr<GraphNode>> nodes) {
	for (auto n : nodes) {
		n->dist = numeric_limits<int>::max();
		n->visited = false;
		n->parent = nullptr;
	}
}

void testBfsGrid(vector<Point_2> points) {
	vector<shared_ptr<GraphNode>> nodes;
	for (auto p : points) {
		nodes.push_back(shared_ptr<GraphNode>(new GraphNode(p)));
	}
	CGAL::Timer cost;
	double totalTime = 0;
	for (int k = 0; k < 10; k++) {
		cout << "Starting construction of grid graph." << endl;
		cost.reset(); cost.start();
		GridGraph g(nodes.begin(), nodes.end());
		for (auto a : g.getCells()) {
			cout << a.first.first << "," << a.first.second << endl;
		}
		cost.stop();
		cout << "Construction of grid graph finished." << endl;
		int nodesLength = nodes.size();
		for (int i = 0; i < 10; i++) {
			int idx = ceil(rand()*nodesLength / RAND_MAX);
			cout << "idx: " << idx << " - " << nodes[idx]->p << endl;
			shared_ptr<GraphNode> n = nodes[idx];
			cout << "grid bfs start" << endl;
			cost.start();
			gridQuery(g, n);
			cost.stop();
			cout << "grid bfs end" << endl;
			//for (auto n : nodes) {
			//	cout << (*n).p << " " << (*n).dist << endl;
			//}
			resetGraphNodes(nodes);
		}
		cout << "Time for construction of graph G and running 10 iterations of bfs algorithm: " << cost.time() << endl;
		totalTime += cost.time();
	}
	cout << "Average time for construction of graph G and running 10 iterations of bfs algorithm (repeated 10 times): " << totalTime / 10.0 << endl;
}