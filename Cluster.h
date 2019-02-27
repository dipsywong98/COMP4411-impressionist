#pragma once
#include <Eigen/Dense>
#include "node.h"
#include <vector>
using namespace Eigen;

class Cluster
{
public:
	Cluster(){}
	Cluster(MatrixXd m, VectorXd w, double minVar = 0.05);
	~Cluster();
	std::vector<std::pair< VectorXd,MatrixXd>> clusters;
	VectorXd& mean(int index) { return clusters[index].first; }
	MatrixXd& cov(int index) { return clusters[index].second; }
	int size() { return clusters.size(); }
};

