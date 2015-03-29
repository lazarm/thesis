#include <DS1.h>

template <class Iterator> class DS2
{
private:
	vector<DS1 <std::vector<Site_2>::iterator> > A;

public:
	DS2(){};
	~DS2(){};

	void construct(Iterator begin, Iterator beyond);
	vector<DS1 <std::vector<Site_2>::iterator> >::iterator root() {
		vector<DS1 <std::vector<Site_2>::iterator> >::iterator it = A.begin();
		return it; };
	int size() { return A.size(); };
	bool list(int pos) {
		if (2 * pos + 1 > A.size() - 1) return true;
		return false;
	};
	std::tuple<bool, Point_2> search(Point_2 q);
};

template <class Iterator>
void DS2<Iterator>::construct(Iterator begin, Iterator beyond) {
	int j = beyond - begin - 1;
	// size of a tree A is |V| + (|V|-1) = j+1 + (j)
	int a_size = j + 1 + j;
	A.resize(a_size);
	int pos = 0;
	int h = ceil(log2f(j + 1+j));
	//std::cout << "h: " << h << std::endl;
	double breadth;
	vector<int> len;
	len.resize(a_size);// = new int[a_size];
	len[0] = j+1; // number of objects is j+1, but we are counting from 0....j
	// special code block for root node
	DS1<vector<Site_2>::iterator> ds1root;
	ds1root.construct(begin, beyond);
	A[pos] = ds1root;
	std::cout << "haha " << j << " - " << a_size << endl;
	len[2 * pos + 1] = ceil((j+1) / 2.0);
	len[2 * pos + 2] = floor((j+1) / 2.0);
	++pos;
	int nodelen, posh;
	for (int hi = 1; hi < h; ++hi) {
		breadth = exp2(hi);
		std::cout << "breadth: " << breadth << std::endl;
		posh = 0;
		for (int ni = 0; ni < breadth; ++ni) {
			nodelen = len[pos];
			if (nodelen > 1) {
				std::cout << "pos" << 2 * pos + 1 << std::endl;
				len[2 * pos + 1] = ceil(nodelen / 2.0);
				len[2 * pos + 2] = floor(nodelen / 2.0);
				// treba je inicializirati daljsi A za "prazne" liste na zadnjem nivoju!
				//std::cout << "nodelen > 1" << std::endl;//anja + lazo = < 3sreca:wuw ju; (debejitek/2.0)
			}
			else if (nodelen < 1) {
				//std::cout << "nodelen < 1" << std::endl;
				++pos;
				if (pos == a_size) break;
				continue;
			}
			DS1<vector<Site_2>::iterator> ds1;
			//std::cout << hi << " - " << pos << " - " << posh << std::endl;
			ds1.construct(begin + posh, begin + posh + nodelen);
			
			A[pos] = ds1;
			posh = posh + nodelen;
			//std::cout << "ds1 i size: " << ds1.size() << std::endl;
			++pos;
			if (pos == a_size) {
				break;
			}
		}
	}
	std::cout << "endloop" << std::endl;
}


template <class Iterator>
tuple<bool, Point_2> DS2<Iterator>::search(Point_2 q) {
	vector<DS1 <std::vector<Site_2>::iterator> >::iterator v = root();
	//std::cout << "a size: " << size() << std::endl;
	//std::cout << root()->size() << std::endl;
	Point_2 ps(0, 0);
	std::tuple<bool, Point_2> res = root()->query(q);
	if (std::get<0>(res) == false) {
		return make_tuple(false, ps);
	}
	int idx = 0;
	while (list(idx) == false) {
		DS1<std::vector<Site_2>::iterator> leftC = A[2*idx+1];
		//std::cout << "size i: " << A[2 * idx + 1].size() << std::endl;
		tuple<bool, Point_2> r = leftC.query(q);
		if (std::get<0>(r) == true) { idx = 2 * idx + 1; }
		else { idx = 2 * idx + 2; }
		//std::cout << "idx " << idx << std::endl;
	}
	//std::cout << "hurara" << std::endl;
	DS1<std::vector<Site_2>::iterator> list = A[idx];
	Point_2 p = list.pointInsideLeaf();
	return make_tuple(true,p);
}