#pragma once
#include "impressionistDoc.h"
#include <vector>
#include <functional>
#include "Cluster.h"

class Bayesian
{
public:
	Bayesian(){}
	Bayesian(ImpressionistDoc* pDoc, ImpressionistUI* pUI);
	~Bayesian();
	void init();
	static void getFromClusters(const Cluster& CF, const Cluster& CB, double mualpha, Vector3d C, double sigC, VectorXd& fcolor, VectorXd& bcolor,
		double& palpha);
	bool trySolvePix(Point pt);	//return true if alpha is determined
	void solve(char* triimg_name);
	bool openTriImg(char* iname);
	void outputAlpha();
	void outputMatrix(char* name, const MatrixXd& m, bool fore);
	void kernelFun(Point p, std::function<void(int, int, int, int)> cb, int size = ksize);
	static const int ksize = 25;
	int minLen = 10;// ksize*ksize * 1 / 3;
	MatrixXd foreImg, backImg, origImg, alphaImg;
private:
	ImpressionistDoc* m_pDoc;
	ImpressionistUI* m_pUI;
	int w;
	int h;
	bool *unkn = NULL; //*fore = NULL, *back = NULL, 
	unsigned char *img = NULL;
	// float *alpha = NULL;
	int unknSum = 0;
	int findSum = 0;
	std::vector<std::vector<float>> gaussianKernel;
};

