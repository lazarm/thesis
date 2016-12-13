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
	DT dt;
	Segment_2 st;
	//DT dt;
	void take(shared_ptr<Point_2> q, int i, DH_vertex_handle p);
	bool onLeft(Point_2* p);
	int updateNr(int nr, Point_2 p, Point_2 q);
	void categorize(int i, Point_2* p);
public:
	SSSPTree();
	template <class Iterator>
	SSSPTree(Iterator begin, Iterator end, Segment_2 st);
	~SSSPTree(){};
	DT getDT(){ return dt; }
	vector< vector<vector<Point_2>> > getAllSets();
	void createTreeFromRoot(Point_2 r);
	void clearSets();
	void resetSSSPTreeDTVertices();
	template <class Iterator>
	vector<Point_2> getPoints(Iterator begin, Iterator end);
};

/*
source point r has to be one of the points in the vector defined by begin and end
*/
template <class Iterator>
SSSPTree::SSSPTree(Iterator begin, Iterator end, Segment_2 sta)
{
	st = sta;
	// whenever we get to a point from source r and add it to SSSP, we add it into one of the four sets based on
	// its Nr attribute and its relative position to segment st
	// Delaunay triangulation
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

DH_face_handle getFaceHint(DH_vertex_handle q, int i, DT dt_nearestNeighbour) {
	Point_2* orp = q->point().getDist() == i ? q->point().getParent().get() : &(q->point());
	return dt_nearestNeighbour.getFaceFromPoint(orp);
}

void SSSPTree::take(shared_ptr<Point_2> par2, int i, DH_vertex_handle p) {
	p->point().setVisited(true);
	p->point().setDist(i);
	p->point().setNr(i == 0 ? 0 : updateNr(par2->getNr(), p->point(), *par2));
	p->point().setParent(par2);
	categorize(i, &p->point());
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
	l0.clear(); l1.clear(); r0.clear(); r1.clear();
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

	Point_2 *rPoint = &rVertex->point();
	Point_2* rootPar = nullptr;
	shared_ptr<Point_2> rootParent(nullptr);
	take(rootParent, 0, rVertex);
	wi_1_delaunayVertices.push_back(rVertex);
	int i = 1;
	int sum = 0;
	while (wi_1_delaunayVertices.size() > 0) {
		l0.push_back(vector<Point_2>{}); l1.push_back(vector<Point_2>{});
		r0.push_back(vector<Point_2>{}); r1.push_back(vector<Point_2>{});
		DT dt_nearestNeighbour;
		// built VD is stored as DT on a set of dt vertices, which represents a subset of DT of all points
		dt_nearestNeighbour.insert(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end(), 1);
		deque<DH_vertex_handle> queue(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end());
		vector<DH_vertex_handle> wi_delaunayVertices;
		while (queue.size() > 0) {
			DH_vertex_handle q = queue.front();
			queue.pop_front();
			DH_face_handle origFace = getFaceHint(q, i, dt_nearestNeighbour);
			DH_vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { continue; }
				Point_2* pPoint = &p->point();
				if (!pPoint->getVisited()) {
					Point_2 res = dt_nearestNeighbour.nearest_vertex(*pPoint, origFace)->point();
					if (CGAL::squared_distance(res, *pPoint) <= 1) {
						wi_delaunayVertices.push_back(p);
						queue.push_back(p);
						take(make_shared<Point_2>(res), i, p);
					}
				}
			} while (++p != done);
		}
		wi_1_delaunayVertices.clear();
		wi_1_delaunayVertices = wi_delaunayVertices;
		//l0.push_back(l0i); l1.push_back(l1i); r0.push_back(r0i); r1.push_back(r1i);
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


/*
Re-assigns distances of all points to infinity to start building new SSSP tree from fresh start.
*/
void resetPointDistances(vector<Point_2>::iterator begin, vector<Point_2>::iterator end)
{
	for (auto p = begin; p != end; ++p) {
		(*p).setDist((std::numeric_limits<int>::max)());
	}
}

void SSSPTree::resetSSSPTreeDTVertices()
{
	for (auto v = dt.vertices_begin(); v != dt.vertices_end(); ++v) {
		v->point().setDist((std::numeric_limits<int>::max)());
		v->point().setVisited(false);
	}
}
