#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"

/********************/
// The FLTK header files
#include "FL/Fl.H"
#include "FL/Fl_Window.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Input.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Image.H"
#include "FL/Fl_Timer.H"
#include "FL/Fl_Menu_Bar.H"
#include "FL/Fl_Text_Display.H"
#include "FL/Fl_Text_Buffer.H"
#include "FL/filename.H"
#include "FL/Fl_Check_Button.H"
#include "FL/Fl_Counter.H"
#include "FL/fl_ask.H"
#include "FL/Fl_Valuator.H"
#include "FL/Fl_Nice_Slider.H"
#include "FL/Fl_Value_Slider.H"

#include "FL/x.H"

#if WIN32
#include <Windows.h.>
#endif

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define MENU_HEIGHT 25
#define MARKER_WIN_WIDTH 250
#define MARKER_WIN_HEIGHT 550

#define  DIB_RGB_COLORS  0

#include "Markers.h"
#include <string.h>
#include "Marker.h"
#include "XPoints.h"
#include "MTXPoint.h"
#include "Persistence.h"
#include "Cameras.h"
#include "Facet.h"
#include "Xform3D.h"
#include "MTC.h"
#include <math.h>
#include <stdio.h>
#include <vector>

#if WIN32
#undef USE_BOXES
#else
#undef USE_BOXES
#endif

using namespace MTMarker;
using namespace MTCollection;

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtGuiApplication1Class ui;

};
