#include <VoronoiDiagram.h>
#include <tuple>

class GridGraph
{
private:
	// first 8 points in the vector of every key are neighbouring cell points, 
	// others are points contained in the cell the key represents
	unordered_map<tuple<int, int>, vector<Point_2>> cells;
public:
	GridGraph(){};
	~GridGraph(){};
	template <class Iterator>
	void insert(Iterator begin, Iterator end);
};

template <class Iterator>
void GridGraph::insert(Iterator begin, Iterator end)
{
	tuple<int, int> dim = getGridDimension(begin, end);
	for (auto p = begin; p != end; ++p)
	{
		int xfloor = floor(p->x());
		int yfloor = floor(p->y());
		tuple<int, int> cellPoint = make_tuple<int, int>(floor(p->x()), floor(p->y()));
		if (!cells.count(cellPoint)) {
		    cells.emplace(cellPoint, vector<Point_2>());
		}
		cells.at(cellPoint).push_back(*p);
	}
}

template <class Iterator>
tuple<int, int> getGridDimension(Iterator begin, Iterator end)
{
	double minx = numeric_limits<int>::max();
	double maxx = numeric_limits<int>::min();
	double miny = numeric_limits<int>::max();
	double maxy = numeric_limits<int>::min();

	for (auto p = begin; p != end; ++p)
	{
		double px = p->x();
		double py = p->y();
		if (px > maxx) { maxx = px; }
		if (px < minx) { minx = px; }
		if (py > maxy) { maxy = py; }
		if (py < miny) { miny = py; }
	}
	int xdim = ceil(maxx - minx);
	int ydim = ceil(maxy - miny);
	return tuple<int, int>(xdim, ydim);
}

void gridQuery(GridGraph g, Point_2 r)
{
	//do bfs, neighbours are points from neighbouring cells
}

vector<tuple<int, int>> getNeighbouringCells(GridGraph g, Point_2 v) 
{
	int xfloor = floor(v.x());
	int yfloor = floor(v.y());
	vector<tuple<int, int>> cellPoints
	{ tuple<int, int>(xfloor, yfloor),
	tuple<int, int>(xfloor - 1, yfloor),
	tuple<int, int>(xfloor - 1, yfloor + 1),
	tuple<int, int>(xfloor, yfloor + 1),
	tuple<int, int>(xfloor + 1, yfloor + 1),
	tuple<int, int>(xfloor + 1, yfloor),
	tuple<int, int>(xfloor + 1, yfloor - 1),
	tuple<int, int>(xfloor, yfloor - 1),
	tuple<int, int>(xfloor - 1, yfloor - 1) };
	return cellPoints;
}