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
#include <functional>

extern float frand();
extern double degToRad(double);
extern double radToDeg(double);

extern Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel);

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

	if (pDoc == NULL)
	{
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	SetColor(source);

	switch (ImpBrush::m_DirectionType)
	{
	case SLIDER_RIGHT_CLICK:
		DrawLine(source, target, degToRad(pDoc->getLineAngle()));
		break;
	case GRADIENT:
		{
			Point grad = CalGradient(source, target, [&](int x, int y) {return pDoc->GetOriginalPixel(x,y); });
			const double rad_ = atan2(grad.y, grad.x);
			DrawLine(source, target, rad_);
		}
		break;
	case GRADIENT_ANOTHER:
		{
			Point grad = CalGradient(source, target, [&](int x, int y) {return pDoc->GetAnotherPixel(x,y); });
			const double rad_ = atan2(grad.y, grad.x);
			DrawLine(source, target, rad_);
		}
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
	start = target;
}

void LineBrush::RightMove(const Point source, const Point target)
{
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(start.x, start.y);
	glVertex2d(target.x, target.y);
	glEnd();
}

void LineBrush::RightEnd(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int angle = static_cast<int>(radToDeg(atan2(target.y - start.y, target.x - start.x)));
	dlg->setLineAngle(angle);
}

void LineBrush::DrawLine(const Point source, const Point target, const double rad)
{
	ImpressionistDoc* pDoc = GetDocument();
	int size = pDoc->getSize();

	const int w = pDoc->m_nPaintWidth;
	const int h = pDoc->m_nPaintHeight;
	const bool edgeClipping = pDoc->m_pUI->getEdgeClip();

	const double dsin = size * sin(rad) / 2.0f;
	const double dcos = size * cos(rad) / 2.0f;
	// double x1 = ;
	// double x2 = ;
	// double y1 = ;
	// double y2 = ;
	//
	// if(x1 > w || x2 > w)
	// {
	// 	const double m = (y1 - y2) / (x1 - x2);
	// 	if(x1 > w)
	// 	{
	// 		y1 -= m*(x1 - w);
	// 		x1 = w;
	// 	} else
	// 	{
	// 		y2 -= m*(x2 - w);
	// 		x2 = w;
	// 	}
	// }
	// if(y1 > h || y2 > h)
	// {
	// 	const double m = (x1 - x2) / (y1 - y2);
	// 	if(y1>h)
	// 	{
	// 		x1 -= m*(y1 - h);
	// 		y1 = h;
	// 	} else
	// 	{
	// 		x2 -= m*(y2 - h);
	// 		y2 = h;
	// 	}
	// }

	Point pt0 = Point( target.x + dcos, target.y + dsin), pt1 = Point( target.x - dcos, target.y - dsin);

	if(edgeClipping)
	{
		pt0 = ClippedPoint(target, size/2, rad);
		pt1 = ClippedPoint(target, size/2, M_PI +rad);
	}

	glBegin(GL_LINES);
	glVertex2d(pt0.x, pt0.y);
	glVertex2d(pt1.x, pt1.y);
	glEnd();
}

Point LineBrush::ClippedPoint(const Point origin, const int size, const double rad)
{
	ImpressionistDoc* pDoc = GetDocument();
	const int w = pDoc->m_nPaintWidth;
	const int h = pDoc->m_nPaintHeight;
	// int dist = sqrt(pow(origin.x - target.x, 2) + pow(origin.y + target.y,2)) + 1;
	for(int i = 0; i < size; i++)
	{
		const Point pt = Point(origin.x + i*cos(rad), origin.y + i*sin(rad));
		if(pDoc->IsEdge(pt))
		{
			return pt;
		}
		if(pt.x<=0 || pt.y<=0 || pt.x>= w-1 || pt.y >= h-1)
		{
			return pt;
		}
		if(i == size - 1)
		{
			return pt;
		}
	}
	return origin;
}
