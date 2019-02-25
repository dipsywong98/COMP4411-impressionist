#include "PowerIter.h"



PowerIter::PowerIter(MatrixXd A, double esp, int maxIter)
{
	e = VectorXd::Random(A.cols());
	e.normalize();
	for(int i = 0; i < maxIter; i++)
	{
		e = A*e;
		e.normalize();
		l = e.transpose()*A*e;
		if(VectorXd(A*e - l*e).norm() <= esp)
		{
			break;
		}
	}
}


PowerIter::~PowerIter()
{
}
