
#include <CGAL/Timer.h>
#include <separationMain.h>


int main() {

	/*
	Point_2 a1(0, 0), a2(1, 0), a3(2, 0), a4(3, 0), a5(4, 0);
	Site_2 a1s(0, 0), a2s(1, 0), a3s(2, 0), a4s(3, 0), a5s(4, 0);
	Point_2 pst[5] = { a1, a2, a3, a4, a5 };
	vector<Point_2> pst2;
	vector<GraphNode*> gps;
	GraphNode ag1 = { a1 }, ag2 = { a2 }, ag3 = { a3 }, ag4 = { a4 }, ag5 = { a5 };
	gps.reserve(5005);
	pst2.reserve(5005);
	pst2.push_back(a1), gps.push_back(&ag1);
	pst2.push_back(a2), gps.push_back(&ag2);
	pst2.push_back(a3), gps.push_back(&ag3);
	pst2.push_back(a4), gps.push_back(&ag4);
	pst2.push_back(a5), gps.push_back(&ag5);
	srand(time(0));
	std::ofstream out("out.txt");
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	for (int i = 0; i < 5000; i++)
	{
		double x = (double)rand() / (RAND_MAX);
		double y = ((double)rand() / (RAND_MAX)) * 4;
		pst2.push_back(Point_2(y, x));
		
		GraphNode *xg = new GraphNode(Point_2(y,x));
		gps.push_back(xg);
		cout << y << " " << x << endl;
	}
	
	Segment_2 st(Point_2(2, 0), Point_2(2, 1));
	vector< vector<Point_2> > rst;*/
	double times = 0;
	Point_2 a1(0, 0), a2(0.4, 0.2), a3(1, 0.1), a4(0.1,0.5), a6(0.9,0.6), a7(0.11,1), a8(0.6,1.05), a9(0.97,1.03), a10(0,1.4), a11(0.3,1.3), a12(0.85,1.5), a13(0.04,2), a14(0.62,1.9);
	Point_2 a15(1.07, 1.95), a16(0.02, 2.45), a18(1.07,2.6), a19(0.08, 3.03), a20(0.47, 3), a21(0.98, 3.1);
	Point_2 pst[19] = { a1, a2, a3, a4, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a18, a19, a20, a21 };
	Segment_2 st(Point_2(0.5, 1), Point_2(0.5, 2.3));
	//cout << &a7 << endl;
	vector< vector<Point_2> > rst;
	vector<Point_2> pst2(pst, pst + 19);
	//cout << pst2.size() << endl;
	CGAL::Timer cost;
	for (int k = 0; k < 1; k++) {
		//cost.reset(); cost.start();
		rst.clear();
		
		//rst = constructW(pst2.begin(), pst2.end(), a7, st);
	}
	
	cost.stop();
	//std::cout << "SSSP done in " << cost.time() / 1.0 << " seconds. " << pst2.size() << std::endl;
	/*cout << rst.at(0).size() << " " << rst.at(1).size() << " " << rst.at(2).size() << " " << rst.at(3).size() << endl;
	cout << rst.at(0).size() + rst.at(1).size() + rst.at(2).size() + rst.at(3).size() << endl;
	int ii = 0;
	for (vector< vector<Point_2> >::iterator tz = rst.begin(); tz != rst.end(); ++tz) {
		std::cout << ii << std::endl;
		++ii;
		for (vector<Point_2>::iterator tz2 = (*tz).begin(); tz2 != (*tz).end(); ++tz2) {
			std::cout << (**tz2).x() << "   " << (**tz2).y() << "  dist: " << (**tz2).getDist() << "  nr: " << (**tz2).getNr() << std::endl;
			cout << *((*tz2)->getParent()) << "  dist: " << (*tz2)->getParent()->getDist() << "  nr: " << (*tz2)->getParent()->getNr() << endl;
		}
	}*/
	//constructG(gps.begin(), gps.end());
	//runBfs(&ag1);
	/*
	for (int i = 0; i < 150005; i++)
	{
		GraphNode* n = *(gps.begin()+i);
		bool tobreak = false;
		for (vector< vector<Point_2> >::iterator tz = rst.begin(); tz != rst.end(); ++tz) {
			for (vector<Point_2>::iterator tz2 = (*tz).begin(); tz2 != (*tz).end(); ++tz2) {
				Point_2* uiz = tz2._Ptr;
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
	*/
	
	cout << pst2.size() << endl;
	for (vector<Point_2>::iterator pi = pst2.begin(); pi != pst2.end(); ++pi) {
		cout << pi->x() << "," << pi->y() << endl;
	}
	main_procedure(pst2.begin(), pst2.end(), st);
}