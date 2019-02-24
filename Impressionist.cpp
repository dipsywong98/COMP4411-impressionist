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
using namespace Eigen;
using namespace std;

ImpressionistUI *impUI;
ImpressionistDoc *impDoc;
Bayesian* imp_bayesian;
stringstream ss;

void it(stringstream& ss, string des, const MatrixXd& expect, const MatrixXd& given)
{
	ss << des << ": ";
	MatrixXd diff = expect - given;
	if(diff.squaredNorm() < 0.1)
	{
		ss << "OK" << endl;
		return;
	}
	ss << "Error\nExpected:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
}

void it(stringstream& ss, string des, const VectorXd& expect, const VectorXd& given)
{
	ss << des << ": ";
	VectorXd diff = expect - given;
	if (diff.squaredNorm() < 0.1)
	{
		ss << "OK" << endl;
		return;
	}
	ss << "Error\nExpected:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
}


void it(stringstream& ss, string des, double expect, double given)
{
	ss << des << ": ";
	double diff = expect - given;
	if (diff < 0.1)
	{
		ss << "OK" << endl;
		return;
	}
	ss << "Error\nExpect:" << endl;
	ss << expect << endl;
	ss << "Found:" << endl;
	ss << given << endl;
}

void test()
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
	
	OutputDebugString(ss.str().c_str());
}

int main(int	argc, 
		 char**	argv) 
{
	test();
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