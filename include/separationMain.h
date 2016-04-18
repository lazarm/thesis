#include <rangeTree.h>


template <class Iterator>
tuple<Point_2, Point_2, int> findBest(Iterator begin, Iterator end)
{
	int min_weight = numeric_limits<int>::max();
	tuple<Point_2, Point_2, int> bestPair;
	for (auto tup = begin; tup != end; ++tup) {
		if (get<2>(*tup) < min_weight) {
			bestPair = *tup;
		}
	}
	return bestPair;
}

vector<Point_2> make_cycle(tuple<Point_2, Point_2, int> pair, vector<Point_2> cycle)
{
	// p and q are points to be connected and form a cycle. ps and qs are paths from p to r and from q to r, respectively. 
	Point_2 p = get<0>(pair);
	Point_2 q = get<1>(pair);
	vector<Point_2> ps{ p };
	if (p.getParent() != 0) {
		Point_2 pi = p;
		while (pi.getParent() != 0) {
			pi = *(pi.getParent());
			ps.push_back(pi);
		}
	}
	vector<Point_2> qs{ q };
	if (q.getParent() != 0) {
		Point_2 qi = q;
		while (qi.getParent() != 0) {
			qi = *(qi.getParent());
			qs.push_back(qi);
		}
	}
	//cout << "ps " << ps.size() << endl;
	//cout << "qs " << qs.size() << endl;
	//cout << p.getParent()->x() << endl;
	//cout << p.getParent()->y() << endl;

	cycle.insert(cycle.end(), qs.rbegin(), qs.rend());
	cycle.insert(cycle.end(), ps.begin(), ps.end() - 1);
	return cycle;
}


void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> cycle;
	double totalTime = 0;
	SSSPTree ssspTree(begin, end);
	int k = 0;
	for (auto p = begin; p != end; ++p)
	{
		CGAL::Timer cost;
		cost.reset(); cost.start();
		ssspTree.createTreeFromRoot(*p, st);
		vector< vector<Point_2> > rst = ssspTree.getAllSets();
		int ii = 0;

		//cout << rst.at(0).size() << ", " << rst.at(1).size() << ", " << rst.at(2).size() << ", " << rst.at(3).size() << endl;
		tuple<Point_2, Point_2, int> bestR = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
		tuple<Point_2, Point_2, int> r0rangeTree = findminpairRi(rst.at(0), rst.at(1), rst.at(2).begin(), rst.at(2).end(), st, bestR);
		tuple<Point_2, Point_2, int> r1rangeTree = findminpairRi(rst.at(1), rst.at(0), rst.at(3).begin(), rst.at(3).end(), st, r0rangeTree);
		tuple<Point_2, Point_2, int>  l0l1 = findMinPair(rst.at(0).begin(), rst.at(0).end(), rst.at(1).begin(), rst.at(1).end(), r1rangeTree);
		tuple<Point_2, Point_2, int>  r0r1 = findMinPair(rst.at(2).begin(), rst.at(2).end(), rst.at(3).begin(), rst.at(3).end(), l0l1);
		tuple<Point_2, Point_2, int> result = r0r1;
		//cout << "this best: " << get<2>(result) << endl;
		//cout << "current best: " << get<2>(best_r) << endl;
		if (get<2>(result) < get<2>(best_r)) {
			best_r = result;
			cycle.clear();
			//cout << get<0>(best_r) << "  -   " << get<1>(best_r) << " , dist= " << get<2>(best_r) << endl;
			//cout << *(get<0>(best_r).getParent()) << "  -  " << *(get<1>(best_r).getParent()) << endl;
			//cout << get<0>(best_r).x() << "," << get<1>(best_r).x() << endl;
			//cout << get<0>(best_r).y() << "," << get<1>(best_r).y() << endl;
			cout << "len: " << get<2>(best_r) << endl;
			//cout << *p << endl;
			shared_ptr<Point_2> pp = get<0>(best_r).getParent();
			//cout << pp->getParent() << endl;
			cycle = make_cycle(result, cycle);
			//cout << cycle.size() << endl;
		}
		ssspTree.resetSSSPTreeDTVertices();
		ssspTree.clearSets();
		cost.stop();
		totalTime += cost.time();
	}
	cout << "time: " << totalTime << endl;
	cout << get<2>(best_r) << endl;
	if (get<2>(best_r) == (numeric_limits<int>::max)()) {
		cout << "Could not separate points " << st.source().x() << "," << st.source().y() << " and " << st.end().x() << "," << st.end().y()
			<< " with this set of unit disks." << endl;
	}
	else {
		cout << "Points " << st.source().x() << "," << st.source().y() << " and " << st.end().x() << "," << st.end().y()
			<< " can be separated by connecting points " << get<0>(best_r).x() << "," << get<0>(best_r).y() << " and "
			<< get<1>(best_r).x() << "," << get<1>(best_r).y() << " and thus forming a following cycle: " << endl;

		for (auto c : cycle) {
			cout << c << endl;
		}
		cout << "Number of points in cycle: " << cycle.size() << endl;
		cout << "Number of points in cycle: " << get<2>(best_r) << endl;
	}
}


void testSeparationBruteForce(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st) {
	SSSPTree ssspTree(begin, end);
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	int k = 0;
	double totalTime = 0;
	vector<Point_2> cycle;
	for (auto p = begin; p != end; ++p)
	{
		CGAL::Timer cost;
		cost.reset(); cost.start();
		ssspTree.createTreeFromRoot(*p, st);
		vector< vector<Point_2> > rst = ssspTree.getAllSets();
		cout << "lens:" << endl;
		tuple<Point_2, Point_2, int> best_r1 = bruteForce(rst.at(0).begin(), rst.at(0).end(), rst.at(1).begin(), rst.at(1).end(), best_r);
		cout << get<2>(best_r1) << endl;
		tuple<Point_2, Point_2, int> best_r2 = bruteForce(rst.at(2).begin(), rst.at(2).end(), rst.at(3).begin(), rst.at(3).end(), best_r1);
		cout << get<2>(best_r2) << endl;
		tuple<Point_2, Point_2, int> best_r3 = bruteForceSegment(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), st, best_r2);
		cout << get<2>(best_r3) << endl;
		tuple<Point_2, Point_2, int> result = bruteForceSegment(rst.at(1).begin(), rst.at(1).end(), rst.at(2).begin(), rst.at(2).end(), st, best_r3);
		cout << get<2>(result) << endl;
		if (get<2>(result) < get<2>(best_r)) {
			best_r = result;
			cycle.clear();
			cout << "len: " << get<2>(best_r) << endl;
			cycle = make_cycle(result, cycle);
		}
		ssspTree.resetSSSPTreeDTVertices();
		ssspTree.clearSets();
		cost.stop();
		totalTime += cost.time();
	}
	cout << "time: " << totalTime << endl;
	cout << get<2>(best_r) << endl;
	if (get<2>(best_r) == (numeric_limits<int>::max)()) {
		cout << "Could not separate points " << st.source().x() << "," << st.source().y() << " and " << st.end().x() << "," << st.end().y()
			<< " with this set of unit disks." << endl;
	}
	else {
		cout << "Points " << st.source().x() << "," << st.source().y() << " and " << st.end().x() << "," << st.end().y()
			<< " can be separated by connecting points " << get<0>(best_r).x() << "," << get<0>(best_r).y() << " and "
			<< get<1>(best_r).x() << "," << get<1>(best_r).y() << " and thus forming a following cycle: " << endl;

		for (auto c : cycle) {
			cout << c << endl;
		}
		cout << "Number of points in cycle: " << cycle.size() << endl;
		cout << "Number of points in cycle: " << get<2>(best_r) << endl;
	}
}

struct GNode {
public:
	Point_2 *v;
	vector<int> neighbours;
	GNode(Point_2* u) {
		v = u;
		//cout << "add: " << &u << endl;
	}
};


void constructGSeparation(vector<shared_ptr<GNode>> nodes)
{
	int k = 0;
	for (int i = 0; i < nodes.size() - 1; i++)
	{
		Point_2 *v = nodes[i]->v;
		for (int j = i + 1; j < nodes.size(); j++)
		{
			Point_2 *u = nodes[j]->v;
			//cout << v->point << " --- " << u->point() << endl;
			if (CGAL::squared_distance(*v, *u) <= 1)
			{
				k++;
				nodes[i]->neighbours.push_back(j);
				//nodes[j]->neighbours.push_back(i);
				cout << i << " -> " << j << endl;
			}
		}
	}
	//cout << "constructG k: " << k << endl;
}

void testSepGeneral(vector<Point_2> points, Segment_2 st) {
	SSSPTree tree(points.begin(), points.end());
	// we need to iterate through points again, this time for all points associated with dt vertices, because
	vector<shared_ptr<GNode>> nodes;
	DT dt = tree.getDT();
	int c = 0;
	cout << &dt << endl;
	for (auto p = dt.points_begin(); p != dt.points_end(); ++p) {
		//cout << "v: " << p->point() << endl;
		cout << p.ptr() << endl;
		nodes.push_back(shared_ptr<GNode>(new GNode(p.ptr())));
	}
	constructGSeparation(nodes);
	int best = (numeric_limits<int>::max)();
	CGAL::Timer cost;
	cost.reset();
	double ttime = 0;
	for (auto p : nodes) {
		tree.createTreeFromRoot(*(p->v), st);
		cost.start();
		dt = tree.getDT();
		cost.stop();
		ttime += cost.time();
		for (auto s = dt.points_begin(); s != dt.points_end(); ++s) {
			cout << s->getDist() << endl;
		}
		for (auto a : nodes) {
			if (a == p) { continue; }
			Point_2 *ap = a->v;
			cout << *ap << endl;
			cout << ap->getDist() << endl;
			for (auto b : a->neighbours) {
				if (nodes[b] == p) { continue; }
				Point_2 *bp = nodes[b]->v;
				//cout << bp << endl;
				cout << ap->getNr() << " - " << bp->getNr() << endl;
				if (ap->getNr() + bp->getNr() + updateNr(0, *ap, *bp, st) % 2 == 1) {
					if (ap->getDist() + bp->getDist() < best) {
						best = ap->getDist() + bp->getDist();
						cout << "best: " << *ap << "  -.-   " << *bp << " , r: " << *(p->v) <<endl;
					}
				}
			}
		}
		/*for (auto v = dt.points_begin(); v != dt.points_end(); ++v) {
			v->setDist((std::numeric_limits<int>::max)());
			v->setVisited(false);
		}*/
		tree.resetSSSPTreeDTVertices();
	}
	cout << best << endl;
	cout << "time6: " << ttime << "  , time1200: " << ttime * 200 << endl;
}