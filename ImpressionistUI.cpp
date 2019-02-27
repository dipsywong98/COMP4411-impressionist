//
// impressionistUI.h
//
// The user interface part for the program.
//

#include <FL/fl_ask.h>

#include <math.h>

#include "Bayesian.h"
#include "MosaicBuilder.h"
#include "VideoProcessor.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include <sstream>

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the 
project.  You can copy and paste these into your code and then change them to 
make them look how you want.  Descriptions for all of the widgets here can be found 
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------
	
	//----To install a window--------------------------
	Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
		myWindow->user_data((void*)(this));	// record self to be used by static callback functions
		
		// install menu bar
		myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
		Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
			{ "&File",		0, 0, 0, FL_SUBMENU },
				{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
				{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
				{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
				{ 0 },
			{ "&Edit",		0, 0, 0, FL_SUBMENU },
				{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
				{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
				{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
				{ 0 },
			{ "&Help",		0, 0, 0, FL_SUBMENU },
				{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
				{ 0 },
			{ 0 }
		};
		myMenubar->menu(myMenuItems);
    m_mainWindow->end();

	//----The window callback--------------------------
	// One of the callbacks
	void ImpressionistUI::cb_load(Fl_Menu_* o, void* v) 
	{	
		ImpressionistDoc *pDoc=whoami(o)->getDocument();

		char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
		if (newfile != NULL) {
			pDoc->loadImage(newfile);
		}
	}


//------------Slider---------------------------------------

	//----To install a slider--------------------------
	Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
	mySlider->user_data((void*)(this));	// record self to be used by static callback functions
	mySlider->type(FL_HOR_NICE_SLIDER);
    mySlider->labelfont(FL_COURIER);
    mySlider->labelsize(12);
	mySlider->minimum(1);
	mySlider->maximum(40);
	mySlider->step(1);
	mySlider->value(m_nMyValue);
	mySlider->align(FL_ALIGN_RIGHT);
	mySlider->callback(cb_MyValueSlides);

	//----The slider callback--------------------------
	void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
	}
	

//------------Choice---------------------------------------
	
	//----To install a choice--------------------------
	Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
	myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
	Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
	  {"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
	  {"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
	  {"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
	  {0}
	};
	myChoice->menu(myChoiceMenu);
	myChoice->callback(cb_myChoice);
	
	//-----The choice callback-------------------------
	void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
		ImpressionistDoc* pDoc=pUI->getDocument();

		int type=(int)v;

		pDoc->setMyType(type);
	}


//------------Button---------------------------------------

	//---To install a button---------------------------
	Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
	myButton->user_data((void*)(this));   // record self to be used by static callback functions
	myButton->callback(cb_myButton);

	//---The button callback---------------------------
	void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
		ImpressionistDoc* pDoc = pUI->getDocument();
		pDoc->startPainting();
	}


//---------Light Button------------------------------------
	
	//---To install a light button---------------------
	Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
	myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
	myLightButton->callback(cb_myLightButton);

	//---The light button callback---------------------
	void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
	{
		ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

		if (pUI->myBool==TRUE) pUI->myBool=FALSE;
		else pUI->myBool=TRUE;
	}

//----------Int Input--------------------------------------

    //---To install an int input-----------------------
	Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
	myInput->user_data((void*)(this));   // record self to be used by static callback functions
	myInput->callback(cb_myInput);

	//---The int input callback------------------------
	void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
	{
		((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
	}

//------------------------------------------------------------------------------------------------
*/

//------------------------------------- Help Functions --------------------------------------------

double ImpressionistUI::getAutoFillRandom() const
{
    return m_AutoFillRandom;
}

void ImpressionistUI::setAutoFillRandom(double auto_fill_random)
{
    m_AutoFillRandom = auto_fill_random;
}

bool ImpressionistUI::getAutoFillEnableRandom()
{
    return m_AutoFillEnableRandom;
}

void ImpressionistUI::setAutoFillEnableRandom(bool flag)
{
    m_AutoFillEnableRandom = flag;
}

int ImpressionistUI::getEdgeThreshold()
{
    return m_EdgeThreshold;
}

void ImpressionistUI::setEdgeThreshold(int threshold)
{
    m_EdgeThreshold = threshold;
}

bool ImpressionistUI::getEdgeClip()
{
    return m_EdgeClip;
}

void ImpressionistUI::setEdgeClip(bool flag)
{
    m_EdgeClip = flag;
}

bool ImpressionistUI::getIsNormalizedKernel()
{
    return m_IsNormalizedKernel;
}

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)
{
    return ((ImpressionistUI*)(o->parent()->user_data()));
}

bool ImpressionistUI::parseKernel()
{
    kernel.clear();
    std::string input(m_KernelStr);
    if (input.empty())
        return false;
    std::stringstream ss(input);
    int size;
    ss >> size;
    for (int i = 0; i < size; i++) {
        std::vector<float> row;
        for (int j = 0; j < size; j++) {
            float tmp;
            if (!ss.rdbuf()->in_avail())
                return false;
            ss >> tmp;
            row.push_back(tmp);
        }
        kernel.push_back(row);
    }
    return true;
}

//--------------------------------- Callback Functions --------------------------------------------

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
    if (newfile != NULL) {
        pDoc->loadImage(newfile);
    }
}

void ImpressionistUI::cb_load_another_image(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
    if (newfile != NULL) {
        pDoc->loadAnotherImage(newfile);
    }
}

void ImpressionistUI::cb_load_mural_image(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
    if (newfile != NULL) {
        pDoc->loadMuralImage(newfile);
    }
}

void ImpressionistUI::cb_load_alpha_map_image(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
    if (newfile != NULL) {
        pDoc->loadAlphaMap(newfile);
    }
}

//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    char* newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp");
    if (newfile != NULL) {
        pDoc->saveImage(newfile);
    }
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_* o, void* v)
{
    whoami(o)->m_brushDialog->show();
}

void ImpressionistUI::cb_painterly(Fl_Menu_* o, void* v)
{
    whoami(o)->m_painterlyDialog->show();
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    pDoc->clearCanvas();
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_* o, void* v)
{
    whoami(o)->m_mainWindow->hide();
    whoami(o)->m_brushDialog->hide();
}

//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_* o, void* v)
{
    fl_message("Impressionist FLTK version for CS341, Spring 2002");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush
// choice.
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget* o, void* v)
{
    ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
    ImpressionistDoc* pDoc = pUI->getDocument();

    int type = (int)v;

    if (type == (int)ALPHA_MAP) {
        if (pDoc->m_ucAlphaMap == NULL) {
            fl_alert("Please load alpha map first");
            pUI->m_BrushTypeChoice->value(0);
            pUI->m_painterlyBrushTypeChoice->value(0);
            return;
        }
    }
    pUI->m_BrushTypeChoice->value(type);
    pUI->m_painterlyBrushTypeChoice->value(type);

    pDoc->setBrushType(type);
}

void ImpressionistUI::cb_directionChoice(Fl_Widget* o, void* v)
{
    ImpressionistUI* pUI = ((ImpressionistUI*)(o->user_data()));
    ImpressionistDoc* pDoc = pUI->getDocument();

    int type = (int)v;

    if (type == GRADIENT_ANOTHER) {
        if (pDoc->m_ucAnother == NULL) {
            fl_alert("Please Load Another Image First");
            type = 0;
            pUI->m_BrushDirectionChoice->value(0);
            return;
        }
    }

    pDoc->setDirectionType(type);
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
    ImpressionistDoc* pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

    pDoc->clearCanvas();
}

void ImpressionistUI::cb_auto_fill(Fl_Widget* o, void* v)
{
    ImpressionistDoc* pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();
    pDoc->autoFill();
}

//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_nSize = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_lineWidthSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_LineWidth = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_lineAngleSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_LineAngle = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_alphaSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_Alpha = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_undo(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();
    pDoc->undo();
}

void ImpressionistUI::cb_swap_content(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();
    pDoc->swapContent();
}

void ImpressionistUI::cb_auto_fill_menu(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();
    pDoc->autoFill();
}

void ImpressionistUI::cb_autoFillStrikeSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_AutoFillStrike = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_autoFillRandomSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_AutoFillRandom = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_enable_random(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_AutoFillEnableRandom = bool(((Fl_Check_Button*)o)->value());
}

void ImpressionistUI::cb_edge_clip(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_EdgeClip = bool(((Fl_Check_Button*)o)->value());
}

void ImpressionistUI::cb_edgeThresholdSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_EdgeThreshold = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_find_edge(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_origView->prepareFindEdge();
}

void ImpressionistUI::cb_view_original(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    pDoc->m_ucBitmap = pDoc->m_ucOriginal;
    pDoc->m_pUI->m_origView->refresh();
}

void ImpressionistUI::cb_view_another(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    pDoc->m_ucBitmap = pDoc->m_ucAnother;
    pDoc->m_pUI->m_origView->refresh();
}

void ImpressionistUI::cb_view_edge(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();

    pDoc->m_ucBitmap = pDoc->m_ucEdge;
    pDoc->m_pUI->m_origView->refresh();
}

void ImpressionistUI::cb_edit_kernel(Fl_Widget* o, void* v)
{
    // ((ImpressionistUI*)(o->user_data()))->m_KernelStr = (char*)(((Fl_Input *)o)->value());
    ImpressionistUI* pUI = (ImpressionistUI*)(o->user_data());

    const char* str = ((Fl_Input*)o)->value();
    strcpy(pUI->m_KernelStr, str);
}

void ImpressionistUI::cb_apply_kernel(Fl_Widget* o, void* v)
{
    ImpressionistUI* pUI = (ImpressionistUI*)(o->user_data());
    if (pUI->parseKernel()) {
        pUI->m_paintView->applyKernel();
    }
}

void ImpressionistUI::cb_normalize_kernel(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_IsNormalizedKernel = bool(((Fl_Check_Button*)o)->value());
    // ImpressionistUI* pUI = (ImpressionistUI*)(o->user_data());
    // float total = pUI->parseKernel();
    // std::string newKernelStr;
    // std::stringstream ss(newKernelStr);
    // int size = pUI->kernel.size();
    // ss << size;
    // for(int i = 0; i<size; i++)
    // {
    // 	for(int j=0; j<size; j++)
    // 	{
    // 		pUI->kernel[i][j] /= total;
    // 		ss<< " " << pUI->kernel[i][j];
    // 	}
    // }
    // newKernelStr = ss.str();
    // strcpy(pUI->m_KernelStr, newKernelStr.c_str());
    // pUI->m_KernelInput->value(pUI->m_KernelStr);
}

void ImpressionistUI::cb_warpStrengthSlides(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_warpStrength = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_paint(Fl_Widget* o, void* v)
{
    ImpressionistUI* pUI = (ImpressionistUI*)(o->user_data());
    pUI->m_paintView->preparePainterly();
}

void ImpressionistUI::cb_painterly_threshold(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyThreshold = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_curvature(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyCurvature = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_blur(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyBlur = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_grid_size(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyGridSize = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_min_stroke(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyMinStroke = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_max_stroke(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyMaxStroke = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_alpha(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyAlpha = double(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_layers(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyLayers = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_painterly_r0(Fl_Widget* o, void* v)
{
    ((ImpressionistUI*)(o->user_data()))->m_painterlyR0 = int(((Fl_Slider*)o)->value());
}

void ImpressionistUI::cb_bayesian_open_and_solve(Fl_Menu_* o, void* v)
{
    ImpressionistDoc* pDoc = whoami(o)->getDocument();
    pDoc->runBayesian();
}

void ImpressionistUI::cb_open_colors_dialog(Fl_Widget* o, void* v)
{
    whoami(dynamic_cast<Fl_Menu_*>(o))->m_colorPickerDialog->show();
}

//---------------------------------- per instance functions --------------------------------------

//------------------------------------------------
// Return the ImpressionistDoc used
//------------------------------------------------
ImpressionistDoc* ImpressionistUI::getDocument()
{
    return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show()
{
    m_mainWindow->show();
    m_paintView->show();
    m_origView->show();
}

//------------------------------------------------
// Change the paint and original window sizes to
// w by h
//------------------------------------------------
void ImpressionistUI::resize_windows(int w, int h)
{
    m_paintView->size(w, h);
    m_origView->size(w, h);
}

//------------------------------------------------
// Set the ImpressionistDoc used by the UI to
// communicate with the brushes
//------------------------------------------------
void ImpressionistUI::setDocument(ImpressionistDoc* doc)
{
    m_pDoc = doc;

    m_origView->m_pDoc = doc;
    m_paintView->m_pDoc = doc;
}

//------------------------------------------------
// Return the brush size
//------------------------------------------------
int ImpressionistUI::getSize()
{
    return m_nSize;
}

//-------------------------------------------------
// Set the brush size
//-------------------------------------------------
void ImpressionistUI::setSize(int size)
{
    m_nSize = size;

    if (size <= 40)
        m_BrushSizeSlider->value(m_nSize);
}

int ImpressionistUI::getLineWidth()
{
    return m_LineWidth;
}

void ImpressionistUI::setLineWidth(int width)
{
    m_LineWidth = width;

    if (width >= 1 && width <= 40)
        m_LineWidthSlider->value(width);
}

int ImpressionistUI::getLineAngle()
{
    return m_LineAngle;
}

void ImpressionistUI::setLineAngle(int angle)
{
    m_LineAngle = angle;

    if (angle >= 0 && angle <= 359)
        m_LineAngleSlider->value(angle);
}

double ImpressionistUI::getAlpha()
{
    return m_Alpha;
}

void ImpressionistUI::setAlpha(double angle)
{
    m_Alpha = angle;

    if (angle >= 0 && angle <= 1)
        m_LineAngleSlider->value(angle);
}

int ImpressionistUI::getAutoFillStrike()
{
    return m_AutoFillStrike;
}

void ImpressionistUI::setAutoFillStrike(int strike)
{
    m_AutoFillStrike = strike;
}

// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
    { "&File", 0, 0, 0, FL_SUBMENU },
    { "&Load Image...", FL_ALT + 'l', (Fl_Callback*)ImpressionistUI::cb_load_image },
    { "&Save Image...", FL_ALT + 's', (Fl_Callback*)ImpressionistUI::cb_save_image },
    { "&Brushes...", FL_ALT + 'b', (Fl_Callback*)ImpressionistUI::cb_brushes },
    { "&Painterly...", FL_ALT + 'b', (Fl_Callback*)ImpressionistUI::cb_painterly },
    { "&Clear Canvas", FL_ALT + 'c', (Fl_Callback*)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER },
    { "Colors...", FL_ALT + 'k', cb_open_colors_dialog, nullptr, FL_MENU_DIVIDER },
    { "&Quit", FL_ALT + 'q', (Fl_Callback*)ImpressionistUI::cb_exit },
    { 0 },
    { "&Display", 0, 0, 0, FL_SUBMENU },
    { "Original", 0, (Fl_Callback*)ImpressionistUI::cb_view_original },
    { "Another", 0, (Fl_Callback*)ImpressionistUI::cb_view_another },
    { "Edge", 0, (Fl_Callback*)ImpressionistUI::cb_view_edge },
    { 0 },
    { "&Bonus", 0, 0, 0, FL_SUBMENU },
    { "&Swap Content", FL_ALT + 's', (Fl_Callback*)ImpressionistUI::cb_swap_content },
    { "&Undo", FL_ALT + 'z', (Fl_Callback*)ImpressionistUI::cb_undo },
    { "&Auto Fill", FL_ALT + 'f', (Fl_Callback*)ImpressionistUI::cb_auto_fill_menu },
    { "&Load Another Img", FL_ALT + 'l', (Fl_Callback*)ImpressionistUI::cb_load_another_image },
    { "&Load Mural Img", FL_ALT + 'm', (Fl_Callback*)ImpressionistUI::cb_load_mural_image },
    { "&Load Alpha Map Img", FL_ALT + 'a', (Fl_Callback*)ImpressionistUI::cb_load_alpha_map_image },
    { "Bayesian", 0, (Fl_Callback*)ImpressionistUI::cb_bayesian_open_and_solve },
    { "Video", 0, nullptr, nullptr, FL_SUBMENU },
    { "Video Auto-Fill", 0, VideoProcessor::cbVideoAutoFill },
    { "Video Painterly", 0, VideoProcessor::cbVideoPaintly },
    { nullptr },
    { "Mosaic...", 0, MosaicBuilder::openUi },
    { 0 },
    { "&Help", 0, 0, 0, FL_SUBMENU },
    { "&About", FL_ALT + 'a', (Fl_Callback*)ImpressionistUI::cb_about },
    { 0 },

    { 0 }
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE + 1] = {
    { "Points", FL_ALT + 'p', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_POINTS },
    { "Lines", FL_ALT + 'l', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_LINES },
    { "Circles", FL_ALT + 'c', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_CIRCLES },
    { "Scattered Points", FL_ALT + 'q', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_SCATTERED_POINTS },
    { "Scattered Lines", FL_ALT + 'm', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_SCATTERED_LINES },
    { "Scattered Circles", FL_ALT + 'd', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)BRUSH_SCATTERED_CIRCLES },
    { "Alpha Map", FL_ALT + 'a', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)ALPHA_MAP },
    { "Curved Brush", FL_ALT + 'a', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)CURVED_BRUSH },
    { "Warp Brush", FL_ALT + 'a', (Fl_Callback*)ImpressionistUI::cb_brushChoice, (void*)WARP_BRUSH },
    { 0 }
};

Fl_Menu_Item ImpressionistUI::brushDirectionMenu[NUM_DIRECTION_TYPE + 1] = {
    { "Slider/Right Mouse", FL_ALT + 's', (Fl_Callback*)ImpressionistUI::cb_directionChoice, (void*)SLIDER_RIGHT_CLICK },
    { "Gradient", FL_ALT + 'g', (Fl_Callback*)ImpressionistUI::cb_directionChoice, (void*)GRADIENT },
    { "Brush Direction", FL_ALT + 'b', (Fl_Callback*)ImpressionistUI::cb_directionChoice, (void*)BRUSH_DIRECTION },
    { "Gradient of Another Img", FL_ALT + 'i', (Fl_Callback*)ImpressionistUI::cb_directionChoice, (void*)GRADIENT_ANOTHER },
    { 0 }
};

//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI()
{

    m_KernelStr = new char[100];
    memset(m_KernelStr, 0, 100);

    // Create the main window
    m_mainWindow = new Fl_Window(600, 300, "Impressionist");
    m_mainWindow->user_data((void*)(this)); // record self to be used by static callback functions
    // install menu bar
    m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
    m_menubar->menu(menuitems);

    // Create a group that will hold two sub windows inside the main
    // window
    Fl_Group* group = new Fl_Group(0, 25, 600, 275);

    // install paint view window
    m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view"); //0jon
    m_paintView->box(FL_DOWN_FRAME);

    // install original view window
    m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view"); //300jon
    m_origView->box(FL_DOWN_FRAME);
    m_origView->deactivate();

    group->end();
    Fl_Group::current()->resizable(group);
    m_mainWindow->end();

    // init values

    m_nSize = 10;
    m_LineWidth = 1;
    m_LineAngle = 0;
    m_Alpha = 1;

    // brush dialog definition
    m_brushDialog = new Fl_Window(400, 350, "Brush Dialog");
    // Add a brush type choice to the dialog
    m_BrushTypeChoice = new Fl_Choice(50, 10, 150, 25, "&Brush");
    m_BrushTypeChoice->user_data((void*)(this)); // record self to be used by static callback functions
    m_BrushTypeChoice->menu(brushTypeMenu);
    m_BrushTypeChoice->callback(cb_brushChoice);

    m_BrushDirectionChoice = new Fl_Choice(80, 50, 150, 25, "&Direction");
    m_BrushDirectionChoice->user_data((void*)(this)); // record self to be used by static callback functions
    m_BrushDirectionChoice->menu(brushDirectionMenu);
    m_BrushDirectionChoice->callback(cb_directionChoice);

    m_ClearCanvasButton = new Fl_Button(240, 10, 150, 25, "&Clear Canvas");
    m_ClearCanvasButton->user_data((void*)(this));
    m_ClearCanvasButton->callback(cb_clear_canvas_button);

    // Add brush size slider to the dialog
    m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
    m_BrushSizeSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
    m_BrushSizeSlider->labelfont(FL_COURIER);
    m_BrushSizeSlider->labelsize(12);
    m_BrushSizeSlider->minimum(1);
    m_BrushSizeSlider->maximum(100);
    m_BrushSizeSlider->step(1);
    m_BrushSizeSlider->value(m_nSize);
    m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
    m_BrushSizeSlider->callback(cb_sizeSlides);

    m_LineWidthSlider = new Fl_Value_Slider(10, 100, 300, 20, "Line Width");
    m_LineWidthSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_LineWidthSlider->type(FL_HOR_NICE_SLIDER);
    m_LineWidthSlider->labelfont(FL_COURIER);
    m_LineWidthSlider->labelsize(12);
    m_LineWidthSlider->minimum(1);
    m_LineWidthSlider->maximum(40);
    m_LineWidthSlider->step(1);
    m_LineWidthSlider->value(m_LineWidth);
    m_LineWidthSlider->align(FL_ALIGN_RIGHT);
    m_LineWidthSlider->callback(cb_lineWidthSlides);

    m_LineAngleSlider = new Fl_Value_Slider(10, 120, 300, 20, "Line Angle");
    m_LineAngleSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_LineAngleSlider->type(FL_HOR_NICE_SLIDER);
    m_LineAngleSlider->labelfont(FL_COURIER);
    m_LineAngleSlider->labelsize(12);
    m_LineAngleSlider->minimum(0);
    m_LineAngleSlider->maximum(359);
    m_LineAngleSlider->step(1);
    m_LineAngleSlider->value(m_LineAngle);
    m_LineAngleSlider->align(FL_ALIGN_RIGHT);
    m_LineAngleSlider->callback(cb_lineAngleSlides);

    m_AlphaSlider = new Fl_Value_Slider(10, 140, 300, 20, "Alpha");
    m_AlphaSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_AlphaSlider->type(FL_HOR_NICE_SLIDER);
    m_AlphaSlider->labelfont(FL_COURIER);
    m_AlphaSlider->labelsize(12);
    m_AlphaSlider->minimum(0);
    m_AlphaSlider->maximum(1);
    m_AlphaSlider->step(0.01);
    m_AlphaSlider->value(m_Alpha);
    m_AlphaSlider->align(FL_ALIGN_RIGHT);
    m_AlphaSlider->callback(cb_alphaSlides);

    m_AutoFillStrikeSlider = new Fl_Value_Slider(10, 160, 200, 20, "Strike");
    m_AutoFillStrikeSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_AutoFillStrikeSlider->type(FL_HOR_NICE_SLIDER);
    m_AutoFillStrikeSlider->labelfont(FL_COURIER);
    m_AutoFillStrikeSlider->labelsize(12);
    m_AutoFillStrikeSlider->minimum(1);
    m_AutoFillStrikeSlider->maximum(10);
    m_AutoFillStrikeSlider->step(1);
    m_AutoFillStrikeSlider->value(m_AutoFillStrike);
    m_AutoFillStrikeSlider->align(FL_ALIGN_RIGHT);
    m_AutoFillStrikeSlider->callback(cb_autoFillStrikeSlides);

    m_AutoFillRandomSlider = new Fl_Value_Slider(10, 180, 200, 20, "% Dif");
    m_AutoFillRandomSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_AutoFillRandomSlider->type(FL_HOR_NICE_SLIDER);
    m_AutoFillRandomSlider->labelfont(FL_COURIER);
    m_AutoFillRandomSlider->labelsize(12);
    m_AutoFillRandomSlider->minimum(0);
    m_AutoFillRandomSlider->maximum(1);
    m_AutoFillRandomSlider->step(0.01);
    m_AutoFillRandomSlider->value(m_AutoFillRandom);
    m_AutoFillRandomSlider->align(FL_ALIGN_RIGHT);
    m_AutoFillRandomSlider->callback(cb_autoFillRandomSlides);

    m_AutoFillButton = new Fl_Button(240, 160, 150, 25, "&Auto Fill");
    m_AutoFillButton->user_data((void*)(this));
    m_AutoFillButton->callback(cb_auto_fill);

    m_AutoFillEnableRandomButton = new Fl_Check_Button(240, 185, 150, 25, "Enable Random");
    m_AutoFillEnableRandomButton->value(m_AutoFillEnableRandom);
    m_AutoFillEnableRandomButton->user_data((void*)(this));
    m_AutoFillEnableRandomButton->callback(cb_enable_random);

    m_EdgeThresholdSlider = new Fl_Value_Slider(10, 210, 200, 20, "Edge Threshold");
    m_EdgeThresholdSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_EdgeThresholdSlider->type(FL_HOR_NICE_SLIDER);
    m_EdgeThresholdSlider->labelfont(FL_COURIER);
    m_EdgeThresholdSlider->labelsize(12);
    m_EdgeThresholdSlider->minimum(1);
    m_EdgeThresholdSlider->maximum(500);
    m_EdgeThresholdSlider->step(1);
    m_EdgeThresholdSlider->value(m_EdgeThreshold);
    m_EdgeThresholdSlider->align(FL_ALIGN_RIGHT);
    m_EdgeThresholdSlider->callback(cb_edgeThresholdSlides);

    m_FindEdgeButton = new Fl_Button(240, 210, 150, 25, "Find Edge");
    m_FindEdgeButton->user_data((void*)(this));
    m_FindEdgeButton->callback(cb_find_edge);

    m_EdgeClipButton = new Fl_Check_Button(240, 235, 150, 25, "Edge Clip");
    m_EdgeClipButton->value(m_EdgeClip);
    m_EdgeClipButton->user_data((void*)(this));
    m_EdgeClipButton->callback(cb_edge_clip);

    m_KernelInput = new Fl_Input(40, 250, 150, 50, "Kernel");
    m_KernelInput->value(m_KernelStr);
    m_KernelInput->user_data((void*)(this));
    m_KernelInput->callback(cb_edit_kernel);

    m_KernelApplyButton = new Fl_Button(240, 260, 150, 25, "Apply");
    m_KernelApplyButton->user_data((void*)(this));
    m_KernelApplyButton->callback(cb_apply_kernel);

    m_KernelNormalizeButton = new Fl_Check_Button(240, 285, 150, 25, "Normalize");
    m_KernelNormalizeButton->user_data((void*)(this));
    m_KernelNormalizeButton->value(m_IsNormalizedKernel);
    m_KernelNormalizeButton->callback(cb_normalize_kernel);

    m_WarpStrengthSlider = new Fl_Value_Slider(10, 300, 200, 20, "WarpStrength");
    m_WarpStrengthSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_WarpStrengthSlider->type(FL_HOR_NICE_SLIDER);
    m_WarpStrengthSlider->labelfont(FL_COURIER);
    m_WarpStrengthSlider->labelsize(12);
    m_WarpStrengthSlider->minimum(0);
    m_WarpStrengthSlider->maximum(20);
    m_WarpStrengthSlider->step(0.01);
    m_WarpStrengthSlider->value(m_warpStrength);
    m_WarpStrengthSlider->align(FL_ALIGN_RIGHT);
    m_WarpStrengthSlider->callback(cb_warpStrengthSlides);

    m_brushDialog->end();

    m_painterlyDialog = new Fl_Window(400, 325, "Painterly Dialog");
    m_painterlyBrushTypeChoice = new Fl_Choice(50, 10, 150, 25, "&Brush");
    m_painterlyBrushTypeChoice->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyBrushTypeChoice->menu(brushTypeMenu);
    m_painterlyBrushTypeChoice->callback(cb_brushChoice);
    // Add brush size slider to the dialog
    m_painterlyThresholdSlider = new Fl_Value_Slider(10, 80, 300, 20, "Threshold");
    m_painterlyThresholdSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyThresholdSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyThresholdSlider->labelfont(FL_COURIER);
    m_painterlyThresholdSlider->labelsize(12);
    m_painterlyThresholdSlider->minimum(0);
    m_painterlyThresholdSlider->maximum(250);
    m_painterlyThresholdSlider->step(1);
    m_painterlyThresholdSlider->value(m_painterlyThreshold);
    m_painterlyThresholdSlider->align(FL_ALIGN_RIGHT);
    m_painterlyThresholdSlider->callback(cb_painterly_threshold);

    m_painterlyCurvatureSlider = new Fl_Value_Slider(10, 100, 300, 20, "Curvature");
    m_painterlyCurvatureSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyCurvatureSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyCurvatureSlider->labelfont(FL_COURIER);
    m_painterlyCurvatureSlider->labelsize(12);
    m_painterlyCurvatureSlider->minimum(0);
    m_painterlyCurvatureSlider->maximum(1);
    m_painterlyCurvatureSlider->step(0.01);
    m_painterlyCurvatureSlider->value(m_painterlyCurvature);
    m_painterlyCurvatureSlider->align(FL_ALIGN_RIGHT);
    m_painterlyCurvatureSlider->callback(cb_painterly_curvature);

    m_painterlyBlurSlider = new Fl_Value_Slider(10, 120, 300, 20, "Blur");
    m_painterlyBlurSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyBlurSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyBlurSlider->labelfont(FL_COURIER);
    m_painterlyBlurSlider->labelsize(12);
    m_painterlyBlurSlider->minimum(0);
    m_painterlyBlurSlider->maximum(1);
    m_painterlyBlurSlider->step(0.01);
    m_painterlyBlurSlider->value(m_painterlyBlur);
    m_painterlyBlurSlider->align(FL_ALIGN_RIGHT);
    m_painterlyBlurSlider->callback(cb_painterly_blur);

    m_painterlyGridSizeSlider = new Fl_Value_Slider(10, 140, 300, 20, "Grid Size");
    m_painterlyGridSizeSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyGridSizeSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyGridSizeSlider->labelfont(FL_COURIER);
    m_painterlyGridSizeSlider->labelsize(12);
    m_painterlyGridSizeSlider->minimum(0);
    m_painterlyGridSizeSlider->maximum(1);
    m_painterlyGridSizeSlider->step(0.01);
    m_painterlyGridSizeSlider->value(m_painterlyGridSize);
    m_painterlyGridSizeSlider->align(FL_ALIGN_RIGHT);
    m_painterlyGridSizeSlider->callback(cb_painterly_grid_size);

    m_painterlyMinStrokeSlider = new Fl_Value_Slider(10, 160, 300, 20, "Min Stroke");
    m_painterlyMinStrokeSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyMinStrokeSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyMinStrokeSlider->labelfont(FL_COURIER);
    m_painterlyMinStrokeSlider->labelsize(12);
    m_painterlyMinStrokeSlider->minimum(0);
    m_painterlyMinStrokeSlider->maximum(30);
    m_painterlyMinStrokeSlider->step(1);
    m_painterlyMinStrokeSlider->value(m_painterlyMinStroke);
    m_painterlyMinStrokeSlider->align(FL_ALIGN_RIGHT);
    m_painterlyMinStrokeSlider->callback(cb_painterly_min_stroke);

    m_painterlyMaxStrokeSlider = new Fl_Value_Slider(10, 180, 300, 20, "Max Stroke");
    m_painterlyMaxStrokeSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyMaxStrokeSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyMaxStrokeSlider->labelfont(FL_COURIER);
    m_painterlyMaxStrokeSlider->labelsize(12);
    m_painterlyMaxStrokeSlider->minimum(0);
    m_painterlyMaxStrokeSlider->maximum(30);
    m_painterlyMaxStrokeSlider->step(1);
    m_painterlyMaxStrokeSlider->value(m_painterlyMaxStroke);
    m_painterlyMaxStrokeSlider->align(FL_ALIGN_RIGHT);
    m_painterlyMaxStrokeSlider->callback(cb_painterly_max_stroke);

    m_painterlyAlphaSlider = new Fl_Value_Slider(10, 200, 300, 20, "Alpha");
    m_painterlyAlphaSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyAlphaSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyAlphaSlider->labelfont(FL_COURIER);
    m_painterlyAlphaSlider->labelsize(12);
    m_painterlyAlphaSlider->minimum(0);
    m_painterlyAlphaSlider->maximum(1);
    m_painterlyAlphaSlider->step(0.01);
    m_painterlyAlphaSlider->value(m_painterlyAlpha);
    m_painterlyAlphaSlider->align(FL_ALIGN_RIGHT);
    m_painterlyAlphaSlider->callback(cb_painterly_alpha);

    m_painterlyLayersSlider = new Fl_Value_Slider(10, 220, 300, 20, "Layers");
    m_painterlyLayersSlider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyLayersSlider->type(FL_HOR_NICE_SLIDER);
    m_painterlyLayersSlider->labelfont(FL_COURIER);
    m_painterlyLayersSlider->labelsize(12);
    m_painterlyLayersSlider->minimum(1);
    m_painterlyLayersSlider->maximum(5);
    m_painterlyLayersSlider->step(1);
    m_painterlyLayersSlider->value(m_painterlyLayers);
    m_painterlyLayersSlider->align(FL_ALIGN_RIGHT);
    m_painterlyLayersSlider->callback(cb_painterly_layers);

    m_painterlyR0Slider = new Fl_Value_Slider(10, 240, 300, 20, "R0 Level");
    m_painterlyR0Slider->user_data((void*)(this)); // record self to be used by static callback functions
    m_painterlyR0Slider->type(FL_HOR_NICE_SLIDER);
    m_painterlyR0Slider->labelfont(FL_COURIER);
    m_painterlyR0Slider->labelsize(12);
    m_painterlyR0Slider->minimum(0);
    m_painterlyR0Slider->maximum(5);
    m_painterlyR0Slider->step(1);
    m_painterlyR0Slider->value(m_painterlyR0);
    m_painterlyR0Slider->align(FL_ALIGN_RIGHT);
    m_painterlyR0Slider->callback(cb_painterly_r0);

    m_painterlyButton = new Fl_Button(240, 260, 150, 25, "Paint!");
    m_painterlyButton->user_data((void*)(this));
    m_painterlyButton->callback(cb_painterly_paint);
    m_painterlyDialog->end();

    m_colorPickerDialog = new Fl_Window(380, 300, "Colors...");
    {
        m_colorChooser = new Fl_Color_Chooser(0, 0, 380, 300, "Color Blending");
        m_colorChooser->rgb(1.0, 1.0, 1.0);
    }
    m_colorPickerDialog->end();
}
