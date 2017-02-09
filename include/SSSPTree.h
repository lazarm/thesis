#include <deque>
#include <CGAL/intersections.h>
#include <unordered_map>
#include "NNTree.h"

using namespace std;

class SSSPTree
{
private:
	vector<vector<Point_2>> l0;
	vector<vector<Point_2>> l1;
	vector<vector<Point_2>> r0;
	vector<vector<Point_2>> r1;
	Segment_2 st;
	bool useForSeparation;
	bool useCategorization;
	DT dt;
	bool onLeft(Point_2* p);
	void categorize(int i, Point_2* p);
	void clearSets();
public:
	SSSPTree();
	template <class Iterator>
	SSSPTree(Iterator begin, Iterator end);
	template <class Iterator>
	SSSPTree(Iterator begin, Iterator end, Segment_2 st) : SSSPTree(begin, end, st, true) {};
	template <class Iterator>
	SSSPTree(Iterator begin, Iterator end, Segment_2 st, bool useCategorization);
	~SSSPTree(){};
	DT getDT(){ return dt; }
	vector< vector<vector<Point_2>> > getAllSets();
	void createTreeFromRoot(Point_2 r);
	int updateNr(int nr, Point_2 p, Point_2 q);
	void resetSSSPTreeDTVertices();
};

template <class Iterator>
SSSPTree::SSSPTree(Iterator begin, Iterator end)
{
	useForSeparation = false;
	dt.insert(begin, end);
}

/*
source point r has to be one of the points in the vector defined by begin and end
*/
template <class Iterator>
SSSPTree::SSSPTree(Iterator begin, Iterator end, Segment_2 sta, bool useCategorization2)
{
	st = sta;
	useForSeparation = true;
	useCategorization = useCategorization2;
	dt.insert(begin, end);
}

void SSSPTree::clearSets()
{
	l0.clear();
	l1.clear();
	r0.clear();
	r1.clear();
}

bool SSSPTree::onLeft(Point_2 *p)
{
	Line_2 line = st.supporting_line();

	// if the point lies on negative (right side) of the segment, return false
	// if the point lies on positive side or ON the segment, return true
	if (line.oriented_side(*p) == CGAL::ON_NEGATIVE_SIDE) return false;
	return true;

}

int SSSPTree::updateNr(int nr, Point_2 p, Point_2 q)
{
	int pnr = nr;
	Segment_2 s(p, q);
	if (CGAL::do_intersect(s, st)) {
		// it may be the case that p or q lies on the segment st. Points lying on the segment are categorized as "left"
		// points so we have to check that one point is left and the other one is right.
		if ((onLeft(&p) && !onLeft(&q)) || (!onLeft(&p) && onLeft(&q))) {
			pnr = (nr + 1) % 2;
		}
	}
	return pnr;
}

// Put point p in one of the four categories 
void SSSPTree::categorize(int i, Point_2 *p)
{
	// TODO: is it faster to build vectors for each iteration and then set them at once
	if (p->getNr() == 0 && onLeft(p)) {
		l0[i].push_back(*p);
	}
	else if (p->getNr() == 1 && onLeft(p)) {
		l1[i].push_back(*p);
	}
	else if (p->getNr() == 0 && !onLeft(p)) {
		r0[i].push_back(*p);
	}
	else if (p->getNr() == 1 && !onLeft(p)) {
		r1[i].push_back(*p);
	}
}

/*
wip and wi are vectors that in i-th iteration store points/vertices with dist=i
we want to store and return points of Point_2 type, but we also need vertex handles
in order to retrieve neighbours of a vertex in DT

we have to retrieve vertex handle in DT that corresponds to the point r of type Point_2
locate type loc must obviously be of type vertex
li corresponds to the index of DT vertex that's incident to resulting DT face handle
alternativa: Vertex_handle handle = T.nearest_vertex(Point_2(1, 1));
*/
void SSSPTree::createTreeFromRoot(Point_2 r)
{
	clearSets();
	l0.push_back(vector<Point_2>{}); l1.push_back(vector<Point_2>{});
	r0.push_back(vector<Point_2>{}); r1.push_back(vector<Point_2>{});
	vector<DH_vertex_handle> wi_1_delaunayVertices;
	DH_Locate_type loc;
	int li;
	DH_face_handle h = dt.locate(r, loc, li);
	DH_vertex_handle rVertex;
	if (loc == DT::VERTEX) {
		rVertex = h->vertex(li);
	}
	else {
		cout << "Point r does not coincide with any of DT vertices." << endl;
		return;
	}
	Point_2 *rPoint;
	rPoint = &rVertex->point();
	rPoint->setDist(0);
	shared_ptr<Point_2> rootParent(nullptr);
	rPoint->setParent(rootParent);
	rPoint->setVisited(true);
	if (useForSeparation) {
		rPoint->setNr(0);
		categorize(0, rPoint);
	}
	wi_1_delaunayVertices.push_back(rVertex);
	int i = 1;
	int sum = 0;
	while (wi_1_delaunayVertices.size() > 0) {
		l0.push_back(vector<Point_2>{}); l1.push_back(vector<Point_2>{});
		r0.push_back(vector<Point_2>{}); r1.push_back(vector<Point_2>{});
		DTWithFaceMap dt_nearestNeighbour;
		dt_nearestNeighbour.insert(wi_1_delaunayVertices);
		deque<DH_vertex_handle> queue(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end());
		vector<DH_vertex_handle> wi_delaunayVertices;
		while (queue.size() > 0) {
			DH_vertex_handle q = queue.front();
			queue.pop_front();
			DH_face_handle origFace;
			if (i == 1) {
				origFace = rVertex->face();
			}
			else if (q->point().getDist() == i) {
				origFace = dt_nearestNeighbour.getFaceFromPoint(q->point().getParent().get());
			}
			else {
				origFace = dt_nearestNeighbour.getFaceFromPoint(&(q->point()));
			}
			DH_vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { continue; }
				Point_2* pPoint = &p->point();
				if (!pPoint->getVisited()) {
					Point_2 res = dt_nearestNeighbour.nearest_vertex(*pPoint, origFace)->point();
					if (CGAL::squared_distance(res, *pPoint) <= 1) {
						shared_ptr<Point_2> par2 = make_shared<Point_2>(res);
						p->point().setVisited(true);
						p->point().setDist(i);
						p->point().setParent(par2);
						wi_delaunayVertices.push_back(p);
						queue.push_back(p);
						if (useForSeparation) {
							p->point().setNr(i == 0 ? 0 : updateNr(par2->getNr(), p->point(), *par2));
							if (useCategorization)
								categorize(i, &p->point());
						}
					}
				}
			} while (++p != done);
		}
		wi_1_delaunayVertices.clear();
		wi_1_delaunayVertices = wi_delaunayVertices;
		i++;
		
	}
}

vector< vector<vector<Point_2>> > SSSPTree::getAllSets()
{
	vector< vector<vector<Point_2>> > allSets;
	allSets.push_back(l0);
	allSets.push_back(l1);
	allSets.push_back(r0);
	allSets.push_back(r1);
	return allSets;
}

void SSSPTree::resetSSSPTreeDTVertices()
{
	for (auto v = dt.vertices_begin(); v != dt.vertices_end(); ++v) {
		v->point().setDist((std::numeric_limits<int>::max)());
		v->point().setVisited(false);
	}
}
