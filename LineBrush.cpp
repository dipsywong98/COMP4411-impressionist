//
// LineBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "LineBrush.h"
#include <ostream>
#include <iostream>

extern float frand();
extern double degToRad(double);
extern double radToDeg(double);

LineBrush::LineBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void LineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int width = pDoc->getLineWidth();
	glLineWidth((float)width);
	prev = source;
	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	SetColor(source);

	switch(ImpBrush::m_DirectionType)
	{
	case SLIDER_RIGHT_CLICK:
		DrawLine(source, target, degToRad(pDoc->getLineAngle()));
		break;
	case BRUSH_DIRECTION:
		const double rad = atan2(prev.y - source.y, prev.x - source.x);
		DrawLine(source, target, rad);
		prev = source;
		break;
	}

}

void LineBrush::BrushEnd(const Point source, const Point target)
{
}

void LineBrush::RightBegin(const Point source, const Point target)
{
	start = source;
}

void LineBrush::RightMove(const Point source, const Point target)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(start.x, start.y);
	glVertex2d(source.x, source.y);
	glEnd();
}

void LineBrush::RightEnd(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int angle = static_cast<int>(radToDeg(atan2(source.y - start.y, source.x - start.x)));
	dlg->setLineAngle(angle);
}

void LineBrush::DrawLine(const Point source, const Point target, const double rad)
{
	ImpressionistDoc* pDoc = GetDocument();
	int size = pDoc->getSize();
	glBegin(GL_LINES);
	const double dsin = size*sin(rad) / 2.0f;
	const double dcos = size*cos(rad) / 2.0f;
	glVertex2d(source.x + dcos, source.y + dsin);
	glVertex2d(target.x - dcos, target.y - dsin);
	glEnd();
}
