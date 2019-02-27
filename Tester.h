#pragma once
#include <iosfwd>
#include <string>
#include <Eigen/Dense>
using namespace Eigen;

class Tester
{
public:
	Tester(std::stringstream& ss);
	~Tester();

	bool it(std::string des, const MatrixXd& expect, const MatrixXd& given, double esp = 0.1) const;
	bool it(std::string des, const VectorXd& expect, const VectorXd& given, double esp = 0.1) const;
	bool it(std::string des, double expect, double given, double esp = 0.1);
	bool it(std::string des, bool flag) const;

	std::stringstream& ss;
};
