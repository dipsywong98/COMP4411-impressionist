#pragma once
#include "ImpBrush.h"
#include <string>
#include <FL/gl.h>
#include "ImageUtils.h"
#include <FL/Fl_Double_Window.H>
#include <random>

class PatternBrush: public ImpBrush
{
public:
	int step = 0;
	static Fl_Callback cb_set_patterns;
	static std::vector< ImageWrapper<GLubyte> > patterns;
	static volatile bool isGenerating;

	static void setPatterns(const std::string& directoryName);

	explicit PatternBrush(ImpressionistDoc* pDoc = nullptr, char* name = nullptr);
	void BrushBegin(const Point source, const Point target) override;
	void BrushMove(const Point source, const Point target) override;
	void BrushEnd(const Point source, const Point target) override;

private:
	static std::random_device rd;
	static std::default_random_engine gen;
	static std::uniform_int_distribution<> dis;

	static std::vector< ImageWrapper<GLubyte> > scaledPatterns;

	static int size;
};
