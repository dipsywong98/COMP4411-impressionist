#pragma once
#include <Eigen/Dense>
using namespace Eigen;

class Node
{
public:
	Node(MatrixXd m, VectorXd w);
	float W;

	//weighted mean
	Vector3d mu;

	//difference of itself and mean
	MatrixXd d;

	//c as covariance
	MatrixXd c;
	MatrixXd t;

	float l = 0;	//eigenvalue
	VectorXd e;	//eigenvector
};

