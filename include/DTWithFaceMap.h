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
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_hierarchy_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Lazy_exact_nt.h>

//typedef CGAL::Homogeneous<long> Rep_class;
typedef CGAL::Cartesian<double> EK;
typedef CGAL::Lazy_exact_nt<CGAL::Gmpq>  NT;

typedef CGAL::Triangulation_vertex_base_2<EK>             Vbb;
typedef CGAL::Triangulation_hierarchy_vertex_base_2<Vbb> Vb;
typedef CGAL::Triangulation_face_base_2<EK>               Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>      Tds;
typedef CGAL::Delaunay_triangulation_2<EK, Tds>                                    DT;
typedef CGAL::Triangulation_hierarchy_2<DT>									DTHier;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
typedef CGAL::Voronoi_diagram_2<DT, AT, AP>                                    VD;
// typedef for the result type of the point location

//typedef CGAL::Point_2<Rep_class> Point;
typedef EK::Segment_2		  Segment_2;
typedef EK::Line_2			  Line_2;
typedef EK::Direction_2		  Direction_2;
typedef AT::Site_2                    Site_2;
typedef EK::Point_2             Point_2;
typedef VD::Locate_result             Locate_result;
typedef VD::Vertex_handle             Vertex_handle;
typedef VD::Face_handle               Face_handle;
typedef VD::Halfedge_handle           Halfedge_handle;
typedef VD::Ccb_halfedge_circulator   Ccb_halfedge_circulator;
typedef VD::Face_iterator             Face_iterator;
typedef DT::Vertex_handle             DH_vertex_handle;
typedef DT::Locate_type				  DH_Locate_type;
typedef DT::Face_handle				  DH_face_handle;
typedef DT::Vertex_circulator         DH_vertex_circulator;


using namespace std;
class DTWithFaceMap: public DT
{
private:
	unordered_map < const Point_2*, Face_handle> pointsToFaceHandlers;
public:
	DTWithFaceMap() {};
	~DTWithFaceMap() {};
	
	template <class Iterator>
	DTWithFaceMap(Iterator a, Iterator b) : DT(a, b) {}
	Face_handle getFaceFromPoint(const Point_2* p) { return pointsToFaceHandlers[p]; }
	
	template < class Iterator >
	ptrdiff_t insert(Iterator first, Iterator last) {
		size_type n = this->number_of_vertices();
		vector<Point_2> points;
		for (Iterator it = first; it != last; ++it) {
			points.push_back((*it)->point());
		}
		spatial_sort(points.begin(), points.end(), geom_traits());
		Face_handle f;
		for (typename vector<Point_2>::const_iterator p = points.begin(), end = points.end();
			p != end; ++p) {
			f = DT::insert(*p, f)->face();
			pointsToFaceHandlers[&(*p)] = f;
		}

		return this->number_of_vertices() - n;
	}
};
