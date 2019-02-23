//
// ImpBrush.cpp
//
// The implementation of virtual brush. All the other brushes inherit from it.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ImpBrush.h"

// Static class member initializations
int			ImpBrush::c_nBrushCount	= 0;
ImpBrush**	ImpBrush::c_pBrushes	= NULL;

const ::Point& Point::operator+=(const Point& point)
{
	this->x += point.x;
	this->y += point.y;
	return *this;
}

ImpBrush::ImpBrush(ImpressionistDoc*	pDoc, 
				   char*				name) :
					m_pDoc(pDoc), 
					m_pBrushName(name),
					m_DirectionType(SLIDER_RIGHT_CLICK)
{
}

//---------------------------------------------------
// Return m_pDoc, which connects the UI and brushes
//---------------------------------------------------
ImpressionistDoc* ImpBrush::GetDocument(void)
{
	return m_pDoc;
}

//---------------------------------------------------
// Return the name of the current brush
//---------------------------------------------------
char* ImpBrush::BrushName(void)
{
	return m_pBrushName;
}

void ImpBrush::setDirectionType(int type)
{
	m_DirectionType = type;
}

//----------------------------------------------------
// Set the color to paint with to the color at source,
// which is the coord at the original window to sample 
// the color from
//----------------------------------------------------
void ImpBrush::SetColor (const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();

	GLubyte color[4];

	memcpy ( color, pDoc->GetOriginalPixel( source ), 3 );

	auto* color_chooser = pDoc->m_pUI->m_colorChooser;
	auto mul_r = 0.0;
	auto mul_g = 0.0;
	auto mul_b = 0.0;
	const auto mode = color_chooser->mode();
	switch (mode)
	{
	case 0: //RGB
	case 1: //Byte
	case 2: //Hex
		mul_r = color_chooser->r();
		mul_g = color_chooser->g();
		mul_b = color_chooser->b();
	case 3: //HSV
		Fl_Color_Chooser::hsv2rgb(
			color_chooser->hue(),
			color_chooser->saturation(),
			color_chooser->value(),
			mul_r,
			mul_g,
			mul_b
		);
	default: {}
	}

	color[0] = static_cast<unsigned char>(color[0] * mul_r);
	color[1] = static_cast<unsigned char>(color[1] * mul_g);
	color[2] = static_cast<unsigned char>(color[2] * mul_b);

	color[3] = GLubyte(255 * pDoc->getAlpha());
	
	glColor4ubv( color );

}

void ImpBrush::BrushBegin(int x0, int y0, int R, unsigned char* refImg, unsigned char* canvas)
{
	BrushBegin(Point(x0, y0), Point(x0, y0));
}