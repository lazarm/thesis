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
	void insert(Iterator begin, Iterator end);
	Unordered_map getCells() { return cells; }
};

template <class Iterator>
void GridGraph::insert(Iterator begin, Iterator end)
{
	//tuple<int, int> dim = getGridDimension(begin, end);
	for (auto p = begin; p != end; ++p)
	{
		pair<int, int> cellPoint(floor((*p)->p.x()), floor((*p)->p.y()));
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
	int xfloor = int(floor(v.p.x()));
	int yfloor = int(floor(v.p.y()));
	vector<pair<int, int>> cellPoints{ pair<int,int>(xfloor, yfloor) };
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
		n->neighbours.clear();
	}
}

void gridGraphTest(vector<Point_2> points) {
	vector<shared_ptr<GraphNode>> nodes;
	for (auto p : points) {
		nodes.push_back(shared_ptr<GraphNode>(new GraphNode(p.x(), p.y())));
	}
	GridGraph g;
	g.insert(nodes.begin(), nodes.end());
	for (auto a : g.getCells()) {
		cout << a.first.first << " , " << a.first.second  << " - " << a.second.size() << endl;
	}
	for (auto r : nodes) {
		gridQuery(g, r);
		for (auto n : nodes) {
			cout << (*n).p << " " << (*n).dist << endl;
		}
		resetGridGraphNodes(nodes);
		this_thread::sleep_for(chrono::seconds(5));
	}
}