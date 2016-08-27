
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

vector<Point_2> pointGenerator(int e, int holes, boolean narrow, int size, boolean connected, vector<Point_2> exs)
{
	vector<Point_2> points;
	points.insert(points.end(), exs.begin(), exs.end());
	size_t k = points.size();
	//double x1 = pow(2, e - 1);
	//double y1 = pow(2, e - 1);
	//cout << x1 << ", " << y1 << endl;
	//points.push_back(Point_2(x1, y1));
	//k++;
	while (k < size) { 
		double x = pow(2, e + 2) * ((double)rand() / (RAND_MAX));
		double y = pow(2, e) * (double)rand() / (RAND_MAX);
		Point_2 p(x, y);
		if (connected && points.size() > 0) {
			int c = 0;
			boolean skip = false;
			for (auto p2 : points) {
				if (CGAL::squared_distance(p, p2) <= 1) {
					c++;
					if (c > 5 && points.size() < 999)
						skip = true;
					else if (c > 5 && points.size() >= 999)
						skip = true;
					if (skip) break;
				}
			}
			if (c == 0 || skip) {
				continue;
			}
		}
		if (holes == 0) {
			
			if (!connected || points.size() < 1 ) {
				//cout << p << endl;
				//this_thread::sleep_for(chrono::seconds(5));
				points.push_back(p);
				k++;
				//cout << k << endl;
			}
			else {
				int c = 0;
				for (auto p2 : points) {
					if (CGAL::squared_distance(p, p2) <= 1) {
						//points.push_back(p);
						//k++;
						//cout << k << endl;
						//break;
						c++;
						if (points.size() < 500 && c > 5) { break; }
						else if (points.size() >= 500 && c > 5) {
							//cout << "too much" << endl;
							break;
						}
					}
				}
				if (c > 0 && ((points.size() < 500 && c <= 5) || (points.size() >= 500 && c <= 5))) {
					points.push_back(p);
					k++;
					cout << k << endl;
				}
			}
		}
		else if (holes == 1) {
			if (narrow) {
				if (x < 2 || x > pow(2, e + 2) - 2 || y < 2 || y > pow(2, e) - 2) {
					points.push_back(Point_2(x, y));
					k++;
					//cout << k << endl;
				}
			}
			else {
				//cout << x << "," << y << endl;
				if (x < pow(2, e) || x > 3 * pow(2, e) || y < pow(2, e - 2) || y > 3 * pow(2, e - 2)) {
					points.push_back(Point_2(x, y));
					k++;
					//cout << k << endl;
				}
			}
		}
		else if (holes == 4) {
			if (narrow) {
				if (x < 2 || (x > pow(2, e + 1) - 1 && x < pow(2, e + 1) + 1) || x > pow(2, e + 2) - 2 ||
					y < 2 || (y > pow(2, e - 1) - 1 && y < pow(2, e - 1) + 1) || y > pow(2, e) - 2) {
					points.push_back(Point_2(x, y));
					k++;
				}
			}
			else {
				if (x < pow(2, e+1) / 3 || (x > 5*pow(2, e) / 3 && x < 7 * pow(2, e) / 3) || x > 5*pow(2, e + 1) / 3 ||
					y < pow(2, e - 1) / 3 || (y > 5 * pow(2, e - 2) / 3 && y < 7 * pow(2, e - 2) / 3) || y > 5 * pow(2, e - 1) / 3) {
					points.push_back(Point_2(x, y));
					k++;
				}
			}
		}
	}
	return points;
}


void writeToFile(vector<Point_2> points, int space, int holes, boolean narrow)
{
	ofstream myfile;
	size_t size = points.size();
	string filename = "points-" + to_string(space) + "-" + to_string(holes) + "-" + to_string(size);
	if (narrow) filename += "-narrow.txt";
	else filename += ".txt";
	myfile.open(filename);
	for (auto p : points) {
		myfile << p.x() << " " << p.y() << endl;
	}
	myfile.close();
}

vector<Point_2> readFromFile(string filename) {
	ifstream ifs(filename);
	assert(ifs);
	vector<Point_2> points;
	Point_2 p;
	while (ifs >> p) { 
		points.push_back(p); 
	}
	ifs.close();
	return points;
}

int main() {

	Point_2 a1(0, 0), a2(1, 0), a3(2, 0), a4(3, 0), a5(4, 0), a6(0.5, 0), a7(1.5, 0), a8(2.5,0), a9(3.5,0);
	Site_2 a1s(0, 0), a2s(1, 0), a3s(2, 0), a4s(3, 0), a5s(4, 0);
	Point_2 pst[9] = { a1, a2, a3, a4, a5, a6, a7, a8, a9 };
	vector<Point_2> pst2;
	vector<shared_ptr<GraphNode>> gps;
	GraphNode ag1 = { a1 }, ag2 = { a2 }, ag3 = { a3 }, ag4 = { a4 }, ag5 = { a5 };
	//gps.reserve(2009);
	vector<Point_2> bfs_points{ a1, a2, a3, a4, a5, a6 };
	//testBfsGrid(pst2);
	vector<int> le{ 4, 8, 16, 32, 64, 128 };
	/*for (int i = 0; i < 6; i++) {
		vector<Point_2> gend = pointGenerator(i, 4, false, 2000, false, vector<Point_2>{});
		writeToFile(gend, le[i], 4, false);
	}*/
	//testBfsGrid(bfs_points);
	vector<int> ss{ 128, 64, 32};
	for (auto a : ss) {
		vector<Point_2> readPoints = readFromFile("points-" + to_string(a) + "-4-5000-narrow.txt");
		Segment_2 st2(Point_2(a/4, a/4-2.5), Point_2(a/4, a/4+1));
		//Segment_2 st2(Point_2(3 * a / 4, a / 24.0 + 0.5), Point_2(3 * a / 4, a / 4 - (a / 24.0 + 0.5)));
		cout << st2.source() << "   " << st2.end() << endl;
		//testSepGeneral(readPoints, st2);
		main_procedure(readPoints.begin(), readPoints.end(), st2);
		//cout << readPoints.size() << endl;
		//cout << bfs_points.size() << endl;
		//testSSSPT(readPoints);
		//testBfs(readPoints);
		//testBfsGrid(readPoints);
		//Segment_2 st(Point_2(16, 4), Point_2(32, 10));
		//main_procedure(readPoints.begin(), readPoints.end(), st);
	}
	//testSepGeneral(bfs_points, st);
	//vector<Point_2> readPoints = readFromFile("points-32-0-2000.txt");
	//testSSSPT(readPoints);
	vector<Point_2> readPoints = readFromFile("points-32-1-11000-stuffed.txt");
	//testSSSPT(readPoints);
	//testBfs(readPoints);
	//testBfsGrid(readPoints);
	Segment_2 st2(Point_2(16, 4), Point_2(16, 9));
	//main_procedure(readPoints.begin(), readPoints.end(), st2);
	//testSepGeneral(readPoints, st2);
	//testSeparationBruteForce(readPoints.begin(), readPoints.end(), st2);
}

// CpuUsage.cpp : Defines the entry point for the console application.
//

