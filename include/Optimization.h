#include <SSSPTree.h>

template <class Iterator>
tuple<Point_2, Point_2, int> findminpairRi(vector<Point_2> li, vector<Point_2> lj, Iterator r0s, Iterator r0t, Segment_2 st,
	tuple<Point_2, Point_2, int> bestR)
{
	if (r0t - r0s == 0) { return bestR; }
	//RangeTree rangeTreei = buildRangeTree(r0.begin(), r0.end(), st);
	cout << "ht" << endl;
	RangeTree rangeTreei = buildRangeTree(r0s, r0t, st);
	int minWeight = get<2>(bestR);
	for (auto a = li.begin(); a != li.end(); ++a)
	{
		vector<Point_2> queryResults;
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		rangeTree_query(&rangeTreei, a_dual, back_inserter(queryResults));
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
		for (vector<Point_2>::iterator pointB = queryResults.begin(); pointB != queryResults.end(); ++pointB)
		{
			if (weight_a + pointB->getDist() < minWeight) {
				minWeight = weight_a + pointB->getDist();
				bestR = make_tuple(*a, *pointB, minWeight);
			}
		}
	}

	for (auto a = lj.begin(); a != lj.end(); ++a)
	{
		vector<Point_2> queryResults;
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		rangeTree_query_complement(&rangeTreei, a_dual, back_inserter(queryResults));
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
		for (vector<Point_2>::iterator pointB = queryResults.begin(); pointB != queryResults.end(); ++pointB)
		{
			if (weight_a + pointB->getDist() < minWeight) {
				minWeight = weight_a + pointB->getDist();
				bestR = make_tuple(*a, *pointB, minWeight);
			}
		}
	}
	return bestR;
}

template <class Iterator>
tuple<Point_2, Point_2, int> findMinPair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend, 
	tuple<Point_2, Point_2, int> bestR)
{
	if (setBbegin == setBend || setAbegin == setAend) { return bestR; }
	NNTree ds2(setBbegin, setBend);
	//ds2.construct(setBbegin, setBend);
	int minWeight = get<2>(bestR);
	for (Iterator a = setAbegin; a != setAend; ++a)
	{
		if (a->getDist() < minWeight) {
			tuple<bool, Point_2> query = ds2.search(Point_2(*a));
			if (get<0>(query))
			{
				Point_2 bStar = get<1>(query);
				int weight = (*a).getDist() + bStar.getDist();
				if (weight < minWeight) {
					bestR = make_tuple(*a, bStar, weight);
					minWeight = weight;
				}
			}
		}
	}
	return bestR;
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
