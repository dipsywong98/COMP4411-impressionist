//
// impressionist.cpp
//
// The main driver program for the other parts. We have two major components,
// UI and Doc.
// They do have a link to each other as their member such that they can 
// communicate.
//

#include <stdio.h>

#include <FL/Fl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "impressionistUI.h"
#include "impressionistDoc.h"
#include <functional>
#include "Bayesian.h"
#include "node.h"
#include <Eigen/Dense>
#include <iostream>
#include "Cluster.h"
#include "PowerIter.h"
using namespace Eigen;
using namespace std;

ImpressionistUI *impUI;
ImpressionistDoc *impDoc;
Bayesian* imp_bayesian;
stringstream ss;

bool it(stringstream& ss, string des, const MatrixXd& expect, const MatrixXd& given, double esp = 0.1)
{
	ss << des << ": ";
	MatrixXd diff = expect - given;
	if(diff.squaredNorm() < esp)
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

bool it(stringstream& ss, string des, const VectorXd& expect, const VectorXd& given, double esp = 0.1)
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


bool it(stringstream& ss, string des, double expect, double given, double esp = 0.1)
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

bool it(stringstream& ss, string des, bool flag)
{
	ss << des << ": ";
	ss << (flag ? "OK" : "Error") << endl;
	return flag;
}

void testNode()
{
	MatrixXd m(8,3);
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
	MatrixXd exp_t(8,3);
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

	it(ss, "W", 3.6, n.W);
	it(ss, "mu", exp_mu, n.mu);
	it(ss, "d", exp_d, n.d);
	it(ss, "t", exp_t, n.t);
	it(ss, "cov", exp_c, n.c);
	it(ss, "lambda", 128.5476, n.l);
	VectorXd ce = n.c * n.e;
	VectorXd le = n.l * n.e;
	it(ss, "evector", ce, le);
}

void testCluster()
{
	MatrixXd m(6, 3);
	m << 23, 90, 23,
		24, 91, 23,
		100, 20, 30,
		99, 21, 29,
		100, 20, 330,
		99, 21, 329;
	VectorXd w(6);
	w << 1,1,1,1,1,1;

	Cluster c(m, w,5);

	Vector3d mu0, mu1, mu2;
	mu0 << 99.5, 20.5, 329.5;
	mu1 << 99.5, 20.5, 29.5;
	mu2 << 23.5, 90.5, 23.;

	MatrixXd cov0(3,3), cov1(3,3), cov2(3,3);
	cov0 << 2.5001e-01, -2.5000e-01, 2.5000e-01,
		-2.5000e-01, 2.5001e-01, -2.5000e-01,
		2.5000e-01, -2.5000e-01, 2.5001e-01;
	cov1 << 2.5001e-01, -2.5000e-01, 2.5000e-01,
		-2.5000e-01, 2.5001e-01, -2.5000e-01,
		2.5000e-01, -2.5000e-01, 2.5001e-01;
	cov2 << 2.5001e-01, 2.5000e-01, 0.0000e+00,
		2.5000e-01, 2.5001e-01, 0.0000e+00,
		0.0000e+00, 0.0000e+00, 1.0000e-05;
	
	it(ss, "clusters Count", 3, c.clusters.size()) &&
		it(ss, "mu0", mu0, c.clusters[0].first) &&
		it(ss, "mu1", mu1, c.clusters[1].first) &&
		it(ss, "mu2", mu2, c.clusters[2].first) &&
		it(ss, "cov0", cov0, c.clusters[0].second) &&
		it(ss, "cov1", cov1, c.clusters[1].second) &&
		it(ss, "cov2", cov2, c.clusters[2].second);
}

void testEig()
{
	Matrix3d cov1;
	cov1<< 18.7500, -6.9444444, -27.0833333,
		-6.944444, 119.44445444, 20.83333333,
		-27.083333, 20.8333333, 63.19445444;
	// EigenSolver<MatrixXd> es(cov1);
	// ss <<"~1~"<<endl<< es.eigenvalues().real()<<endl << es.eigenvectors().real()<<endl;
	PowerIter ess(cov1);
	ss << "~1~22222\n" << ess.l << endl << ess.e << endl;
	ss << "err" << VectorXd(cov1*ess.e - ess.l*ess.e).norm() << endl;

	Matrix3d cov2;
	cov2 << 1283.81, - 1182.31,  2643.28,
		-1182.31,  1089.14, - 2434.61,
		2643.28, - 2434.61,  20442.9; 
	// EigenSolver<MatrixXd> es2(cov2);
	// ss << "~2~" << es2.eigenvalues().real() << es2.eigenvectors().real() << endl;
	PowerIter ess2(cov2);
	ss << "~2~2222\n" << ess2.l << endl << ess2.e << endl;
	ss << "err" << VectorXd(cov2*ess2.e - ess2.l*ess2.e).norm() << endl;
}

void testConcat()
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


void testSolveClusters()
{
	//prepare input
	Vector3d muF, muB,C;
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

	it(ss, "F", exp_outF, outF, 0.001);
	it(ss, "B", exp_outB, outB, 0.001);
	it(ss, "alpha", exp_outAlpha, outAlpha, 0.001);
}

int main(int	argc, 
		 char**	argv) 
{
	// testNode();
	// testEig();
	// testCluster();
	// testConcat();
	testSolveClusters();
	OutputDebugString(ss.str().c_str());
	return 0;
	impDoc = new ImpressionistDoc();

	// Create the UI
	impUI = new ImpressionistUI();

	imp_bayesian = new Bayesian(impDoc, impUI);
	impDoc->m_bayesian = imp_bayesian;

	// Set the impDoc which is used as the bridge between UI and brushes
	impUI->setDocument(impDoc);
	impDoc->setUI(impUI);

	Fl::visual(FL_DOUBLE|FL_INDEX);

	impUI->show();

	impDoc->loadImage("bayesian/gandalf.bmp");
	imp_bayesian->solve("bayesian/gandalfTrimap.bmp");

	return Fl::run();
}

// global functions
float frand()
{
	return (float)rand()/RAND_MAX;
}

int irand(int max)
{
	return rand()%max;
}

double degToRad(double deg)
{
	return deg * 2 * M_PI / 360.0f;
}

double radToDeg(double rad)
{
	return rad / 2.f / M_PI * 360.0f;
}

double randAlter(double original, double maxAbsPercentageDiff)
{
	return original + original*maxAbsPercentageDiff*(frand()-0.5)*2;
}

Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel)
{
	const int sobel[][3] = {
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};
	int gx = 0, gy = 0;
	// const int img[3][3] = {};
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			const int x = source.x - 1 + i;
			const int y = source.y - 1 + j;
			GLubyte* p = getPixel(x, y);
			const int pix = p[0]+p[1]+p[2];
			gx += sobel[j][i] * pix;
			gy += sobel[i][j] * pix;
		}
	}
	return Point(gx, gy);
}


std::vector<std::vector<float>> getGaussianKernel(float sigma, int size)
{

	std::vector<std::vector<float>> k;

	for (int i = 0; i < size; i++) {
		std::vector<float> row;
		for (int j = 0; j < size; j++) {
			int x = i - size / 2;
			int y = j - size / 2;
			float v = (1 / (2 * M_PI*sigma)) * exp( -(x * x + y * y) / (2 * sigma));
			row.push_back(v);
		}
		k.push_back(row);
	}
	return k;
}

GLubyte* getColor(unsigned char* p, int xx, int yy, int w, int h){
	xx = max(0, min(xx, w - 1));
	yy = max(0, min(yy, h - 1));
	return p + (yy*w + xx) * 3;
};

float colorDist(unsigned char* p0, unsigned char* p1) {
	return sqrt(pow(p0[0] - p1[0], 2) + pow(p0[1] - p1[1], 2) + pow(p0[2] - p1[2], 2));
};

void getMeanColor(GLubyte* buf, unsigned char* p, int xx, int yy, int w, int h, float sigma, int size)
{
	std::vector<std::vector<float>> kernel=  getGaussianKernel(sigma, size);
	memset(buf, 0, 3);
	for (int a = 0; a<size; a++)
	{
		for (int b = 0; b<size; b++)
		{
			for (int d = 0; d<3; d++)
			{
				buf[d] += kernel[a][b] * getColor(p,xx,yy,w,h)[d];
			}
		}
	}
}