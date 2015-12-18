#include <SSSPTree.h>


template <class Iterator>
tuple<Point_2, Point_2,int> findminpair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend, Segment_2 st, 
	tuple<Point_2, Point_2, int> bestR)
{
	if (setBend - setBbegin == 0) { return bestR; }
	CGAL::Timer cost;
	CGAL::Timer all;
	cost.reset(); cost.start(); all.start();
	RangeTree rangeTree = buildRangeTree(setBbegin, setBend, st);
	cost.stop();
	cout << "build rangeTree total: " << cost.time() << endl;
	int minWeight = get<2>(bestR);
	
	cost.reset(); cost.start();
	for (Iterator a = setAbegin; a != setAend; ++a)
	{
		vector<Point_2> queryResults;
		int weight_a = (*a).getDist();
		if (weight_a >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		//cout << "begin query" << endl;
		rangeTree_query(&rangeTree, a_dual, back_inserter(queryResults));
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
		for (vector<Point_2>::iterator pointB = queryResults.begin(); pointB != queryResults.end(); ++pointB)
		{
			//cout << "it" << endl;
			//cout << "min w: " << minWeight << endl;
			//cout << "w-a: " << weight_a << endl;

			if (weight_a + pointB->getDist() < minWeight) { 
				minWeight = weight_a + pointB->getDist();
				bestR = make_tuple(*a, *pointB, minWeight);
			}
		}
	}
	cost.stop();
	cout << "range tree query: " << cost.time() << endl;
	all.stop();
	cout << "findminpair in timer: " << all.time() << endl;
	return bestR;
}

template <class Iterator>
tuple<Point_2, Point_2, int> findMinPair(Iterator setAbegin, Iterator setAend, Iterator setBbegin, Iterator setBend, 
	tuple<Point_2, Point_2, int> bestR)
{
	if (setBbegin == setBend || setAbegin == setAend) { return bestR; }
	VoronoiTree ds2(setBbegin, setBend);
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
