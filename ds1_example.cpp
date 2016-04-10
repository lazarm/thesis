
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
				cout << k << endl;
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
					cout << k << endl;
				}
			}
			else {
				//cout << x << "," << y << endl;
				if (x < pow(2, e) || x > 3 * pow(2, e) || y < pow(2, e - 2) || y > 3 * pow(2, e - 2)) {
					points.push_back(Point_2(x, y));
					k++;
					cout << k << endl;
				}
			}
		}
		else if (holes == 4) {
			if (narrow) {
				if (x < 2 || (x > pow(2, e + 2) - 1 && x < pow(2, e + 2) + 1) || x > pow(2, e + 2) - 2 ||
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
	while (ifs >> p) { points.push_back(p); }
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
	pst2.reserve(5000);
	/*
	pst2.push_back(a1), gps.push_back(&ag1);
	pst2.push_back(a6), gps.push_back(&ag2);
	pst2.push_back(a2), gps.push_back(&ag3);
	pst2.push_back(a7), gps.push_back(&ag4);
	pst2.push_back(a3), gps.push_back(&ag5);
	pst2.push_back(a8); gps.push_back(&ag6);
	pst2.push_back(a4); gps.push_back(&ag7);
	pst2.push_back(a9); gps.push_back(&ag8);
	pst2.push_back(a5); gps.push_back(&ag9);
	*/
	srand(time(0));
	//std::ofstream out("out.txt");
	//std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
	//std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
	for (int i = 0; i < 5000; i++)
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
	

	
	vector<Point_2> bfs_points{ a1, a2, a3, a4, a5, a6 };
	//testBfsGrid(pst2);
	/*vector<int> le{ 4, 8, 16};
	for (int i = 0; i < 3; i++) {
		vector<Point_2> gend = pointGenerator(i, 1, true, 1000, false, vector<Point_2>{});
		writeToFile(gend, le[i], 1, true);
	}*/
	
	vector<int> ss{ 128, 64, 32, 16, 8, 4};
	for (auto a : ss) {
		vector<Point_2> readPoints = readFromFile("points-" + to_string(a) + "-0-5000.txt");
		testBfsGrid(readPoints);
		Segment_2 st(Point_2(7, 2), Point_2(7, 5));
		//main_procedure(readPoints.begin(), readPoints.end(), st);
	}
	//vector<Point_2> readPoints = readFromFile("points-32-0-2000.txt");
	//testSSSPT(readPoints);
	
	/*double x1 = 0, y1 = 0;
	//double xs = x1, ys = y1;
	vector<Point_2> create1000_128;
	int kc = 0;
	for (int i = 1; i < 256; i++) {
		double yss = 1 + 0.2*((double)rand() / (RAND_MAX)) - 0.1;
		create1000_128.push_back(Point_2(x1 + i*0.5, yss));
		double yss2 = 31 + 0.2*((double)rand() / (RAND_MAX)) - 0.1;
		create1000_128.push_back(Point_2(x1 + i*0.5, yss2));
	}
	for (int i = 1; i < 64; i++) {
		double xss = 1 + 0.2*((double)rand() / (RAND_MAX)) - 0.1;
		create1000_128.push_back(Point_2(xss, y1 + i*0.5));
		double xss2 = 127 + 0.2*((double)rand() / (RAND_MAX)) - 0.1;
		create1000_128.push_back(Point_2(xss2, y1 + i*0.5));
	}*/
	/*/
	while (xs < 128) {
		ys = y1;
		while (ys < 16) {
			double xss = xs + 0.5 * ((double)rand() / (RAND_MAX)) - 0.25;
			create2000_64.push_back(Point_2(xss, ys));
			kc++;
			//cout << kc << endl;
			ys = ys + 0.5 + 0.7 * ((double)rand() / (RAND_MAX));
			//cout << ys << endl;
		}
		xs = xs + 0.9 + 0.3 * ((double)rand() / (RAND_MAX));
	}*/
	/*vector<Point_2> gend = pointGenerator(5, 1, true, 1000, true, create1000_128);
	int z = 0;
	while (create1000_128.size() < 1000) {
		create1000_128.push_back(gend[z]);
		z++;
	}
	writeToFile(create1000_128, 128, 1, true);*/
}