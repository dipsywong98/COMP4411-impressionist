#pragma once
#include "impressionistDoc.h"
#include <vector>
#include <functional>

class Bayesian
{
public:
	Bayesian(ImpressionistDoc* pDoc, ImpressionistUI* pUI);
	~Bayesian();
	void init();
	bool trySolvePix(Point pt);	//return true if alpha is determined
	void solve(char* triimg_name);
	bool openTriImg(char* iname);
	void outputAlpha();
	void kernelFun(Point p, std::function<void(int, int, int, int)> cb, int size = ksize);
	static const int ksize = 25;
private:
	ImpressionistDoc* m_pDoc;
	ImpressionistUI* m_pUI;
	int w;
	int h;
	bool *fore = NULL, *back = NULL, *unkn = NULL;
	unsigned char *img = NULL;
	float *alpha = NULL;
	int unknSum = 0;
	int findSum = 0;
	std::vector<std::vector<float>> gaussianKernel;
};

