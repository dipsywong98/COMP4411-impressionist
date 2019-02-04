//
// ScatteredPointBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ScatteredPointBrush.h"

extern float frand();
extern int irand(int max);

ScatteredPointBrush::ScatteredPointBrush(ImpressionistDoc* pDoc, char* name) :
	PointBrush(pDoc, name)
{
}

void ScatteredPointBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	// int size = pDoc->getSize();



	// glPointSize((float)size);
	glPointSize(1.f);
	BrushMove(source, target);
}

void ScatteredPointBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatteredPointBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	int size = dlg->getSize();

	SetColor(source);

	for (int i = 0; i < size+0.3*irand(size*size); i++)
	{

		// glBegin(GL_POINTS);
		const int dx = -size / 2.f + irand(size);
		const int dy = -size / 2.f + irand(size);
		const Point sourcePt(source.x + dx, source.y + dy);
		const Point targetPt(target.x + dx, target.y + dy);
		PointBrush::BrushMove(sourcePt, targetPt);

		// glEnd();
	}
}

void ScatteredPointBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

