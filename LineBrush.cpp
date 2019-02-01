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
	int size = pDoc->getSize();
	int angle = pDoc->getLineAngle();

	glPointSize((float) size);
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

	// glBegin(GL_POINTS);
	int width = pDoc->getLineWidth();
	int size = pDoc->getSize();
	int angle = pDoc->getLineAngle();

	// glPointSize((float)size);
	SetColor(source);

	glBegin(GL_LINES);
	double dsin = size*sin(degToRad(angle));
	double dcos = size*cos(degToRad(angle));
	glVertex2d(source.x + dcos,source.y + dsin);
	glVertex2d(target.x - dcos,target.y - dsin);
	glEnd();

	// glPopMatrix();
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// ImpressionistDoc* pDoc = GetDocument();
	// int width = pDoc->getLineWidth();
	// int size = pDoc->getSize();
	// int angle = pDoc->getLineAngle();
	// glTranslated(-source.x, -source.y, 0);
	// glRotated(-angle, 0, 0, 1);
	// glTranslated(source.x, source.y, 0);
	glPopMatrix();
}

