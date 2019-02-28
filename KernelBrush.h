#pragma once
#include "ImpBrush.h"
#include <vector>
#include "BlurBrush.h"

class KernelBrush :
	public ImpBrush
{
public:
	KernelBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);
	~KernelBrush();
	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	void ApplyKernel(Point source, Point target, GLubyte* ref);
	char* BrushName(void);
	void SetKernel(std::vector<std::vector<float>>kernel) { this->kernel = kernel; }

	std::vector<std::vector<float>>kernel;
	unsigned char *ref = NULL;
};

