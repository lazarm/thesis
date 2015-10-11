#include <deque>
#include <CGAL/intersections.h>
#include <unordered_map>
#include<DS3.h>

using namespace std;

/*
source point r has to be one of the points in the vector defined by begin and end
*/
template <class Iterator>
vector< vector<Point_2> > constructW(Iterator begin, Iterator end, Point_2 r, Segment_2 st)
{
	// whenever we get to a point from source r and add it to SSSP, we add it into one of the four sets based on
	// its Nr attribute and its relative position to segment st
	vector<Point_2> l0;
	vector<Point_2> l1;
	vector<Point_2> r0;
	vector<Point_2> r1;
	vector< vector<Point_2> > allSets;

	// Delaunay triangulation
	DT dt;
	dt.insert(begin, end);

	// wip and wi are vectors that in i-th iteration store points/vertices with dist=i
	// we want to store and return points of Point_2 type, but we also need vertex handles
	// in order to retrieve neighbours of a vertex in DT
	vector<Point_2> wi_1_points;
	vector<Delaunay_vertex_handle> wi_1_delaunayVertices;

	// we have to retrieve vertex handle in DT that corresponds to the point r of type Point_2
	// locate type loc must obviously be of type vertex
	// li corresponds to the index of DT vertex that's incident to resulting DT face handle
	// alternativa: Vertex_handle handle = T.nearest_vertex(Point_2(1, 1));
	Locate_type loc;
	int li;
	Delaunay_face_handle h = dt.locate(r, loc, li);
	Delaunay_vertex_handle rVertex;
	if (loc == DT::VERTEX) {
		 rVertex = h->vertex(li);
	}
	else {
		//exception
		cout << "Point r does not coincide with any of DT vertices." << endl;
	}
	
	Point_2 *rPoint;
	rPoint = &rVertex->point();
	rPoint->setDist(0);
	rPoint->setNr(0);
	shared_ptr<Point_2> rootParent(nullptr);
	rPoint->setParent(rootParent);
	
	wi_1_points.push_back({ *rPoint });
	wi_1_delaunayVertices.push_back(rVertex);
	int i = 1;
	
	while (wi_1_delaunayVertices.size() > 0) {
		VoronoiDiagram<vector<Point_2>::iterator> vd_nearestNeighbour;
		vd_nearestNeighbour.construct(wi_1_points.begin(), wi_1_points.end());
		deque<Delaunay_vertex_handle> queue;
		copy(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end(), back_inserter(queue));
		vector<Delaunay_vertex_handle> wi_delaunayVertices;
		vector<Point_2> wi_points;
		while (queue.size() > 0) {
			Delaunay_vertex_handle q = queue.front();
			queue.pop_front();
			DT::Vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { continue; }
				Point_2 *pPoint; //popravi
				pPoint = &p->point();
				if (pPoint->getDist() == numeric_limits<int>::max()) {
					tuple<bool, Point_2*> nearest = vd_nearestNeighbour.query(*pPoint);
					if (get<0>(nearest) == true) {
						shared_ptr<Point_2> par2 = make_shared<Point_2>(*(get<1>(nearest)));
						
						wi_delaunayVertices.push_back(p);
						queue.push_back(p);

						pPoint->setDist(i);
						pPoint->setNr(updateNr(par2->getNr(), *pPoint, *par2, st));
						pPoint->setParent(par2);
						//cout << pPoint->x() << "," << par2->x() << " - " << pPoint->y() << "," << par2->y() << endl;
						
						wi_points.push_back(*pPoint);
						categorize(&l0, &l1, &r0, &r1, pPoint, st);
					}
				}
			} while (++p != done);
		}

		wi_1_delaunayVertices.clear();
		wi_1_points.clear();
		wi_1_delaunayVertices = wi_delaunayVertices;
		wi_1_points = wi_points;

		i++;
	}/*
	for (vector<Point_2*>::iterator tz2 = l0.begin(); tz2 != l0.end(); ++tz2) {
		std::cout << (**tz2).x() << "   " << (**tz2).y() << "  dist: " << (**tz2).getDist() << "  nr: " << (**tz2).getNr() << std::endl;
		cout << *((*tz2)->getParent()) << "  dist: " << (*tz2)->getParent()->getDist() << "  ad: " << (*tz2)->getParent() << endl;
	}*/
	allSets.push_back(l0);
	allSets.push_back(l1);
	allSets.push_back(r0);
	allSets.push_back(r1);
	return allSets;
}

bool onLeft(Point_2 *p, Segment_2 st)
{
	Line_2 line = st.supporting_line();

	// if the point lies on negative (right side) of the segment, return false
	// if the point lies on positive side or ON the segment, return true
	if (line.oriented_side(*p) == CGAL::ON_NEGATIVE_SIDE) return false;
	return true;

}

int updateNr(int nr, Point_2 p, Point_2 q, Segment_2 st)
{
	int pnr = nr;
	Segment_2 s(p, q);
	if (CGAL::do_intersect(s, st)) {
		// it may be the case that p or q lies on the segment st. Poins lying on the segment are categorized as "left"
		// points so we have to check that one point is left and the other one is right.
		if ((onLeft(&p, st) && !onLeft(&q, st)) || (!onLeft(&p, st) && onLeft(&q, st))) {
			pnr = (nr + 1) % 2;
		}
	}
	return pnr;
}



// Put point p in one of the four categories 
void categorize(vector<Point_2>* l0, vector<Point_2>* l1, vector<Point_2>* r0, vector<Point_2>* r1, Point_2 *p, Segment_2 st)
{
	if (p->getNr() == 0 && onLeft(p, st)) {
		l0->push_back(*p);
	}
	else if (p->getNr() == 1 && onLeft(p, st)) {
		l1->push_back(*p);
	}
	else if (p->getNr() == 0 && !onLeft(p, st)) {
		r0->push_back(*p);
	}
	else if (p->getNr() == 1 && !onLeft(p, st)) {
		r1->push_back(*p);
	}
}

