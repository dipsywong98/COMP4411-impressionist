#include "Tester.h"
#include <string>


Tester::Tester(std::stringstream& ss): ss(ss)
{
	// node();
	// eig();
	// cluster();
	// concat();
	// solveClusters();
	// calAlpha();
	// OutputDebugString(ss.str().c_str());
}


Tester::~Tester()
{
}


bool Tester::it(const string des, const MatrixXd& expect, const MatrixXd& given, double esp) const
{
	ss << des << ": ";
	MatrixXd diff = expect - given;
	if (diff.squaredNorm() < esp)
	{
		ss << "OK" << endl;
		return true;
	}
	ss << "Error\nExpected:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
	return false;
}

bool Tester::it(const string des, const VectorXd& expect, const VectorXd& given, double esp) const
{
	ss << des << ": ";
	VectorXd diff = expect - given;
	if (diff.squaredNorm() < esp)
	{
		ss << "OK" << endl;
		return true;
	}
	ss << "Error\nExpected:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
	return false;
}


auto Tester::it(const string des, double expect, double given, double esp) -> bool
{
	ss << des << ": ";
	double diff = expect - given;
	if (diff < esp)
	{
		ss << "OK" << endl;
		return true;
	}
	ss << "Error\nExpect:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
	return false;
}

bool Tester::it(const string des, bool flag) const
{
	ss << des << ": ";
	ss << (flag ? "OK" : "Error") << endl;
	return flag;
}
