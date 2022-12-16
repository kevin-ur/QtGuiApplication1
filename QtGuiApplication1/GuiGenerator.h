/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/
#ifndef __GUIGENERATOR_H__
#define __GUIGENERATOR_H__

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


class GuiGenerator
{
public:
	GuiGenerator();
	~GuiGenerator();
	void initialINIAccess();
	void setupCameras();
	void refreshMarkerTemplates();
	void EnableHdrLockedMarkers(bool newVal = false);
	int processCurrFrame(bool withDisplay = true);
	void refreshTemplateList();
	void recreateTemplateList();
	void changeTemplateName(int index, string oldName, string newName);
	void deleteTemplate(string Name);
	void enableUI();
	void disableUI();

	void startGrabingFrames();
	void AdjustLigtCoolnessByCoolCard();
	void showIdentifiedMarkers();
	void showIdentifiedXPoints();
	void collectNewSamples();
	double findDistance(double* v1, double* v2);//SI
	double dotProductVectors(double* v1, double* v2);//SI

	static void ctimer_cb(void*);
	static void activateMarkerTemplates(Fl_Menu_Item* m, GuiGenerator*);
	static void activateOptions(Fl_Menu_Item* m, GuiGenerator*);
	static void SetEqualize(Fl_Menu_Item* m, GuiGenerator* data);
	static void SetCurrCamera(Fl_Menu_Item* m, int* cbuffer);
	static void onDeleteTemplateClicked(Fl_Button* , GuiGenerator* );
	static void onRenameTemplateClicked(Fl_Button*, GuiGenerator*);
	static void onSampleFacetClicked(Fl_Button*, GuiGenerator*);
	static void onStopSamplingClicked(Fl_Button*, GuiGenerator*);
	static void onSaveSampleClicked(Fl_Button*, GuiGenerator*);
	static void onAdditionalFacetClicked(Fl_Button*, GuiGenerator*);
	static void onCancelClicked(Fl_Button* button, GuiGenerator* data);
	static void onSliderChange(Fl_Value_Slider* button, GuiGenerator* data);
	static void onEFCounterChange(Fl_Counter*, GuiGenerator*);
	void saveMarkerTemplate(string name);

	void DisplayLRImages();
	void DisplayLRImages(int Side);
	void drawLine(int side, float x1, float y1, float x2, float y2, float r, float g, float b, int stipple);
	void drawCircle(int side, float x, float y, float radius, float r, float g, float b, int stipple);
	void drawText( char* text, int X, int Y);
	void drawImage( Fl_Window *window, int x, int y, int width, int height, unsigned char *pixels, bool firstRowAtBottom = false );
	void ClearWindow( );

	MTMarker::Marker* findMarkerInSet(char* name);

	void detach();


private:
	void drawImages(bool left, bool right, bool middle);

	Persistence* pPers;
	Markers* pMarkers;
	Cameras* pCameras;
	MCamera* pCurrCam;
	MTMarker::Marker* pCurrMarker;
	MTXPoint* pCurrXPoint;
	XPoints* pXPoints;

	Fl_RGB_Image* middleImage;
	Fl_RGB_Image* rightImage;
	Fl_RGB_Image* leftImage;

	Fl_Window *window;
	bool isShowingVectors; 
	bool isShowingXpoints;
	bool isShowingHalfSize;
	bool isDisplayEnabled;
	bool isMarkersProcessingEnabled;
	bool isXPointsProcessingEnabled;
	bool isHdrModeEnabled;
	bool isHdrLockedMarkerEnabled;
	bool isBackGroundProcessingEnabled;
	bool isCam2CamRegistered;
	bool isSingleWindow;
	bool useOpenGL;
	bool isShowingPositions; 
	bool isCaptureEnabled; 
	bool isShowingAngleAndDistance; 
	bool isMarkerTemplatesInitialized;
	bool isOptionsInitialized;
	bool autoLightCoolness;

	int imgIndex;
	int TextLine;
	int equalizeIndex;
	int vectorIndex;
	int xpointsIndex;
	int imageIndex;
	int MarkersprocessingIndex;
	int XPointsprocessingIndex;
	int HdrModeIndex;
	int HdrLockedMarkerIndex;
	int BackGroundProcessingIndex;
	int Cam2CamRegIndex;
	vector<int> CamIndexVector;
	Fl_Menu_Item* menuArray;

	// Options window
	Fl_Window *optionsWindow;
	Fl_Value_Slider *lightCoolSlider;
	Fl_Counter *extrapFramesCounter;
	Fl_Check_Button* autoCoolCheckbox;
	Fl_Check_Button* smallMarkersCheckbox;
	static void onAutoCheckClicked(Fl_Button* button, GuiGenerator* data);
	static void onSMCheckClicked(Fl_Button* button, GuiGenerator* data);
	double lightCoolness;

	// Template window
	Fl_Window *markersTmplsWindow;
	Fl_Button* renameButton;
	Fl_Button* deleteButton;
	Fl_Button* stopSampleButton;
	Fl_Button* saveButton;
	Fl_Button* cancelButton;
	Fl_Timer* ctimer;
	Fl_Text_Buffer* markersListBuffer; 
	Fl_Text_Display *markersList;
	Fl_Button* sampleFacetButton;
	Fl_Input* tmplName;
	Fl_Box* collectingStatus;
	Fl_Counter* jitterValue;
	Fl_Check_Button* moreFacetCheckbox;
	Fl_Box* collectingNum;

	int m_collectedSampleFrames;

	vector<MTCollection::Collection*> sampleVectors;
	vector<Xform3D*> facet1ToCameraXfs;

	bool isCollectingSamples;
	bool isAdditionalFacetAdding;

#ifdef WIN32
	BITMAPINFOHEADER  bmi;
	tagRGBQUAD LUT[256]; // hardwired for 8 bit
#endif

};

#endif
