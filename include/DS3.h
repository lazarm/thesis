#include <VoronoiDiagram.h>

template <class Iterator>
struct Node {
	VoronoiDiagram<Iterator> value;
	shared_ptr<Node> leftChild;
	shared_ptr<Node> rightChild;

	Node(){};
	~Node(){};
	Node(VoronoiDiagram<Iterator> val) {
		value = val;
		leftChild = NULL;
		rightChild = NULL;
	}
    void setValue(VoronoiDiagram<Iterator> val) {
		value = val;
	}
};

template <class Iterator> class DS2
{
private:
	vector<VoronoiDiagram <Iterator> > A;
	shared_ptr<Node<Iterator>> root;
	int size;

public:
	DS2();
	~DS2(){};
	shared_ptr<Node<Iterator>> getRoot(){ return root; };
	void setRoot(shared_ptr<Node<Iterator>> nd) { root = nd; };
	void construct(Iterator begin, Iterator beyond);
	shared_ptr<Node<Iterator>> construct1(Iterator begin, Iterator end);
	
	int getSize() { return size; };
	void setSize(int s) { size = s; };
	bool list(shared_ptr<Node<Iterator>> nd) {
		if (nd->value.size() == 1) return true;
		return false;
	};
	std::tuple<bool, Point_2*> search(Point_2 q);
};

template <class Iterator>
DS2<Iterator>::DS2() {
	setRoot(NULL);
}

template <class Iterator>
void DS2<Iterator>::construct(Iterator begin, Iterator end) {
	// points have to be sorted non-descreasingly by their weight
	sort(begin, end, sortByDist());
	setSize(end - begin);
	// node nd stores ds1root as its value
	shared_ptr< Node<Iterator> > nd = make_shared<Node<Iterator>>();
	if (distance(begin, end) > 1) {
		int k = int(ceil(std::distance(begin, end) / 2.0));
		Iterator begin2 = begin;
		std::advance(begin2, k);
		nd->leftChild = construct1(begin, begin2);
		nd->rightChild = construct1(begin2, end);
		CGAL::spatial_sort(begin, end);
	}
	VoronoiDiagram<vector<Site_2>::iterator> ds1root;
	ds1root.construct(begin, end);
	//std::cout << ds1root.size() << std::endl;
	nd->value = ds1root;
	setRoot(nd);
}

template <class Iterator>
shared_ptr<Node<Iterator>> DS2<Iterator>::construct1(Iterator begin, Iterator end) {
	VoronoiDiagram<vector<Site_2>::iterator> ds1node;
	
	if (std::distance(begin,end) == 1) {
		ds1node.construct(begin, end);
		shared_ptr< Node<Iterator> > nd = make_shared<Node<Iterator>>(ds1node);
		return nd;
	}
	shared_ptr< Node<Iterator> > nd = make_shared<Node<Iterator>>();
	int k = int(ceil(std::distance(begin,end) / 2.0));
	Iterator begin2 = begin;
	std::advance(begin2, k);
	nd->leftChild = construct1(begin, begin2);
	nd->rightChild = construct1(begin2, end);
	CGAL::spatial_sort(begin, end);
	ds1node.construct(begin, end);
	nd->value = ds1node;
	return nd;
}

template <class Iterator>
tuple<bool, Point_2*> DS2<Iterator>::search(Point_2 q) {
	shared_ptr<Node<Iterator>> nd = getRoot();
	Point_2 ps(0, 0);
	std::tuple<bool, Point_2*> res = (nd->value).query(q);
	if (std::get<0>(res) == false) {
		return res;
	}
	// naredi while nd->left ali right nista null
	while (list(nd) == false) {
		shared_ptr<Node<Iterator>> leftC = nd->leftChild;
		tuple<bool, Point_2*> r = leftC->value.query(q); // zakaj ne res namesto r?
		if (std::get<0>(r) == true) { nd = leftC; }
		else { nd = nd->rightChild; }
	}
	// z nd->value.vd.sites_begin in end lahko dobimo ven edini site_2, ki sestavlja vd v listu
	//vector<Site_2> sites(nd->value.vd.sites_begin(), nd->value.vd.sites_end());
	//Point_2 p2 = sites.at(0);
	//VoronoiDiagram<std::vector<Site_2>::iterator> list = nd->value;
	//Point_2 p = list.pointInsideLeaf();
	//tuple<bool, Point_2> result = make_tuple(true, p2);
	res = (nd->value).query(q);
	return res;
}

struct sortByDist
{
	bool operator() (Point_2 L, Point_2 R) { return L.getDist() < R.getDist(); }
};