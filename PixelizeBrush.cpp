//
// PixelizeBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "PixelizeBrush.h"

extern float frand();

PixelizeBrush::PixelizeBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void PixelizeBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	BrushMove(source, target);
}

void PixelizeBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("PixelizeBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	int ksize = pUI->getSize(), csize = pUI->getLineWidth();
	int w = pDoc->m_nPaintWidth, h = pDoc->m_nPaintHeight;

	glPointSize(1.0);
	glBegin(GL_POINTS);
	for(int i = -ksize/2; i < ksize / 2; i++)
	{
		for (int j = -ksize / 2; j < ksize / 2; j++) {

			Point sample, subTarget;
			sample.x = source.x+i - (source.x+i) % csize;
			sample.y = source.y+j - (source.y+j) % csize;
			subTarget.x = target.x + i;
			subTarget.y = target.y + j;

			SetColor(sample);
			glVertex2d(subTarget.x, subTarget.y);
			
		}
	}
	glEnd();
}

void PixelizeBrush::BrushEnd(const Point source, const Point target)
{
}

