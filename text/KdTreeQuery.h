template <class OutputIterator, class FuzzyQueryItem>
OutputIterator search_exists(OutputIterator it, const FuzzyQueryItem& q, Kd_tree_rectangle<FT, D>& b) const
{
	if (is_leaf()) {
		Leaf_node_const_handle node = static_cast<Leaf_node_const_handle>(this);
		if (node->size()>0)
			for (iterator i = node->begin(); i != node->end(); i++)
				if (q.contains(*i))
				{
					*it = *i; ++it;
					break;
				}
	}
	else {
		Internal_node_const_handle node = static_cast<Internal_node_const_handle>(this);
		// after splitting b denotes the lower part of b
		Kd_tree_rectangle<FT, D> b_upper(b);
		b.split(b_upper, node->cutting_dimension(),	node->cutting_value());

		if (q.outer_range_contains(b)) {
			*it = Point_d(0, 0); ++it;
			//it = node->lower()->tree_items(it);
		}
		else
		    if (q.inner_range_intersects(b))
				it = node->lower()->search_exists(it, q, b);
		
		if (q.outer_range_contains(b_upper)) {
			*it = Point_d(0,0); ++it;
			//it = node->upper()->tree_items(it);
		}
		else
			if (q.inner_range_intersects(b_upper))
				it = node->upper()->search_exists(it, q, b_upper);
	};
	return it;
}