#include "AlphaMapBrush.h"

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "AlphaMapBrush.h"

AlphaMapBrush::AlphaMapBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}


AlphaMapBrush::~AlphaMapBrush()
{
}

void AlphaMapBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	BrushMove(source, target);
}

void AlphaMapBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("AlphaMapBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	glPointSize((float)1);
	glBegin(GL_POINTS);

	int aw = pDoc->m_alphaMapWidth;
	int ah = pDoc->m_alphaMapHeight;
	float alphaOriginal = pDoc->m_pUI->getAlpha();

	for(int i = 0; i<aw; i++)
	{
		for(int j=0; j<ah; j++)
		{
			pDoc->m_pUI->setAlpha(pDoc->GetAlpha(i, j));
			SetColor(source);
			glVertex2d(target.x + i - aw/2, target.y + j - ah/2);
		}
	}
	pDoc->m_pUI->setAlpha(alphaOriginal);
	glEnd();
}

void AlphaMapBrush::BrushEnd(const Point source, const Point target)
{
}
