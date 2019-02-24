#pragma once
#include <Eigen/Dense>
#include "node.h"
using namespace Eigen;

class Cluster
{
public:
	Cluster(MatrixXd r, MatrixXd g, MatrixXd b, MatrixXd w);
	~Cluster();
};

