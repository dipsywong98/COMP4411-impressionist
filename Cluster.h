#pragma once
#include <Eigen/Dense>
#include "node.h"
#include <vector>
using namespace Eigen;

class Cluster
{
public:
	Cluster(MatrixXd m, VectorXd w, double minVar = 0.05);
	~Cluster();
	std::vector<std::pair<MatrixXd, VectorXd>> clusters;
};

