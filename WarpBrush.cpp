//
// WarpBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "WarpBrush.h"

extern float frand();
extern std::vector<std::vector<float>> getGaussianKernel(float sigma, int size);
extern GLubyte* getColor(unsigned char* p, int xx, int yy, int w, int h);

WarpBrush::WarpBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void WarpBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();

	kernel = getGaussianKernel(1, size);
	for(int i = 0; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			if(sqrt(pow(i-size/2,2)+pow(j-size/2,2)) > size)
			{
				kernel[i][j] = 0;
			}
		}
	}
	prev = source;
	BrushMove(source, target);
}

void WarpBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("WarpBrush::BrushMove  document is NULL\n");
		return;
	}

	if (source.x > pDoc->m_nPaintWidth || source.y < 0)
	{
		return;
	}

	Point curr = source;
	int size = pDoc->getSize();
	const int w = pDoc->m_nWidth;
	const int h = pDoc->m_nHeight;

	unsigned char* canvas = pDoc->m_ucPainting;

	std::vector<GLubyte> colors_r(size*size);
	std::vector<GLubyte> colors_g(size*size);
	std::vector<GLubyte> colors_b(size*size);

	for (int a = 0; a<size; a++)
	{
		for (int b = 0; b<size; b++)
		{
			float wt = kernel[a][b];
			if (wt == 0) continue;
			int cx = curr.x + a - size / 2;
			int cy = curr.y + b - size / 2;
			if (cx < 0 || cy < 0 || cx >= w - 1 || cy >= h - 1)continue;
			int px = prev.x + a - size / 2;
			int py = prev.x + b - size / 2;
			if (px < 0 || py < 0 || px >= w - 1 || py >= h - 1)continue;
			GLubyte c[3];
			memcpy(c, getColor(canvas, cx, cy, w, h), 3);
			GLubyte p[3];
			memcpy(p, getColor(canvas, px, py, w, h), 3);
			
			colors_r[a*size + b] = wt*p[0] * (1 - wt)*c[0];
			colors_g[a*size + b] = wt*p[1] * (1 - wt)*c[1];
			colors_b[a*size + b] = wt*p[2] * (1 - wt)*c[2];
			
		}
	}

	for (int a = 0; a < size; a++)
	{
		for (int b = 0; b < size; b++)
		{
			float wt = kernel[a][b];
			if (wt == 0) continue;
			int cx = curr.x + a - size / 2;
			int cy = curr.y + b - size / 2;
			if (cx < 0 || cy < 0 || cx >= w - 1 || cy >= h - 1)continue;
			int px = prev.x + a - size / 2;
			int py = prev.x + b - size / 2;
			if (px < 0 || py < 0 || px >= w - 1 || py >= h - 1)continue;

			GLubyte color[] = { colors_r[a*size + b] , colors_g[a*size + b] , colors_b[a*size + b] };

			memcpy(canvas + (cy*w + cx) * 3, color, 3);
		}
	}
	prev = curr;
}

void WarpBrush::BrushEnd(const Point source, const Point target)
{
}

