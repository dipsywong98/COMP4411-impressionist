//
// originalview.cpp
//
// The code maintaining the original view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "originalview.h"
#include <functional>
#include "impressionistUI.h"

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#endif

extern Point CalGradient(const Point source, const Point target, const std::function<GLubyte*(int, int)> getPixel);

OriginalView::OriginalView(int			x, 
						   int			y, 
						   int			w, 
						   int			h, 
						   const char*	l)
							: Fl_Gl_Window(x,y,w,h,l)
{
	m_nWindowWidth	= w;
	m_nWindowHeight	= h;

}

void OriginalView::draw()
{
	if(!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable( GL_DEPTH_TEST );

		// Tell openGL to read from the front buffer when capturing
		// out paint strokes 
		glReadBuffer( GL_FRONT );
		ortho();

	}

	glClear( GL_COLOR_BUFFER_BIT );

	int drawWidth, drawHeight;

	if ( m_pDoc->m_ucBitmap ) 
	{
		// note that both OpenGL pixel storage and the Windows BMP format
		// store pixels left-to-right, BOTTOM-to-TOP!!  thus all the fiddling
		// around with startrow.

		m_nWindowWidth=w();
		m_nWindowHeight=h();

		GLvoid* bitstart;

		// we are not using a scrollable window, so ignore it
		Point scrollpos;	// = GetScrollPosition();
		scrollpos.x=scrollpos.y=0;

		drawWidth	= min( m_nWindowWidth, m_pDoc->m_nWidth );
		drawHeight	= min( m_nWindowHeight, m_pDoc->m_nHeight );

		int	startrow	= m_pDoc->m_nHeight - (scrollpos.y + drawHeight);
		if ( startrow < 0 ) 
			startrow = 0;

		// if (willFindEdge)
		// {
		// 	findEdge();
		// 	willFindEdge = false;
		//
		// 	glPixelStorei(GL_PACK_ALIGNMENT, 1);
		// 	glPixelStorei(GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth);
		//
		// 	glReadPixels(0,
		// 		m_nWindowHeight - drawHeight,
		// 		drawWidth,
		// 		drawHeight,
		// 		GL_RGB,
		// 		GL_UNSIGNED_BYTE,
		// 		m_pDoc->m_ucEdge + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x));
		// 	glFlush();
		// }
		if(willFindEdge)
		{
			findEdge();
			willFindEdge = false;
		}

		bitstart = m_pDoc->m_ucBitmap + 3 * ((m_pDoc->m_nWidth * startrow) + scrollpos.x);

		// just copy image to GLwindow conceptually
		glRasterPos2i( 0, m_nWindowHeight - drawHeight );
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glPixelStorei( GL_UNPACK_ROW_LENGTH, m_pDoc->m_nWidth );
		glDrawBuffer( GL_BACK );
		glDrawPixels( drawWidth, drawHeight, GL_RGB, GL_UNSIGNED_BYTE, bitstart );
		if(cursor.x >= 0 && cursor.y >= 0 && cursor.x < m_nWindowWidth && cursor.y < m_nWindowHeight)
		{
			displayCursor();
		}
	}
			
	glFlush();
}

void OriginalView::refresh()
{
	redraw();
}

void OriginalView::resizeWindow(int	width, 
								int	height)
{
	resize(x(), y(), width, height);
}

void OriginalView::setCursor(const Point pt)
{
	cursor = pt;
	redraw();
}

void OriginalView::displayCursor()
{
	glPointSize(5);
	glBegin(GL_POINTS);
		glColor3d(1, 0, 0);
		glVertex2d(cursor.x, m_nWindowHeight - cursor.y);
	glEnd();
}

void OriginalView::findEdge()
{
	const int threshold = m_pDoc->m_pUI->getEdgeThreshold();

	// glPointSize(1);
	// glBegin(GL_POINTS);
	for(int i=0;i < m_nWindowWidth; i++)
	{
		for(int j=0; j<m_nWindowHeight; j++)
		{
			Point grad = CalGradient(Point(i, j), Point(i, j), [&](int x, int y) {return m_pDoc->GetOriginalPixel(x, y); });
			// if(grad.x !=0 || grad.y != 0)
			// {
			// 	printf("hey");
				const bool color = sqrt(pow(grad.x, 2) + pow(grad.y, 2)) > threshold;
				if(color)
				{
					
				// glColor3d(color, color, color);
				// glVertex2d(i, j);
				m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3] = 255;
				m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3 + 1] = 255;
				m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3 + 2] = 255;
				}
				else
				{
					m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3] = 0;
					m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3 + 1] = 0;
					m_pDoc->m_ucEdge[(j*m_nWindowWidth + i) * 3 + 2] = 0;
				}
			// }
		}
	}
	// glEnd();
	// glFlush();

}

void OriginalView::prepareFindEdge()
{
	m_pDoc->m_ucBitmap = m_pDoc->m_ucEdge;
	willFindEdge = true;
	refresh();
}

