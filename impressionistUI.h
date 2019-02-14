//
// impressionistUI.h
//
// The header file for the UI part
//

#ifndef ImpressionistUI_h
#define ImpressionistUI_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_file_chooser.H>		// FLTK file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>

#include "Impressionist.h"
#include "OriginalView.h"
#include "PaintView.h"

#include "ImpBrush.h"

class ImpressionistUI {
public:
	ImpressionistUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;
								
	PaintView*			m_paintView;
	OriginalView*		m_origView;

// for brush dialog
	Fl_Window*			m_brushDialog;
	Fl_Choice*			m_BrushTypeChoice;
	Fl_Choice*			m_BrushDirectionChoice;

	Fl_Slider*			m_BrushSizeSlider;
	Fl_Slider*			m_LineWidthSlider;
	Fl_Slider*			m_LineAngleSlider;
	Fl_Slider*			m_AlphaSlider;

	Fl_Slider*			m_AutoFillStrikeSlider;
	Fl_Slider*			m_AutoFillRandomSlider;

	Fl_Button*          m_ClearCanvasButton;
	Fl_Button*			m_AutoFillButton;
	Fl_Check_Button* m_AutoFillEnableRandomButton;
	Fl_Slider*			m_EdgeThresholdSlider;
	Fl_Button*			m_FindEdgeButton;

	// Member functions
	void				setDocument(ImpressionistDoc* doc);
	ImpressionistDoc*	getDocument();

	void				show();
	void				resize_windows(int w, int h);

	// Interface to get attribute

	int					getSize();
	void				setSize(int size);
	int getLineWidth();
	void setLineWidth(int width);
	int getLineAngle();
	void setLineAngle(int angle);
	double getAlpha();
	void setAlpha(double alpha);
	int getAutoFillStrike();
	void setAutoFillStrike(int strike);
	double getAutoFillRandom() const;
	void setAutoFillRandom(double auto_fill_random);
	bool getAutoFillEnableRandom();
	void setAutoFillEnableRandom(bool flag);
	int getEdgeThreshold();
	void setEdgeThreshold(int threshold);

private:
	ImpressionistDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int		m_nSize;
	int m_LineWidth;
	int m_LineAngle;
	double m_Alpha;
	int m_AutoFillStrike = 1;
	double m_AutoFillRandom = 0;
	bool m_AutoFillEnableRandom = true;
	int m_EdgeThreshold = 0;

private:
	// Static class members
	static Fl_Menu_Item		menuitems[];
	static Fl_Menu_Item		brushTypeMenu[NUM_BRUSH_TYPE+1];
	static Fl_Menu_Item		brushDirectionMenu[NUM_DIRECTION_TYPE+1];

	static ImpressionistUI*	whoami(Fl_Menu_* o);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_load_image(Fl_Menu_* o, void* v);
	static void cb_load_anther_image(Fl_Menu_* o, void* v);
	static void	cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_brushes(Fl_Menu_* o, void* v);
	static void	cb_clear_canvas(Fl_Menu_* o, void* v);
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
	static void	cb_brushChoice(Fl_Widget* o, void* v);
	static void	cb_directionChoice(Fl_Widget* o, void* v);
	static void	cb_clear_canvas_button(Fl_Widget* o, void* v);
	static void	cb_auto_fill(Fl_Widget* o, void* v);
	static void	cb_sizeSlides(Fl_Widget* o, void* v);
	static void	cb_lineWidthSlides(Fl_Widget* o, void* v);
	static void	cb_lineAngleSlides(Fl_Widget* o, void* v);
	static void	cb_alphaSlides(Fl_Widget* o, void* v);
	static void	cb_undo(Fl_Menu_* o, void* v);
	static void cb_swap_content(Fl_Menu_* o, void* v);
	static void cb_auto_fill_menu(Fl_Menu_* o, void* v);
	static void	cb_autoFillStrikeSlides(Fl_Widget* o, void* v);
	static void	cb_autoFillRandomSlides(Fl_Widget* o, void* v);
	static void	cb_enable_random(Fl_Widget* o, void* v);
	static void	cb_edgeThresholdSlides(Fl_Widget* o, void* v);
	static void	cb_find_edge(Fl_Widget* o, void* v);
};

#endif
