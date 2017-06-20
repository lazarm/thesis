#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include <CGAL/Point_2.h>
#include <CGAL/Cartesian.h>
#include <CGAL/squared_distance_2.h>
#include <CGAL/Lazy_exact_nt.h>

typedef CGAL::Cartesian<double> EK;
typedef EK::Point_2             Point_2;

using namespace std;

vector<Point_2> generateClutteredData(int size) {
	vector<Point_2> points;
	int k = 0;
	while (k < size) {
		double x = 15.5 + ((double)rand() / (RAND_MAX));
		double y = 6 + 2 * ((double)rand() / (RAND_MAX));
		points.push_back(Point_2(x, y));
		k++;
	}
	ofstream myfile;
	myfile.open("2Kand-" + to_string(size) + ".txt");
	for (auto p : points) {
		myfile << p.x() << " " << p.y() << endl;
	}
	myfile.close();
}

vector<Point_2> pointGenerator(int e, int holes, bool narrow, int size, bool connected, vector<Point_2> exs)
{
	vector<Point_2> points;
	points.insert(points.end(), exs.begin(), exs.end());
	size_t k = points.size();
	int currSize = 0;
	while (k < size) {
		double x = pow(2, e + 2) * ((double)rand() / (RAND_MAX));
		double y = pow(2, e) * (double)rand() / (RAND_MAX);
		Point_2 p(x, y);
		if (connected && points.size() > 0) {
			int c = 0;
			bool skip = false;
			for (auto p2 : points) {
				if (CGAL::squared_distance(p, p2) <= 1) {
					c++;
					if (c > 58) {
						skip = true;
						break;
					}
				}
			}
			if (c == 0 || skip) {
				continue;
			}
		}
		if (holes == 0) {

			if (!connected || points.size() < 1) {
				points.push_back(p);
				k++;
				//cout << k << endl;
			}
			else {
				int c = 0;
				for (auto p2 : points) {
					if (CGAL::squared_distance(p, p2) <= 1) {
						c++;
						if (c > 3) { break; }
					}
				}
				// the condition below was probably meant to be c < 5 or points.size > 5 (ie encourage filling domain while there are few points and after a while, 
				// when this becomes harder and harder, drop the max number of neighbours condition)
				if (c > 0 && ((points.size() < 500 && c <= 2) || (points.size() >= 500 && c <= 2))) {
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
				}
			}
			else {
				if (x < pow(2, e) || x > 3 * pow(2, e) || y < pow(2, e - 2) || y > 3 * pow(2, e - 2)) {
					points.push_back(Point_2(x, y));
					k++;
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
				if (x < pow(2, e + 1) / 3 || (x > 5 * pow(2, e) / 3 && x < 7 * pow(2, e) / 3) || x > 5 * pow(2, e + 1) / 3 ||
					y < pow(2, e - 1) / 3 || (y > 5 * pow(2, e - 2) / 3 && y < 7 * pow(2, e - 2) / 3) || y > 5 * pow(2, e - 1) / 3) {
					points.push_back(Point_2(x, y));
					k++;
				}
			}
		}
		if (points.size() != currSize) {
			cout << "size: " << points.size() << endl;
			currSize = points.size();
		}
	}
	return points;
}


void writeToFile(vector<Point_2> points, int space, int holes, bool narrow)
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

int main(int argc, char* argv[]) {
	istringstream ss(argv[1]);
	int numPoints;
	if (!(ss >> numPoints))
		cerr << "Invalid number " << argv[1] << '\n';
	int exp;
	istringstream ss2(argv[2]);
	if (!(ss2 >> exp))
		cerr << "Invalid number " << argv[2] << '\n';
	istringstream ss3(argv[3]);
	int holes;
	if (!(ss3 >> holes))
		cerr << "Invalid number " << argv[3] << '\n';
	int narrow;
	istringstream ss4(argv[4]);
	if (!(ss4 >> narrow))
		cerr << "Invalid number " << argv[4] << '\n';
	bool buildNarrow = narrow == 1;

	vector<Point_2> fixed;
	vector<Point_2> gend = pointGenerator(exp, holes, buildNarrow, numPoints, true, fixed);
	writeToFile(gend, pow(2, exp+2), holes, buildNarrow);
}