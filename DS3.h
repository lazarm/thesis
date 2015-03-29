#include <DS1.h>

template <class Iterator>
struct Node {
	DS1<Iterator> value;
	Node *leftChild;
	Node *rightChild;

	Node(){};
	~Node(){};
	Node(DS1<Iterator> val) {
		value = val;
		leftChild = NULL;
		rightChild = NULL;
	}
    void setValue(DS1<Iterator> val) {
		value = val;
	}
};

template <class Iterator> class DS2
{
private:
	vector<DS1 <Iterator> > A;
	Node<Iterator> *root;
	int size;

public:
	DS2();
	~DS2(){};
	Node<Iterator> *getRoot(){ return root; };
	void setRoot(Node<Iterator> *nd) { root = nd; };
	void construct(Iterator begin, Iterator beyond);
	Node<Iterator> *construct1(Iterator begin, Iterator end);
	
	int getSize() { return size; };
	void setSize(int s) { size = s; };
	bool list(Node<Iterator> *nd) {
		if (nd->value.size() == 1) return true;
		return false;
	};
	std::tuple<bool, Point_2> search(Point_2 q);
};

template <class Iterator>
DS2<Iterator>::DS2() {
	setRoot(NULL);
}

template <class Iterator>
void DS2<Iterator>::construct(Iterator begin, Iterator end) {
	// points are supposed to already be sorted non-descreasingly by their weight
	setSize(end - begin);
	// node nd stores ds1root as its value
	Node<Iterator> *nd = new Node<Iterator>();
	
	int k = int (ceil(std::distance(begin,end) / 2.0));
	Iterator begin2 = begin;
	std::advance(begin2, k);
	nd->leftChild = construct1(begin, begin2);
	nd->rightChild = construct1(begin2, end);
	DS1<vector<Site_2>::iterator> ds1root;
	CGAL::spatial_sort(begin, end);
	ds1root.construct(begin, end);
	std::cout << ds1root.size() << std::endl;
	nd->value = ds1root;
	setRoot(nd);
}

template <class Iterator>
Node<Iterator> *DS2<Iterator>::construct1(Iterator begin, Iterator end) {
	DS1<vector<Site_2>::iterator> ds1node;
	
	if (std::distance(begin,end) == 1) {
		ds1node.construct(begin, end);
		Node<Iterator> *nd = new Node<Iterator>(ds1node);
		return nd;
	}
	Node<Iterator> *nd = new Node<Iterator>();
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
tuple<bool, Point_2> DS2<Iterator>::search(Point_2 q) {
	Node<Iterator> *nd = getRoot();
	Point_2 ps(0, 0);
	std::tuple<bool, Point_2> res = (nd->value).query(q);
	if (std::get<0>(res) == false) {
		return res;
	}
	while (list(nd) == false) {
		Node<Iterator> *leftC = nd->leftChild;
		tuple<bool, Point_2> r = leftC->value.query(q);
		if (std::get<0>(r) == true) { nd = leftC; }
		else { nd = nd->rightChild; }
	}
	DS1<std::vector<Site_2>::iterator> list = nd->value;
	Point_2 p = list.pointInsideLeaf();
	return make_tuple(true, p);
}