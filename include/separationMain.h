#include <rangeTree.h>

/*
Re-assigns distances of all points to infinity to start building new SSSP tree from fresh start.
*/
void resetDistances(vector<Point_2>::iterator begin, vector<Point_2>::iterator end)
{
	for (auto p = begin; p != end; ++p) {
		(*p).setDist(numeric_limits<int>::max());
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
				t_end = Point_2(s.x(), numeric_limits<int>::max());
				s_start = Point_2(s.x(), numeric_limits<int>::min());
			}
			else {
				// navpicno navzdol
				s_start = Point_2(s.x(), numeric_limits<int>::max());
				t_end = Point_2(s.x(), numeric_limits<int>::min());
			}	
		}
		else {
			// st pointing to right
			t_end = Point_2(numeric_limits<int>::max(), line.y_at_x(numeric_limits<int>::max()));
			s_start = Point_2(numeric_limits<int>::min(), line.y_at_x(numeric_limits<int>::min()));
		}
	}
	else {
		s_start = Point_2(numeric_limits<int>::max(), line.y_at_x(numeric_limits<int>::max()));
		t_end = Point_2(numeric_limits<int>::min(), line.y_at_x(numeric_limits<int>::min()));
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


void main_procedure(vector<Point_2>::iterator begin, vector<Point_2>::iterator end, Segment_2 st)
{
	tuple<Point_2, Point_2, int> best_r = make_tuple(Point_2(), Point_2(), numeric_limits<int>::max());
	for (auto p = begin; p != end; ++p)
	{
		vector< vector<Point_2> > rst = constructW(begin, end, *p, st);
		Line_2 st_line = Line_2(st);
		tuple<Segment_2, Segment_2> rays = getRays(st, st_line);

		tuple<Point_2, Point_2, int>  l0r0 = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(2).begin(), rst.at(2).end(),st);
		tuple<Point_2, Point_2, int>  l1r1 = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(3).begin(), rst.at(3).end(), st);

		tuple<Point_2, Point_2, int>  l0r1_t = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), get<0>(rays));
		tuple<Point_2, Point_2, int>  l0r1_s = findminpair(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), get<1>(rays));

		tuple<Point_2, Point_2, int>  l1r0_t = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(2).begin(), rst.at(2).end(), get<0>(rays));
		tuple<Point_2, Point_2, int>  l1r0_s = findminpair(rst.at(1).begin(), rst.at(1).end(), rst.at(2).begin(), rst.at(2).end(), get<1>(rays));

		tuple<Point_2, Point_2, int>  l0l1 = findMinPair(rst.at(0).begin(), rst.at(0).end(), rst.at(1).begin(), rst.at(1).end());
		tuple<Point_2, Point_2, int>  r0r1 = findMinPair(rst.at(2).begin(), rst.at(2).end(), rst.at(3).begin(), rst.at(3).end());

		vector< tuple<Point_2, Point_2, int> > optimals { l0l1, l0r0, l0r1_s, l0r1_t, l1r0_s, l1r0_t, l1r1, r0r1 };
		tuple<Point_2, Point_2, int> result = findBest(optimals.begin(), optimals.end());
		// if result < best_result, ga obdrzi, sicer ne
		if (get<2>(result) < get<2>(best_r)) { best_r = result; }

		resetDistances(begin, end);
	}
}

