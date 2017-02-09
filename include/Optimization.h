#include <SSSPTree.h>

template <class Iterator>
tuple<Point_2, Point_2, int> findminpairRi(vector<Point_2> li, vector<Point_2> lj, Iterator r0s, Iterator r0t, Segment_2 st,
	tuple<Point_2, Point_2, int> bestR)
{
	if (r0t - r0s == 0) { return bestR; }
	if (li.size() == 0 && lj.size() == 0) { return bestR; }
	RangeTree rangeTreei = buildRangeTree(r0s, r0t, st);
	int minWeight = get<2>(bestR);
	for (auto a = li.begin(); a != li.end(); ++a)
	{
		vector<Point_2> queryResults;
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		rangeTree_query(&rangeTreei, a_dual, queryResults);
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
		for (vector<Point_2>::iterator pointB = queryResults.begin(); pointB != queryResults.end(); ++pointB)
		{
			if (weight_a + pointB->getDist() < minWeight) {
				minWeight = weight_a + pointB->getDist();
				bestR = make_tuple(*a, *pointB, minWeight);
				return bestR;
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
		rangeTree_query_complement(&rangeTreei, a_dual, queryResults);
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
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


