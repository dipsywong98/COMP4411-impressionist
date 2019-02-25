#pragma once
#include <Eigen/Dense>
using namespace Eigen;

//solve for eigenvalue and vector using power iteration

class PowerIter
{
public:
	PowerIter(MatrixXd A, double esp = 0.00001, int maxIter = 1000);
	~PowerIter();
	double l;
	VectorXd e;
};

