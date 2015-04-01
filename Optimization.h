#include <W.h>

struct sortByDist
{
	bool operator() (Point_2 L, Point_2 R) { return L.getDist() < R.getDist(); }
};

//tuple<Point_2, Point_2> findMinPair(vector<Point_2> set1, vector<Point_2> set2, Segment_2 st)
//{
//	
//}
//
template <class Iterator>
tuple<Point_2, Point_2, int> findMinPair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend)
{
	DS2<vector<Site_2>::iterator> ds2;
	sort(setBbegin, setBend, sortByDist());
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
