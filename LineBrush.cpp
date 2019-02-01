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

	int size = pDoc->getSize();
	int angle = pDoc->getLineAngle();

	SetColor(source);

	glBegin(GL_LINES);
	double dsin = size*sin(degToRad(angle))/2.0f;
	double dcos = size*cos(degToRad(angle))/2.0f;
	glVertex2d(source.x + dcos,source.y + dsin);
	glVertex2d(target.x - dcos,target.y - dsin);
	glEnd();
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
}

