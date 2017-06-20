#include <DTWithFaceMap.h>

struct DualPoint
{
	Point_2 point;
	Point_2* originalPoint;

	DualPoint(){};
	DualPoint(Point_2* p, Segment_2 st)
	{
		point = imagePoint(*p, st);
		originalPoint = p;
	}

	Point_2 imagePoint(Point_2 b, Segment_2 st)
	{
		Line_2 line_s(b, st.source());
		Direction_2 direction_s(line_s);
		Line_2 line_t(b, st.target());
		Direction_2 direction_t(line_t);

		Point_2 bFI(direction_s.dy() / direction_s.dx(), direction_t.dy() / direction_t.dx());
		//cout << bFI.x() << " -- " << bFI.y() << endl;
		return bFI;
	}
};