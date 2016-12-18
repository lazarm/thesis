#include "GridGraph.h"

class TreeTimeTester
{
private:
	int constructionRepeats;
	int algorithmRepeats;
	vector<Point_2> points;
	void testSSPT();
	void testBfs();
	void testBfsGrid();
public:
	TreeTimeTester(vector<Point_2> pointSet, int constructions, int treeBuilds);
	~TreeTimeTester(){};
	void test();
};


TreeTimeTester::TreeTimeTester(vector<Point_2> pointSet, int iterations, int treeBuilds) {
	constructionRepeats = iterations;
	algorithmRepeats = treeBuilds;
	points = pointSet;
}

void TreeTimeTester::test() {
	testSSPT();
	testBfs();
	testBfsGrid();
}


void TreeTimeTester::testSSPT()
{
	CGAL::Timer cost;
	double totalQueryTime = 0;
	double totalConstructionTime = 0;
	for (int k = 0; k < constructionRepeats; ++k) {
		cost.reset(); cost.start();
		SSSPTree tree(points.begin(), points.end());
		cost.stop();
		cout << "SSSP tree construction finished, dt size is " << tree.getDT().number_of_vertices() << ", time: " << cost.time() << endl;
		totalConstructionTime += cost.time();
		size_t pointsLength = points.size();
		cost.reset();
		for (int i = 0; i < algorithmRepeats; ++i) {
			int idx = ceil(rand()*pointsLength / RAND_MAX);
			Point_2 p = points[idx];
			cost.start();
			tree.createTreeFromRoot(p);
			cost.stop();

			tree.resetSSSPTreeDTVertices();
			tree.clearSets();
		}
		cout << "algorithm repeats: " << algorithmRepeats << endl;
		cout << "time in " << k << " iteration: " << cost.time() << endl;
		totalQueryTime += (cost.time() / double(algorithmRepeats));
	}
	cout << "Average time (5 iterations) for construction of DT: " << totalConstructionTime / double(constructionRepeats) << endl;
	cout << "Average time (5 iterations) for running 50 iterations of bfs algortithm on DT: " << totalQueryTime / double(constructionRepeats) << endl;
}

void TreeTimeTester::testBfs()
{
	vector<shared_ptr<GraphNode>> nodes;
	for (auto p : points) {
		nodes.push_back(shared_ptr<GraphNode>(new GraphNode(p)));
	}
	CGAL::Timer cost;
	double totalQueryTime = 0;
	double totalConstructionTime = 0;
	for (int k = 0; k < constructionRepeats; ++k) {
		cost.reset(); cost.start();
		constructG(nodes.begin(), nodes.end());
		cost.stop();
		totalConstructionTime += cost.time();

		cost.reset();
		size_t nodesLength = nodes.size();
		for (int i = 0; i < algorithmRepeats; ++i) {
			int idx = ceil(rand()*nodesLength / RAND_MAX);
			shared_ptr<GraphNode> n = nodes[idx];
			cost.start();
			runBfs(n, nodes);
			cost.stop();
			resetGraphNodes(nodes);
		}
		totalQueryTime += cost.time() / double(algorithmRepeats);
		resetGraph(nodes);
	}
	cout << "Average time (5 iterations) for construction of graph G: " << totalConstructionTime / double(constructionRepeats) << endl;
	cout << "Average time (5 iterations) for running 50 iterations of bfs algortithm on G: " << totalQueryTime / double(constructionRepeats) << endl;
}

void TreeTimeTester::testBfsGrid() {
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