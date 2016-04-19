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
using nodePointerList = vector<NodePointer>;
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
	//tuple<int, int> dim = getGridDimension(begin, end);
	for (auto p = begin; p != end; ++p)
	{
		pair<int, int> cellPoint = computeGridPoint(**p);
		if (!cells.count(cellPoint)) {
		    cells.emplace(cellPoint, nodePointerList());
		}
		cells.at(cellPoint).push_back(*p);
	}
}

nodePointerList getNeighbouringCells(GridGraph g, GraphNode v)
{
	intPair gridPoint = computeGridPoint(v);
	int xfloor = gridPoint.first;
	int yfloor = gridPoint.second;
	vector<intPair> cellPoints{ gridPoint };
	nodePointerList neighbourCandidates;
	nodePointerList cands;
	intPair l(xfloor, yfloor);
	Unordered_map cells = g.getCells();
	if (cells.count(l)) {
		cands = cells.at(l);
		neighbourCandidates.insert(neighbourCandidates.end(), cands.begin(), cands.end());
	}
	l = intPair(xfloor - 1, yfloor);
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
	//return Q;
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
	vector<NodePointer> neighbours{};
	while (Q.size() > 0)
	{
		shared_ptr<GraphNode> v = Q.front();
		Q.pop_front();
		// looping through neighbours vector is done twice. Dont do that
		// move for block to separate function and call it 9 times
		//cost.stop();
		//cost.start();
		gridPoint = computeGridPoint(*v);
		if (gridPoint != oldGridPoint) {
			neighbours.clear();
			for (auto cellOffset : neighbourCellOffsets) {
				l = intPair(gridPoint.first + cellOffset.first, gridPoint.second + cellOffset.second);
				Unordered_map::iterator it = gCells.find(l);
				if (it != gCells.end()) {
					for (auto u : it->second) {
						if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
							u->visited = true;
							u->dist = v->dist + 1;
							u->parent = v;
							Q.push_back(u);
						}
					}
					neighbours.insert(neighbours.end(), it->second.begin(), it->second.end());
				}
			}
		}
		else {
			for (auto u : neighbours) {
				if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
					u->visited = true;
					u->dist = v->dist + 1;
					u->parent = v;
					Q.push_back(u);
				}
			}
		}
		oldGridPoint = gridPoint;
		
		/*
		for (auto cellOffset : neighbourCellOffsets) {
			l = intPair(gridPoint.first + cellOffset.first, gridPoint.second + cellOffset.second);
			Unordered_map::iterator it = gCells.find(l);
			if (it != gCells.end()) {
				nodePointerList neighbours = it->second;
				for (auto u : neighbours) {
					if (CGAL::squared_distance(u->p, v->p) <= 1 && !u->visited) {
						u->visited = true;
						u->dist = v->dist + 1;
						u->parent = v;
						Q.push_back(u);
					}
				}
			}
		}*/
		/*
		for (auto offset : neighbourCellOffsets) {
		    checkNeighbours(v, &Q, offset, g);
		}*/
		//cout << "q size: " << Q.size() << endl;
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
	double totalQueryTime = 0;
	double totalConstructionTime = 0;
	for (int k = 0; k < 2; k++) {
		cost.reset(); cost.start();
		//usage = getUsage(&prevProcKernel, &prevProcUser, true);
		GridGraph g(nodes.begin(), nodes.end());
		//usage = getUsage(&prevProcKernel, &prevProcUser);
		cost.stop();
		double testTime = cost.time();
		totalConstructionTime += cost.time();
		//cout << "Construction of grid graph finished in: " << cost.time() << endl;
		//cout << "Construction of grid graph (usage) finished in: " << usage << endl;
		cost.reset();
		size_t nodesLength = nodes.size();
		for (int i = 0; i < 50; i++) {
			int idx = ceil(rand()*nodesLength / RAND_MAX);
			//cout << "idx: " << idx << " - " << nodes[idx]->p << endl;
			NodePointer n = nodes[idx];
			//cout << "grid bfs start" << endl;
			//usage = getUsage(&prevProcKernel, &prevProcUser, true);
			double queryTime = gridQuery(g, n);
			//usage = getUsage(&prevProcKernel, &prevProcUser);
			testTime += queryTime;
			//cout << "query time: " << queryTime << endl;
			//cout << "query time (usage): " << usage << endl;
			//cost.start();
			//gridQuery(g, n);
			//cost.stop();
			//cout << "grid bfs end" << endl;
			//for (auto n : nodes) {
			//	cout << (*n).p << " " << (*n).dist << endl;
			//}
			resetGraphNodes(nodes);
		}
		//cout << "Average time (50 iterations) of bfs algorithm on grid graph: " << testTime/50.0 << endl;
		totalQueryTime += testTime/50.0;
	}
	cout << "Average time (5 iterations) for construction of grid graph: " << totalConstructionTime / 2.0 << endl;
	cout << "Average time (5 iterations) for running 50 iterations of bfs algorithm on grid graph: " << totalQueryTime / 2.0 << endl;
}