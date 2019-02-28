//
// paintview.h
//
// The header file for painting view of the input images
//

#ifndef PAINTVIEW_H
#define PAINTVIEW_H

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <vector>
#include <chrono>
#include <random>

class ImpressionistDoc;

class PaintView : public Fl_Gl_Window
{
public:
	PaintView(int x, int y, int w, int h, const char* l);
	void draw();
	void prepareAutoFill();
	void preparePainterly();
	int handle(int event);

	void refresh();
	
	void resizeWindow(int width, int height);

	void SaveCurrentContent();

	void RestoreContent();

	void autoFill();

	void applyKernel();
	void kernelHelper(unsigned char* before, unsigned char* target, const std::vector<std::vector<float>>& kernel, int w, int h, bool
	                  isNormalized);

	void paintLayer(unsigned char* canvas, unsigned char* ref, int r);
	void painterly();

	ImpressionistDoc *m_pDoc;

	bool willAutoFill = false;
	bool willPainterly = false;
private:
	GLvoid* m_pPaintBitstart;
	int		m_nDrawWidth,
			m_nDrawHeight,
			m_nStartRow, 
			m_nEndRow,
			m_nStartCol, 
			m_nEndCol,
			m_nWindowWidth, 
			m_nWindowHeight;
	std::default_random_engine rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	Point scrollpos{0, 0};
	int startrow = 0;

	void updateViewport();
	void updatePainting();


};

#endif