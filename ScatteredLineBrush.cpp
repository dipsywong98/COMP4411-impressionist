//
// ScatteredLineBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ScatteredLineBrush.h"

extern float frand();
extern int irand(int max);

ScatteredLineBrush::ScatteredLineBrush(ImpressionistDoc* pDoc, char* name) :
	LineBrush(pDoc, name)
{
}

void ScatteredLineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	// int size = pDoc->getSize();



	// glPointSize((float)size);
	glPointSize(1.f);
	BrushMove(source, target);
}

void ScatteredLineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatteredLineBrush::BrushMove  document is NULL\n");
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
		LineBrush::BrushMove(sourcePt, targetPt);
	}
}

void ScatteredLineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

