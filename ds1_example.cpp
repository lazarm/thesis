
#include <CGAL/Timer.h>
#include <separationMain.h>
#include <thread>
#include <chrono>

void cpuKd(vector<Point_2> points, int k, int s) {
	cout << "cpu for kd tree." << endl;
	CGAL::Timer cost;
	cost.reset(); cost.start();
	for (int i = 0; i < k; i++) {
		shared_ptr<KDTree> a(new KDTree(points.begin(), points.begin() + s));
	}
	cost.stop();
	cout << "Storing " << k << " kd trees of size " << s << " took total " << cost.time() << " time, and on average " << cost.time() / k;
}

void cpuVd(vector<Point_2> points, int k, int s) {
	cout << "cpu for vd tree." << endl;
	CGAL::Timer cost;
	cost.reset(); cost.start();
	for (int i = 0; i < k; i++) {
		VD vd(points.begin(), points.begin() + s);
	}
	cost.stop();
	cout << "Storing " << k << " VDs of size " << s << " took total " << cost.time() << " time, and on average " << cost.time() / k << endl;
}

void ramKd(vector<Point_2> points, int k, int s) {
	vector<shared_ptr<KDTree>> treeList;
	cout << "ram for kd tree, check current ram" << endl;
	this_thread::sleep_for(chrono::seconds(10));
	for (int i = 0; i < k; i++) {
		treeList.push_back(shared_ptr<KDTree>(new KDTree(points.begin(), points.begin() + s)));
	}
	cout << "finished kd, check ram again" << endl;
	this_thread::sleep_for(chrono::seconds(10));
}

void ramVd(vector<Point_2> points, int k, int s) {
	vector<VD> vdList;
	cout << "ram for vd, check current ram" << endl;
	this_thread::sleep_for(chrono::seconds(10));
	for (int i = 0; i < k; i++) {
		//VD vd(points.begin(), points.begin() + s);
		vdList.push_back(VD(points.begin(), points.begin() + s));
	}
	cout << "finished vd, check ram again" << endl;
	this_thread::sleep_for(chrono::seconds(10));
}

vector<Point_2> readPointsFromFile(string file) {
	vector<Point_2> points;
	ifstream ifs(file);
	assert(ifs);
	
	Point_2 p;
	while (ifs >> p) { points.push_back(p); }
	return points;
}

int main() {

	Point_2 a1(0, 0), a2(1, 0), a3(2, 0), a4(3, 0), a5(4, 0), a6(0.5, 0), a7(1.5, 0), a8(2.5,0), a9(3.5,0);
	Site_2 a1s(0, 0), a2s(1, 0), a3s(2, 0), a4s(3, 0), a5s(4, 0);
	Point_2 pst[9] = { a1, a2, a3, a4, a5, a6, a7, a8, a9 };
	vector<Point_2> pst2;
	vector<GraphNode*> gps;
	GraphNode ag1 = { a1 }, ag2 = { a2 }, ag3 = { a3 }, ag4 = { a4 }, ag5 = { a5 };
	//gps.reserve(2009);
	pst2.reserve(25009);
	pst2.push_back(a1), gps.push_back(&ag1);
	pst2.push_back(a6), gps.push_back(&ag2);
	pst2.push_back(a2), gps.push_back(&ag3);
	pst2.push_back(a7), gps.push_back(&ag4);
	pst2.push_back(a3), gps.push_back(&ag5);
	pst2.push_back(a8);
	pst2.push_back(a4);
	pst2.push_back(a9);
	pst2.push_back(a5);
	srand(time(0));
	//std::ofstream out("out.txt");
	//std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	//std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	for (int i = 0; i < 1000000; i++)
	{
		double y = (double)rand() / (RAND_MAX);
		double x = ((double)rand() / (RAND_MAX)) * 4;
		pst2.push_back(Point_2(x, y));
		
		//GraphNode *xg = new GraphNode(Point_2(y,x));
		//gps.push_back(xg);
		//cout << y << " " << x << endl;
	}
	
	Segment_2 st(Point_2(2, 0.3), Point_2(2, 0.6));
	vector< vector<Point_2> > rst;
	


	vector<Point_2> bfs_points { a1, a2, a3, a4, a5, a6 };
	testBfs(bfs_points, 0);
}