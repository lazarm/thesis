#include <bfs.h>

#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Tree_traits.h>
#include <CGAL/Range_tree_k.h>
#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <iterator>
#include <windows.h>


typedef CGAL::Range_tree_map_traits_2<DualPoint, VoronoiTree<vector<Site_2>::iterator> > rangeTraits;
typedef rangeTraits::Key RangeKey;
typedef rangeTraits::Pure_key RangePure_key;
typedef rangeTraits::Interval Interval;
typedef CGAL::Range_tree_2<rangeTraits> RangeTree;

typedef CGAL::Range_tree_node<
	std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
	std::pair<DualPoint, DualPoint>,
	CGAL::tree_point_traits<
		std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
		std::pair<DualPoint, DualPoint>,
		DualPoint,
		CGAL::T_Key_1<DualPoint, std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>>,
		CGAL::C_Low_1<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_High_1<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_Compare_1<DualPoint >>
	> RangeNode;

typedef CGAL::Range_tree_node<
	std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
	std::pair<DualPoint, DualPoint>,
	CGAL::tree_point_traits<
		std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
		std::pair<DualPoint, DualPoint>,
		DualPoint,
		CGAL::T_Key_2<DualPoint, std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>>,
		CGAL::C_Low_2<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_High_2<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_Compare_2<DualPoint >>
	> RangeNode1;

typedef CGAL::Range_tree_d<
	std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
	std::pair<DualPoint, DualPoint>,
	CGAL::tree_point_traits<
		std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
		std::pair<DualPoint, DualPoint>,
		DualPoint,
		CGAL::T_Key_2<DualPoint, std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>>,
			CGAL::C_Low_2<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_High_2<DualPoint, std::pair<DualPoint, DualPoint >>,
		CGAL::C_Compare_2<DualPoint >>
	> RangeTree1;

typedef CGAL::Tree_base <
	std::pair<DualPoint, VoronoiTree<vector<Site_2>::iterator>>,
	std::pair<DualPoint, DualPoint>
	> Tree_base;

/* From a vector of type Point_2 representing a group of points R_ make a vector of type Key storing
a pair of DualPoint and empty class VoronoiTree. Build range tree from that.


after that, for each a from L_ make a query with this tree(two queries, one with interval((-inf,-inf),(ax,ay)),
the other one with interval((ax,ay),(inf,inf)). Modify range tree so that when it hits an internal node
s.t. all the points in its subtree are contained in the interval, the query stops at this internal node and the algorithm
runs VoronoiTree query with the original point of a. Return the pair resulting from this query.
*/
template <class Iterator>
RangeTree buildRangeTree(Iterator begin, Iterator end, Segment_2 st)
{
	vector<RangeKey> inputList;
	//cout << distance(begin, end) << endl;
	for (Iterator it = begin; it != end; ++it)
	{
		VoronoiTree<vector<Site_2>::iterator> ds2; //PAZI DA TO NE BO VEDNO REFERENCA NA ISTI OBJEKT!!!
		Point_2 pt = *it;
		unique_ptr < RangePure_key> dp(new RangePure_key(it._Ptr, st));
		//cout << "point: " << pt << "  /  dual: " << dp->point << endl;
		inputList.push_back(RangeKey(*dp, ds2));
	}
	
	RangeTree rangeTree(inputList.begin(), inputList.end());
	ds2ObjectsConstruction1(rangeTree.range_tree_2->root());
	return rangeTree;
}

/*
Takes a node of a secondary range tree and builds VoronoiTree structures bottom-up: based on structures of its left and right child it
effectively retrieves all points contained in its left and right subtree and build VoronoiTree structure with them.
Sites are created only at leaf nodes based on dual points of b and they get associated with original points so that when a 
query is run and some site/point is returned, we can retrieve its original point b with all attributes contained in it 
(such as weight).
*/
void ds2ObjectsConstruction(RangeNode1* node)
{

	if (node->left_link == 0) {
		// leaf node
		DualPoint point = node->object.first;
		
		vector<Point_2> vdSites;
		vdSites.push_back(*point.originalPoint);
		node->object.second.construct(vdSites.begin(), vdSites.end());
		return;
	}
	// recursively construct VoronoiTree structures for both childs of node
	ds2ObjectsConstruction(node->left_link);
	ds2ObjectsConstruction(node->right_link);

	// root node of VoronoiTree structure representing the object of left and right child of node
	shared_ptr< Node< vector<Site_2>::iterator> > ds2LeftRootNode = node->left_link->object.second.getRoot();
	shared_ptr< Node< vector<Site_2>::iterator> > ds2RightRootNode = node->right_link->object.second.getRoot();
	// vector of voronoi sites of VD at root node of VoronoiTree structure
	vector<Site_2> leftVoronoiSites(ds2LeftRootNode->value.sites_begin(),ds2LeftRootNode->value.sites_end());
	vector<Site_2> rightVoronoiSites(ds2RightRootNode->value.sites_begin(), ds2RightRootNode->value.sites_end());
	vector<Site_2> vdSites;
	
	vdSites.reserve(leftVoronoiSites.size() + rightVoronoiSites.size());
	vdSites.insert(vdSites.end(), leftVoronoiSites.begin(), leftVoronoiSites.end());
	vdSites.insert(vdSites.end(), rightVoronoiSites.begin(), rightVoronoiSites.end());
	node->object.second.construct(vdSites.begin(), vdSites.end());
	
	
}


/*
Takes a node of a primary range tree and constructs VoronoiTree structures for all nodes in secondary range tree coming from this node.
Recursively does the same for its left and right child.
*/
void ds2ObjectsConstruction1(RangeNode* node)
{
	//rekurzivno klici left in right node, prej ali po node, nima veze
	// za node klici sublayer, dobis range_tree_d=1
	// potem lahko klices zgornjo funkcijo
	if (node->left_link == 0) { return; }
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

	queue<RangeNode*> nodes_to_visit;
	nodes_to_visit.push(node);
	
	while (!nodes_to_visit.empty()) {
		//cout << "haha0" << endl;
		RangeNode* nd = nodes_to_visit.front();
		//cout << "haha1" << endl;
		nodes_to_visit.pop();
		//cout << "haha2" << endl;
		if (nd->left_link != 0) {
			Tree_base* baseTree = nd->sublayer;
			//cout << "haha3" << endl;
			RangeTree1* secondaryTree = dynamic_cast<RangeTree1 *>(baseTree);
			//cout << "haha4" << endl;
			RangeNode1* secondaryRoot = secondaryTree->root();
			
			ds2ObjectsConstruction(secondaryRoot);
			DWORDLONG used = totalPhysMem - memInfo.ullAvailPhys;
			cout << "used: " << used * 100 / totalPhysMem << endl;
			//cout << "haha5" << endl;
			nodes_to_visit.push(nd->left_link);
			nodes_to_visit.push(nd->right_link);
			
		}
		else {
			
		}
	}
	/*
	Tree_base* baseTree = node->sublayer;
	RangeTree1* secondaryTree = dynamic_cast<RangeTree1 *>(baseTree);
	RangeNode1* secondaryRoot = secondaryTree->root();

	ds2ObjectsConstruction1(node->left_link);
	ds2ObjectsConstruction1(node->right_link);

	ds2ObjectsConstruction(secondaryRoot);*/
}

/*
From dual point a_dual make two interval windows - bounding boxes, corresponding to the upper left and the lower right quadrant, where
a_dual is the origin. Windows are [(-inf,ay), (ax,inf)] and [(ax,-inf), (inf,ay)]. Original queries are modified at the point where the 
algorithm hits a node such that all points in its subtree should be contained in a result. The modified version runs a query on 
node's VoronoiTree structure and if the query is successful, we get some point b*, from which we can retrieve the dual point (dual of dual is
original point). That point is contained in the iterator (result).
At the end we are left with a set of points s.t. dist(a,b) <= 1 and ab intersects the segment st. We choose b* with minimum
w_a + w_b*. 
*/
template <class Iterator>
Iterator rangeTree_query(RangeTree* rangeTree, DualPoint* a, Iterator it)
{
	DualPoint dp1 = DualPoint();
	DualPoint dp2 = DualPoint();
	DualPoint dp3 = DualPoint();
	DualPoint dp4 = DualPoint();
	dp1.point = Point_2((numeric_limits<int>::min)(), a->point.y());
	dp2.point = Point_2(a->point.x(), (numeric_limits<int>::max)());
	dp3.point = Point_2(a->point.x(), (numeric_limits<int>::min)());
	dp4.point = Point_2((numeric_limits<int>::max)(), a->point.y());

	Interval win1 = Interval(dp1, dp2);
	Interval win2 = Interval(dp3, dp4);

	double x1 = a->originalPoint->x();
	double y1 = a->originalPoint->y();
	Point_2 a_orig = *(a->originalPoint);

	rangeTree->window_query_modified(win1, a_orig, it);
	rangeTree->window_query_modified(win2, a_orig, it);

	return it;
}


