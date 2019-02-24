#include "node.h"

// extern std::stringstream ss;

// intensity in r,g,b channels, w as weight
Node::Node(MatrixXd m, VectorXd w)
{
	// Vector3d mean;
	// mean << r.mean(), g.mean(), b.mean();
	int size = w.size();
	W = w.sum();

	//weighted mean
	// mu = m.transpose()*w / W;
	mu(0) = (m.col(0).dot(w)) / W;
	mu(1) = (m.col(1).dot(w)) / W;
	mu(2) = (m.col(2).dot(w)) / W;

	//difference of itself and mean
	d = MatrixXd(size, 3);
	d << m;
	for(int i=0; i<size;i++)
	{
		d.row(i) -= mu;
	}

	//c as covariance
	c = MatrixXd(3, 3);
	t = MatrixXd(size, 3);
	for (int i = 0; i< size; i++)
	{
		float sqi = sqrt(w(i));
		for (int j = 0; j<3; j++)
		{
			t(i, j) = d(i, j)*sqi;
		}
	}
	c = t.transpose() * t * (1.0 / W);
	
	EigenSolver<MatrixXd> es(c);
	VectorXd ls = es.eigenvalues().real();	//c is a spsd matrix, all eigenvalue and eigenvector real
	for (int i = 0; i < ls.size(); i++)
	{
		if (abs(ls(i))>l)
		{
			l = ls.real()(i);
			e = es.eigenvectors().real().row(i);
		}
	}

	// ss << "eigenvectors" << std::endl<< es.eigenvectors().real()<<std::endl<< "lambdas"<<es.eigenvalues().real()<<std::endl;
}