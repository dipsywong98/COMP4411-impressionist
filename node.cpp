#include "node.h"
#include "Bayesian.h"

// intensity in r,g,b channels, w as weight
Node::Node(MatrixXd r, MatrixXd g, MatrixXd b, MatrixXd w)
{
	// Vector3d mean;
	// mean << r.mean(), g.mean(), b.mean();
	ksize = Bayesian::ksize;
	W = w.sum();

	//weighted mean
	mur = r.dot(w) / W;
	mug = g.dot(w) / W;
	mub = b.dot(w) / W;

	//difference of itself and mean
	dr = r - MatrixXd::Constant(mur);
	dg = g - MatrixXd::Constant(mug);
	db = g - MatrixXd::Constant(mub);

	//c as covariance
	MatrixXd tr(ksize, ksize);
	MatrixXd tg(ksize, ksize);
	MatrixXd tb(ksize, ksize);
	for (int i = 0; i< ksize; i++)
	{
		float sqi = sqrt(w(i));
		for (int j = 0; j<ksize; j++)
		{
			tr(i, j) = dr(i, j)*sqi;
			tg(i, j) = dg(i, j)*sqi;
			tb(i, j) = db(i, j)*sqi;
		}
	}
	cr = tr.transpose() * tr * (1.0 / W);
	cg = tg.transpose() * tg * (1.0 / W);
	cb = tb.transpose() * tb * (1.0 / W);

	EigenSolver<MatrixXd> esr(cr);
	EigenSolver<MatrixXd> esg(cg);
	EigenSolver<MatrixXd> esb(cb);
	VectorXd lrs = esr.eigenvalues();
	VectorXd lgs = esg.eigenvalues();
	VectorXd lbs = esb.eigenvalues();
	er;	//eigenvector
	eg;	//eigenvector
	eb;	//eigenvector
	lr = 0;	//eigenvalue
	lg = 0;	//eigenvalue
	lb = 0;	//eigenvalue
	for (int i = 0; i < lrs.size(); i++)
	{
		if (abs(lrs(i))>lr)
		{
			lr = abs(lrs(i));
			er = esr.eigenvectors().col(i);
		}
		if (abs(lgs(i))>lr)
		{
			lg = abs(lgs(i));
			eg = esg.eigenvectors().col(i);
		}
		if (abs(lbs(i))>lr)
		{
			lb = abs(lbs(i));
			eb = esb.eigenvectors().col(i);
		}
	}
}