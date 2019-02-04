//
// ScatteredCircleBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ScatteredCircleBrush.h"

extern float frand();
extern int irand(int max);

ScatteredCircleBrush::ScatteredCircleBrush(ImpressionistDoc* pDoc, char* name) :
	CircleBrush(pDoc, name)
{
}

void ScatteredCircleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	// int size = pDoc->getSize();



	// glPointSize((float)size);
	glPointSize(1.f);
	BrushMove(source, target);
}

void ScatteredCircleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatteredCircleBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	int size = dlg->getSize();

	SetColor(source);

	for (int i = 0; i < 0.3*irand(size); i++)
	{
		const double dx = -size / 2.f + irand(size);
		const double dy = -size / 2.f + irand(size);
		const Point sourcePt(source.x + dx, source.y + dy);
		const Point targetPt(target.x + dx, target.y + dy);
		CircleBrush::BrushMove(sourcePt, targetPt);
	}
}

void ScatteredCircleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

