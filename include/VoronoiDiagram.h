// standard includes
#include <iostream>
#include <fstream>
#include <cassert>
#include <vector>
// includes for defining the Voronoi diagram adaptor
#include <CGAL/Cartesian.h>
#include <CGAL/Homogeneous.h>
#include <CGAL/Point_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Line_2.h>
#include <CGAL/Direction_2.h>
#include <CGAL/spatial_sort.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/squared_distance_2.h>

//typedef CGAL::Homogeneous<long> Rep_class;
typedef CGAL::Cartesian<double> Rep_class;

// typedefs for defining the adaptor
typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
typedef CGAL::Delaunay_triangulation_2<Rep_class>                            DT;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT, AT, AP>                                    VD;
// typedef for the result type of the point location

//typedef CGAL::Point_2<Rep_class> Point;
typedef Rep_class::Segment_2		  Segment_2;
typedef Rep_class::Line_2			  Line_2;
typedef Rep_class::Direction_2		  Direction_2;
typedef AT::Site_2                    Site_2;
typedef Rep_class::Point_2             Point_2;
typedef VD::Locate_result             Locate_result;
typedef VD::Vertex_handle             Vertex_handle;
typedef VD::Face_handle               Face_handle;
typedef VD::Halfedge_handle           Halfedge_handle;
typedef VD::Ccb_halfedge_circulator   Ccb_halfedge_circulator;
typedef VD::Face_iterator             Face_iterator;
typedef DT::Vertex_handle             Delaunay_vertex_handle;
typedef DT::Locate_type				  Locate_type;
typedef DT::Face_handle				  Delaunay_face_handle;
typedef DT::Vertex_circulator         Vertex_circulator;



using namespace std;
class VoronoiDiagram: public VD
{	
public:
	VoronoiDiagram() {};
	~VoronoiDiagram() {};
	template <class Iterator>
	VoronoiDiagram(Iterator a, Iterator b) : VD(a, b) {}
	std::tuple<bool, Point_2*> query(Point_2 q);
	int size() { return number_of_faces(); }
	Face_handle insert(const Delaunay_vertex_handle& t);
	Face_handle insert(const Site_2& t);
	template<class Iterator>
	size_type insert(Iterator first, Iterator beyond);

};


tuple<bool, Point_2*> VoronoiDiagram::query(Point_2 q) {
	
	Locate_result lr = locate(q);
	Delaunay_vertex_handle df;
	if (Vertex_handle* v = boost::get<Vertex_handle>(&lr)) {
		df = (*v)->site(0);
	}
	else if (Face_handle* f = boost::get<Face_handle>(&lr)) {
		df = (*f)->dual();
	}
	else if (Halfedge_handle* e = boost::get<Halfedge_handle>(&lr)) {
		df = (*e)->up();
	}
	Point_2 faceSitePoint = df->point();
	Point_2 *fcp = &df->point();
	double dist = CGAL::squared_distance(*fcp, q);
	
	if (dist <= 1) {
		return std::tuple<bool, Point_2*> {true, fcp};
	}
	else {
		return std::tuple<bool, Point_2*> {false, fcp};
	}
}

inline Face_handle VoronoiDiagram::insert(const Delaunay_vertex_handle& t) {

    return VD::insert(t->point(), Has_site_inserter());
}

inline Face_handle VoronoiDiagram::insert(const Site_2& t) {

    return VD::insert(t, Has_site_inserter());
}

template<class Iterator>
inline VD::size_type VoronoiDiagram::insert(Iterator first, Iterator beyond) {
	VD::size_type counter = 0;
	for (Iterator it = first; it != beyond; ++it, ++counter) {
		insert(*it);
	}
	return counter;
}