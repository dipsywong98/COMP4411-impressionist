#include "CurvedBrush.h"
#include "impressionistDoc.h"
#include <functional>


extern Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel);

CurvedBrush::CurvedBrush(ImpressionistDoc* pDoc, char* name) :
	CircleBrush(pDoc, name)
{
}


CurvedBrush::~CurvedBrush()
{
}


void CurvedBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	glPointSize((float)size);

	BrushMove(source, target);
}

void CurvedBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	int size = pDoc->getSize();

	if (pDoc == NULL) {
		printf("CurvedBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}
	Point grad;
	CircleBrush::BrushMove(source, target);
	Point s = source, t = target;
	int maxPoint = 100;
	do
	{
		if (s.x < 0 || s.x > pDoc->m_nWidth || s.y < 0 || s.y > pDoc->m_nHeight)break;
		grad = CalGradient(s, t, [&](int x, int y) {return pDoc->GetOriginalPixel(x, y); });
		float gn = grad.norm2();
		s.x += size * grad.y / gn;
		t.x += size * grad.y / gn;
		s.y += size * grad.x / gn;
		t.y += size * grad.x / gn;
		CircleBrush::BrushMove(s, t);
		// s += grad.T();
		// t += grad.T();
	} while (--maxPoint && grad.norm2() > 0.01);
}

void CurvedBrush::BrushEnd(const Point source, const Point target)
{
}


