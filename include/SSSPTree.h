#include <deque>
#include <CGAL/intersections.h>
#include <unordered_map>
#include <NNTree.h>

using namespace std;

class SSSPTree
{
private:
	vector<Point_2> l0;
	vector<Point_2> l1;
	vector<Point_2> r0;
	vector<Point_2> r1;
	DT dt;
public:
	SSSPTree();
	template <class Iterator>
	SSSPTree(Iterator begin, Iterator end);
	~SSSPTree(){};
	DT getDT(){ return dt; }
	vector< vector<Point_2> > getAllSets();
	void createTreeFromRoot(Point_2 r, Segment_2 st);
	void clearSets();
	void resetSSSPTreeDTVertices();
};

/*
source point r has to be one of the points in the vector defined by begin and end
*/
template <class Iterator>
SSSPTree::SSSPTree(Iterator begin, Iterator end)
{
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
		// it may be the case that p or q lies on the segment st. Points lying on the segment are categorized as "left"
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

void SSSPTree::createTreeFromRoot(Point_2 r, Segment_2 st)
{
	// wip and wi are vectors that in i-th iteration store points/vertices with dist=i
	// we want to store and return points of Point_2 type, but we also need vertex handles
	// in order to retrieve neighbours of a vertex in DT
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

	categorize(&l0, &l1, &r0, &r1, rPoint, st);
	wi_1_delaunayVertices.push_back(rVertex);
	int i = 1;
	int kount = 0;
	while (wi_1_delaunayVertices.size() > 0) {
		VoronoiDiagram vd_nearestNeighbour;
		// biult VD is stored as DT on a set of dt vertices, which represents a subset of DT of all points
		vd_nearestNeighbour.insert(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end());
		deque<Delaunay_vertex_handle> queue;
		copy(wi_1_delaunayVertices.begin(), wi_1_delaunayVertices.end(), back_inserter(queue));
		vector<Delaunay_vertex_handle> wi_delaunayVertices;
		while (queue.size() > 0) {
			Delaunay_vertex_handle q = queue.front();
			queue.pop_front();
			DT::Vertex_circulator p = dt.incident_vertices(q), done(p);
			do {
				// besides usual vertices, DT also stores an infinite vertex, which we have to exclude
				if (dt.is_infinite(p->handle()) == true) { continue; }
				Point_2 *pPoint; //popravi
				pPoint = &p->point();
				if (!pPoint->getVisited()) {
					tuple<bool, Point_2*> nearest = vd_nearestNeighbour.query(*pPoint);
					if (get<0>(nearest) == true) {
						shared_ptr<Point_2> par2 = make_shared<Point_2>(*(get<1>(nearest)));

						wi_delaunayVertices.push_back(p);
						queue.push_back(p);

						pPoint->setVisited(true);
						pPoint->setDist(i);
						//pPoint->setNr(updateNr(par2->getNr(), *pPoint, *par2, st));
						pPoint->setParent(par2);
						//cout << pPoint->x() << "," << par2->x() << " - " << pPoint->y() << "," << par2->y() << endl;
						kount++;
						//categorize(&l0, &l1, &r0, &r1, pPoint, st);
					}
				}
			} while (++p != done);
		}

		wi_1_delaunayVertices.clear();
		wi_1_delaunayVertices = wi_delaunayVertices;

		i++;
	}
	//cout << "connected: " << kount << endl;
	/*for (auto v = dt.vertices_begin(); v != dt.vertices_end(); ++v) {
		if (!v->point().getVisited()) {
			cout << "not v: " << v->point() << endl;
		}
	}*/
}

vector< vector<Point_2> > SSSPTree::getAllSets()
{
	vector< vector<Point_2> > allSets;
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


void testSSSPT(vector<Point_2> points)
{
	CGAL::Timer cost;
	double totalQueryTime = 0;
	double totalConstructionTime = 0;
	Segment_2 st(Point_2(2, 0.3), Point_2(2, 0.7));
	for (int k = 0; k < 5; ++k) {
		//cout << "Starting construction of graph G." << endl;
		cost.reset(); cost.start();
		SSSPTree tree(points.begin(), points.end());
		cost.stop();
		//cout << "SSSP tree construction finished, dt size is " << tree.getDT().number_of_vertices() << ", time: " << cost.time() << endl;
		totalConstructionTime += cost.time();
		size_t pointsLength = points.size();
		cost.reset();
		for (int i = 0; i < 50; ++i) {
			int idx = ceil(rand()*pointsLength / RAND_MAX);
			//cout << "idx: " << idx << endl;
			Point_2 p = points[idx];
			cost.start();
			tree.createTreeFromRoot(p, st);
			cost.stop();
			//cout << p << endl;
			
			//cout << "sets" << endl;
				
			//cout << tree.getAllSets().at(0).size() << endl;
			//cout << tree.getAllSets().at(1).size() << endl;
			//cout << tree.getAllSets().at(2).size() << endl;
			//cout << tree.getAllSets().at(3).size() << endl;
			//for (auto n : nodes) {
			//	cout << (*n).p << " " << (*n).dist << endl;
			//}
			tree.resetSSSPTreeDTVertices();
			tree.clearSets();
		}
		//cout << "Average time (50 iterations) of bfs algorithm on DT: " << cost.time()/50.0 << endl;
		totalQueryTime += (cost.time()/50.0);
	}
	cout << "Average time (5 iterations) for construction of DT: " << totalConstructionTime / 5.0 << endl;
	cout << "Average time (5 iterations) for running 50 iterations of bfs algortithm on DT: " << totalQueryTime / 5.0 << endl;
}
