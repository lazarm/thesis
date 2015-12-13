#include <DualPoint.h>
#include <math.h>


template <class Iterator> class VoronoiTree
{
private:
	vector<vector<Point_2> > A;
	int size;
public:
	VoronoiTree();
	VoronoiTree(Iterator begin, Iterator end);
	~VoronoiTree(){};
	vector<vector<Point_2> > getA() { return A; }
	void insert(Iterator begin, Iterator beyond);
	
	int getSize() { return size; };
	void setSize(int s) { size = s; };

	std::tuple<bool, Point_2*> search(Point_2 q);
};

template <class Iterator>
VoronoiTree<Iterator>::VoronoiTree() {
	return;
}

template <class Iterator>
VoronoiTree<Iterator>::VoronoiTree(Iterator begin, Iterator end) {
	int treeSize = 2*nextPowerOfTwo(int(distance(begin,end))) - 1;
	A.reserve(treeSize);
	insert(begin, end);
}

template <class Iterator>
void VoronoiTree<Iterator>::insert(Iterator begin, Iterator end) {
	
	if (A.size() == 0) {
		int treeSize = 2 * nextPowerOfTwo(int(distance(begin, end))) - 1;
		A.reserve(treeSize);
	}
	
	// points have to be sorted non-descreasingly by their weight
	sort(begin, end, sortByDist());
	setSize(A.capacity());
	vector<Point_2> vd_root;
	vd_root.insert(vd_root.end(), begin, end);
	A.push_back(vd_root);
	int i = 1;
	int size = A.capacity();
	for (int v = 1; v < log2(A.capacity() + 1); v++) {
		int line_sum = 0;
		for (int k = 0; k < pow(2, v); k = k + 2) {
			int parent_size = A[(i-1)/2].size();
			if (parent_size > 1) {
				vector<Point_2> vd_left;
				vector<Point_2> vd_right;
				vd_left.insert(vd_left.end(), begin + line_sum, begin + line_sum + int(ceil(parent_size / 2)));
				vd_right.insert(vd_right.end(), begin + line_sum + int(ceil(parent_size / 2)), begin + line_sum + parent_size);
				A.push_back(vd_left);
				//A[i] = vd_left;
				A.push_back(vd_right);
				//A[i + 1] = vd_right;
			}
			i += 2;
			line_sum += parent_size;
		}
	}
}

template <class Iterator>
tuple<bool, Point_2*> VoronoiTree<Iterator>::search(Point_2 q) {
	tuple<bool, Point_2*> res = make_tuple(true, new Point_2(0, 0));
	/*
	tuple<bool, Point_2*> res = A[0].query(q);
	if (std::get<0>(res) == false) {
		return res;
	}
	
	int i = 0;
	while (2*i+1 < A.size()) {
		res = A[2 * i + 1].query(q);
		if (get<0>(res) == true)
			i = 2 * i + 1;
		else
			i = 2 * i + 2;
	}
	// same value as in last loop iteration, if leaf node is left child
	res = A[i].query(q);*/
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

struct sortByDist
{
	bool operator() (Point_2 L, Point_2 R) { return L.getDist() < R.getDist(); }
};