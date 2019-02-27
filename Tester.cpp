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
	// OutputDebugstd::string(ss.str().c_str());
}


Tester::~Tester()
{
}


bool Tester::it(const std::string des, const MatrixXd& expect, const MatrixXd& given, double esp) const
{
	ss << des << ": ";
	MatrixXd diff = expect - given;
	if (diff.squaredNorm() < esp)
	{
		ss << "OK" << std::endl;
		return true;
	}
	ss << "Error\nExpected:" << std::endl;
	ss << expect << std::endl;
	ss << "Found:" << std::endl;
	ss << given << std::endl;
	return false;
}

bool Tester::it(const std::string des, const VectorXd& expect, const VectorXd& given, double esp) const
{
	ss << des << ": ";
	VectorXd diff = expect - given;
	if (diff.squaredNorm() < esp)
	{
		ss << "OK" << std::endl;
		return true;
	}
	ss << "Error\nExpected:" << std::endl;
	ss << expect << std::endl;
	ss << "Found:" << std::endl;
	ss << given << std::endl;
	return false;
}


auto Tester::it(const std::string des, double expect, double given, double esp) -> bool
{
	ss << des << ": ";
	double diff = expect - given;
	if (diff < esp)
	{
		ss << "OK" << std::endl;
		return true;
	}
	ss << "Error\nExpect:" << std::endl;
	ss << expect << std::endl;
	ss << "Found:" << std::endl;
	ss << given << std::endl;
	return false;
}

bool Tester::it(const std::string des, bool flag) const
{
	ss << des << ": ";
	ss << (flag ? "OK" : "Error") << std::endl;
	return flag;
}
