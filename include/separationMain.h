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

tuple<Point_2, Point_2, int> findminpairRiWithTree(vector<Point_2> li, vector<Point_2> lj, Segment_2 st,
	tuple<Point_2, Point_2, int> bestR, RangeTree* rangeTreei)
{
	int minWeight = get<2>(bestR);
	for (auto a = li.begin(); a != li.end(); ++a)
	{
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		tuple<bool, Point_2> res = rangeTree_query(rangeTreei, a_dual);
		delete(a_dual);
		Point_2 nearest = get<1>(res);
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
	}

	for (auto a = lj.begin(); a != lj.end(); ++a)
	{
		int weight_a = (*a).getDist();
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		tuple<bool, Point_2> res = rangeTree_query_complement(rangeTreei, a_dual);
		delete(a_dual);

		Point_2 nearest = get<1>(res);
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
	}
	return bestR;
}

tuple<Point_2, Point_2, int> querySetOnTree(vector<Point_2> l, RangeTree* rangeTree0, RangeTree* rangeTree1, 
	Segment_2 st, tuple<Point_2, Point_2, int> bestR)
{
	int minWeight = get<2>(bestR);
	for (auto a = l.begin(); a != l.end(); ++a)
	{
		int weight_a = (*a).getDist();
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		tuple<bool, Point_2> res = rangeTree_query(rangeTree0, a_dual);
		Point_2 nearest = get<1>(res);
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
		// najprej preveri rezultate iz queryResults, preden naredis query complement
		res = rangeTree_query_complement(rangeTree1, a_dual);
		delete(a_dual);
		nearest = get<1>(res);
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
	}
	return bestR;
}

tuple<bool, Point_2, Point_2> queryKdTree(vector<Point_2>* p0, vector<Point_2>* p1) {
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	if (p1->size() > 0 && p0->size() > 0) {
		KDTree kdtree(p1->begin(), p1->end());
		for (auto pla : *p0) {
			tuple<bool, Point_2> result = kdtree.kd_query(pla);
			if (get<0>(result)) {
				Point_2 plb = get<1>(result);
				return tuple<bool, Point_2, Point_2>(true, pla, plb);
			}
		}
	}
	return tuple<bool, Point_2, Point_2>(false, Point_2(), Point_2());
}

tuple<Point_2, Point_2, int> findminpairTree(vector<Point_2> l0i1, vector<Point_2> l1i1, vector<Point_2> l0i, vector<Point_2> l1i, 
	vector<Point_2> r0i, vector<Point_2> r1i, Segment_2 st, tuple<Point_2, Point_2, int> bestR)
{
	if (l0i.size() + l1i.size() + l0i1.size() + l1i1.size() == 0) { 
		tuple<bool, Point_2, Point_2> nnRes = queryKdTree(&l1i, &l0i);
		if (!get<0>(nnRes)) {
			nnRes = queryKdTree(&r1i, &r0i);
		}
		if (get<0>(nnRes)) {
			Point_2 pla = get<1>(nnRes);
			Point_2 plb = get<2>(nnRes);
			return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
		}
		return bestR; 
	}
	int minWeight = get<2>(bestR);
	RangeTree tree0 = buildRangeTree(r0i.begin(), r0i.end(), st);
	RangeTree tree1 = buildRangeTree(r1i.begin(), r1i.end(), st);

	tuple<Point_2, Point_2, int> q1 = querySetOnTree(l0i1, &tree0, &tree1, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }
	q1 = querySetOnTree(l1i1, &tree1, &tree0, st, bestR);
	if (get<2>(q1) < minWeight) { return q1; }
	// najprej ostala dva para kd
	tuple<bool, Point_2, Point_2> nnRes = queryKdTree(&l1i, &l0i);
	if (!get<0>(nnRes)) {
		nnRes = queryKdTree(&r1i, &r0i);
	}
	if (get<0>(nnRes)) {
		Point_2 pla = get<1>(nnRes);
		Point_2 plb = get<2>(nnRes);
		return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
	}
	// continue
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
	// L0i x R0i-1, L1i x R0i-1
	tuple<Point_2, Point_2, int> res1 = findminpairRi(l0i, l1i, r0i1.begin(), r0i1.end(), st, best_r);
	if (get<2>(res1) < get<2>(best_r)) {
		return res1;
	}
	// L1i x R1i-1, L0i x R1i-1
	res1 = findminpairRi(l1i, l0i, r1i1.begin(), r1i1.end(), st, best_r);
	if (get<2>(res1) < get<2>(best_r)) {
		return res1;
	}
	if (r0i.size() == 0 && r1i.size() > 0 && (l0i.size() + l1i.size() + l0i1.size() + l1i1.size() > 0)) {
		// L1i-1 x R1i, L0i-1 x R1i
		RangeTree tree1 = buildRangeTree(r1i.begin(), r1i.end(), st);
		res1 = findminpairRiWithTree(l1i1, l0i1, st, best_r, &tree1);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
		// najprej ostala dva para kd
		tuple<bool, Point_2, Point_2> nnRes = queryKdTree(&l1i, &l0i);
		if (!get<0>(nnRes)) {
			nnRes = queryKdTree(&r1i, &r0i);
		}
		if (get<0>(nnRes)) {
			Point_2 pla = get<1>(nnRes);
			Point_2 plb = get<2>(nnRes);
			return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
		}
		// drevo r1i daj gor, zgradi, ce vsaj eden on l1i, l0i, l1i1, l0i1 ni prazen 
		res1 = findminpairRiWithTree(l1i, l0i, st, best_r, &tree1);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
	}
	else if (r0i.size() > 0 && r1i.size() == 0 && (l0i.size() + l1i.size() + l0i1.size() + l1i1.size() > 0)) {
		RangeTree tree1 = buildRangeTree(r0i.begin(), r0i.end(), st);
		res1 = findminpairRiWithTree(l0i1, l1i1, st, best_r, &tree1);
		if (get<2>(res1) < get<2>(best_r)) {
			return res1;
		}
		// najprej ostala dva para kd
		tuple<bool, Point_2, Point_2> nnRes = queryKdTree(&l1i, &l0i);
		if (!get<0>(nnRes)) {
			nnRes = queryKdTree(&r1i, &r0i);
		}
		if (get<0>(nnRes)) {
			Point_2 pla = get<1>(nnRes);
			Point_2 plb = get<2>(nnRes);
			return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
		}
		// drevo r0i daj gor, zgradi, ce vsaj eden on l1i, l0i, l1i1, l0i1 ni prazen 
		res1 = findminpairRiWithTree(l0i, l1i, st, best_r, &tree1);
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
	else {
		tuple<bool, Point_2, Point_2> nnRes = queryKdTree(&l1i, &l0i);
		if (!get<0>(nnRes)) {
			nnRes = queryKdTree(&r1i, &r0i);
		}
		if (get<0>(nnRes)) {
			Point_2 pla = get<1>(nnRes);
			Point_2 plb = get<2>(nnRes);
			return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
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
	vector<vector<Point_2>> groups{ l0i1, l1i, l0i, l1i1, r0i1, r1i, r0i, r1i1 };
	vector<tuple<int, int>> pairs{ tuple<int, int>(0, 1), tuple<int, int>(2, 3), tuple<int, int>(4, 5),
		tuple<int, int>(6, 7)};
	for (auto pair : pairs) {
		vector<Point_2> p0 = groups[get<0>(pair)];
		vector<Point_2> p1 = groups[get<1>(pair)];
		tuple<bool, Point_2, Point_2> result = queryKdTree(&p0, &p1);
		if (get<0>(result)) {
			Point_2 pla = get<1>(result);
			Point_2 plb = get<2>(result);
			return tuple<Point_2, Point_2, int>(pla, plb, pla.getDist() + plb.getDist());
		}

	}
	return best_r;
}

tuple<Point_2, Point_2, int> getDistToLCA(tuple<Point_2, Point_2, int> best) {
	Point_2 p = get<0>(best);
	Point_2 q = get<1>(best);
	vector<Point_2> ps;
	if (p.getParent() == 0) { ps.push_back(p); }
	else {
		ps.push_back(p);
		Point_2 pi = p;
		while (pi.getParent() != 0) {
			pi = *(pi.getParent());
			ps.push_back(pi);
		}
	}
	vector<Point_2> qs;
	if (q.getParent() == 0) { qs.push_back(q); }
	else {
		qs.push_back(q);
		Point_2 qi = q;
		while (qi.getParent() != 0) {

			qi = *(qi.getParent());
			qs.push_back(qi);
		}
	}
	vector<Point_2>::reverse_iterator longer;
	vector<Point_2>::reverse_iterator shorter;
	Point_2 lca = *(ps.end() - 1);
	for (longer = ps.rbegin(), shorter = qs.rbegin(); (longer != ps.rend() && shorter != qs.rend()); ++longer, ++shorter)
	{
		if (*longer == *shorter) { lca = *longer; }
		else { break; }
	}
	int lca_dist = lca.getDist();
	if (lca_dist > 0)
		cout << "lca dist: " << lca_dist << endl;
	return tuple<Point_2, Point_2, int>{p, q, get<2>(best)-2 * lca_dist};
}

void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	CGAL::Timer cost;
	cost.reset(); cost.start();
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> cycle;
	SSSPTree ssspTree(begin, end, st);
	int k = 0;
	for (auto p = begin; p != end; ++p)
	{
		ssspTree.createTreeFromRoot(*p);
		vector< vector<vector<Point_2>> > rst = ssspTree.getAllSets();
		vector<vector<Point_2>> l0 = rst[0]; vector<vector<Point_2>> l1 = rst[1];
		vector<vector<Point_2>> r0 = rst[2]; vector<vector<Point_2>> r1 = rst[3];
		int i = 1;
		size_t maxdist = l0.size();

		while (2 * i <= min(get<2>(best_r), maxdist*2-1)) {
			tuple<Point_2, Point_2, int> res1 = nnSeparation(&l0, &l1, &r0, &r1, i);
			/*if (get<2>(res1) < (numeric_limits<int>::max)())
				res1 = getDistToLCA(res1);*/
			if (get<2>(res1) < get<2>(best_r)) {
				cout << "dist: " << get<2>(res1) << endl;
				cout << k + 1 << endl;
				best_r = res1;
				break;
			}
			res1 = rangeSeparation(&l0, &l1, &r0, &r1, i, st);
			/*if (get<2>(res1) < (numeric_limits<int>::max)())
				res1 = getDistToLCA(res1);*/
			if (get<2>(res1) < get<2>(best_r)) {
				cout << "dist: " << get<2>(res1) << endl;
				cout << k + 1 << endl;
				best_r = res1;
				break;
			}
			i++;
		}
		k++;
		ssspTree.resetSSSPTreeDTVertices();
	}
	cost.stop();
	if (get<2>(best_r) < (numeric_limits<int>::max)()) {
		cycle = make_cycle(best_r, cycle);
	}
	cout << "time: " << cost.time() << endl;
	cout << get<2>(best_r) << endl;
	for (int i = 0; i < cycle.size()-1; i++) {
		assert(CGAL::squared_distance(cycle[i], cycle[i + 1]) <= 1);
	}
	assert(CGAL::squared_distance(cycle[cycle.size()-1], cycle[0]) <= 1);
	/*if (get<2>(best_r) == (numeric_limits<int>::max)()) {
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
	}*/
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
	for (int i = 0; i < nodes.size() - 1; i++)
	{
		Point_2 v = nodes[i]->v;
		for (int j = i + 1; j < nodes.size(); j++)
		{
			Point_2 u = nodes[j]->v;
			if (CGAL::squared_distance(v, u) <= 1)
			{
				nodes[i]->neighbours.push_back(j);
			}
		}
	}
}

void constructDTSeparation(SSSPTree* tree) {
	for (DH_Vertex_iterator it = tree->dt.finite_vertices_begin(); it != tree->dt.finite_vertices_end(); ++it) {
		DH_Vertex_iterator it2 = it;
		advance(it2, 1);
		Point_2 p1 = it->handle()->point();
		while (it2 != tree->dt.finite_vertices_end()) {
			Point_2 p2 = it2->handle()->point();
			if (CGAL::squared_distance(p1, p2) <= 1) {
				it->handle()->point().neighbours.push_back(&(it2->handle()->point()));
			}
			++it2;
		}
	}
}

void testSepGeneral(vector<Point_2> points, Segment_2 st) {
	CGAL::Timer cost;
	cost.reset(); cost.start();
	SSSPTree tree(points.begin(), points.end(), st, false);
	constructDTSeparation(&tree);
	int best = (numeric_limits<int>::max)();
	tuple<Point_2, Point_2, int> connects;
	for (DH_Vertex_iterator it = tree.dt.finite_vertices_begin(); it != tree.dt.finite_vertices_end(); ++it) {
		Point_2 p = it->handle()->point();
		tree.createTreeFromRoot(p);
		for (DH_Vertex_iterator it2 = tree.dt.finite_vertices_begin(); it2 != tree.dt.finite_vertices_end(); ++it2) {
			Point_2 ap = it2->handle()->point();
			for (Point_2* bpPtr : ap.neighbours) {
				Point_2 bp = *bpPtr;
				if (ap.getNr() + bp.getNr() + tree.updateNr(0, ap, bp) % 2 == 1) {
					if (ap.getVisited() && bp.getVisited() && ap.getDist() + bp.getDist() < best) {
						best = ap.getDist() + bp.getDist();
						connects = tuple<Point_2, Point_2, int>(ap, bp,best);
						cout << "best: " << best << endl;
					}
				}
			}
		}
		tree.resetSSSPTreeDTVertices();
	}
	cost.stop();
	vector<Point_2> cycle{};
	cycle = make_cycle(connects, cycle);
	//for (auto c : cycle) {
	//	cout << c << endl;
	//}
	cout << best << endl;
	cout << "total time: " << cost.time() << endl;
}
