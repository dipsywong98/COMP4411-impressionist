#include "Bayesian.h"
#include <FL/fl_ask.H>
#include <list>
#include "Matrix.h"
#include <Eigen/Dense>
using Eigen::MatrixXd;

extern std::vector<std::vector<float>> getGaussianKernel(float sigma, int size);

Bayesian::Bayesian(ImpressionistDoc* pDoc, ImpressionistUI* pUI)
{
	m_pDoc = pDoc;
	m_pUI = pUI;
	gaussianKernel = getGaussianKernel(8, ksize);
	for (int i = 0; i < ksize; i++)for (int j = 0; j < ksize; j++) gaussianKernel[i][j] /= gaussianKernel[ksize / 2][ksize / 2];
}

Bayesian::~Bayesian()
{
	delete[] fore;
	delete[] back;
	delete[] unkn;
}

void Bayesian::init()
{
}

bool Bayesian::trySolvePix(Point pt)
{
	int x = pt.x, y = pt.y;
	Matrix<float> a(ksize, ksize);
	Matrix<float> F(ksize, ksize);
	Matrix<float> B(ksize, ksize);
	for(int i=0;i<ksize;i++)
	{
		for(int j=0;j<ksize;j++)
		{
			a.mV[i][j] = alpha[j*w + i];
			F.mV[i][j] = fore[j*w + i];
			B.mV[i][j] = back[j*w + i];
		}
	}
	return 1;
}

void Bayesian::solve(char* iname)
{
	w = m_pDoc->m_nWidth;
	h = m_pDoc->m_nHeight;
	if(openTriImg(iname))
	{
		std::list<Point> unknPts;
		for (int i = 0; i<w*h; i++)
		{
			if (unkn[i])
			{
				unknPts.emplace_back(i % w, i / w);
			}
		}
		while(findSum<unknSum)
		{
			std::list<Point>::iterator pPt = unknPts.begin();
			while (pPt != unknPts.end()) {
				if(trySolvePix(*pPt))
				{
					unknPts.erase(pPt);
					findSum++;
				}
				pPt++;
			}
		}
	}
}

bool Bayesian::openTriImg(char* iname)
{
	unsigned char*	data;
	int				width, height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// check if the dimension matches the original image
	if (w != width || h != height)
	{
		fl_alert("Dimension does not match");
		return 0;
	}

	// release old another image
	delete[] fore;
	delete[] back;
	delete[] unkn;
	delete[] alpha;

	fore = new bool[w*h];
	back= new bool[w*h];
	unkn= new bool[w*h];
	alpha = new float[w*h];
	memset(fore, 0, w*h);
	memset(back, 0, w*h);
	memset(unkn, 0, w*h);
	memset(alpha, 0, w*h);
	img = m_pDoc->m_ucOriginal;
	unknSum = 0;
	findSum = 0;

	for(int i=0; i<w*h; i++)
	{
		if (data[i * 3] == 0) {
			back[i] = 1;
		}
		else if (data[i * 3] == 255) {
			fore[i] = 1;
			alpha[i] = 1;
		}
		else {
			unkn[i] = 1;
			alpha[i] = NAN;
			unknSum++;
		}
	}


	delete[] data;

	return 1;
}
