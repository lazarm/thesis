#include <CGAL/Timer.h>
#include <separationMain.h>
#include <thread>
#include <chrono>

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

int main(int argc, char* argv[]) {
	// first two points are used for st segment!
	vector<Point_2> allPoints = readFromFile(string(argv[1]));
	int holes = 1;
	int size = 8;
	if (string(argv[1]).find("-4-") != std::string::npos) {
		holes = 4;
	}
	if (string(argv[1]).find("-16-") != std::string::npos) {
		size = 16;
	}
	else if (string(argv[1]).find("-32-") != std::string::npos) {
		size = 32;
	}
	else if (string(argv[1]).find("-64-") != std::string::npos) {
		size = 64;
	}
	else if (string(argv[1]).find("-128-") != std::string::npos) {
		size = 128;
	}
	int k = log2(size / 4);
	int x1, y1, y2;
	y2 = size / 4 + 2;
	if (holes == 1) {
		x1 = size / 2;
		y1 = size / 8;
	}
	else {
		x1 = size / 4;
		y1 = pow(2, k) - pow(2, k - 1) / 3 - pow(2, k - 2) / 2;
		if (string(argv[1]).find("narrow") != std::string::npos) {
			y1 = pow(2, k) - 2 - (pow(2, k - 1) - 3) / 2;
		}
	}
	Segment_2 st(Point_2(x1, y1), Point_2(x1, y2));
	cout << "st " << st << endl;
	vector<Point_2> readPoints(allPoints.begin(), allPoints.end());
	istringstream ss2(argv[2]);
	int repeats;
	if (!(ss2 >> repeats))
		cerr << "Invalid number " << argv[3] << '\n';
	/*for (int i = 0; i < repeats; i++) {
		testSepGeneral(readPoints, st);
	}*/
	
	std::srand(unsigned(std::time(0)));
	std::random_shuffle(readPoints.begin(), readPoints.end());
	for (int i = 0; i < repeats; i++) {
		main_procedure(readPoints.begin(), readPoints.end(), st);
	}
}