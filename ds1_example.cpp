
#include <CGAL/Timer.h>
#include <bfs.h>


int main() {

	Site_2 p1(0, 1), p2(4, 0), p3(0, 0), p4(4, 3);
	Site_2 points[4] = { p1, p2, p3, p4 };
	Point_2 trt(p1);
	/*
	Site_2 points2[1000];
	for (int i = 0; i < 1000; i++) {
	double x = -1000 + i;
	points2[i] = Site_2(x, x*x);
	}*/
	CGAL::Timer cost;
	std::vector<Site_2> points2;
	for (int i = 0; i < 1000; ++i) {
		double x = -1000 + i;
		points2.push_back(Site_2(x, x*x));
	}
	cost.reset(); cost.start();
	//CGAL::spatial_sort(points2.begin(), points2.end());
	DS1 < std::vector<Site_2>::iterator > structure;
	structure.construct(points2.begin(), points2.end());
	cost.stop();
	std::cout << "ds1 done in " << cost.time() << " seconds." << std::endl;
	Point_2 q(1, 0);
	q.setDist(2);
	Site_2 tui(q);
	cout << "tui: " << tui.getDist() << endl;
	std::tuple<bool, Point_2> res = structure.query(q);
	bool success = std::get<0>(res);
	Point_2 ps = std::get<1>(res);
	std::cout << success << " " << ps.x() << ", " << ps.y() << std::endl;
	std::cout << "tojeto " << q.getDist() << std::endl;


	vector<Site_2> points3(points, points + 4);
	cost.reset(); cost.start();
	DS2<vector<Site_2>::iterator> ds2;
	ds2.construct(points2.begin(), points2.end());
	cost.stop();
	std::cout << "ds2 done in " << cost.time() << " seconds." << std::endl;
	Point_2 q2(-900, 810000.2);
	tuple<bool, Point_2> rez = ds2.search(q2);

	Point_2 rezp = std::get<1>(rez);
	std::cout << "ds2 : " << rezp.x() << ", " << rezp.y() << std::endl;

	Point_2 a1(0, 0), a2(1, 0), a3(2, 0), a4(3, 0), a5(4, 0);
	Point_2 pst[5] = { a1, a2, a3, a4, a5 };
	vector<Point_2> pst2;
	vector<GraphNode*> gps;
	GraphNode ag1 = { a1 }, ag2 = { a2 }, ag3 = { a3 }, ag4 = { a4 }, ag5 = { a5 };
	gps.reserve(105);
	pst2.reserve(105);
	pst2.push_back(a1), gps.push_back(&ag1);
	pst2.push_back(a2), gps.push_back(&ag2);
	pst2.push_back(a3), gps.push_back(&ag3);
	pst2.push_back(a4), gps.push_back(&ag4);
	pst2.push_back(a5), gps.push_back(&ag5);
	srand(time(0));
	for (int i = 0; i < 100; i++)
	{
		double x = (double)rand() / (RAND_MAX);
		double y = ((double)rand() / (RAND_MAX)) * 4;
		pst2.push_back(Point_2(y, x));
		
		GraphNode *xg = new GraphNode(Point_2(y,x));
		gps.push_back(xg);
	}
	cost.reset(); cost.start();

	Segment_2 st(Point_2(2, 0), Point_2(2, 1));
	vector< vector<Point_2> > rst = constructW(pst2.begin(), pst2.end(), a1, st);
	
	constructG(gps.begin(), gps.end());
	runBfs(&ag1);
	
	for (int i = 0; i < 105; i++)
	{
		GraphNode* n = *(gps.begin()+i);
		bool tobreak = false;
		for (vector< vector<Point_2> >::iterator tz = rst.begin(); tz != rst.end(); ++tz) {
			for (vector<Point_2>::iterator tz2 = (*tz).begin(); tz2 != (*tz).end(); ++tz2) {
				if ((*tz2).x() == n->p.x() && (*tz2).y() == n->p.y()) {
					if ((*gps.at(i)).dist != (*tz2).getDist()) {
						cout << n->dist << " +-+ " << (*tz2).getDist() << endl;
						cout << n->p.x() << "  xy" << i << "  " << n->p.y() << endl;
					}
					tobreak = true;
					break;
				}
			}
			if (tobreak) break;
		}
	}
	
	cout << rst.at(0).size() << " " << rst.at(1).size() << " " << rst.at(2).size() << " " << rst.at(3).size() << endl;

	int ii = 0;
	for (vector< vector<Point_2> >::iterator tz = rst.begin(); tz != rst.end(); ++tz) {
		std::cout << ii << std::endl;
		++ii;
		for (vector<Point_2>::iterator tz2 = (*tz).begin(); tz2 != (*tz).end(); ++tz2) {
			std::cout << (*tz2).x() << "   " << (*tz2).y() << "  dist: " << (*tz2).getDist() << "  nr: " << (*tz2).getNr() << std::endl;
		}
	}

	cost.stop();
	std::cout << "SSSP done in " << cost.time() / 1 << " seconds. " << pst2.size() << std::endl;

	tuple<Point_2, Point_2, int> pair = findMinPair(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end());
	cout << "RESULT FI: " << get<0>(pair) << " ,dist: " << get<0>(pair).getDist() << " |  " << get<1>(pair) << " ,dist: " << get<1>(pair).getDist() << "  weight " << get<2>(pair) << endl;
	tuple<Point_2, Point_2, int> pair2 = bruteForce(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end());
	cout << "RESULT bruteForce: " << get<0>(pair2) << " ,dist: " << get<0>(pair2).getDist() << " |  " << get<1>(pair2) << " ,dist: " << get<1>(pair2).getDist() << "  weight " << get<2>(pair2) << endl;

	tuple<Point_2, Point_2, int> pair3 = bruteForceSegment(rst.at(0).begin(), rst.at(0).end(), rst.at(3).begin(), rst.at(3).end(), st);
	cout << "RESULT bruteForceSegment: " << get<0>(pair3) << " ,dist: " << get<0>(pair3).getDist() << " |  " << get<1>(pair3) << " ,dist: " << get<1>(pair3).getDist() << "  weight " << get<2>(pair3) << endl;	

}