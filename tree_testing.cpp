#include <CGAL/Timer.h>
#include <thread>
#include <chrono>
#include "TreeTimeTester.h"


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

	vector<Point_2> readPoints = readFromFile(string(argv[1]));
	istringstream ss(argv[2]);
	int constructions;
	if (!(ss >> constructions))
		cerr << "Invalid number " << argv[2] << '\n';
	istringstream ss2(argv[3]);
	int repeats;
	if (!(ss2 >> repeats))
		cerr << "Invalid number " << argv[3] << '\n';
	TreeTimeTester tester(readPoints, constructions, repeats);
	tester.test();
}