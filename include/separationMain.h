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
				s_start = Point_2(s.x(), -(numeric_limits<int>::max)());
			}
			else {
				// navpicno navzdol
				s_start = Point_2(s.x(), (numeric_limits<int>::max)());
				t_end = Point_2(s.x(), -(numeric_limits<int>::max)());
			}	
		}
		else {
			// st pointing to right
			t_end = Point_2((numeric_limits<int>::max)(), line.y_at_x((numeric_limits<int>::max)()));
			s_start = Point_2(-(numeric_limits<int>::max)(), line.y_at_x(-(numeric_limits<int>::max)()));
		}
	}
	else {
		s_start = Point_2((numeric_limits<int>::max)(), line.y_at_x((numeric_limits<int>::max)()));
		t_end = Point_2(-(numeric_limits<int>::max)(), line.y_at_x(-(numeric_limits<int>::max)()));
	}
	t_plus = Segment_2(t, t_end);
	s_minus = Segment_2(s_start, s);
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
    // p and q are points to be connected and form a cycle. ps and qs are paths from p to r and from q to r, respectively. 
	Point_2 p = get<0>(pair);
	Point_2 q = get<1>(pair);
	vector<Point_2> ps {p};
	if (p.getParent() != 0) {
		Point_2 pi = p;
		while (pi.getParent() != 0) {
			pi = *(pi.getParent());
			ps.push_back(pi);
		}
	}
	vector<Point_2> qs {q};
	if (q.getParent() != 0) {
		Point_2 qi = q;
		while (qi.getParent() != 0) {
			qi = *(qi.getParent());
			qs.push_back(qi);
		}
	}
	cycle.insert(cycle.end(), qs.rbegin(), qs.rend());
	cycle.insert(cycle.end(), ps.begin(), ps.end() - 1);
}


void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
	vector<Point_2> cycle;

	int k = 0;
	for (auto p = begin; p != end; ++p)
	{
		k = k + 1;
		if (k != 16) { continue; }
		//cout << (k-1) << endl;
		//cout << "p: " << *p << endl;
		//cout << "root = " << *p << endl;
		CGAL::Timer cost;
		cost.reset(); cost.start();
		SSSPTree<vector<Point_2>::iterator> ssspTree(begin, end, *p, st);
		cost.stop();
		cout << "sssp tree time: " << cost.time() << endl;
		vector< vector<Point_2> > rst = ssspTree.getAllSets();
		int ii = 0;

		cout << rst.at(0).size() << ", " << rst.at(1).size() << ", " << rst.at(2).size() << ", " << rst.at(3).size() << endl;

		Line_2 st_line = Line_2(st);
		tuple<Segment_2, Segment_2> rays = getRays(st, st_line);
		tuple<Point_2, Point_2, int> bestR = make_tuple(Point_2(), Point_2(), (numeric_limits<int>::max)());
		cost.reset(); cost.start();
		tuple<Point_2, Point_2, int> r0rangeTree = findminpairRi(rst.at(0), rst.at(1), rst.at(2).begin(), rst.at(2).end(), st, bestR);
		tuple<Point_2, Point_2, int> r1rangeTree = findminpairRi(rst.at(1), rst.at(0), rst.at(3).begin(), rst.at(3).end(), st, r0rangeTree);
		cost.stop();
		//cout << "first: " << cost.time() << endl;
		cost.reset(); cost.start();
		tuple<Point_2, Point_2, int>  l0l1 = findMinPair(rst.at(0).begin(), rst.at(0).end(), rst.at(1).begin(), rst.at(1).end(), r1rangeTree);
		cost.stop();
		//cout << "seventh: " << cost.time() << endl;
		cost.reset(); cost.start();
		tuple<Point_2, Point_2, int>  r0r1 = findMinPair(rst.at(2).begin(), rst.at(2).end(), rst.at(3).begin(), rst.at(3).end(), l0l1);
		cost.stop();
		//cout << "eighth: " << cost.time() << endl;
		tuple<Point_2, Point_2, int> result = r0r1;
		if (get<2>(result) < get<2>(best_r)) { 
			best_r = result;
			cycle.clear();
			//cout << get<0>(best_r) << "  -   " << get<1>(best_r) << " , dist= " << get<2>(best_r) << endl;
			//cout << *(get<0>(best_r).getParent()) << "  -  " << *(get<1>(best_r).getParent()) << endl;
			cout << get<0>(best_r).x() << "," << get<1>(best_r).x() << endl;
			cout << get<0>(best_r).y() << "," << get<1>(best_r).y() << endl;
			cout << get<2>(best_r) << endl;
			shared_ptr<Point_2> pp = get<0>(best_r).getParent();
			//cout << pp->getParent() << endl;
			make_cycle(result, cycle);
		}
		break;
		//cout << "make_cycle" << endl;
		resetPointDistances(begin, end);
	}
	if (cycle.empty()) {
		cout << "Could not separate points " << st.source().x() << "," << st.source.y() << " and " << st.end().x() << "," << st.end().y()
			<< " with this set of unit disks." << endl;
	}
	else {
		cout << "Points " << st.source().x() << "," << st.source.y() << " and " << st.end().x() << "," << st.end().y()
			<< " can be separated by connecting points " << get<0>(best_r).x() << "," << get<0>(best_r).y() << " and "
			<< get<1>(best_r).x() << "," << get<1>(best_r).y() << " and thus forming a following cycle: " << endl;

		for (auto c : cycle) {
			cout << c << endl;
		}
	}
}
