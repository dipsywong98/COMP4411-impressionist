#pragma once
#include <iosfwd>
#include <string>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;

class Tester
{
public:
	Tester(std::stringstream& ss);
	~Tester();

	bool it(string des, const MatrixXd& expect, const MatrixXd& given, double esp = 0.1) const;
	bool it(string des, const VectorXd& expect, const VectorXd& given, double esp = 0.1) const;
	bool it(string des, double expect, double given, double esp = 0.1);
	bool it(string des, bool flag) const;

	stringstream& ss;
};
