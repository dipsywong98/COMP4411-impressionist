//
// paintview.cpp
//
// The code maintaining the painting view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>


extern double randAlter(double, double);

#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6


#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#endif

static int		eventToDo;
static int		isAnEvent=0;
static Point	coord;

PaintView::PaintView(int			x, 
					 int			y, 
					 int			w, 
					 int			h, 
					 const char*	l)
						: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;
}


void PaintView::draw()
{
	#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
	#endif // !MESA

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(!valid())
	{

		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		ortho();

		glClear( GL_COLOR_BUFFER_BIT );
	}

	Point scrollpos;// = GetScrollPosition();
	scrollpos.x = 0;
	scrollpos.y	= 0;

	m_nWindowWidth	= w();
	m_nWindowHeight	= h();

	int drawWidth, drawHeight;
	drawWidth = min( m_nWindowWidth, m_pDoc->m_nPaintWidth );
	drawHeight = min( m_nWindowHeight, m_pDoc->m_nPaintHeight );

	int startrow = m_pDoc->m_nPaintHeight - (scrollpos.y + drawHeight);
	if ( startrow < 0 ) startrow = 0;

	m_pPaintBitstart = m_pDoc->m_ucPainting + 
		3 * ((m_pDoc->m_nPaintWidth * startrow) + scrollpos.x);

	m_nDrawWidth	= drawWidth;
	m_nDrawHeight	= drawHeight;

	m_nStartRow		= startrow;
	m_nEndRow		= startrow + drawHeight;
	m_nStartCol		= scrollpos.x;
	m_nEndCol		= m_nStartCol + drawWidth;

	if ( m_pDoc->m_ucPainting && !isAnEvent) 
	{
		RestoreContent();
	}

	bool willSave = false;

	if ( m_pDoc->m_ucPainting && isAnEvent) 
	{

		// Clear it after processing.
		isAnEvent	= 0;	

		Point source( coord.x + m_nStartCol, m_nEndRow - coord.y );
		Point target( coord.x, m_nWindowHeight - coord.y );
		
		// This is the event handler
		switch (eventToDo) 
		{
		case LEFT_MOUSE_DOWN:
			m_pDoc->recordHistory();
			m_pDoc->m_pCurrentBrush->BrushBegin( source, target );
			break;
		case LEFT_MOUSE_DRAG:
			m_pDoc->m_pCurrentBrush->BrushMove( source, target );
			break;
		case LEFT_MOUSE_UP:
			m_pDoc->m_pCurrentBrush->BrushEnd( source, target );
			// SaveCurrentContent();
			// RestoreContent();
			willSave = true;
			break;
		case RIGHT_MOUSE_DOWN:
			m_pDoc->m_pCurrentBrush->RightBegin(source, target);
			break;
		case RIGHT_MOUSE_DRAG:
			RestoreContent();
			m_pDoc->m_pCurrentBrush->RightMove(source, target);
			break;
		case RIGHT_MOUSE_UP:
			RestoreContent();
			m_pDoc->m_pCurrentBrush->RightEnd(source, target);
			break;

		default:
			printf("Unknown event!!\n");		
			break;
		}
	}

	if(willAutoFill)
	{
		autoFill();
		willAutoFill = false;
		willSave = true;
	}


	glFlush();


#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
#endif // !MESA

	if(willSave)
	{
		SaveCurrentContent();
		// RestoreContent();
	}
}

void PaintView::prepareAutoFill()
{
	willAutoFill = true;
	redraw();
}


int PaintView::handle(int event)
{
	switch(event)
	{
	case FL_ENTER:
		// refresh();
	    // redraw();
		break;
	case FL_PUSH:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DOWN;
		else
			eventToDo=LEFT_MOUSE_DOWN;
		isAnEvent=1;
		redraw();
		break;
	case FL_DRAG:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_DRAG;
		else
			eventToDo=LEFT_MOUSE_DRAG;
		isAnEvent=1;
		redraw();
		m_pDoc->m_pUI->m_origView->setCursor(coord);
		break;
	case FL_RELEASE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button()>1)
			eventToDo=RIGHT_MOUSE_UP;
		else
			eventToDo=LEFT_MOUSE_UP;
		isAnEvent=1;
		redraw();
		break;
	case FL_MOVE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		m_pDoc->m_pUI->m_origView->setCursor(coord);
		break;
	default:
		return 0;
		break;

	}



	return 1;
}

void PaintView::refresh()
{
	redraw();
}

void PaintView::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
}

void PaintView::SaveCurrentContent()
{
	// Tell openGL to read from the front buffer when capturing
	// out paint strokes
	glReadBuffer(GL_FRONT);

	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glPixelStorei( GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	
	glReadPixels( 0, 
				  m_nWindowHeight - m_nDrawHeight, 
				  m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart );
}


void PaintView::RestoreContent()
{
	glDrawBuffer(GL_BACK);

	glClear( GL_COLOR_BUFFER_BIT );

	glRasterPos2i( 0, m_nWindowHeight - m_nDrawHeight );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth );
	glDrawPixels( m_nDrawWidth, 
				  m_nDrawHeight, 
				  GL_RGB, 
				  GL_UNSIGNED_BYTE, 
				  m_pPaintBitstart);

//	glDrawBuffer(GL_FRONT);
}

void PaintView::autoFill()
{
	m_pDoc->recordHistory();
	willAutoFill = false;

	const double r = m_pDoc->m_pUI->getAutoFillRandom();//max percentage different
	const bool randAttr = m_pDoc->m_pUI->getAutoFillEnableRandom();//true;//randomize attributes

	const int size = m_pDoc->getSize();
	const int lineWidth = m_pDoc->getLineWidth();
	const int lineAngle = m_pDoc->getLineAngle();
	const int w = m_pDoc->m_nWidth;
	const int h = m_pDoc->m_nHeight;
	const int s = m_pDoc->m_pUI->getAutoFillStrike();

	std::vector<Point> points;
	for (int i = 0; i < w; i += s)
	{
		for (int j = 0; j < h; j += s)
		{
			points.emplace_back(randAlter(i,r), randAlter(j,r));
		}
	}
	unsigned const seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::shuffle(points.begin(), points.end(),std::default_random_engine(seed));

	for(auto& pt: points){
		const int i = pt.x;
		const int j = pt.y;
		// const Point source(i + m_nStartCol, m_nEndRow - j);
		if(randAttr)
		{
			m_pDoc->m_pUI->setSize(randAlter(size,r));
			m_pDoc->m_pUI->setLineWidth(randAlter(lineWidth,r));
			m_pDoc->m_pUI->setLineAngle(randAlter(lineAngle,r));
		}
		const Point target(i, j + m_nWindowHeight - m_nDrawHeight);
		m_pDoc->m_pCurrentBrush->BrushBegin(target, target);
		m_pDoc->m_pCurrentBrush->BrushEnd(target, target);
		
	}
	// glFlush();

	// SaveCurrentContent();
	// RestoreContent();
	m_pDoc->m_pUI->setSize(size);
	m_pDoc->m_pUI->setLineWidth(lineWidth);
	m_pDoc->m_pUI->setLineAngle(lineAngle);
}

void PaintView::applyKernel()
{
	int size = m_pDoc->m_pUI->kernel.size();
	const int w = m_pDoc->m_nWidth;
	const int h = m_pDoc->m_nHeight;
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j< h; j++)
		{
			int c[]={0,0,0};
			for (int a=0; a<size; a++)
			{
				for(int b=0; b<size; b++)
				{
					for(int d=0;d<3;d++)
					{
						c[d] += m_pDoc->m_pUI->kernel[a][b] * m_pDoc->GetOriginalPixel(i + a - size / 2, j + b - size / 2)[d];
					}
				}
			}
			m_pDoc->m_ucPainting[(j*m_nWindowWidth + i) * 3] = c[0];
			m_pDoc->m_ucPainting[(j*m_nWindowWidth + i) * 3 + 1] = c[1];
			m_pDoc->m_ucPainting[(j*m_nWindowWidth + i) * 3 + 2] = c[2];
		}
	}
	refresh();
}
