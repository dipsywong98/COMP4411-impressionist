// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "impressionist.h"
#include "bitmap.h"

class ImpressionistUI;

class ImpressionistDoc 
{
public:
	ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char *iname);			// called by the UI to load image
	int loadImageFromData(unsigned char* dataPtr, int width, int height);
	int loadAnotherImage(char* iname);
	int loadMuralImage(char* iname);
	int loadAlphaMap(char* iname);
	int		saveImage(char *iname);			// called by the UI to save image


	int     clearCanvas();                  // called by the UI to clear the drawing canvas
	void	setBrushType(int type);			// called by the UI to set the brushType
	int		getSize();						// get the UI size
	void	setSize(int size);				// set the UI size
	int getLineWidth();
	int getLineAngle();
	double getAlpha();
	char*	getImageName();					// get the current image name
	void setDirectionType(int type);
	void undo();
	void swapContent();
	void recordHistory();
	void autoFill();
	int getEdgeThreshold();


	// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth, 
					m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth, 
					m_nPaintHeight;	
	// Bitmaps for original image and painting.
	unsigned char*	m_ucBitmap;
	unsigned char*	m_ucOriginal;
	unsigned char*	m_ucPainting;
	unsigned char*	m_ucEdge;
	unsigned char* m_ucAlphaMap;


	// The current active brush.
	ImpBrush*			m_pCurrentBrush;	
	// Size of the brush.
	int m_nSize;							

	ImpressionistUI*	m_pUI;

	unsigned char* m_ucHistory; //for undo
	unsigned char* m_ucAnother; //for another image
	int m_alphaMapWidth;
	int m_alphaMapHeight;

	// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte* GetOriginalPixel( int x, int y );
	double GetAlpha( int x, int y );
	GLubyte* GetAnotherPixel(int x, int y);
	bool IsEdge(int x, int y);
	// Get the color of the original picture at the specified point	
	GLubyte* GetOriginalPixel( const Point p );  
	GLubyte* GetAnotherPixel( const Point p );
	bool IsEdge(const Point p);


private:
	char			m_imageName[256];

};

extern void MessageBox(char *message);

#endif
