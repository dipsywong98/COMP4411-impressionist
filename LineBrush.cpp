//
// LineBrush.cpp
//
// The implementation of Line Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "LineBrush.h"

extern float frand();

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

	glPointSize((float) size);
	glLineWidth((float)width);
	//glRotate((float) angle, 0, 0, 1);

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
	glLineWidth((float)size);

	// glVertex2d(target.x, target.y);

	// glEnd();
	// double angle = pDoc->getLineAngle();
	// glBegin(GL_LINES);
	// glVertex2d(source.x-width/2, source.y);
	// glVertex2d(target.x+width/2, target.y);
	// SetColor(source);
	// //glRotate(angle, 0, 0, 1);
	// glEnd();
	glLineWidth(width);
	SetColor(source);
	glBegin(GL_LINES);
	glVertex2d(source.x+size,source.y);
	glVertex2d(target.x-size,target.y);
	glRotatef(angle, 0.0f, 0.0f, 1.0f);
	glEnd();
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}

