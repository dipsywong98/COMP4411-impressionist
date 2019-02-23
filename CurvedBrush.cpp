#include "CurvedBrush.h"

extern Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel);
extern double degToRad(double);

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
		glBegin(GL_POLYGON);

		for (int i = 0; i < 360; i++)
		{
			double rad = degToRad(i);
			glVertex2f(
				static_cast<GLfloat>(pt.x) + cos(rad)*R / 2.f,
				static_cast<GLfloat>(pt.y) + sin(rad)*R / 2.f
			);
		}

		glEnd();

		if(i+1<K.size())
		{
			Point pt1 = K[i + 1];
			float t = atan2(pt1.y - pt.y, pt1.x - pt.x);
			float dx = R*cos(t);
			float dy = R*sin(t);
			int x0 = pt.x, y0 = pt.y, x1 = pt1.x, y1 = pt1.y;
			float d = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
			float Rx = dx*R / d, Ry = dy*R / d;
			glBegin(GL_POLYGON);
				glVertex2d(x0 + Ry, y0 - Rx);
				glVertex2d(x1 + Ry, y1 - Rx);
				glVertex2d(x1 - Ry, y1 + Rx);
				glVertex2d(x0 - Ry, y0 + Rx);
			glEnd();
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

	std::function<GLubyte*(unsigned char*, int, int)> getColor = [&w](unsigned char* p,int xx, int yy)->GLubyte* {
		return p + (yy*w + xx) * 3;
	};

	std::function<float(unsigned char*, unsigned char*)> colorDist = [](unsigned char* p0, unsigned char* p1)->float {
		return sqrt(pow(p0[0] - p1[0], 2) + pow(p0[1] - p1[1], 2) + pow(p0[2] - p1[2], 2));
	};

	GLubyte* strokeColor = getColor(refImg, x0, y0);
	SetColor(strokeColor);

	for (int i = 0; i < maxStrokeLength; i++) {
		GLubyte* refColor = getColor(refImg, x, y);
		GLubyte* canvasColor = getColor(canvas, x, y);
		if (i > minStrokeLength && colorDist(refColor, canvasColor) < colorDist(refColor, strokeColor)) {
			return K;
		}

				// detect vanishing gradient
		Point grad = CalGradient(Point(x, y), Point(x, y), [&](int x, int y) {return getColor(refImg, x, y); });
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
	color4[3] = GLubyte(255 * pDoc->getAlpha());

	glColor4ubv(color4);
}
