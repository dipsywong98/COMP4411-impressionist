#pragma once
#include <iosfwd>
#include <string>
#include <Eigen/Dense>
#include "Tester.h"
using namespace Eigen;
using namespace std;

class Test: public Tester
{
public:
	Test(stringstream ss);
	~Test();
	void node();
	void cluster();
	void eig();
	void concat();
	void solveClusters();
	void testCalAlpha();
};
