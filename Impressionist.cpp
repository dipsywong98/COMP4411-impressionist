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

#include "Impressionist.h"
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

ImpressionistUI *impUI;
ImpressionistDoc *impDoc;
Bayesian* imp_bayesian;
std::stringstream ss;

int main(int	argc, 
		 char**	argv) 
{
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

	// OutputDebugString(ss.str().c_str());

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