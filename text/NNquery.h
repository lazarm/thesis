tuple<bool, Point_2> NNTree::search(Point_2 q) {
	//tuple<bool, Point_2*> res = make_tuple(true, new Point_2(0, 0));
	tuple<bool, Point_2> res = query(q, 0);
	if (std::get<0>(res) == false) {
		return res;
	}
	
	int i = 0;
	while (2*i+1 < A.size()) {
		res = query(q, 2 * i + 1);
		if (get<0>(res) == true)
			i = 2 * i + 1;
		else
			i = 2 * i + 2;
	}
	// same value as in last loop iteration, if leaf node is left child
	res = query(q, i);
	return res;
}

tuple<bool, Point_2> NNTree::query(Point_2 q, int idx)
{
	Fuzzy_circle exact_range(q, 1);
	list<Point_2> result;
	A[idx]->search_exists(back_inserter(result), exact_range);
	if (result.size() == 0) {
		return tuple<bool, Point_2> {false, Point_2(0, 0)};
	}
	else {
		Point_2 first = result.front();
		return tuple<bool, Point_2> {true, first};
	}
}