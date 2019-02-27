#include "Bayesian.h"
#include <FL/fl_ask.H>
#include <list>
#include <iostream>
#include <Eigen/Dense>
#include "Cluster.h"
#include <Eigen/src/Core/util/ForwardDeclarations.h>
#include <Eigen/src/Core/util/ForwardDeclarations.h>
using namespace Eigen;

extern std::stringstream ss;

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
	delete[] unkn;
}

void Bayesian::init()
{
}

void Bayesian::getFromClusters(const Cluster& CF, const Cluster& CB,double mualpha, Vector3d C, double sigC, VectorXd& fcolor, VectorXd& bcolor,
	double& palpha)
{
	const Matrix3d I = Matrix3d::Identity();
	double invSigC2 = 1 / pow(sigC, 2);
	double maxScore = -INFINITY;
	int maxIter = 50;
	const double esp = 1e-6;
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

			//iteratively find for good choice of alpha, F, B
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

				VectorXd X = A.partialPivLu().solve(b);
				std::stringstream sss,ssF,ssB;
				sss << X;
				std::string str = sss.str();
				for (int i = 0; i < 6; i++) X(i) = max(0, min(X(i), 1));
				Vector3d F, B;
				F << X(0), X(1), X(2);
				B << X(3), X(4), X(5);

				ssF << F;
				ssB << B;

				VectorXd FB = F - B;
				double cbfb = VectorXd(C - B).dot(FB);
				alphak = cbfb / FB.squaredNorm();
				alphak = max(0, min(alphak, 1));

				double LC = -Vector3d(C - alphak*F - (1 - alphak)*B).squaredNorm() * invSigC2;
				Vector3d dF(F - muF), dB(B - muB);
				double LF = -MatrixXd(dF.transpose()*invSigF*dF)(0,0) / 2;
				double LB = -MatrixXd(dB.transpose()*invSigB*dB)(0,0) / 2;

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
				prevScore = score;
			}
		}
	}
}

bool Bayesian::trySolvePix(Point pt)
{
	int x = pt.x, y = pt.y;
	// int unknCnt = 0;
	// kernelFun(pt, [&](int i, int j, int x, int y)
	// {
	// 	unknCnt += unkn[y*w + x];
	// }, 3);
	// if (unknCnt > ksize/2)return 0; // need more than half for evaluation

	std::vector<Point> flist;
	std::vector<Point> blist;

	double mualpha = 0;
	int cnt = 0;

	kernelFun(pt, [&](int i, int j, int x, int y)
	{
		double a = alphaImg(y*w + x);
		if(a>1)
		{
			
		}
		if (!unkn[y*w+x] && a==a) {
			mualpha += a;
			double wf = pow(a, 2) * gaussianKernel[i][j];
			if (wf > 0)
			{
				flist.emplace_back(x, y, wf);
			}
			double wb = pow(1 - a, 2) * gaussianKernel[i][j];
			if (wb > 0)
			{
				blist.emplace_back(x, y, wb);
			}
			cnt++;
		}
		else
		{
			
		}
	});

	if(flist.size()<minLen || blist.size()<minLen)
	{
		return 0;
	}

	mualpha /= cnt;

	VectorXd Fw(flist.size());
	VectorXd Bw(blist.size());
	MatrixXd F(flist.size(), 3), B(blist.size(), 3);

	for (int i = 0; i<flist.size(); i++)
	{
		int x = flist[i].x;
		int y = flist[i].y;
		F.row(i) = foreImg.row(y*w + x);
		Fw(i) = flist[i].a;
	}
	for (int i = 0; i<blist.size(); i++)
	{
		int x = blist[i].x;
		int y = blist[i].y;
		B.row(i) = backImg.row(y*w + x);
		Bw(i) = blist[i].a;
	}

	Cluster cF(F, Fw);
	Cluster cB(B, Bw);
	VectorXd Fcolor(3), Bcolor(3), Ccolor(3);
	Ccolor = origImg.row(y*w+x);
	double palpha;
	getFromClusters(cF,cB, mualpha,Ccolor,0.01,Fcolor, Bcolor,palpha);
	foreImg.row(y*w + x) = Fcolor;
	backImg.row(y*w + x) = Bcolor;
	alphaImg(y*w + x) = palpha;
	return 1;
}

void Bayesian::solve(char* iname)
{
	w = m_pDoc->m_nWidth;
	h = m_pDoc->m_nHeight;
	if(openTriImg(iname))
	{
		std::list<Point> unknPts;
		unknPts.clear();
		for (int i = 0; i<w*h; i++)
		{
			if (unkn[i])
			{
				unknPts.emplace_back(i % w, i / w);
			}
		}
		
		while(!unknPts.empty())
		{
			int prevPtCnt = unknPts.size();
			bool bounced = false;
			std::list<Point>::iterator pPt = unknPts.begin();
			while (pPt != unknPts.end()) {
				Point pixel = *pPt;
				if(pixel.x == 342 && pixel.y == 142)
				{
					ss << unknPts.size()<< std::endl;
				}
				if(trySolvePix(pixel))
				{
					unknPts.erase(pPt++);
					unkn[pixel.x + w*pixel.y] = 0;
					findSum++;
					if (!bounced && minLen*1.1 <= ksize*ksize * 1 / 3)
					{
						minLen *= 1.1;
						bounced = true;
					}
				}else
				{
					pPt++;
				}
			}
			if(unknPts.size() == prevPtCnt)
			{
				minLen *= 0.9;
			}
		}
		outputMatrix("foreground.bmp", foreImg,true);
		outputMatrix("background.bmp", backImg,false);
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

	delete[] unkn;
	unkn= new bool[w*h];
	memset(unkn, 0, w*h);
	img = m_pDoc->m_ucOriginal;
	unknSum = 0;
	findSum = 0;

	foreImg = MatrixXd(w*h, 3);
	backImg = MatrixXd(w*h, 3);
	origImg = MatrixXd(w*h, 3);
	alphaImg = VectorXd(w*h);

	for(int i=0; i<w*h; i++)
	{
		Vector3d C;
		C << img[i*3]/255.f, img[i*3+1]/255.f, img[i*3+2]/255.f;
		origImg.row(i) = C;
		if (data[i * 3] == 0) {
			backImg.row(i) = C;
			foreImg.row(i) = VectorXd::Zero(3);
			alphaImg(i) = 0;
		}
		else if (data[i * 3] == 255) {
			backImg.row(i) = VectorXd::Zero(3);
			foreImg.row(i) = C;
			alphaImg(i) = 1;
		}
		else {
			unkn[i] = 1;
			alphaImg(i) = NAN;
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
		m_pDoc->m_ucPainting[i * 3] = alphaImg(i)*255;
		m_pDoc->m_ucPainting[i * 3+1] = alphaImg(i) *255;
		m_pDoc->m_ucPainting[i * 3+2] = alphaImg(i) *255;
	}
	m_pDoc->saveImage("bayesian_out.bmp");
}



void Bayesian::outputMatrix(char* name, const MatrixXd& m, bool fore)
{
	memset(m_pDoc->m_ucPainting, 0, w*h * 3);
	for (int i = 0; i<w*h; i++)
	{
		double alpha = alphaImg(i);
		if (!fore) alpha = 1 - alpha;
		m_pDoc->m_ucPainting[i * 3] = m(i,0) * 255 * alpha;
		m_pDoc->m_ucPainting[i * 3 + 1] = m(i,1) * 255 * alpha;
		m_pDoc->m_ucPainting[i * 3 + 2] = m(i,2) * 255 * alpha;
	}
	m_pDoc->saveImage(name);
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
