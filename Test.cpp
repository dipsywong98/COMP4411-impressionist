#include "Tester.h"
#include <string>
#include "Test.h"
#include "node.h"
#include "Cluster.h"
#include "PowerIter.h"
#include "Bayesian.h"

Test::Test(stringstream ss):Tester(ss)
{
}


Test::~Test()
{
}

void Test::node()
{
	MatrixXd m(8, 3);
	m << 0, 0, 0,
		10, 0, 0,
		10, 0, 0,
		10, 20, 0,
		0, 30, 10,
		0, 20, 20,
		0, 10, 20,
		0, 0, 10;
	// ss << "m:"<<m;
	VectorXd w(8);
	w << .1, .2, .3, .4, .5, .6, .7, .8;
	VectorXd exp_mu(3);
	exp_mu << 2.5, 11.6666666667, 10.833333333;
	MatrixXd exp_c(3, 3);
	exp_c << 18.7500, -6.9444444, -27.0833333,
		-6.944444, 119.44445444, 20.83333333,
		-27.083333, 20.8333333, 63.19445444;
	MatrixXd exp_d(8, 3);
	exp_d << -2.5, -11.666666667, -10.83333333,
		7.5, -11.666666667, -10.83333333,
		7.5, -11.666666667, -10.83333333,
		7.5, 8.33333333, -10.83333333,
		-2.5, 18.33333333, -0.83333333,
		-2.5, 8.33333333, 9.166666667,
		-2.5, -1.6666667, 9.166666667,
		-2.5, -11.6666667, -0.83333333;
	MatrixXd exp_t(8, 3);
	exp_t << -0.79056942, -3.68932394, -3.4258008,
		3.35410197, -5.21749195, -4.84481395,
		4.10791918, -6.3900965, -5.93366104,
		4.74341649, 5.27046277, -6.8516016,
		-1.76776695, 12.96362432, -0.58925565,
		-1.93649167, 6.45497224, 7.10046947,
		-2.09165007, -1.39443338, 7.66938358,
		-2.23606798, -10.43498389, -0.74535599;
	VectorXd exp_e(3);
	exp_e << -0.4343782, 0.82788275, -0.35486016;
	Node n(m, w);

	it("W", 3.6, n.W);
	it("mu", exp_mu, n.mu);
	it("d", exp_d, n.d);
	it("t", exp_t, n.t);
	it("cov", exp_c, n.c);
	it("lambda", 128.5476, n.l);
	VectorXd ce = n.c * n.e;
	VectorXd le = n.l * n.e;
	it("evector", ce, le);
}

void Test::cluster()
{
	MatrixXd m(6, 3);
	m << 23, 90, 23,
		24, 91, 23,
		100, 20, 30,
		99, 21, 29,
		100, 20, 330,
		99, 21, 329;
	VectorXd w(6);
	w << 1, 1, 1, 1, 1, 1;

	Cluster c(m, w, 5);

	Vector3d mu0, mu1, mu2;
	mu0 << 99.5, 20.5, 329.5;
	mu1 << 99.5, 20.5, 29.5;
	mu2 << 23.5, 90.5, 23.;

	MatrixXd cov0(3, 3), cov1(3, 3), cov2(3, 3);
	cov0 << 2.5001e-01, -2.5000e-01, 2.5000e-01,
		-2.5000e-01, 2.5001e-01, -2.5000e-01,
		2.5000e-01, -2.5000e-01, 2.5001e-01;
	cov1 << 2.5001e-01, -2.5000e-01, 2.5000e-01,
		-2.5000e-01, 2.5001e-01, -2.5000e-01,
		2.5000e-01, -2.5000e-01, 2.5001e-01;
	cov2 << 2.5001e-01, 2.5000e-01, 0.0000e+00,
		2.5000e-01, 2.5001e-01, 0.0000e+00,
		0.0000e+00, 0.0000e+00, 1.0000e-05;

	it("clusters Count", 3, c.clusters.size()) &&
		it("mu0", mu0, c.clusters[0].first) &&
		it("mu1", mu1, c.clusters[1].first) &&
		it("mu2", mu2, c.clusters[2].first) &&
		it("cov0", cov0, c.clusters[0].second) &&
		it("cov1", cov1, c.clusters[1].second) &&
		it("cov2", cov2, c.clusters[2].second);
}

void Test::eig()
{
	Matrix3d cov1;
	cov1 << 18.7500, -6.9444444, -27.0833333,
		-6.944444, 119.44445444, 20.83333333,
		-27.083333, 20.8333333, 63.19445444;
	// EigenSolver<MatrixXd> es(cov1);
	// ss <<"~1~"<<endl<< es.eigenvalues().real()<<endl << es.eigenvectors().real()<<endl;
	PowerIter ess(cov1);
	ss << "~1~22222\n" << ess.l << endl << ess.e << endl;
	ss << "err" << VectorXd(cov1*ess.e - ess.l*ess.e).norm() << endl;

	Matrix3d cov2;
	cov2 << 1283.81, -1182.31, 2643.28,
		-1182.31, 1089.14, -2434.61,
		2643.28, -2434.61, 20442.9;
	// EigenSolver<MatrixXd> es2(cov2);
	// ss << "~2~" << es2.eigenvalues().real() << es2.eigenvectors().real() << endl;
	PowerIter ess2(cov2);
	ss << "~2~2222\n" << ess2.l << endl << ess2.e << endl;
	ss << "err" << VectorXd(cov2*ess2.e - ess2.l*ess2.e).norm() << endl;
}

void Test::concat()
{
	Matrix2d A11, A12, A21, A22;
	A11 << 1, 2, 3, 4;
	A12 << 5, 6, 7, 8;
	A21 << 9, 10, 11, 12;
	A22 << 13, 14, 15, 16;
	ss << A11 << endl << A12 << endl << A21 << endl << A22 << endl;
	Matrix4d A;
	A << A11, A12, A21, A22;
	ss << A;
}


void Test::solveClusters()
{
	//prepare input
	Vector3d muF, muB, C;
	muF << 0.99807551, 0.99810218, 0.99781268;
	muB << 0.05767444, 0.10651794, 0.10343995;
	C << 0.09019607843137255,
		0.14901960784313725,
		0.12941176470588237;
	double muAlpha = 0.22276029055690072;
	Matrix3d sigF, sigB;
	sigF << 2.57055642e-05, 1.45723084e-05, 1.51951667e-05,
		1.45723084e-05, 2.56979531e-05, 1.52722067e-05,
		1.51951667e-05, 1.52722067e-05, 2.72805095e-05;
	sigB << 0.00044299, 0.00065237, 0.00064783,
		0.00065237, 0.00119343, 0.00119539,
		0.00064783, 0.00119539, 0.0013698;
	Cluster CF, CB;
	CF.clusters.emplace_back(muF, sigF);
	CB.clusters.emplace_back(muB, sigB);

	//prepare output
	VectorXd outF(3), outB(3), exp_outF(3), exp_outB(3);
	exp_outF << 0.99806594, 0.99812179, 0.99779378;
	exp_outB << 0.05975562, 0.11276062, 0.10279639;
	double exp_outAlpha = 0.03426974751738858, outAlpha;

	Bayesian::getFromClusters(CF, CB, muAlpha, C, 0.01, outF, outB, outAlpha);

	it("F", exp_outF, outF, 0.001);
	it("B", exp_outB, outB, 0.001);
	it("alpha", exp_outAlpha, outAlpha, 0.001);
}

void Test::testCalAlpha()
{
	VectorXd F(3), B(3), C(3);
	F << 0.9951071, 0.99679727, 0.99607272;
	B << 0.0, 0.0, 0.0;
	C << 0.09019607843137255, 0.14901960784313725, 0.12941176470588237;
	double exp_alpha = 0.1233871;
	VectorXd FB = F - B;
	double cbfb = VectorXd(C - B).dot(FB);
	double alpha = cbfb / FB.squaredNorm();
	it("alpha", exp_alpha, alpha);
}