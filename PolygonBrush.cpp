#include "PolygonBrush.h"
//
// PolygonBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <ostream>
#include <iostream>

extern float frand();
extern int irand(int max);
extern double degToRad(double);

PolygonBrush::PolygonBrush(ImpressionistDoc* pDoc, char* name,bool scattered) :
	ImpBrush(pDoc, name),scattered(scattered)
{
}
void PolygonBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	BrushMove(source, target);
}

void PolygonBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("PolygonBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	int size = pDoc->getSize();
	if(!scattered)
	{	
		SetColor(source);
		DrawPolygon(target, size,nSides?nSides: irand(10));
	}else
	{
		for (int i = 0; i < 0.3*irand(size); i++)
		{
			const double dx = -size / 2.f + irand(size);
			const double dy = -size / 2.f + irand(size);
			const Point sourcePt(source.x + dx, source.y + dy);
			const Point targetPt(target.x + dx, target.y + dy);
			if (sourcePt.x > pDoc->m_nPaintWidth || sourcePt.y < 0)
			{
				continue;
			}
			SetColor(sourcePt);
			DrawPolygon(targetPt, size, nSides ? nSides : irand(10));
		}
	}
}

void PolygonBrush::BrushEnd(const Point source, const Point target)
{
}

void PolygonBrush::DrawPolygon(const Point target, int size, int nSides)
{
	glBegin(GL_POLYGON);
	if (nSides < 3)nSides = 3;
	double incre = 360.f / nSides;
	int offset = irand(360);
	for (double i = 0; i < 360; i+=incre)
	{
		double rad = degToRad(i + offset);
		glVertex2f(
			static_cast<GLfloat>(target.x) + cos(rad)*size / 2.f,
			static_cast<GLfloat>(target.y) + sin(rad)*size / 2.f
		);
	}

	glEnd();
}