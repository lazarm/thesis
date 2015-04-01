#include <deque>
#include <CGAL/intersections.h>
#include <unordered_map>
#include <CustomPoint.h>

using namespace std;

template <class Iterator>
vector< vector<Point_2> > constructW(Iterator begin, Iterator end, Point_2 r, Segment_2 st)
{
	// a vector which in i-th position stores a vector of DT points with distance to s equal to i
	vector< vector<Point_2> > GPs;
	// Delaunay triangulation
	DT dt;
	dt.insert(begin, end);
	cout << dt.number_of_vertices() << " " << dt.number_of_faces() << endl;
	//while building the tree, we need to set nr value for each p with retrieving p's parent. To do this,
	// we build a hashmap that for each p stores its parent. We need that information also for optimization problem
	// FI(A,B), because a and b mustn't be neighbours in a tree (ab € Er)
	unordered_map <int, Point_2*> parents;
	// wip and wi are vectors that in i-th iteration store points/vertices with dist=i
	// we want to store and return points of Point_2 type, but we also need vertex handles
	// in order to retrieve neighbours of a vertex in DT
	vector<Point_2> wip;
	vector<Delaunay_vertex_handle> wi;

	// we have to retrieve vertex handle in DT that corresponds to the point s of type Point_2
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
		cout << "loc ni vertex!" << endl;
	}
	Point_2 *sPoint; 
	sPoint = &sVertex->point();
	sPoint->setDist(0);
	sPoint->setNr(0);
	sPoint->setId(0);
	wip.push_back(*sPoint);
	GPs.push_back({ *sPoint });
	wi.push_back(sVertex);
	int i = 1;
	int idi = 1;
	
	while (wi.size() > 0) {
		cout << "begin W" << endl;
		DS1<vector<Point_2>::iterator> nnWi;

		nnWi.construct((GPs[i-1]).begin(), (GPs[i-1]).end());
		deque<Delaunay_vertex_handle> Q;
		copy(wi.begin(), wi.end(), back_inserter(Q));
		vector<Delaunay_vertex_handle> wii;
		vector<Point_2> wipi;
		
		while (Q.size() > 0) {
			Delaunay_vertex_handle q = Q.front();
			//cout << q->point().x() << "  " << q->point().y() << endl;
			Q.pop_front();
			//cout << "qSize " << Q.size() << endl;
			DT::Vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				//std::cout << "q:  " << q->point().x() << ", " << q->point().y() << "   , p: " << p->point().x() << ", " << p->point().y() << std::endl;
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { break; }
				Point_2 *pPoint;
				pPoint = &p->point();
				tuple<bool, Point_2> nearest = nnWi.query(*pPoint);
				//cout << p->point() << endl;
				//std::cout << "tocka: " << std::get<1>(nearest).x() << ", " << std::get<1>(nearest).y() << std::endl;
				if (get<0>(nearest) == true && pPoint->getDist() == numeric_limits<int>::max()) {
					pPoint->setDist(i);
					pPoint->setId(idi);
					
					wii.push_back(p);
					Q.push_back(p);
					// p is added to Q and Wi either by q € Q and Wi-1 or r € Q and Wi
					// if former is the case, p's parent is q, if latter, p's parent is r's parent
					// use that info to check if segment p-parent(p) intersects st;
					Point_2 *qPoint;
					qPoint = &q->point();
					if (qPoint->getDist() + 1 == i) {
						parents[idi++] = qPoint;
						pPoint->setNr(updateNr(qPoint->getNr(), *pPoint, *qPoint, st));
						cout << "nr: " << pPoint->getNr() << endl;
					}
					else if (qPoint->getDist() == i) {
						Point_2 *qParent = parents[qPoint->getId()];
						parents[idi++] = qParent;
						pPoint->setNr(updateNr(qParent->getNr(), *pPoint, *qParent, st));
						cout << "nr: " << pPoint->getNr() << endl;
					}
					wipi.push_back(*pPoint);
				}
			} while (++p != done);
		}
		// if Q size was zero, no points were added to wi, so we shouldnt push empty vector to GPs
		if (wipi.empty() == false) {
			GPs.push_back(wipi);
		}
		wi.clear();
		wi = wii;
		i = i + 1;
	}
	for (auto it = parents.begin(); it != parents.end(); ++it) {
		//cout << it->first <<  "  |  " << it->second->x() << " " << it->second->y() << endl;
	}
	return GPs;
}

int updateNr(int nr, Point_2 p, Point_2 q, Segment_2 st)
{
	int pnr = nr;
	Segment_2 s(p, q);
	//cout << "p: " << p.x() << " " << p.y() << "  |  q: " << q.x() << " " << q.y() << endl;
	if (CGAL::do_intersect(s, st)) {
		cout << "intersect!" << endl;
		pnr = (nr + 1) % 2;
	}
	//cout << "pppppppnnnnnnnnrrrrr " << pnr << endl;
	return pnr;
}

/*
after some tree Tr is built and p € P has defined values for dist(r,p) and N(r,p), put each p in L0,L1,R0 or R1
*/
template <class Iterator>
vector< vector<Site_2> > categorize(Iterator begin, Iterator end, Segment_2 st)
{
	vector<Site_2> l0;
	vector<Site_2> l1;
	vector<Site_2> r0;
	vector<Site_2> r1;
	vector< vector<Site_2> > allSets;

	for (vector<Point_2>::iterator p = begin; p != end; ++p)
	{
		if (p->getNr() == 0 && onLeft(*p, st)) {
			l0.push_back(Site_2 (*p));
		}
		else if (p->getNr() == 1 && onLeft(*p, st)) {
			l1.push_back(Site_2(*p));
		}
		else if (p->getNr() == 0 && !onLeft(*p, st)) {
			r0.push_back(Site_2 (*p));
		}
		else if (p->getNr() == 1 && !onLeft(*p, st)) {
			r1.push_back(Site_2 (*p));
		}
	}
	allSets.push_back(l0);
	allSets.push_back(l1);
	allSets.push_back(r0);
	allSets.push_back(r1);
	return allSets;
}

bool onLeft(Point_2 p, Segment_2 st)
{
	Line_2 line = st.supporting_line();

	if (line.oriented_side(p) == CGAL::ON_POSITIVE_SIDE) return true;
	return false;

	// what if a point lies on the line?
}