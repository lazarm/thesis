#include <DualPoint.h>
#include <math.h>

#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_sphere.h>
#include <CGAL/Kd_tree.h>
#include <list>

typedef CGAL::Search_traits_2<Rep_class> NNTreeTraits;
typedef CGAL::Fuzzy_sphere<NNTreeTraits> Fuzzy_circle;
typedef CGAL::Kd_tree<NNTreeTraits> KDTree;


class VoronoiTree
{
private:
	vector<shared_ptr<KDTree>> A;
	int size;
public:
	VoronoiTree();
	template <class Iterator>
	VoronoiTree(Iterator begin, Iterator end);
	~VoronoiTree(){};
	vector<shared_ptr<KDTree>> getA() { return A; }
	template <class Iterator>
	void insert(Iterator begin, Iterator beyond);
	
	int getSize() { return size; };
	void setSize(int s) { size = s; };
	shared_ptr<KDTree> getRoot() { return A[0]; }
	std::tuple<bool, Point_2> search(Point_2 q);
	tuple<bool, Point_2> query(Point_2 q, int index);
};

VoronoiTree::VoronoiTree() {
	return;
}

template <class Iterator>
VoronoiTree::VoronoiTree(Iterator begin, Iterator end) {
	int treeSize = 2*nextPowerOfTwo(int(distance(begin,end))) - 1;
	A.reserve(treeSize);
	insert(begin, end);
}

template <class Iterator>
void VoronoiTree::insert(Iterator begin, Iterator end) {
	
	if (A.size() == 0) {
		int treeSize = 2 * nextPowerOfTwo(int(distance(begin, end))) - 1;
		A.reserve(treeSize);
	}
	
	// points have to be sorted non-descreasingly by their weight
	sort(begin, end, sortByDist());
	setSize(A.capacity());
	//KDTree vd_root;
	//vd_root.insert(begin, end);
	A.push_back(shared_ptr<KDTree>(new KDTree(begin, end)));
	int i = 1;
	int size = A.capacity();
	for (int v = 1; v < log2(A.capacity() + 1); v++) {
		int line_sum = 0;
		for (int k = 0; k < pow(2, v); k = k + 2) {
			int parent_size = A[(i-1)/2]->size();
			if (parent_size > 1) {
				A.push_back(shared_ptr<KDTree>(new KDTree(begin + line_sum, begin + line_sum + int(ceil(parent_size / 2)))));
				//A[i] = vd_left;
				A.push_back(shared_ptr<KDTree>(new KDTree(begin + line_sum + int(ceil(parent_size / 2)), begin + line_sum + parent_size)));
				//A[i + 1] = vd_right;
			}
			i += 2;
			line_sum += parent_size;
		}
	}
}

tuple<bool, Point_2> VoronoiTree::search(Point_2 q) {
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

int nextPowerOfTwo(int val) {
	val--;
	val = (val >> 1) | val;
	val = (val >> 2) | val;
	val = (val >> 4) | val;
	val = (val >> 8) | val;
	val = (val >> 16) | val;
	val++; // Val is now the next highest power of 2.
	return val;
}

tuple<bool, Point_2> VoronoiTree::query(Point_2 q, int idx)
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

struct sortByDist
{
	bool operator() (Point_2 L, Point_2 R) { return L.getDist() < R.getDist(); }
};