#ifndef IMPBRUSH_H
#define IMPBRUSH_H

//
// ImpBrush.h
//
// The header file of virtual brush. All the other brushes inherit from it.
//

#include <stdlib.h>
#include <cmath>

// Each brush type has an associated constant.
enum
{
	BRUSH_POINTS = 0,
	BRUSH_LINES,
	BRUSH_CIRCLES,
	BRUSH_SCATTERED_POINTS,
	BRUSH_SCATTERED_LINES,
	BRUSH_SCATTERED_CIRCLES,
	ALPHA_MAP,
	CURVED_BRUSH,
	WARP_BRUSH,
	BLUR_BRUSH,
	SHARPEN_BRUSH,
	PIXELIZE_BRUSH,
	BRUSH_PATTERN,
	POLYGON_BRUSH,
	SCATTERED_POLYGON_BRUSH,
	NUM_BRUSH_TYPE // Make sure this stays at the end!
};

enum
{
	SLIDER_RIGHT_CLICK = 0,
	GRADIENT,
	BRUSH_DIRECTION,
	GRADIENT_ANOTHER,
	NUM_DIRECTION_TYPE
};


class ImpressionistDoc; // Pre-declaring class

class Point 
{
public:
	Point() {};
	Point(int xx, int yy) { x = xx; y = yy; };
	Point(int xx, int yy, double aa) { x = xx; y = yy; a = aa; };
	Point operator =(Point pt) { x = pt.x; y = pt.y; return *this; }
	float norm2() const { return sqrt(x*x + y*y); }
	const ::Point& operator+=(const Point& point);
	Point T() const { return Point(y, x); };

	int x, y;
	double a;
};

class ImpBrush 
{
protected:
	ImpBrush::ImpBrush( ImpressionistDoc* pDoc = NULL, char* name = NULL );
	int m_DirectionType;

public:
	// The implementation of your brush should realize these virtual functions
	virtual void BrushBegin( const Point source, const Point target ) = 0;
	virtual void BrushMove( const Point source, const Point target ) = 0;
	virtual void BrushEnd( const Point source, const Point target ) = 0;
	virtual void RightBegin(const Point source, const Point target){}
	void setDirectionType(int type);
	virtual void RightMove(const Point source, const Point target){};
	virtual void RightEnd(const Point source, const Point target){};

	// according to the source image and the position, determine the draw color
	void SetColor( const Point source );
	void BrushBegin(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas);

	// get Doc to communicate with it
	ImpressionistDoc* GetDocument( void );

	// Return the name of the brush (not used in this version).
	char* BrushName( void );
	
	static int			c_nBrushCount;	// How many brushes we have,
	static ImpBrush**	c_pBrushes;		// and what they are.

private:
	ImpressionistDoc*	m_pDoc;

	// Brush's name (not used in this version).
	char*				m_pBrushName;
};

#endif