#include <deque>
#include <CGAL/intersections.h>
#include <unordered_map>
#include <CustomPoint.h>

using namespace std;

/*
source point r has to be one of the points in the array defined by begin and end
*/

template <class Iterator>
vector< vector<Point_2> > constructW(Iterator begin, Iterator end, Point_2 r, Segment_2 st)
{
	// whenever we get to a point from source r and add it to SSSP, we add it into one of the four sets based on
	// its Nr attribute and its relative position to segment st
	vector<Site_2> l0;
	vector<Site_2> l1;
	vector<Site_2> r0;
	vector<Site_2> r1;
	vector< vector<Site_2> > allSets;

	// Delaunay triangulation
	DT dt;
	dt.insert(begin, end);

	// wip and wi are vectors that in i-th iteration store points/vertices with dist=i
	// we want to store and return points of Point_2 type, but we also need vertex handles
	// in order to retrieve neighbours of a vertex in DT
	vector<Point_2> wip;
	vector<Delaunay_vertex_handle> wi;

	// we have to retrieve vertex handle in DT that corresponds to the point r of type Point_2
	// locate type loc must obviously be of type vertex
	// li corresponds to the index of DT vertex that's incident to resulting DT face handle
	// alternativa: Vertex_handle handle = T.nearest_vertex(Point_2(1, 1));
	Locate_type loc;
	int li;
	Delaunay_face_handle h = dt.locate(r, loc, li);
	Delaunay_vertex_handle sVertex;
	if (loc == DT::VERTEX) {
		 sVertex = h->vertex(li);
	}
	else {
		//exception
		cout << "Point r does not coincide with any of DT vertives." << endl;
	}
	
	Point_2 *sPoint;
	sPoint = &sVertex->point();
	sPoint->setDist(0);
	sPoint->setNr(0);
	wip.push_back({ *sPoint });
	wi.push_back(sVertex);
	int i = 1;
	
	while (wi.size() > 0) {
		DS1<vector<Point_2>::iterator> nnWi;

		nnWi.construct(wip.begin(), wip.end());
		deque<Delaunay_vertex_handle> Q;
		copy(wi.begin(), wi.end(), back_inserter(Q));
		vector<Delaunay_vertex_handle> wii;
		vector<Point_2> wipi;
		
		while (Q.size() > 0) {
			Delaunay_vertex_handle q = Q.front();
			Q.pop_front();
			DT::Vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { break; }
				Point_2 *pPoint;
				pPoint = &p->point();
				tuple<bool, Point_2> nearest = nnWi.query(*pPoint);
				if (get<0>(nearest) == true && pPoint->getDist() == numeric_limits<int>::max()) {
					pPoint->setDist(i);
					Point_2 w(get<1>(nearest));
					wii.push_back(p);
					Q.push_back(p);
					pPoint->setNr(updateNr(w.getNr(), *pPoint, w, st));
					wipi.push_back(*pPoint);
					categorize(&l0, &l1, &r0, &r1, pPoint,st);
				}
			} while (++p != done);
		}

		wi.clear();
		wip.clear();
		wi = wii;
		wip = wipi;
		i++;
	}
	allSets.push_back(l0);
	allSets.push_back(l1);
	allSets.push_back(r0);
	allSets.push_back(r1);
	return allSets;
}

int updateNr(int nr, Point_2 p, Point_2 q, Segment_2 st)
{
	int pnr = nr;
	Segment_2 s(p, q);
	if (CGAL::do_intersect(s, st)) {
		pnr = (nr + 1) % 2;
	}
	return pnr;
}

bool onLeft(Point_2 *p, Segment_2 st)
{
	Line_2 line = st.supporting_line();

	if (line.oriented_side(*p) == CGAL::ON_POSITIVE_SIDE) return true;
	return false;

	// what if a point lies on the line?
}


//template <class Iterator>
void categorize(vector<Site_2>* l0, vector<Site_2>* l1, vector<Site_2>* r0, vector<Site_2>* r1, Point_2 *p, Segment_2 st)
{
	if (p->getNr() == 0 && onLeft(p, st)) {
		l0->push_back(Site_2(*p));
	}
	else if (p->getNr() == 1 && onLeft(p, st)) {
		l1->push_back(Site_2(*p));
	}
	else if (p->getNr() == 0 && !onLeft(p, st)) {
		r0->push_back(Site_2(*p));
	}
	else if (p->getNr() == 1 && !onLeft(p, st)) {
		r1->push_back(Site_2(*p));
	}
}

