#include <rangeTree.h>


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

	cycle.insert(cycle.end(), qs.rbegin(), qs.rend());
	cycle.insert(cycle.end(), ps.begin(), ps.end() - 1);
	return cycle;
}

tuple<Point_2, Point_2, int> querySetOnTree(vector<Point_2> l, RangeTree* rangeTree0, RangeTree* rangeTree1, 
	Segment_2 st, tuple<Point_2, Point_2, int> bestR)
{
	int minWeight = get<2>(bestR);
	for (auto a = l.begin(); a != l.end(); ++a)
	{
		vector<Point_2> queryResults;
		int weight_a = (*a).getDist();
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		rangeTree_query(rangeTree0, a_dual, back_inserter(queryResults));
		rangeTree_query_complement(rangeTree1, a_dual, back_inserter(queryResults));
		delete(a_dual);
		for (vector<Point_2>::iterator pointB = queryResults.begin(); pointB != queryResults.end(); ++pointB)
		{
			if (weight_a + pointB->getDist() < minWeight) {
				minWeight = weight_a + pointB->getDist();
				bestR = make_tuple(*a, *pointB, minWeight);
				return bestR;
			}
		}
	}
	return bestR;
}

tuple<Point_2, Point_2, int> findminpairTree(vector<Point_2> l0i1, vector<Point_2> l1i1, vector<Point_2> l0i, vector<Point_2> l1i, 
	vector<Point_2> r0i, vector<Point_2> r1i, Segment_2 st, tuple<Point_2, Point_2, int> bestR)
{
	if (l0i.size() + l1i.size() + l0i1.size() + l1i1.size() == 0) { return bestR; }
	int minWeight = get<2>(bestR);
	RangeTree tree0 = buildRangeTree(r0i.begin(), r0i.end(), st);
	RangeTree tree1 = buildRangeTree(r1i.begin(), r1i.end(), st);

	tuple<Point_2, Point_2, int> q1 = querySetOnTree(l0i1, &tree0, &tree1, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }
	q1 = querySetOnTree(l1i1, &tree1, &tree0, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }
	q1 = querySetOnTree(l0i, &tree0, &tree1, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }
	querySetOnTree(l1i, &tree1, &tree0, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }

	return bestR;
}

tuple<Point_2, Point_2, int> rangeSeparation(vector<vector<Point_2>>* l0, vector<vector<Point_2>>* l1,
	vector<vector<Point_2>>* r0, vector<vector<Point_2>>* r1, int i, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> l0i = (*l0)[i];
	vector<Point_2> l1i = (*l1)[i];
	vector<Point_2> l1i1 = (*l1)[i - 1];
	vector<Point_2> l0i1 = (*l0)[i - 1];
	vector<Point_2> r0i = (*r0)[i];
	vector<Point_2> r1i = (*r1)[i];
	vector<Point_2> r1i1 = (*r1)[i - 1];
	vector<Point_2> r0i1 = (*r0)[i - 1];
	tuple<Point_2, Point_2, int> res1 = findminpairRi(l0i, l1i, r0i1.begin(), r0i1.end(), st, best_r);
	if (get<2>(res1) < get<2>(best_r)) {
		return res1;
	}
	res1 = findminpairRi(l1i, l0i, r1i1.begin(), r1i1.end(), st, best_r);
	if (get<2>(res1) < get<2>(best_r)) {
		return res1;
	}
	if (r0i.size() == 0 && r1i.size() > 0) {
		res1 = findminpairRi(l1i1, l0i1, r1i.begin(), r1i.end(), st, best_r);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
		res1 = findminpairRi(l1i, l0i, r1i.begin(), r1i.end(), st, best_r);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
	}
	else if (r0i.size() > 0 && r1i.size() == 0) {
		res1 = findminpairRi(l0i1, l1i1, r0i.begin(), r0i.end(), st, best_r);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
		res1 = findminpairRi(l0i, l1i, r0i.begin(), r0i.end(), st, best_r);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
	}
	else if (r0i.size() > 0 && r1i.size() > 0) {
		res1 = findminpairTree(l0i1, l1i1, l0i, l1i, r0i, r1i, st, best_r);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
	}

	return best_r;
}

tuple<Point_2, Point_2, int> nnSeparation(vector<vector<Point_2>>* l0, vector<vector<Point_2>>* l1,
	vector<vector<Point_2>>* r0, vector<vector<Point_2>>* r1, int i)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> l0i = (*l0)[i];
	vector<Point_2> l1i = (*l1)[i];
	vector<Point_2> l1i1 = (*l1)[i - 1];
	vector<Point_2> l0i1 = (*l0)[i - 1];
	vector<Point_2> r0i = (*r0)[i];
	vector<Point_2> r1i = (*r1)[i];
	vector<Point_2> r1i1 = (*r1)[i - 1];
	vector<Point_2> r0i1 = (*r0)[i - 1];
	// skrajsaj tako, da uporabis zgornje vektorje direkt v groups tabeli
	vector<vector<Point_2>> groups{ l0i1, l1i, l0i, l1i1, r0i1, r1i, r0i, r1i1 };
	vector<tuple<int, int>> pairs{ tuple<int, int>(0, 1), tuple<int, int>(2, 3), tuple<int, int>(4, 5),
		tuple<int, int>(6, 7), tuple<int, int>(1, 2), tuple<int, int>(5, 6) };
	for (auto pair : pairs) {
		vector<Point_2> p0 = groups[get<0>(pair)];
		vector<Point_2> p1 = groups[get<1>(pair)];
		if (p1.size() > 0 && p0.size() > 0) {
			KDTree kdtree(p1.begin(), p1.end());
			for (auto pla : p0) {
				tuple<bool, Point_2> result = kdtree.kd_query(pla);
				if (get<0>(result)) {
					Point_2 plb = get<1>(result);
					best_r = tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
					return best_r;
				}
			}
		}
	}
	return best_r;
}

void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> cycle;
	double totalTime = 0;
	SSSPTree ssspTree(begin, end);
	cout << ssspTree.getDT().number_of_vertices() << endl;
	int k = 0;
	for (auto p = begin; p != end; ++p)
	{
		CGAL::Timer cost;
		cost.reset(); cost.start();
		ssspTree.createTreeFromRoot(*p, st);
		vector< vector<vector<Point_2>> > rst = ssspTree.getAllSets();
		vector<vector<Point_2>> l0 = rst[0]; vector<vector<Point_2>> l1 = rst[1];
		vector<vector<Point_2>> r0 = rst[2]; vector<vector<Point_2>> r1 = rst[3];
		int i = 1;
		size_t maxdist = l0.size();

		while (2 * i <= min(get<2>(best_r), maxdist*2-1)) {
			tuple<Point_2, Point_2, int> res1 = nnSeparation(&l0, &l1, &r0, &r1, i);
			if (get<2>(res1) < get<2>(best_r)) {
				best_r = res1;
				break;
			}
			res1 = rangeSeparation(&l0, &l1, &r0, &r1, i, st);
			if (get<2>(res1) < get<2>(best_r)) {
				best_r = res1;
				break;
			}
			i++;
		}

		ssspTree.resetSSSPTreeDTVertices();
		ssspTree.clearSets();
		cost.stop();
		totalTime += cost.time();
	}
	if (get<2>(best_r) < (numeric_limits<int>::max)()) {
		cout << "len: " << get<2>(best_r) << endl;
		cycle = make_cycle(best_r, cycle);
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
	}
}

struct GNode {
public:
	Point_2 v;
	vector<int> neighbours;
	GNode(Point_2 u) {
		v = u;
	}
};


void constructGSeparation(vector<shared_ptr<GNode>> nodes)
{
	int k = 0;
	for (int i = 0; i < nodes.size() - 1; i++)
	{
		Point_2 v = nodes[i]->v;
		for (int j = i + 1; j < nodes.size(); j++)
		{
			Point_2 u = nodes[j]->v;
			if (CGAL::squared_distance(v, u) <= 1)
			{
				k++;
				nodes[i]->neighbours.push_back(j);
			}
		}
	}
}

void testSepGeneral(vector<Point_2> points, Segment_2 st) {
	CGAL::Timer cost;
	cost.reset(); cost.start();
	SSSPTree tree(points.begin(), points.end());
	// we need to iterate through points again, this time for all points associated with dt vertices, because
	vector<shared_ptr<GNode>> nodes;
	vector<Point_2> dtPoints;
	DT dt = tree.getDT();
	int c = 0;
	for (auto p : points) {
		nodes.push_back(shared_ptr<GNode>(new GNode(p)));
	}
	constructGSeparation(nodes);
	int negh_sub = 0;
	int best = (numeric_limits<int>::max)();
	double ttime = 0;
	int k = 0;
	tuple<Point_2, Point_2, int> connects;
	for (auto p : nodes) {
		k++;
		tree.createTreeFromRoot(p->v, st);
		cost.stop();
		dt = tree.getDT();
		// get dt points, exclude from time measuring
		for (auto g : nodes) {
			Locate_type loc;
			int li;
			Delaunay_face_handle h = dt.locate(g->v, loc, li);
			Delaunay_vertex_handle rVertex;
			if (loc == DT::VERTEX) {
				rVertex = h->vertex(li);
				dtPoints.push_back(rVertex->point());
			}
		}
		// end
		cost.start();

		int j = 0;
		for (auto ap : dtPoints) {
			shared_ptr<GNode> jnode = nodes[j];
			for (auto b : jnode->neighbours) {
				Point_2 bp = dtPoints[b];
				if (ap.getNr() + bp.getNr() + updateNr(0, ap, bp, st) % 2 == 1) {
					if (ap.getVisited() && bp.getVisited() && ap.getDist() + bp.getDist() < best) {
						best = ap.getDist() + bp.getDist();
						connects = tuple<Point_2, Point_2, int>(ap, bp,best);
					}
				}
			}
			j++;
		}
		tree.resetSSSPTreeDTVertices();
		cost.stop();
		dtPoints.clear();
		cost.start();
	}
	cost.stop();
	vector<Point_2> cycle{};
	cycle = make_cycle(connects, cycle);
	for (auto c : cycle) {
		cout << c << endl;
	}
	cout << "Number of points in cycle: " << cycle.size() << endl;
	cout << best << endl;
	cout << "total time: " << cost.time() << endl;
}