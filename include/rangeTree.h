#include <GridGraph.h>

#include <CGAL/Range_segment_tree_traits.h>
#include <CGAL/Tree_traits.h>
#include <CGAL/Range_tree_k.h>
#include <iostream>
#include <utility>
#include <vector>
#include <queue>
#include <iterator>
#include <windows.h>
#include <psapi.h>


typedef CGAL::Range_tree_map_traits_2<DualPoint, NNTree> rangeTraits;
typedef rangeTraits::Key RangeKey;
typedef rangeTraits::Pure_key RangePure_key;
typedef rangeTraits::Interval Interval;
typedef CGAL::Range_tree_2<rangeTraits> RangeTree;

//using namespace std;
using keyValue = pair<DualPoint, NNTree>;
using dualKey = pair<DualPoint, DualPoint>;
using pointTraits = CGAL::tree_point_traits < keyValue, dualKey, DualPoint,
	CGAL::T_Key_1<DualPoint, keyValue>,
	CGAL::C_Low_1<DualPoint, dualKey>,
	CGAL::C_High_1<DualPoint, dualKey>,
	CGAL::C_Compare_1<DualPoint >
	>;
using pointTraits1 = CGAL::tree_point_traits < keyValue, dualKey, DualPoint,
	CGAL::T_Key_2<DualPoint, keyValue>,
	CGAL::C_Low_2<DualPoint, dualKey>,
	CGAL::C_High_2<DualPoint, dualKey>,
	CGAL::C_Compare_2<DualPoint >
>;


typedef CGAL::Range_tree_node<keyValue, dualKey, pointTraits> RangeNode;

typedef CGAL::Range_tree_node<keyValue, dualKey, pointTraits1> RangeNode1;

typedef CGAL::Range_tree_d<keyValue, dualKey, pointTraits1> RangeTree1;

typedef CGAL::Tree_base <keyValue, dualKey> Tree_base;

/* From a vector of type Point_2 representing a group of points R_ make a vector of type Key storing
a pair of DualPoint and empty class NNTree. Build range tree from that.
After that, for each a from L_ make a query with this tree(two queries, one with interval((-inf,-inf),(ax,ay)),
the other one with interval((ax,ay),(inf,inf)). Modify range tree so that when it hits an internal node
s.t. all the points in its subtree are contained in the interval, the query stops at this internal node and the algorithm
runs NNTree query with the original point of a. Return the pair resulting from this query.
*/
template <class Iterator>
RangeTree buildRangeTree(Iterator begin, Iterator end, Segment_2 st)
{
	vector<RangeKey> inputList;
	//cout << distance(begin, end) << endl;
	CGAL::Timer cost;
	cost.reset(); cost.start();
	for (Iterator it = begin; it != end; ++it)
	{
		NNTree voronoiTree;
		Point_2 pt = *it;
		unique_ptr < RangePure_key> dp(new RangePure_key(it._Ptr, st));
		//cout << "point: " << pt << "  /  dual: " << dp->point << endl;
		inputList.push_back(RangeKey(*dp, voronoiTree));
	}
	cost.stop();
	//cout << "inputList: " << cost.time() << endl;
	cost.reset(); cost.start();
	RangeTree rangeTree(inputList.begin(), inputList.end());
	cost.stop();
	//cout << "build range tree: " << cost.time() << endl;
	cost.reset(); cost.start();
	traverse_and_populate_with_data(rangeTree.range_tree_2->root());
	cost.stop();
	//cout << "traverse range tree: " << cost.time() << endl;
	//this_thread::sleep_for(chrono::seconds(2));
	return rangeTree;
}

/*
Takes a node of a secondary range tree and builds NNTree structures bottom-up: based on structures of its left and right child it
effectively retrieves all points contained in its left and right subtree and build NNTree structure with them.
Sites are created only at leaf nodes based on dual points of b and they get associated with original points so that when a 
query is run and some site/point is returned, we can retrieve its original point b with all attributes contained in it 
(such as weight).
*/
void build_VD_trees_on_layer2(RangeNode1* node)
{
	if (node->left_link == 0) {
		// leaf node, leave nn tree empty, only key is accessed?
		DualPoint point = node->object.first;
		vector<Point_2> vdSites;
		vdSites.push_back(*point.originalPoint);
		node->object.second.insert(vdSites.begin(), vdSites.end());
		return;
	}
	// recursively construct NNTree structures for both childs of node
	build_VD_trees_on_layer2(node->left_link);
	build_VD_trees_on_layer2(node->right_link);

	vector<Site_2> vdSites;
	shared_ptr<KDTree> root_left = node->left_link->object.second.getRoot();
	shared_ptr<KDTree> root_right = node->right_link->object.second.getRoot();
	vdSites.insert(vdSites.end(), root_left->begin(), root_left->end());
	vdSites.insert(vdSites.end(), root_right->begin(), root_right->end());
	node->object.second.insert(vdSites.begin(), vdSites.end());
}

/*
Takes a node of a primary range tree and constructs NNTree structures for all nodes in secondary range tree coming from this node.
Recursively does the same for its left and right child.
*/
void traverse_and_populate_with_data(RangeNode* node)
{
	//rekurzivno klici left in right node, prej ali po node, nima veze
	// za node klici sublayer, dobis range_tree_d=1
	// potem lahko klices zgornjo funkcijo
	if (node->left_link == 0) { return; }
	/*MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	*/
	queue<RangeNode*> nodes_to_visit;
	nodes_to_visit.push(node);
	CGAL::Timer cost;
	double total_cast = 0;
	while (!nodes_to_visit.empty()) {
		RangeNode* nd = nodes_to_visit.front();
		nodes_to_visit.pop();
		if (nd->left_link != 0) {
			Tree_base* baseTree = nd->sublayer;
			RangeTree1* secondaryTree = dynamic_cast<RangeTree1 *>(baseTree);
			RangeNode1* secondaryRoot = secondaryTree->root();
			cost.reset(); cost.start();
			build_VD_trees_on_layer2(secondaryRoot);
			cost.stop();
			total_cast += cost.time();
			nodes_to_visit.push(nd->left_link);
			nodes_to_visit.push(nd->right_link);
		}
	}
	//cout << "total cast: " << total_cast << endl;
}

/*
From dual point a_dual make two interval windows - bounding boxes, corresponding to the upper left and the lower right quadrant, where
a_dual is the origin. Windows are [(-inf,ay), (ax,inf)] and [(ax,-inf), (inf,ay)]. Original queries are modified at the point where the 
algorithm hits a node such that all points in its subtree should be contained in a result. The modified version runs a query on 
node's NNTree structure and if the query is successful, we get some point b*, from which we can retrieve the dual point (dual of dual is
original point). That point is contained in the iterator (result).
At the end we are left with a set of points s.t. dist(a,b) <= 1 and ab intersects the segment st. We choose b* with minimum
w_a + w_b*. 
*/
template <class Iterator>
Iterator rangeTree_query(RangeTree* rangeTree, DualPoint* a, Iterator it)
{
	DualPoint xinf = DualPoint();
	DualPoint infy = DualPoint();
	xinf.point = Point_2(a->point.x(), -(numeric_limits<double>::infinity)());
	infy.point = Point_2((numeric_limits<double>::infinity)(), a->point.y());

	Interval second_quadrant = Interval(xinf, infy);

	Point_2 a_orig = *(a->originalPoint);
	rangeTree->window_query_modified(second_quadrant, a_orig, it);

	return it;
}

template <class Iterator>
Iterator rangeTree_query_complement(RangeTree* rangeTree, DualPoint* a, Iterator it)
{
	DualPoint infinfNeg = DualPoint();
	DualPoint xy = DualPoint();
	DualPoint infinfPos = DualPoint();

	infinfNeg.point = Point_2(-(numeric_limits<double>::infinity)(), -(numeric_limits<double>::infinity)());
	infinfPos.point = Point_2((numeric_limits<double>::infinity)(), (numeric_limits<double>::infinity)());
	xy.point = Point_2(a->point.x(), a->point.y());

	Interval first_quadrant = Interval(xy, infinfPos);
	Interval third_quadrant = Interval(infinfNeg, xy);

	Point_2 a_orig = *(a->originalPoint);
	rangeTree->window_query_modified(first_quadrant, a_orig, it);
	rangeTree->window_query_modified(third_quadrant, a_orig, it);
	return it;
}


