#include <rangeTree.h>

/*
Re-assigns distances of all points to infinity to start building new SSSP tree from fresh start.
*/
void resetPointDistances(vector<Point_2>::iterator begin, vector<Point_2>::iterator end)
{
	for (auto p = begin; p != end; ++p) {
		(*p).setDist((std::numeric_limits<int>::max)());
	}
}

/*
Returns two segments acting as rays (one endpoint goes to int max value in one of the directions),
where sources are endpoints of segment st.
*/
tuple<Segment_2, Segment_2> getRays(Segment_2 st, Line_2 line)
{
	// four cases: is st pointing left or right? Is st horizontal or not?
	Segment_2 t_plus;
	Segment_2 s_minus;
	Point_2 t_end;
	Point_2 s_start;
	Point_2 s = st.source();
	Point_2 t = st.end();
	if (s.x() <= t.x()) {
		if (s.x() == t.x()) {
			if (s.y() < t.y()) {
				// navpicno navzgor
				t_end = Point_2(s.x(), (numeric_limits<int>::max)());
				s_start = Point_2(s.x(), (numeric_limits<int>::min)());
			}
			else {
				// navpicno navzdol
				s_start = Point_2(s.x(), (numeric_limits<int>::max)());
				t_end = Point_2(s.x(), (numeric_limits<int>::min)());
			}	
		}
		else {
			// st pointing to right
			t_end = Point_2((numeric_limits<int>::max)(), line.y_at_x((numeric_limits<int>::max)()));
			s_start = Point_2((numeric_limits<int>::min)(), line.y_at_x((numeric_limits<int>::min)()));
		}
	}
	else {
		s_start = Point_2((numeric_limits<int>::max)(), line.y_at_x((numeric_limits<int>::max)()));
		t_end = Point_2((numeric_limits<int>::min)(), line.y_at_x((numeric_limits<int>::min)()));
	}
	t_plus = Segment_2(t, t_end);
	s_minus = Segment_2(s, s_start);
	tuple<Segment_2, Segment_2> rays = make_tuple(t_plus, s_minus);
	return rays;
}

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

void make_cycle(tuple<Point_2, Point_2, int> pair, vector<Point_2> cycle)
{
	/*
	use the lowest common ancestor algorithm
    p and q are points to be connected and form a cycle. To find a set of points defining the cycle, we need to find p's and q's
	lowest common ancestor. ps and qs are paths from p to r and from q to r, respectively. LCA is found by comparing backward lists
	of these paths looking for the first point where lists differ. LCA is the parent of such point.
	O(n) + O(n) + O(n) = O(n) in worse-case scenario (where BFS tree is almost a path)
    */
	Point_2 p = get<0>(pair);
	Point_2 q = get<1>(pair);
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
	Point_2 lca = *(ps.end()-1);
	for (longer = ps.rbegin(), shorter = qs.rbegin(); (longer != ps.rend() && shorter != qs.rend()); ++longer, ++shorter)
	{
		if (*longer == *shorter) { lca = *longer; }
		else { break; }
	}
	int lca_dist = lca.getDist();
	vector<Point_2> p_to_lca(ps.begin(), ps.begin() + (ps.size() - lca_dist)); //  to get [p,lca)
	vector<Point_2> q_to_lca(qs.rbegin() + lca_dist+1, qs.rend()); // get (lca, q]

	cycle.insert(cycle.end(), qs.rbegin(), qs.rbegin() + lca_dist+1); // [r, lca]
	cycle.insert(cycle.end(), q_to_lca.begin(), q_to_lca.end()); // (lca,q]
	cycle.insert(cycle.end(), p_to_lca.begin(), p_to_lca.end()); // [p,lca)
	// merge p_to_lca and q_to_lca, kako je s clear in reserve, shrink to fit, reserve itd
	/*
	for (auto i : ps)
	{
		cout << i << endl;
	}
	cout << "qs" << endl;
	for (auto i : qs)
	{
		cout << i << endl;
	}*/

	/*
	cout << "cycle" << endl;
	for (auto i : cycle)
	{
		cout << i << endl;
	}*/
}


void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> cycle;
	// reserve n memory cells. Maximum number of used cells is n, but probably less of them will be filled.
	// It's probably better to do reserve only once with big value , and clear each time in the loop
	//cycle.reserve(distance(begin,end));
	int k = 0;
	for (auto p = begin; p != end; ++p)
	{
		k = k + 1;
		if (k != 16) { continue; }
		cout << (k-1) << endl;
		
		//cout << "root = " << *p << endl;
		SSSPTree<vector<Point_2>::iterator> ssspTree(begin, end, *p, st);
		vector< vector<Point_2> > rst = ssspTree.getAllSets();
		int ii = 0;/*
		for (vector< vector<Point_2> >::iterator tz = rst.begin(); tz != rst.end(); ++tz) {
			std::cout << ii << std::endl;
			++ii;
			for (vector<Point_2>::iterator tz2 = (*tz).begin(); tz2 != (*tz).end(); ++tz2) {
				std::cout << (*tz2).x() << "   " << (*tz2).y() << "  dist: " << (*tz2).getDist() << "  nr: " << (*tz2).getNr() << std::endl;
				cout << *(tz2->getParent()) << "  dist: " << tz2->getParent()->getDist() << "  nr: " << tz2->getParent()->getNr() << endl;
			}
		}*/
		cout << "st" << endl;
		cout << st.source().x() << "," << st.target().x() << endl;
		cout << st.source().y() << "," << st.target().y() << endl;
		cout << rst.at(0).size() << ", " << rst.at(1).size() << ", " << rst.at(2).size() << ", " << rst.at(3).size() << endl;

		Line_2 st_line = Line_2(st);
		tuple<Segment_2, Segment_2> rays = getRays(st, st_line);
		tuple<Point_2, Point_2, int> bestR = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());

		tuple<Point_2, Point_2, int>  l0r0 = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(2).begin(), rst.at(2).end(),st, bestR);
		tuple<Point_2, Point_2, int>  l1r1 = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(3).begin(), rst.at(3).end(), st,l0r0);
		cout << "1" << endl;
		tuple<Point_2, Point_2, int>  l0r1_t = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), get<0>(rays), l1r1);
		tuple<Point_2, Point_2, int>  l0r1_s = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), get<1>(rays), l0r1_t);
		cout << "2" << endl;
		tuple<Point_2, Point_2, int>  l1r0_t = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(2).begin(), rst.at(2).end(), get<0>(rays), l0r1_s);
		tuple<Point_2, Point_2, int>  l1r0_s = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(2).begin(), rst.at(2).end(), get<1>(rays), l1r0_t);
		cout << "3" << endl;
		tuple<Point_2, Point_2, int>  l0l1 = findMinPair(rst.at(0).begin(), rst.at(0).end(), rst.at(1).begin(), rst.at(1).end(), l1r0_s);
		cout << "4" << endl;
		tuple<Point_2, Point_2, int>  r0r1 = findMinPair(rst.at(2).begin(), rst.at(2).end(), rst.at(3).begin(), rst.at(3).end(), l0l1);
		cout << "5" << endl;
		tuple<Point_2, Point_2, int> result = r0r1;
		// if result < best_result, ga obdrzi, sicer ne
		if (get<2>(result) < get<2>(best_r)) { 
			best_r = result;
			cycle.clear();
			//cout << get<0>(best_r) << "  -   " << get<1>(best_r) << " , dist= " << get<2>(best_r) << endl;
			//cout << *(get<0>(best_r).getParent()) << "  -  " << *(get<1>(best_r).getParent()) << endl;
			cout << get<0>(best_r).x() << "," << get<1>(best_r).x() << endl;
			cout << get<0>(best_r).y() << "," << get<1>(best_r).y() << endl;
			shared_ptr<Point_2> pp = get<0>(best_r).getParent();
			//cout << pp->getParent() << endl;
			make_cycle(result, cycle);
		}
		break;
		//cout << "make_cycle" << endl;
		resetPointDistances(begin, end);
	}
	/*
	cycle je ocitno (Tr, e), ampak ponavadi je del cycla itak "rep", ki ga lahko odstranis? Res pa je, da bomo slej ko prej
	prisli do takega r, da bomo dobili enak cikel brez tega repa. Implementacijsko je najbrz bolj zahtevno, ce bi rep kar odstranili,
	ker dist vrednosti se nanasajo na r.
	kaj pa optimizacija? ce prides recimo do a-ja na levi strani, ki ima dist >= min_dist-1, ga lahko kar zavrzes
	*/
}
