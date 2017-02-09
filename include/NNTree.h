#include <DualPoint.h>
#include <math.h>

#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <CGAL/Fuzzy_sphere.h>
#include <CGAL/Kd_tree.h>
#include <list>

typedef CGAL::Search_traits_2<EK> NNTreeTraits;
typedef CGAL::Fuzzy_sphere<NNTreeTraits> Fuzzy_circle;
typedef CGAL::Kd_tree<NNTreeTraits> KDTree;


class NNTree
{
private:
	vector<shared_ptr<KDTree>> A;
	int size;
public:
	NNTree();
	template <class Iterator>
	NNTree(Iterator begin, Iterator end);
	~NNTree(){};
	vector<shared_ptr<KDTree>> getA() { return A; }
	template <class Iterator>
	void insert(Iterator begin, Iterator beyond);
	
	int getSize() { return size; };
	void setSize(size_t s) { size = s; };
	shared_ptr<KDTree> getRoot() { return A[0]; }
	std::tuple<bool, Point_2> search(Point_2 q);
	tuple<bool, Point_2> query(Point_2 q, int index);
};

NNTree::NNTree() {
	return;
}

template <class Iterator>
NNTree::NNTree(Iterator begin, Iterator end) {
	int treeSize = 2*nextPowerOfTwo(int(distance(begin,end))) - 1;
	A.reserve(1);
	insert(begin, end);
}

template <class Iterator>
void NNTree::insert(Iterator begin, Iterator end) {
	setSize(A.capacity());
	A.push_back(shared_ptr<KDTree>(new KDTree(begin, end)));
}

tuple<bool, Point_2> NNTree::search(Point_2 q) {
	tuple<bool, Point_2> res = query(q, 0);
	return res;
}

tuple<bool, Point_2> NNTree::query(Point_2 q, int idx)
{
	Fuzzy_circle exact_range(q, 1);
	list<Point_2> result;
	return A[idx]->search_exists(exact_range);
}
