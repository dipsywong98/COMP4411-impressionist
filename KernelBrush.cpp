#include "KernelBrush.h"
//
// KernelBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <ostream>
#include <iostream>

extern float frand();
extern double degToRad(double);
extern GLubyte* getColor(unsigned char* p, int xx, int yy, int w, int h);

KernelBrush::KernelBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}
void KernelBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;
	int size = pUI->getSize();

	int w = pDoc->m_nPaintWidth, h = pDoc->m_nPaintHeight;
	ref = new unsigned char[w*h * 3];
	memcpy(ref, pDoc->m_ucPainting, w*h * 3);

	BrushMove(source, target);
}

void KernelBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("KernelBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	int w = pDoc->m_nPaintWidth, h = pDoc->m_nPaintHeight;
	int size = pDoc->getSize();

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			int tx = target.x + i - size / 2;
			int ty = target.y + j - size / 2;
			int sx = source.x + i - size / 2;
			int sy = source.y + j - size / 2;
			if (sx<0 || sy<0 || sx>w - 1 || sy>h - 1)continue;
			// if (tx<0 || ty<0 || tx>w - 1 || ty>h - 1)continue;
			ApplyKernel(Point(sx, sy), Point(tx, ty), ref);
		}
	}
	pUI->m_paintView->RestoreContent();
	glFlush();
}

void KernelBrush::BrushEnd(const Point source, const Point target)
{
	delete[] ref;
}

void KernelBrush::ApplyKernel(const Point source, const Point target, GLubyte* ref)
{
	ImpressionistDoc* pDoc = GetDocument();
	int w = pDoc->m_nPaintWidth, h = pDoc->m_nPaintHeight;
	int size = pDoc->getSize();
	int r = 0, g = 0, b = 0;

	for (int i = 0; i < kernel.size(); i++)
	{
		for (int j = 0; j < kernel.size(); j++)
		{
			int x = source.x + i - kernel.size() / 2;
			int y = source.y + j - kernel.size() / 2;
			if (x<0 || y<0 || x>w - 1 || y>h - 1)continue;
			r += getColor(ref, x, y, w, h)[0] * kernel[i][j];
			g += getColor(ref, x, y, w, h)[1] * kernel[i][j];
			b += getColor(ref, x, y, w, h)[2] * kernel[i][j];
		}
	}

	pDoc->m_ucPainting[(source.y*w + source.x) * 3] = max(0,min(r,255));
	pDoc->m_ucPainting[(source.y*w + source.x) * 3 + 1] = max(0,min(g,255));
	pDoc->m_ucPainting[(source.y*w + source.x) * 3 + 2] = max(0,min(b,255));
}