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
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		tuple<bool, Point_2> res = rangeTree_query(&rangeTreei, a_dual);
		Point_2 nearest = get<1>(res);
		delete(a_dual);
		//cout << "query res size: " << queryResults.size() << endl;
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
	}

	for (auto a = lj.begin(); a != lj.end(); ++a)
	{
		int weight_a = (*a).getDist();
		// weight_b is at least 1. If weight_a+1 is already >= minWeight, we know point a won't give us better result
		if (weight_a + 1 >= minWeight) { continue; }
		DualPoint* a_dual = new DualPoint(a._Ptr, st);
		tuple<bool, Point_2> res = rangeTree_query_complement(&rangeTreei, a_dual);
		Point_2 nearest = get<1>(res);
		delete(a_dual);
		if (get<0>(res) && weight_a + nearest.getDist() < minWeight) {
			minWeight = weight_a + nearest.getDist();
			return make_tuple(*a, nearest, minWeight);
		}
	}
	return bestR;
}


