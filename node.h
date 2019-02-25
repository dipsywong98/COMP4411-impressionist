#pragma once
#include <Eigen/Dense>
#include <vector>
using namespace Eigen;

class Node
{
public:
	Node();
	Node(MatrixXd m, VectorXd w);
	MatrixXd m;
	VectorXd w;
	float W;

	//weighted mean
	VectorXd mu;

	//difference of itself and mean
	MatrixXd d;

	//c as covariance
	MatrixXd c;
	MatrixXd t;

	float l = 0;	//eigenvalue
	VectorXd e;	//eigenvector
};

