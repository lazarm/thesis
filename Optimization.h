#include <W.h>


template <class Iterator>
tuple<Point_2, Point_2> findminpair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend, Segment_2 st)
{
	
}

template <class Iterator>
tuple<Point_2, Point_2, int> findMinPair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend)
{
	DS2<vector<Site_2>::iterator> ds2;
	ds2.construct(setBbegin, setBend);
	tuple<Point_2, Point_2, int> bestPair;
	int minWeight = numeric_limits<int>::max();
	for (Iterator a = setAbegin; a != setAend; ++a)
	{
		tuple<bool, Point_2> query = ds2.search(Point_2(*a));
		if (get<0>(query)) 
		{
			Point_2 bStar(get<1>(query));
			int weight = (*a).getDist() + bStar.getDist();
			if (weight < minWeight) {
				bestPair = make_tuple(*a, bStar, weight);
				minWeight = weight;
			}
		}
	}
	return bestPair;
}

template <class Iterator>
tuple<Point_2, Point_2, int> bruteForce(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend)
{
	int minWeight = numeric_limits<int>::max();
	tuple<Point_2, Point_2, int> bestPair;
	for (Iterator a = setAbegin; a != setAend; ++a) {
		for (Iterator b = setBbegin; b != setBend; ++b) {
			if (CGAL::squared_distance(*a, *b) <= 1 && ((*a).getDist() + (*b).getDist()) < minWeight)
			{
				bestPair = make_tuple(*a, *b, (*a).getDist() + (*b).getDist());
				minWeight = (*a).getDist() + (*b).getDist();
			}
		}
	}
	return bestPair;
}

template <class Iterator>
tuple<Point_2, Point_2, int> bruteForceSegment(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend, Segment_2 st)
{
	int minWeight = numeric_limits<int>::max();
	tuple<Point_2, Point_2, int> bestPair;
	for (Iterator a = setAbegin; a != setAend; ++a) {
		for (Iterator b = setBbegin; b != setBend; ++b) {
			Segment_2 ab(*a, *b);
			if (CGAL::squared_distance(*a, *b) <= 1 && CGAL::do_intersect(st,ab) && ((*a).getDist() + (*b).getDist()) < minWeight)
			{
				bestPair = make_tuple(*a, *b, (*a).getDist() + (*b).getDist());
				minWeight = (*a).getDist() + (*b).getDist();
			}
		}
	}
	return bestPair;
}

Point_2 imagePoint(Point_2 b, Segment_2 st)
{
	Line_2 line_s(b, st.source());
	Direction_2 direction_s(line_s);
	Line_2 line_t(b, st.target());
	Direction_2 direction_t(line_t);

	Point_2 bFI(direction_s.dy() / direction_s.dx(), direction_t.dy() / direction_t.dx());
	return bFI;
}