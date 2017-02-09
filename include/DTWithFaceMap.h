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
#include <CGAL/spatial_sort.h>
#include <CGAL/Line_2.h>
#include <CGAL/Direction_2.h>
#include <CGAL/spatial_sort.h>
#include <CGAL/Spatial_sort_traits_adapter_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_hierarchy_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Lazy_exact_nt.h>

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
typedef DT::Vertex_iterator			  DH_Vertex_iterator;
typedef DT::Locate_type				  DH_Locate_type;
typedef DT::Face_handle				  DH_face_handle;
typedef DT::Vertex_circulator         DH_vertex_circulator;

struct MyLessX {
	bool operator()(const DH_vertex_handle& p, const DH_vertex_handle& q) const
	{
		return p->point().x() < q->point().x();
	}
};

struct MyLessY {
	bool operator()(const DH_vertex_handle& p, const DH_vertex_handle& q) const
	{
		return p->point().y() < q->point().y();
	}
};

struct MySpatialSortingTraits {
	typedef MyLessX Less_x_2;
	typedef MyLessY Less_y_2;

	Less_x_2 less_x_2_object() const
	{
		return Less_x_2();
	}
	Less_y_2 less_y_2_object() const
	{
		return Less_y_2();
	}
};

struct DT_property_map
{
	typedef DH_vertex_handle key_type; ///< typedef to `T`
	typedef Point_2 value_type; ///< typedef to `T`
	typedef Point_2& reference; ///< typedef to `T&`
	typedef boost::lvalue_property_map_tag category; ///< `boost::lvalue_property_map_tag`
	/// Access a property map element.
	/// @param k a key which is returned as mapped value.
	reference operator[](key_type& k) const { return k->point(); }

	typedef DT_property_map Self;
	/// \name Put/get free functions
	/// @{
	friend const value_type& get(const Self&, const key_type& k) { return k->point(); }
	friend         reference get(const Self&, key_type& k) { return k->point(); }
	friend void put(const Self&, key_type& k, const value_type& v) { k->point() = v; }
	/// @}
};

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
	
	ptrdiff_t insert(vector<DH_vertex_handle> points) {
		size_type n = this->number_of_vertices();
		MySpatialSortingTraits sst;
		/*CGAL::Spatial_sort_traits_adapter_2<EK, DT_property_map> traits;
		CGAL::spatial_sort(points.begin(), points.end(), traits);*/
		Face_handle f;
		for (vector<DH_vertex_handle>::iterator p = points.begin(); p != points.end(); ++p) {
			f = DT::insert((*p)->point(), f)->face();
			pointsToFaceHandlers[&((*p)->point())] = f;
		}

		return this->number_of_vertices() - n;
	}
};
