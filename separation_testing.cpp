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
	Segment_2 st2(allPoints[0], allPoints[1]);
	vector<Point_2> readPoints(allPoints.begin() + 2, allPoints.end());
	istringstream ss2(argv[2]);
	int repeats;
	if (!(ss2 >> repeats))
		cerr << "Invalid number " << argv[3] << '\n';
	// TODO: test method for separation algorithm
	main_procedure(readPoints.begin(), readPoints.end(), st2);
	testSepGeneral(readPoints, st2);
}