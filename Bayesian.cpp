#include "Bayesian.h"
#include <FL/fl_ask.H>
#include <list>
#include <iostream>
#include <Eigen/Dense>
using namespace Eigen;

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
	int unknCnt = 0;
	kernelFun(pt, [&](int i, int j, int x, int y)
	{
		unknCnt += unkn[y*w + x];
	}, 3);
	if (unknCnt == 9)return 0; // need more than half for evaluation

	MatrixXd a(ksize,ksize);

	std::list<Point> flist;
	std::list<Point> blist;
	VectorXd Fw;
	VectorXd Bw;

	kernelFun(pt, [&](int i, int j, int x, int y)
	{
		a(i, j) = alpha[y*w + x];
		if (fore[y*w + x])flist.emplace_back(x, y);
		if (back[y*w + x])blist.emplace_back(x, y);
		if (!unkn[y*w + x])
		{
			Fw << pow(a(i, j), 2) * gaussianKernel[i][j];
			Bw << pow(1 - a(i, j), 2) * gaussianKernel[i][j];
		}
	});

	// Vector3d F,B;
	// F << F_r, F_g, F_b;
	// B << B_r, B_g, B_b;

	std::cout << a;

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
				Point pixel = *pPt;
				if(trySolvePix(pixel))
				{
					unknPts.erase(pPt++);
					unkn[pixel.x + w*pixel.y] = 0;
					findSum++;
				}else
				{
				pPt++;
				}
			}
		}
		outputAlpha();
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

void Bayesian::outputAlpha()
{
	memset(m_pDoc->m_ucPainting, 0, w*h * 3);
	for(int i = 0; i<w*h; i++)
	{
		m_pDoc->m_ucPainting[i * 3] = alpha[i]*255;
		m_pDoc->m_ucPainting[i * 3+1] = alpha[i]*255;
		m_pDoc->m_ucPainting[i * 3+2] = alpha[i]*255;
	}
	m_pDoc->saveImage("bayesian_out.bmp");
}

void Bayesian::kernelFun(Point p, std::function<void(int, int, int, int)> cb, int size)
{
	for(int i = 0; i<size; i++)
	{
		for(int j=0;j<size;j++)
		{
			int x = p.x + i - size / 2;
			int y = p.y + j - size / 2;
			if(x<0||y<0||x>w-1||y>h-1)continue;
			cb(i, j, x, y);
		}
	}
}
