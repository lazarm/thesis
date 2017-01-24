Face_handle getFaceFromPoint(const Point_2* p) {
 return pointsToFaceHandlers[p];
}

ptrdiff_t insert(vector<DH_vertex_handle> points) {
 size_type n = this->number_of_vertices();
 Face_handle f;
 for (vector<DH_vertex_handle>::iterator p = points.begin(); p != points.end(); ++p) {
  f = DT::insert((*p)->point(), f)->face();
  pointsToFaceHandlers[&((*p)->point())] = f;
 }
 return this->number_of_vertices() - n;
}
