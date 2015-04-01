#include<DS3.h>

class CustomPoint
{
private:
	Point_2 point_2;
	Point_2 *parent;
	int dist;
	int nr;

public:
	CustomPoint() {};
	~CustomPoint() {};

	CustomPoint(double x, double y)
	{
		point_2 = Point_2(x,y);
		parent = NULL;
		nr = 0;
		dist = std::numeric_limits<int>::max();
	}

	int getDist() { return dist; }
	int getNr() { return nr; }
	int setDist(int d) { dist = d; }
	int setNr(int nrp) { nr = nrp; }
	Point_2 point() { return point_2; }
};