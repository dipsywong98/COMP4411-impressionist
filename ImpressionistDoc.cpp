// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//

#include <FL/fl_ask.H>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "ImpBrush.h"

// Include individual brush headers here.
#include "PointBrush.h"
#include "LineBrush.h"
#include "CircleBrush.h"
#include "ScatteredPointBrush.h"
#include "ScatteredLineBrush.h"
#include "ScatteredCircleBrush.h"
#include <future>
#include "AlphaMapBrush.h"
#include "CurvedBrush.h"
#include "WarpBrush.h"
#include "Bayesian.h"
#include "KernelBrush.h"
#include "PixelizeBrush.h"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

double ImpressionistDoc::viewportTracerRatio = 0.5;

ImpressionistDoc::ImpressionistDoc() 
{
	// Set NULL image name as init. 
	m_imageName[0]	='\0';	

	m_nWidth		= -1;
	m_ucBitmap		= NULL;
	m_ucPainting	= NULL;
	m_ucHistory	= NULL;
	m_ucAnother	= NULL;
	m_ucEdge = NULL;
	m_ucOriginal = NULL;
	m_ucAlphaMap = NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount	= NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes	= new ImpBrush* [ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS]	= new PointBrush( this, "Points" );

	// Note: You should implement these 5 brushes.  They are set the same (PointBrush) for now
	ImpBrush::c_pBrushes[BRUSH_LINES]				
		= new LineBrush( this, "Lines" );
	ImpBrush::c_pBrushes[BRUSH_CIRCLES]				
		= new CircleBrush( this, "Circles" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]	
		= new ScatteredPointBrush( this, "Scattered Points" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]		
		= new ScatteredLineBrush( this, "Scattered Lines" );
	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]	
		= new ScatteredCircleBrush( this, "Scattered Circles" );
	ImpBrush::c_pBrushes[ALPHA_MAP]
		= new AlphaMapBrush( this, "Alpha Map" );
	ImpBrush::c_pBrushes[CURVED_BRUSH]
		= new CurvedBrush( this, "Curved Brush" );
	ImpBrush::c_pBrushes[WARP_BRUSH]
		= new WarpBrush( this, "Warp Brush" );
	ImpBrush::c_pBrushes[BLUR_BRUSH]
		= new KernelBrush( this, "Blur Brush" );
	ImpBrush::c_pBrushes[SHARPEN_BRUSH]
		= new KernelBrush( this, "Sharpen Brush" );
	ImpBrush::c_pBrushes[PIXELIZE_BRUSH]
		= new PixelizeBrush( this, "Pixelize Brush" );

	std::vector<std::vector<float>> blurKernel = {
		{1,2,1},
	{2,4,2},
	{1,2,1} };
	double s = 0;
	for (int i = 0; i < 3; i++)for (int j = 0; j < 3; j++)s += blurKernel[i][j];
	for (int i = 0; i < 3; i++)for (int j = 0; j < 3; j++)blurKernel[i][j] /= s;

	std::vector<std::vector<float>> sharpKernel = {
		{-1,-1,-1},
		{-1,9,-1},
		{-1,-1,-1}
	};

	((KernelBrush*)ImpBrush::c_pBrushes[BLUR_BRUSH])->SetKernel(blurKernel);
	((KernelBrush*)ImpBrush::c_pBrushes[SHARPEN_BRUSH])->SetKernel(sharpKernel);

	// make one of the brushes current
	m_pCurrentBrush	= ImpBrush::c_pBrushes[0];

}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui) 
{
	m_pUI	= ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName() 
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush	= ImpBrush::c_pBrushes[type];
}

void ImpressionistDoc::setDirectionType(int type)
{
	m_pCurrentBrush->setDirectionType(type);
}

void ImpressionistDoc::swapContent()
{
	unsigned char* temp = m_ucPainting;
	m_ucPainting = m_ucBitmap;
	m_ucBitmap = temp;
	m_ucOriginal = temp;
	m_pUI->m_origView->refresh();
	m_pUI->m_paintView->refresh();
}

void ImpressionistDoc::undo()
{
	memcpy(m_ucPainting, m_ucHistory, m_nWidth*m_nHeight * 3 * sizeof(unsigned char));
	m_pUI->m_paintView->refresh();
}

void ImpressionistDoc::recordHistory()
{
	memcpy(m_ucHistory, m_ucPainting, m_nWidth*m_nHeight * 3 * sizeof(unsigned char));
}

void ImpressionistDoc::autoFill()
{

	// m_pUI->m_origView->refresh();
	m_pUI->m_paintView->prepareAutoFill();
	// m_pUI->m_paintView->willAutoFill = true;
		// m_pUI->m_paintView->draw();
	// m_pUI->m_origView->refresh();

	// std::future<void> result_future = std::async([&]{m_pUI->m_origView->refresh(); });
	// result_future.get();
}

int ImpressionistDoc::getEdgeThreshold()
{
	return m_pUI->getEdgeThreshold();
}

void ImpressionistDoc::runBayesian()
{
	char* newfile = fl_file_chooser("Open File?", "*.bmp", getImageName());
	if (newfile != NULL) {
		m_bayesian->solve(newfile);
		if(m_ucAnother)
		{
			for(int i = 0; i<m_nWidth*m_nHeight; i++)
			{
				double alpha = m_ucPainting[i*3] / 255.0;
				m_ucPainting[i*3+2] = max(0, min(alpha*m_ucOriginal[i*3] + (1 - alpha)*m_ucAnother[i*3],255));
				m_ucPainting[i*3+1] = max(0, min(alpha*m_ucOriginal[i*3+1] + (1 - alpha)*m_ucAnother[i*3+1],255));
				m_ucPainting[i*3] = max(0, min(alpha*m_ucOriginal[i*3+2] + (1 - alpha)*m_ucAnother[i*3+2],255));
			}
		}
		// saveImage("test.bmp");
		m_pUI->m_paintView->refresh();
	}
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}
int ImpressionistDoc::getLineWidth()
{
	return m_pUI->getLineWidth();
}
int ImpressionistDoc::getLineAngle()
{
	return m_pUI->getLineAngle();
}

double ImpressionistDoc::getAlpha()
{
	return m_pUI->getAlpha();
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname) 
{
	int width, height;

	const auto data = readBMP(iname, width, height);

	return loadImageFromData(data, width, height);
}

int ImpressionistDoc::loadImageFromData(unsigned char* dataPtr, const int width, const int height)
{
	if (dataPtr == nullptr)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth = width;
	m_nPaintWidth = width;
	m_nHeight = height;
	m_nPaintHeight = height;

	// release old storage
	delete[] m_ucHistory;
	if (m_ucBitmap == m_ucOriginal)
	{
		delete[] m_ucOriginal;
	} else
	{
		delete[] m_ucBitmap;
		delete[] m_ucOriginal;
	}
	delete[] m_ucPainting;
	delete[] m_ucEdge;
	delete[] viewport.dataPtr;

	m_ucBitmap = dataPtr;
	m_ucOriginal = dataPtr;

	// allocate space for draw view
	viewport.dim = { width, height };
	viewport.dataPtr = new unsigned char[viewport.dim.getLength()];
	m_ucPainting = new unsigned char[width*height * 3];
	m_ucHistory = new unsigned char[width*height * 3];
	m_ucEdge = new unsigned char[width*height * 3];
	memset(m_ucPainting, 0, width*height * 3);
	memset(m_ucHistory, 0, width*height * 3);
	memset(m_ucEdge, 0, width*height * 3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(),
		m_pUI->m_mainWindow->y(),
		width * 2,
		height + 25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);
	m_pUI->m_paintView->refresh();


	return 1;
}

int ImpressionistDoc::loadAnotherImage(char *iname)
{
	// try to open the image to read
	unsigned char*	data;
	int				width, height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// check if the dimension matches the original image
	if(m_nWidth != width || m_nPaintWidth != width || m_nHeight != height || m_nPaintHeight != height)
	{
		fl_alert("Dimension does not match");
		return 0;
	}

	// release old another image
	if (m_ucAnother) delete[] m_ucAnother;

	m_ucAnother = data;
	m_ucBitmap = m_ucAnother;
	m_pUI->m_origView->refresh();

	return 1;
}

int ImpressionistDoc::loadMuralImage(char* iname)
{
	// try to open the image to read
	unsigned char*	data;
	int				width, height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// check if the dimension matches the original image
	if (m_nWidth != width || m_nPaintWidth != width || m_nHeight != height || m_nPaintHeight != height)
	{
		fl_alert("Dimension does not match");
		return 0;
	}

	// release old another image
	if (m_ucOriginal) delete[] m_ucOriginal;

	m_ucOriginal = data;
	m_ucBitmap = m_ucOriginal;

	return 1;
}

int ImpressionistDoc::loadAlphaMap(char* iname)
{
	unsigned char*	data;
	int				width, height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	m_alphaMapWidth = width;
	m_alphaMapHeight = height;

	// release old another image
	if (m_ucAlphaMap) delete[] m_ucAlphaMap;

	m_ucAlphaMap = data;

	return 1;
}


//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname) 
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas() 
{

	// Release old storage
	if ( m_ucPainting ) 
	{
		delete [] m_ucPainting;

		// allocate space for draw view
		m_ucPainting	= new unsigned char [m_nPaintWidth*m_nPaintHeight*3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight*3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}
	
	return 0;
}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( int x, int y )
{
	if ( x < 0 ) 
		x = 0;
	else if ( x >= m_nWidth ) 
		x = m_nWidth-1;

	if ( y < 0 ) 
		y = 0;
	else if ( y >= m_nHeight ) 
		y = m_nHeight-1;

	return (GLubyte*)(m_ucOriginal + 3 * (y*m_nWidth + x));
}

double ImpressionistDoc::GetAlpha(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_alphaMapWidth)
		x = m_alphaMapWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_alphaMapHeight)
		y = m_alphaMapHeight - 1;
	GLubyte* p = m_ucAlphaMap + 3 * (y*m_alphaMapWidth + x);
	return (p[0] + p[1] + p[2]) / 765.0f;
}

GLubyte* ImpressionistDoc::GetAnotherPixel(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return (GLubyte*)(m_ucAnother + 3 * (y*m_nWidth + x));
}

bool ImpressionistDoc::IsEdge(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return *(m_ucEdge + 3 * (y*m_nWidth + x)) == 255;
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel( const Point p )
{
	return GetOriginalPixel( p.x, p.y );
}
GLubyte* ImpressionistDoc::GetAnotherPixel( const Point p )
{
	return GetAnotherPixel( p.x, p.y );
}

bool ImpressionistDoc::IsEdge(const Point p)
{
	return IsEdge(p.x, p.y);
}
