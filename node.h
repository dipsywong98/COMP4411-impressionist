#pragma once
#include <Eigen/Dense>
using namespace Eigen;

class Node
{
public:
	Node(MatrixXd r, MatrixXd g, MatrixXd b, MatrixXd w);
	int ksize;
	float W;

	//weighted mean
	float mur;
	float mug;
	float mub;

	//difference of itself and mean
	MatrixXd dr;
	MatrixXd dg;
	MatrixXd db;

	//c as covariance
	MatrixXd cr;
	MatrixXd cg;
	MatrixXd cb;

	float lr = 0;	//eigenvalue
	float lg = 0;	//eigenvalue
	float lb = 0;	//eigenvalue
	VectorXd er;	//eigenvector
	VectorXd eg;	//eigenvector
	VectorXd eb;	//eigenvector
};

