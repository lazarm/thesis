#include <bfs.h>
#include <unordered_map>

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2> &p) {
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);

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
};

template <class Iterator>
void GridGraph::insert(Iterator begin, Iterator end)
{
	tuple<int, int> dim = getGridDimension(begin, end);
	for (auto p = begin; p != end; ++p)
	{
		int xfloor = floor(p->x());
		int yfloor = floor(p->y());
		pair<int, int> cellPoint(floor(p->x()), floor(p->y()));
		if (!cells.count(cellPoint)) {
		    cells.emplace(cellPoint, vector<shared_ptr<GraphNode>>());
		}
		cells.at(cellPoint).push_back(*p);
	}
}

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
}

vector<shared_ptr<GraphNode>> getNeighbouringCells(GridGraph g, GraphNode v, Unordered_map cells)
{
	int xfloor = int(floor(v.p.x()));
	int yfloor = int(floor(v.p.y()));
	vector<pair<int, int>> cellPoints{ pair<int,int>(xfloor, yfloor) };
	vector<shared_ptr<GraphNode>> neighbourCandidates;
	vector<shared_ptr<GraphNode>> cands;
	pair<int, int> l(xfloor, yfloor);
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
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = pair<int, int>(xfloor - 1, yfloor + 1);
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	return neighbourCandidates;
}

void gridQuery(GridGraph g, shared_ptr<GraphNode> r, Unordered_map cells)
{
	r->dist = 0;
	r->visited = true;
	deque <shared_ptr<GraphNode>> Q;
	Q.push_back(r);
	while (Q.size() > 0)
	{
		shared_ptr<GraphNode> v = Q.front();
		Q.pop_front();
		vector<shared_ptr<GraphNode>> neighbours = getNeighbouringCells(g, *v, cells);
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
