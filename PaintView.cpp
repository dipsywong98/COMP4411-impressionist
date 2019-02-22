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
	} else if (willPainterly)
	{
		painterly();
		willPainterly = false;
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

void PaintView::preparePainterly()
{
	willPainterly = true;
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
	glReadBuffer(GL_BACK);

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
	const int w = m_pDoc->m_nWidth;
	const int h = m_pDoc->m_nHeight;
	const bool isNormalized = m_pDoc->m_pUI->getIsNormalizedKernel();
	unsigned char * before = new unsigned char[w*h * 3];
	memcpy(before, m_pDoc->m_ucPainting, w*h * 3);

	kernelHelper(before, m_pDoc->m_ucPainting, m_pDoc->m_pUI->kernel, w, h, isNormalized);

	refresh();
	delete[] before;
}

void PaintView::kernelHelper(unsigned char* before, unsigned char* target, const std::vector<std::vector<float>>& kernel, int w, int h, bool isNormalized)
{
	int size = kernel.size();
	float total = 0;
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			total += kernel[i][j];
		}
	}
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j< h; j++)
		{
			float c[] = { 0,0,0 };
			for (int a = 0; a<size; a++)
			{
				for (int b = 0; b<size; b++)
				{
					for (int d = 0; d<3; d++)
					{
						const int x = max(0, min(i - (size) / 2 + a, w - 1));
						const int y = max(0, min(j - (size) / 2 + b, h - 1));
						float delta = kernel[a][b] * before[(y*w + x) * 3 + d];
						if (isNormalized)
						{
							delta /= total;
						}
						c[d] += delta;
					}
				}
			}
			target[(j*w + i) * 3] = max(0, min(c[0], 255));
			target[(j*w + i) * 3 + 1] = max(0, min(c[1], 255));
			target[(j*w + i) * 3 + 2] = max(0, min(c[2], 255));
		}
	}
}

void PaintView::painterly()
{
	const int w = m_pDoc->m_nWidth;
	const int h = m_pDoc->m_nHeight;
	// fl_alert("yo painterly!");
	// std::vector<int> radii = { 18,15,10 };
	// const std::vector<std::vector<float>>& GaussianBlur = {
	// 	{
	// 		
	// 	}}
	int r0 = m_pDoc->m_pUI->m_painterlyR0;
	int layers = m_pDoc->m_pUI->m_painterlyLayers;
	std::vector<int> radii;
	for(int i=0; i<layers; i++)
	{
		radii.push_back(pow(2, r0));
		r0 = max(0, r0 - 1);
	}
	unsigned char* canvas = new unsigned char[w*h * 3];
	memset(canvas, 255, w*h * 3);
	for(int r: radii)
	{
		unsigned char* ref = new unsigned char[w*h * 3];
		memcpy(ref, m_pDoc->m_ucOriginal, w*h * 3);
		paintLayer(canvas, ref, r);
		delete[] ref;
	}

	delete[] canvas;
}


void PaintView::paintLayer(unsigned char* canvas, unsigned char* ref, int r)
{
	const int w = m_pDoc->m_nWidth;
	const int h = m_pDoc->m_nHeight;
	int grid= m_pDoc->getSize();
	int threshold = 200;

	std::vector<Point> strokes;
	std::vector<Point> nob;
	std::vector<float> diff(w*h);
	// float* diff = new float[w*h ];
	// memset(diff, 0, w*h);
	for(int i = 0; i<w; i++)
	{
		for(int j = 0; j<h; j++)
		{
			for(int c=0; c<3; c++)
			{
				diff[j*w + i] += pow(canvas[(j*w + i) * 3 + c] - ref[(j*w + i) * 3 + c], 2);
			}
			diff[j*w + i] = sqrt(diff[j*w + i]);
		}
	}

	for(int x=0; x<w; x+=grid)
	{
		for(int y=0; y<h; y+=grid)
		{
			//cal area sum
			float sumError = 0;
			Point argMax(x,y);
			float error = diff[y*w + x];
			int gridSum = 0;
			for(int a = max(x-grid/2,0); a<x+grid/2 && a<w; a++)
			{
				for (int b = max(y - grid / 2,0); b < y + grid / 2 && b<h; b++)
				{
					float e = diff[b*w + a];
					sumError += e;
					gridSum++;
					if(e>error)
					{
						argMax = Point(a, b);
						error = e;

					}
				}
			}
			sumError /= gridSum;

			if(sumError > threshold)
			{
				// std::vector<Point>s = makeStrokes(r, x, y, ref, w);
				strokes.push_back(argMax);
			}else
			{
				nob.push_back(argMax);
			}
		}
	}

	std::random_shuffle(strokes.begin(), strokes.end());
	std::random_shuffle(nob.begin(), nob.end());
	int sizeOriginal = m_pDoc->getSize();
	for(auto&& p:strokes)
	{
		// for(auto&& p:s)
		// {
		m_pDoc->m_pUI->setSize(r);
		m_pDoc->m_pCurrentBrush->BrushBegin(p, p);
		m_pDoc->m_pCurrentBrush->BrushEnd(p, p);
		// }
	}
	m_pDoc->m_pUI->setSize(sizeOriginal);
	glFlush();
	SaveCurrentContent();
	memcpy(canvas, m_pDoc->m_ucPainting, w*h * 3);

	// delete[] diff;
}