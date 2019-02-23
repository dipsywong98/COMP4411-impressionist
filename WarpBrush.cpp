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
extern void getMeanColor(GLubyte* buf, unsigned char* p, int xx, int yy, int w, int h, float sigma, int size);

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
	// curr.x = prev.x + size / 2;
	Wrappify(prev, curr, size);
	prev = curr;
}

void WarpBrush::BrushEnd(const Point source, const Point target)
{
}

void WarpBrush::Wrappify(const Point origin, const Point end, int size)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;
	const int w = pDoc->m_nWidth;
	const int h = pDoc->m_nHeight;
	int sx = origin.x, sy = origin.y;
	int tx = end.x, ty = end.y;
	double rad = atan2(ty - sy, tx - sx);
	double c = cos(rad);
	double s = sin(rad);
	unsigned char* canvas = pDoc->m_ucPainting;
	glPointSize((float)1);
	glBegin(GL_POINTS);
	for(int x0p = sx-size/2; x0p < sx+size/2; x0p++)
	{
		if (x0p<0 || x0p>w - 1)continue;
		for(int y0p = sy-size/2; y0p < sy+size/2; y0p++)
		{
			if (y0p<0 || y0p>h - 1)continue;
			if (sqrt(pow(x0p - sx, 2) + pow(y0p - sy, 2)) > size/2) continue;
			double u0p = (x0p - sx)*c - s*(y0p - sy);
			double v0p = (x0p - sx)*s + c*(y0p - sy);
			double r = sqrt(pow(v0p, 2) + pow(u0p - (size - 1) / 2,2));
			double u0 = u0p - (size - r) / 2;
			double v0 = v0p;
			double x0 = sx + c*u0 + s*v0;
			double y0 = sy + -s*u0 + c*v0;

			GLubyte buf[3];
			getMeanColor(buf, canvas, x0, y0, w, h, 1, 3);
			SetColor(buf);
			glVertex2d(x0p,y0p);
		}
	}
	glEnd();
}

void WarpBrush::SetColor(GLubyte* color3)
{
	ImpressionistDoc* pDoc = GetDocument();


	GLubyte color4[4];

	memcpy(color4, color3, 3);
	color4[3] = GLubyte(255 * pDoc->m_pUI->m_painterlyAlpha);

	glColor4ubv(color4);
}
