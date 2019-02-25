#include "Bayesian.h"
#include <FL/fl_ask.H>
#include <list>
#include <iostream>
#include <Eigen/Dense>
#include "Cluster.h"
#include <Eigen/src/Core/util/ForwardDeclarations.h>
#include <Eigen/src/Core/util/ForwardDeclarations.h>
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

void Bayesian::getFromClusters(const Cluster& CF, const Cluster& CB,double mualpha, Vector3d C, double sigC, Vector3d& fcolor, Vector3d& bcolor,
	double& palpha)
{
	const Matrix3d I = Matrix3d::Identity();
	double invSigC2 = 1 / pow(sigC, 2);
	double maxScore = -INFINITY;
	int maxIter = 50;
	double esp = 1e-6;
	for(int fi = 0; fi < CF.clusters.size(); fi++)
	{
		const VectorXd &muF = CF.clusters[fi].first;
		const MatrixXd &sigF = CF.clusters[fi].second;
		MatrixXd invSigF = sigF.inverse();
		for(int bi=0; bi< CB.clusters.size(); bi++)
		{
			
			const VectorXd &muB = CB.clusters[bi].first;
			const MatrixXd &sigB = CB.clusters[bi].second;
			MatrixXd invSigB = sigB.inverse();
			double alphak = mualpha;
			double prevScore = NAN;

			for(int k = 0; k<maxIter; k++)
			{
				MatrixXd A11 = invSigF + I*pow(alphak, 2)*invSigC2;
				MatrixXd Axx = I*alphak*(1 - alphak)*invSigC2;
				MatrixXd A22 = invSigB + I*pow(1 - alphak, 2)*invSigC2;
				MatrixXd A(6, 6);
				A << A11, Axx, Axx, A22;

				VectorXd b1 = invSigF * muF + C*alphak * invSigC2;
				VectorXd b2 = invSigB * muB + C*(1-alphak) * invSigC2;
				VectorXd b(6);
				b << b1, b2;

				VectorXd X = A.colPivHouseholderQr().solve(b);
				Vector3d F, B;
				F << X(0), X(1), X(2);
				B << X(3), X(4), X(5);

				alphak = Vector3d(F - B).normalized().dot(Vector3d(C - B));

				double LC = -Vector3d(C - alphak*F - (1 - alphak)*B).squaredNorm() * invSigC2;
				Vector3d dF(F - muF), dB(B - muB);
				double LF = MatrixXd(-dF.transpose()*invSigF*dF)(0,0) / 2;
				double LB = MatrixXd(-dB.transpose()*invSigB*dB)(0,0) / 2;

				double score = LC + LF + LB;

				if(score > maxScore)
				{
					maxScore = score;
					fcolor = F;
					bcolor = B;
					palpha = alphak;
				}
				if(prevScore == prevScore && abs(prevScore - score) < esp)
				{
					break;
				}
			}
		}
	}
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

	std::vector<Point> flist;
	std::vector<Point> blist;
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

	MatrixXd F(flist.size(), 3), B(blist.size(), 3);
	for(int i = 0; i<flist.size();i++)
	{
		int x = flist[i].x;
		int y = flist[i].y;
		F.row(i) << img[y*w + x], img[y*w + x + 1], img[y*w + x + 2];
	}
	for(int i = 0; i<blist.size();i++)
	{
		int x = blist[i].x;
		int y = blist[i].y;
		B.row(i) << img[y*w + x], img[y*w + x + 1], img[y*w + x + 2];
	}
	Cluster cF(F, Fw);
	Cluster cB(B, Bw);
	Vector3d Fcolor, Bcolor, Ccolor;
	Ccolor << img[y*w + x], img[y*w + x + 1], img[y*w + x + 2];
	double mualpha = a.mean();
	double palpha;
	getFromClusters(cF,cB, mualpha,Ccolor,0.01,Fcolor, Bcolor,palpha);
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
