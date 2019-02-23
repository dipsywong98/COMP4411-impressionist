#include "CurvedBrush.h"

extern Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel);
extern double degToRad(double);
extern GLubyte* getColor(unsigned char* p, int xx, int yy, int w, int h);
extern float colorDist(unsigned char* p0, unsigned char* p1);

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
	int x0 = source.x, y0 = source.y;
	int R = pDoc->getSize();
	unsigned char* refImg = pDoc->m_ucOriginal;
	unsigned char* canvas = pDoc->m_ucPainting;
	BrushBegin(x0, y0, R, refImg, canvas);
}

void CurvedBrush::BrushBegin(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas)
{
	std::vector<Point> K = makeCurvedBrush(x0, y0, R, refImg, canvas);
	for(int i = 0; i< K.size(); i++)
	{
		Point pt = K[i];
		CircleBrush::DrawCircle(pt, R);

		if(i+1<K.size())
		{
			Point pt1 = K[i + 1];
			int x0 = pt.x, y0 = pt.y, x1 = pt1.x, y1 = pt1.y;
			float d = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
			float t = atan2(pt1.y - pt.y, pt1.x - pt.x);
			for(int i=1; i<R; i+=R/2)
			{
				CircleBrush::DrawCircle(Point(x0+i*cos(t),y0+i*sin(t)), R);
			}
		}
	}
}

void CurvedBrush::BrushEnd(const Point source, const Point target)
{
}

std::vector<Point> CurvedBrush::makeCurvedBrush(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* pUI = pDoc->m_pUI;
	std::vector<Point> K = { Point(x0,y0) };
	int x = x0, y = y0;
	float lastDx = 0, lastDy = 0;
	int maxStrokeLength = pUI->m_painterlyMaxStroke;
	int minStrokeLength = pUI->m_painterlyMinStroke;
	float fc = pUI->m_painterlyCurvature;
	const int w = pDoc->m_nWidth;
	const int h = pDoc->m_nHeight;

	GLubyte* strokeColor = getColor(refImg, x0, y0,w,h);
	SetColor(strokeColor);

	for (int i = 0; i < maxStrokeLength; i++) {
		//detect out of boundary
		if(x<0||y<0||x>=w||y>=h)
		{
			return K;
		}


		GLubyte* refColor = getColor(refImg, x, y,w,h);
		GLubyte* canvasColor = getColor(canvas, x, y,w,h);
		if (i > minStrokeLength && colorDist(refColor, canvasColor) < colorDist(refColor, strokeColor)) {
			return K;
		}

		// detect vanishing gradient
		Point grad = CalGradient(Point(x, y), Point(x, y), [&](int x, int y) {return getColor(refImg, x, y,w,h); });
		float gradStrength = grad.norm2();
		if (gradStrength == 0) {
			return K;
		}

		float gx = grad.x / gradStrength;
		float gy = grad.y / gradStrength;
		float dx = -gy;
		float dy = gx;

				// if necessary, reverse direction 
		if (lastDx * dx + lastDy * dy < 0) {
			dx = -dx;
			dy = -dy;
		}
				// filter the stroke direction
		dx = fc*dx + (1 - fc)*lastDx;
		dy = fc*dy + (1 - fc)*lastDy;
		float m = sqrt(dx*dx + dy*dy);
		dx /= m;
		dy /= m;
		x += R*dx;
		y += R*dy;
		lastDx = dx;
		lastDy = dy;
		K.emplace_back(x, y);
	}
	return K;

}

void CurvedBrush::SetColor(GLubyte* color3)
{
	ImpressionistDoc* pDoc = GetDocument();


	GLubyte color4[4];

	memcpy(color4, color3, 3);
	color4[3] = GLubyte(255 * pDoc->m_pUI->m_painterlyAlpha);

	glColor4ubv(color4);
}
