//
// impressionist.cpp
//
// The main driver program for the other parts. We have two major components,
// UI and Doc.
// They do have a link to each other as their member such that they can 
// communicate.
//

#include <stdio.h>

#include <FL/Fl.h>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>

#include "impressionistUI.h"
#include "impressionistDoc.h"

ImpressionistUI *impUI;
ImpressionistDoc *impDoc;

int main(int	argc, 
		 char**	argv) 
{
	impDoc = new ImpressionistDoc();

	// Create the UI
	impUI = new ImpressionistUI();

	// Set the impDoc which is used as the bridge between UI and brushes
	impUI->setDocument(impDoc);
	impDoc->setUI(impUI);

	Fl::visual(FL_DOUBLE|FL_INDEX);

	impUI->show();

	return Fl::run();
}

// global functions
float frand()
{
	return (float)rand()/RAND_MAX;
}

int irand(int max)
{
	return rand()%max;
}

double degToRad(double deg)
{
	return deg * 2 * M_PI / 360.0f;
}

double radToDeg(double rad)
{
	return rad / 2.f / M_PI * 360.0f;
}

double randAlter(double original, double maxAbsPercentageDiff)
{
	return original + original*maxAbsPercentageDiff*(frand()-0.5)*2;
}