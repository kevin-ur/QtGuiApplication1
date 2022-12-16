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
#include "GuiGenerator.h"

#define OPENGL
#ifdef OPENGL
#include "FL/gl.h"
#endif


#if WIN32
#include <Windows.h.>
#include <direct.h>
#include <errno.h>
// FL.math creates some conflict, so declare by hand
extern "C" {
double  __cdecl cos(double);
double  __cdecl sin(double);
}
#else
#include <math.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif


bool left_turn;

GuiGenerator::GuiGenerator()
{
	this->pPers = new Persistence();
	this->pMarkers = new Markers();
	this->isShowingVectors = true;
	this->isShowingHalfSize = false;
	this->isShowingXpoints = false;
	this->isDisplayEnabled = true;
	this->isMarkersProcessingEnabled = true;
	this->isXPointsProcessingEnabled = true;
	this->isHdrModeEnabled = false;
	this->isHdrLockedMarkerEnabled = false;
	this->isBackGroundProcessingEnabled = false;
	this->isCam2CamRegistered = true;
	this->isSingleWindow = false;
	this->isCollectingSamples = false;
	this->isAdditionalFacetAdding = false;
	this->m_collectedSampleFrames = 0;
	this->isShowingPositions = true; 
	this->isCaptureEnabled = true;
	this->isShowingAngleAndDistance = true;
	this->isMarkerTemplatesInitialized = false;
	this->isOptionsInitialized = false;
	this->markersListBuffer = NULL;
	this->markersList = NULL;


	this->pCurrCam = NULL;

}

/****************************/
/** Destructor */
GuiGenerator::~GuiGenerator()
{
}


/****************************/
/** */
void GuiGenerator::initialINIAccess()
{

	char currDir[255];
	mtUtils::getCurrPath(currDir);
#ifdef WIN32
	strcat(currDir,"\\MTDemoCPP.ini");
#else
	strcat(currDir,"/MTDemoCPP.ini");
#endif
	this->pPers->setPath(currDir);

	this->pPers->setSection ("General");
#if 0
	//Setting the FrameInterleave property in the Markers object
	int defaultFrameInterleave = 0;
	this->pMarkers->setPredictiveFramesInterleave( this->pPers->retrieveInt("PredictiveFramesInterleave", defaultFrameInterleave) );
#endif

	//Setting the TemplateMatchToleranceMM property in the Markers object
	double defaultTempMatchToleranceMM = 1.0;
	double defaultLightCoolness = 0.1;

	this->pMarkers->setTemplateMatchToleranceMM( this->pPers->retrieveDouble("TemplateMatchToleranceMM", defaultTempMatchToleranceMM) );
	this->lightCoolness = this->pPers->retrieveDouble("LightCoolness", defaultLightCoolness);
	
	int s = (this->pPers->retrieveInt("SingleWindow", 1) );
	isSingleWindow = (s == 0? true:true);
	s = (this->pPers->retrieveInt("UseOpenGL", 1) );
	useOpenGL = (s == 0? false:true);
	cout << "Open Gl: " << useOpenGL<< endl;

	bool defaultSmallerXPFootprint = true;
	int defaultExtrapolatedFrames = 5;

	bool SmallerXPFootprint = (bool)(this->pPers->retrieveInt("DetectSmallMarkers", defaultSmallerXPFootprint));
	int ExtrapolatedFrames = this->pPers->retrieveInt("ExtrapolatedFrames", defaultExtrapolatedFrames);

	this->pMarkers->setSmallerXPFootprint(SmallerXPFootprint);
	this->pMarkers->setExtrapolatedFrames(ExtrapolatedFrames);

	//could have been clipped
	this->pPers->saveInt("ExtrapolatedFrames", this->pMarkers->getExtrapolatedFrames());

	//Test if Markers dir exists, otherwise create
	char currPath[255];
	mtUtils::getCurrPath(currPath);
#ifdef WIN32
	strcat(currPath, "\\Markers\\");
#else
	strcat(currPath, "/Markers/");
#endif

	
#ifdef WIN32
	int result = _mkdir(currPath); 
	if (result != 0 && errno != EEXIST) {
		fl_message("Could not create Markers folder! Please create it manually.");
		return;
	}

#else
	int result = mkdir(currPath, 777); 
	int e = errno;
	if (result != 0 && e != EEXIST) {
		fl_message("Could not create Markers folder! Please create it manually.");
		return;
	}
#endif

}

/****************************/
/** */
void GuiGenerator::setupCameras()
{
	this->pCameras = new Cameras();
	int result = this->pCameras->AttachAvailableCameras();

	if (result == 0 &&  this->pCameras->getCount() >= 1 ) {
		//this->pCurrCam = this->pCameras->m_vCameras[0];
		this->pCurrCam = this->pCameras->getCamera(0);
		if (this->pCurrCam->getXRes() > 1200 ) this->isShowingHalfSize = true;
		cout << " Camera attached successfully " << endl;
	} else {
		cout << " No camera available or missing calibration file. Please also check that MTHome system environment variable is set " << endl;
		fl_message(" No camera available or missing calibration file. Please also check that MTHome system environment variable is set ");
		exit(1);
	}
}

/****************************/
/** */
void GuiGenerator::detach()
{
	this->pCameras->Detach();
}
/****************************/
/** */
void GuiGenerator::refreshMarkerTemplates()
{
	vector<string> templateNames;
	mtUtils::getFileNamesFromDirectory( templateNames,"Markers", true);

	int result = 0;
	result = this->pMarkers->clearTemplates();
		cout << "Cleared templates..." << endl;
	

	char currentFolderPath[MT_MAX_STRING_LENGTH];
	mtCompletionCode st;

	mtUtils::getCurrPath(currentFolderPath);
#ifdef WIN32
		strcat(currentFolderPath, "\\Markers\\");
#else
		strcat(currentFolderPath, "/Markers/");
#endif

	st = Markers_LoadTemplates(currentFolderPath);

}

void GuiGenerator::EnableHdrLockedMarkers(bool newVal)
{
	// direct call to the MTC API
	//Markers_AutoAdjustShortCycleHdrExposureLockedMarkersSet(true);
	//Markers_AutoAdjustHdrExposureLockMarkersNamesSet("DrillTag", "JawTag");

	// Use the Wrapper
	this->pMarkers->setAutoAdjustShortCycleHdrExposureLockedMarkers(newVal);
	this->pMarkers->setAutoAdjustHdrExposureLockMarkersNames("DrillTag", "JawTag");
	
	// retrieving values
	//bool isHdrLockedMarkerEnabled = this->pMarkers->getAutoAdjustShortCycleHdrExposureLockedMarkers();
	//std::string MarName1;
	//std::string MarName2;
	//this->pMarkers->getAutoAdjustHdrExposureLockMarkersNames(MarName1, MarName2);
}

/****************************/
/** The callback function for the ctimer. */
void GuiGenerator::ctimer_cb (void *calldata)
{
	GuiGenerator* data = (GuiGenerator*)calldata;
	static mtHandle fpsH = 0;
	static int frames;
	double secs;
	data->TextLine = 60;

	if ( fpsH == 0) fpsH = StopWatch_New();
	bool r;
	int result;
	StopWatch_Go(fpsH);

	data->isMarkersProcessingEnabled = data->menuArray[data->MarkersprocessingIndex].value();
	data->isXPointsProcessingEnabled = data->menuArray[data->XPointsprocessingIndex].value();
	data->isHdrModeEnabled = data->menuArray[data->HdrModeIndex].value();
	data->isHdrLockedMarkerEnabled = data->menuArray[data->HdrLockedMarkerIndex].value();
	data->isBackGroundProcessingEnabled = data->menuArray[data->BackGroundProcessingIndex].value();

	if (data->isBackGroundProcessingEnabled && !data->pMarkers->getBackGroundProcess()) {
		data->pMarkers->setBackGroundProcess(true);
		cout << "BackGround Processing Enabled" << endl;
	} else if (!data->isBackGroundProcessingEnabled && data->pMarkers->getBackGroundProcess()) {
		data->pMarkers->setBackGroundProcess(false);
		cout << "BackGround Processing Disabled" << endl;
	}

	if (!data->pMarkers->getBackGroundProcess()) {
		r = data->pCameras->grabFrame(NULL);
		//if (!r )	cout << "Warning! in grabing a frame or Grab frame time out" << endl;
		if (r)
			frames++;
	}
	if (data->pMarkers->getBackGroundProcess()) {
		data->pMarkers->getIdentifiedMarkersFromBackgroundThread(NULL);
		frames++;
	}

	if (data->isMarkersProcessingEnabled || data->isXPointsProcessingEnabled ) { // if markers processing is enabled
		bool withDisplay = (bool)data->menuArray[data->imageIndex].value();
		result = data->processCurrFrame(withDisplay );
		if (result != 0)
			cout << "Error in processing a frame" << endl;
	} else {
		if ( data->menuArray[data->imageIndex].value()) { // if Display Images
			data->DisplayLRImages();
		} else { // Clear
			data->ClearWindow();
		}
	}

	bool IsAutoExposureLockedMarkersEnabled = data->pMarkers->getAutoAdjustShortCycleHdrExposureLockedMarkers();
	if (!IsAutoExposureLockedMarkersEnabled && data->isHdrLockedMarkerEnabled) {
		cout << "HDR Locked Markers Enabled" << endl;
		data->EnableHdrLockedMarkers(true);		
		IsAutoExposureLockedMarkersEnabled = true;
	}
	else if (IsAutoExposureLockedMarkersEnabled && !data->isHdrLockedMarkerEnabled) {
		cout << "HDR Locked Markers Disabled" << endl;
		data->EnableHdrLockedMarkers(false);
		IsAutoExposureLockedMarkersEnabled = false;
	}

	bool IsHdrEnabled = data->pCurrCam->getHdrModeEnabled();
	if (!IsHdrEnabled && data->isHdrModeEnabled) {
		cout << "HDR Enabled" << endl;
		data->pCurrCam->setHdrModeEnabled(true);
	}
	else if (IsHdrEnabled && !data->isHdrModeEnabled) {
		if (!IsAutoExposureLockedMarkersEnabled) {
			cout << "HDR Disabled" << endl;
			data->pCurrCam->setHdrModeEnabled(false);
		}
	}


	StopWatch_ElapsedSecsGet(fpsH, &secs);
	if (secs > 1) {
		char str[MT_MAX_STRING_LENGTH];
		sprintf(str,"MicronTrackerCPP demo: %4.1f Hz",frames/secs);
		data->window->label((const char *)str);
		StopWatch_ResetAndGo(fpsH);
		frames = 0;
	}

	if (data->menuArray[data->Cam2CamRegIndex].value())
		Markers_AutoAdjustCam2CamRegistrationSet(true);
	else
		Markers_AutoAdjustCam2CamRegistrationSet(false);

	Fl::add_timeout(0.01, ctimer_cb, data);
}


/****************************/
/** */
int GuiGenerator::processCurrFrame(bool withDisplay)
{

	if (this->pMarkers->getBackGroundProcess() == false) {
		if (isMarkersProcessingEnabled)
			this->pMarkers->processFrame(NULL);
		if (isXPointsProcessingEnabled)
			this->pXPoints->processFrame(NULL);
	}

	this->pPers->setSection ("General");
	int s = (this->pPers->retrieveInt("AutoCool", 1) );
	bool autoCool = (s == 0? false:true);
	if (autoCool) {
		AdjustLigtCoolnessByCoolCard();
	}
	if (withDisplay) {
		this->DisplayLRImages();
	} else {
		ClearWindow();
	}

	if (isMarkersProcessingEnabled)
		this->showIdentifiedMarkers();
	if (isXPointsProcessingEnabled)
		this->showIdentifiedXPoints();
	

	if (this->isCollectingSamples)
		this->collectNewSamples();

	// this->showUnidentifiedVectors();
	return 0;

}
/****************************/
/** */
MTMarker::Marker* GuiGenerator::findMarkerInSet(char* name)
{
	MTMarker::Marker* foundMarker;
	for(int i=0; i<this->pMarkers->getTemplateCount(); i++)
	{
		std::string currName;
		this->pMarkers->getTemplateItemName(i, currName);
		if (currName.compare( name)==0)
		{
			foundMarker = new MTMarker::Marker(this->pMarkers->getTemplateItem(i));
			return foundMarker;
		}
        // TODO:verify this
		else
			return NULL;
	}
	return NULL;
}
		
/****************************/
/** */
void GuiGenerator::collectNewSamples()
{
	static char buffer[255];
	MTCollection::Collection* col = new MTCollection::Collection( this->pMarkers->unidentifiedVectors(this->pCurrCam) );
	if (col->count() == 2 )
	{
		this->collectingStatus->label("Collecting samples");
		this->sampleVectors.push_back(col);
		this->m_collectedSampleFrames++;
		// Additional facets
		if (this->isAdditionalFacetAdding)
		{
			MTCollection::Collection* identifiedFacetsCol = new MTCollection::Collection(this->pCurrMarker->identifiedFacets(NULL));
			if (identifiedFacetsCol->count() > 0)
			{
				// Also compute and save the xform of facet1 to sensor
				if (this->pCurrMarker->marker2CameraXf(this->pCurrCam->Handle()) != (Xform3D*)NULL)
					this->facet1ToCameraXfs.push_back(this->pCurrMarker->marker2CameraXf(this->pCurrCam->Handle()));
				else // Seen by a camera not registered with this one
				{
					this->collectingStatus->label("Cannot sample: unregistered camera");
					this->sampleVectors.pop_back();
					this->m_collectedSampleFrames--;
				}
			}
			else // Not seeing a known facet
			{
				this->collectingStatus->label("Cannot sample: no known facet!");
				this->sampleVectors.pop_back();
				this->m_collectedSampleFrames--;
			}
			delete identifiedFacetsCol;
		}
 		sprintf(buffer, "Collected: %d\n", this->m_collectedSampleFrames);
		this->collectingStatus->label(buffer);
	}
	else if(col->count() < 2) {
		this->collectingStatus->label("No new facet detected.");
	} else if(col->count() > 2) {
		sprintf(buffer, "More than 2 vectors detected [%d].\n", col->count());
		this->collectingStatus->label(buffer);
	}
	//delete col;

}

void GuiGenerator::drawLine(int side, float x1, float y1, float x2, float y2, float r, float g, float b, int stipple)
{
if (useOpenGL) {
#ifdef OPENGL
  int xoffset, yoffset;

  if (side == 1) 
	xoffset = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
  else
	xoffset = 0;

  yoffset = pCurrCam->getYRes()*(this->isShowingHalfSize ? 0.5 : 1);
  gl_start();
  glPushMatrix();
  glMatrixMode(GL_MODELVIEW );
  glColor3f(r,g,b);
  glLineWidth (2);
  glPushAttrib (GL_LINE_BIT);
  glLineStipple (3, stipple);
  glBegin (GL_LINE_STRIP);
  glVertex2d (xoffset + x1*(this->isShowingHalfSize ? 0.5 : 1), yoffset - y1*(this->isShowingHalfSize ? 0.5 : 1));
  glVertex2d (xoffset + x2*(this->isShowingHalfSize ? 0.5 : 1), yoffset - y2*(this->isShowingHalfSize ? 0.5 : 1));
  glEnd ();
  glPopAttrib ();
  glPopMatrix();
  glFinish();
#endif
} else {

	float xoffset, yoffset;

	if (side == 1) 
		xoffset = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
	else
		xoffset = 0;
	yoffset = MENU_HEIGHT;
	fl_color(fl_rgb_color(r*255, g*255, b*255));
	fl_line(xoffset + x1*(this->isShowingHalfSize ? 0.5 : 1), yoffset + y1*(this->isShowingHalfSize ? 0.5 : 1), xoffset + x2*(this->isShowingHalfSize ? 0.5 : 1), yoffset + y2*(this->isShowingHalfSize ? 0.5 : 1)) ;

}
}

void GuiGenerator::drawCircle(int side, float x, float y, float radius, float r, float g, float b, int stipple)
{
if (useOpenGL) {
#ifdef OPENGL
	int xoffset, yoffset;

	if (side == 1) 
		xoffset = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
	else
		xoffset = 0;
  yoffset = pCurrCam->getYRes()*(this->isShowingHalfSize ? 0.5 : 1);
  gl_start();
  glPushMatrix();
  glColor3f(r,g,b);
  glLineWidth (2);
  glPushAttrib (GL_LINE_BIT);
  glLineStipple (3, stipple);
  glBegin(GL_POLYGON);
	for( double ang=0; ang <= 2*PI; ang += 0.1)
	{
		double c = cos(ang);
		double s = sin(ang);
		glVertex2d( xoffset + x * (this->isShowingHalfSize ? 0.5 : 1) + radius * c, 
                     yoffset - y *(this->isShowingHalfSize ? 0.5 : 1) + radius * s);  
	}
  glEnd();
  glPopAttrib ();
  glPopMatrix();
  glFinish();
#endif
} else {
	int xoffset, yoffset;

	if (side == 1) 
		xoffset = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
	else
		xoffset = 0;
	yoffset = MENU_HEIGHT;
	fl_color(fl_rgb_color(r*255, g*255, b*255));
	fl_circle( x * (this->isShowingHalfSize ? 0.5 : 1) + xoffset,  y*(this->isShowingHalfSize ? 0.5 : 1)+ yoffset,  (float)radius);

}
}


/****************************/

double GuiGenerator::findDistance(double* v1, double* v2)
{
	double acc = 0.0;
	for (int i=0; i< 3; i++)
		acc = acc + ( (v1[i] - v2[i]) * (v1[i] - v2[i]) );
	return sqrt(acc);
}

/****************************/
double GuiGenerator::dotProductVectors(double* v1, double* v2)
{
	double result = 0;
	for (int i=0; i<3; i++)
		result += v1[i]*v2[i];
	return result;
}

void GuiGenerator::AdjustLigtCoolnessByCoolCard()
{
	MTCollection::Collection* markersCollection = new MTCollection::Collection(this->pMarkers->identifiedMarkers(this->pCurrCam));
	if (markersCollection->count() == 0) {
		delete markersCollection; 
		return;
	}
	int markerNum = 1;
	MTMarker::Marker* m;

	for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
	{
		m = new MTMarker::Marker(markersCollection->itemI(markerNum));
		if (m->wasIdentified(this->pCurrCam) )
		{
		   if ( strncmp( m->getName(),"COOL",4 ) == 0 ||  
		   	( strncmp( m->getName(),"cool",4 ) == 0) ||
		   	( strncmp( m->getName(),"Cool",4 ) == 0)) {
		      MTCollection::Collection* facetsCollection = new MTCollection::Collection(m->identifiedFacets(this->pCurrCam));
			  for (int facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
			  {
				  Facet* f = new Facet(facetsCollection->itemI(facetNum));
				  if (facetNum == 1) {
					vector<Vector *> ColorVector;
					ColorVector = (f->IdentifiedVectors());
					if ( ColorVector[0] != NULL) {
						this->pPers->setSection ("General");
						this->pCurrCam->AdjustCoolnessFromColorVector(ColorVector[0]->Handle());
						this->pPers->saveDouble("LightCoolness",this->pCurrCam->getLightCoolness());
					}
					delete ColorVector[0];
					delete ColorVector[1];
				  }
				  delete f;
			  }
			  delete facetsCollection;
		   }
		}
		free(m);
	}
	delete markersCollection;

	
}

/****************************/
void GuiGenerator::showIdentifiedXPoints()
{
	MTCollection::Collection* xpointsCollection = new MTCollection::Collection(this->pXPoints->detectedXPoints(this->pCurrCam)); 

	if (xpointsCollection->count() == 0) {
		delete xpointsCollection; 
		return;
	}

	double x[3], y[3];
	double x3,y3,z3;
	int XPNum=1;
	MTXPoint* XP;
	double radius = 5;
	// here we need the left side presentation: coordinates in sensor space, distances etc	
	Xform3D* Marker2CurrCameraXf = NULL;
	if(this->isShowingPositions)
	{
		for (XPNum = 1; XPNum <= xpointsCollection->count(); XPNum++)
		{
			XP = new MTXPoint(xpointsCollection->itemI(XPNum));
			XP->Position2D(&x[0], &y[0], &x[1], &y[1], &x[2], &y[2]);
			XP->Position3D(&x3, &y3, &z3);
			XP->setIndex(XPNum);

			drawCircle(0, x[0], y[0], radius, 1.0, 0, 0, 0xFFFF);
		}
	}

	char s[100];
	sprintf(s, "%d", xpointsCollection->count());
	strcat(s, " XPs are detected by Current Camera");
	this->drawText(s, 10/(this->isShowingHalfSize ? 0.5 : 1), 40/(this->isShowingHalfSize ? 0.5 : 1));

}

	

/****************************/
void GuiGenerator::showIdentifiedMarkers()
{
	MTCollection::Collection* markersCollection = new MTCollection::Collection(this->pMarkers->identifiedMarkers(NULL)); //this->pCurrCam));

	if (markersCollection->count() == 0) {
		delete markersCollection; 
		return;
	}

	int markerNum = 1;
	int facetNum = 1;
	MTMarker::Marker* m;
	// here we need the left side presentation: coordinates in sensor space, distances etc	
	MTMarker::Marker* marker;
	// here we need the left side presentation: coordinates in sensor space, distances etc	
	Xform3D* Marker2CurrCameraXf = NULL;
	if(this->isShowingPositions)
	{
		for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
		{
			marker = new MTMarker::Marker(markersCollection->itemI(markerNum));
			Marker2CurrCameraXf = marker->marker2CameraXf(this->pCurrCam->Handle());
			// There may be a situation where marker was identified by another camera (not CurrCamera)
			// and the identifying camera is not registered with CurrCamera. In this case, the pose is
			// not known in CurrCamera coordinates and Marker2CurrCameraXf is Nothing.
			if(Marker2CurrCameraXf != NULL)
			{
				if(this->isShowingXpoints)
				{
					Facet* ft = new Facet(marker->identifiedFacets(this->pCurrCam));
					MTCollection::Collection* c = new MTCollection::Collection(marker->getTemplateFacets());
					Vector* LV = (ft->IdentifiedVectors())[0];
					Vector* SV = (ft->IdentifiedVectors())[1];
					static double XPHistory[3][20][31];
					long NextXPI[31] = {0};
					double endPos[2][3];
					int i;
					LV->getEndPos2x3( (double*)endPos);
					for ( i=0; i<3; i++)
						XPHistory[i][ NextXPI[markerNum] ][markerNum] = endPos[0][i];
					double AvgXPI[3] = {0,0,0};
					NextXPI[markerNum] =  (NextXPI[markerNum]+1)  % 20;
					for (i=0; i<20; i++)
					{
						AvgXPI[0] = AvgXPI[0] + XPHistory[0][i][markerNum] / 20.0;
						AvgXPI[1] = AvgXPI[1] + XPHistory[1][i][markerNum] / 20.0;
						AvgXPI[2] = AvgXPI[2] + XPHistory[2][i][markerNum] / 20.0;
					}
#if 0
					if (this->isCaptureEnabled)
					{ 
						cout << "Average: (" << AvgXPI[0] << ", " << AvgXPI[1] << ", " <<  AvgXPI[2] << ")" << endl;
					}
					else //snapshot
					{
					}
#endif
					// Show also the averaged location of the LV base in the images
					static double XPImgHistory[2][2][20][31];
					long NextXPImgI[31] = {0};
					double EXP[2][2][2];
					LV->getEndXPoints( (double*)EXP );
					for(i=0; i<2; i++)
					{
						XPImgHistory[i][0][ NextXPImgI[markerNum] ][markerNum] = EXP[i][0][0]; // (x/y, base/head, left/right)
						XPImgHistory[i][1][ NextXPImgI[markerNum] ][markerNum] = EXP[i][0][1]; // (x/y, base/head, left/right)
					}
					double AvgXPImg[2][2] = {0};
					NextXPImgI[markerNum] = (NextXPImgI[markerNum]+1) % 20;
					for (i=0; i<20; i++)
					{
						AvgXPImg[0][0] = AvgXPImg[0][0] + XPImgHistory[0][0][i][markerNum] / 20.0;
						AvgXPImg[0][1] = AvgXPImg[0][1] + XPImgHistory[0][1][i][markerNum] / 20.0;
						AvgXPImg[1][0] = AvgXPImg[1][0] + XPImgHistory[1][0][i][markerNum] / 20.0;
						AvgXPImg[1][1] = AvgXPImg[1][1] + XPImgHistory[1][1][i][markerNum] / 20.0;
					}
#if 0
					if (this->isCaptureEnabled)
					{
						cout << " L-R imx: " << AvgXPImg[0][0] << "-" << AvgXPImg[0][1] << endl;
						cout << " imy: " << AvgXPImg[1][0] << "-" << AvgXPImg[1][1] << endl;
					}
					else
					{
						cout << "L-R imx: " << EXP[0][0][0] << "-" << EXP[0][0][1] << endl;
						cout << "imy: " << EXP[1][0][0] << "-" << EXP[1][0][1] << endl;
					}
#endif
					delete ft; delete c; 
				}
				else
				{
					//Show the XYZ position of the Marker's origin.
					//string s = ".(";
					char buffer[3][100];
					char s[600];
					Xform3D* m2c;
					m2c = marker->marker2CameraXf(this->pCurrCam->Handle());
					for (int i=0; i<3; i++)
					{	
						sprintf(buffer[i], "%.1f",m2c->getShift(i));
					}
					sprintf(s, "%d", markerNum);
					strcat(s, ". (");
					strcat(s, buffer[0]);
					strcat(s, ",");
					strcat(s, buffer[1]);
					strcat(s, ",");
					strcat(s, buffer[2]);
					strcat(s, ")");

					// If there's a tooltip, add it
					Xform3D* t2m; // tooltip to marker xform
					Xform3D* t2c; // tooltip to camera xform
					double svec[3];
					t2m = marker->tooltip2MarkerXf();
					t2m->getShiftVector(svec);
					if (svec[0] != 0 || svec[1] != 0 || svec[2] != 0) { // non-null transform
						strcat(s, " tip(");
						t2c = t2m->concatenate(m2c);
						for (int i=0; i<3; i++) {
							sprintf(buffer[i], "%.1f", t2c->getShift(i));
						}
						strcat(s, buffer[0]);
						strcat(s, ",");
						strcat(s, buffer[1]);
						strcat(s, ",");
						strcat(s, buffer[2]);
						strcat(s, ")");
						delete t2c;
					}
					delete t2m;
					delete m2c;
					this->drawText(s, 10/(this->isShowingHalfSize ? 0.5 : 1), this->TextLine/(this->isShowingHalfSize ? 0.5 : 1));
					this->TextLine += 20;
					//free(t2c);

				}
				delete Marker2CurrCameraXf;
			}
			else // Not known
			{
				// cout << markerNum << ". another (unregistered) camera";
			}
			free(marker);

		}
	}
	

	for (markerNum = 1; markerNum <= markersCollection->count(); markerNum++)
	{
		m = new MTMarker::Marker(markersCollection->itemI(markerNum));
		if (m->wasIdentified(this->pCurrCam) )
		{

			MTCollection::Collection* facetsCollection = new MTCollection::Collection(m->identifiedFacets(this->pCurrCam));
			for (facetNum = 1; facetNum <= facetsCollection->count(); facetNum++)
			{
				Facet* f = new Facet(facetsCollection->itemI(facetNum));
				// get Xpoints and then draw on each image if enabled	
				double LS_LR_BH_XY[2][3][2][2];
				f->getXpoints(this->pCurrCam, (double *)LS_LR_BH_XY);
				int max;
				if (isSingleWindow)
					max = 0;
				else
					max = 1;
				if (menuArray[vectorIndex].value() || menuArray[xpointsIndex].value()){
					int nx = (LS_LR_BH_XY[0][0][0][0] + LS_LR_BH_XY[0][0][1][0])/2 ;
					int ny = (LS_LR_BH_XY[0][0][0][1] + LS_LR_BH_XY[0][0][1][1])/2 ;
					char caption[255];
					if (facetsCollection->count() > 1) {
						sprintf(caption,"%d.%s/%d",markerNum, m->getName(), facetNum);
					} else {
						sprintf(caption,"%d.%s",markerNum, m->getName());
					}
					drawText(caption, nx, ny);
				}

				for (int side=0; side<=max;side ++) {
					if ( menuArray[vectorIndex].value()) {
						// long vector
						drawLine(side, LS_LR_BH_XY[0][side][0][0], LS_LR_BH_XY[0][side][0][1],
							LS_LR_BH_XY[0][side][1][0], LS_LR_BH_XY[0][side][1][1],
							0.0, 0.5, 1.0, 0xFFFF);
						// short vector
						drawLine(side, LS_LR_BH_XY[1][side][0][0], LS_LR_BH_XY[1][side][0][1],
							LS_LR_BH_XY[1][side][1][0], LS_LR_BH_XY[1][side][1][1],
							1.0, 1.0, 0., 0x8888);
					}
					if ( menuArray[xpointsIndex].value()) {
						int radius = 4;
						// Draw a circle arounf each point
						drawCircle(side, LS_LR_BH_XY[0][side][0][0], LS_LR_BH_XY[0][side][0][1], radius, 0, 0, 1.0, 0xFFFF);
						drawCircle(side, LS_LR_BH_XY[0][side][1][0], LS_LR_BH_XY[0][side][1][1], radius, 0, 0, 1.0, 0xFFFF);
						drawCircle(side, LS_LR_BH_XY[1][side][0][0], LS_LR_BH_XY[1][side][0][1], radius, 0, 0, 1.0, 0xFFFF);
						drawCircle(side, LS_LR_BH_XY[1][side][1][0], LS_LR_BH_XY[1][side][1][1], radius, 0, 0, 1.0, 0xFFFF);
					}
				}
				delete f;
			}
			delete facetsCollection;
		}
		free(m);
	}
	// here we need the left side presentation: coordinates in sensor space, distances etc		


	if (this->isShowingAngleAndDistance && markersCollection->count() > 1)
	{
		Xform3D* Mi2Cam = NULL;
		Xform3D* Mj2Cam = NULL;
		for(int i=1; i<markersCollection->count(); i++)
		{
			MTMarker::Marker* m1 = new MTMarker::Marker(markersCollection->itemI(i));
			Mi2Cam = m1->marker2CameraXf(this->pCurrCam->Handle());
			for (int j=i+1; j<markersCollection->count()+1; j++)
			{
				MTMarker::Marker* m2 = new MTMarker::Marker(markersCollection->itemI(j));
				Mj2Cam = m2->marker2CameraXf(this->pCurrCam->Handle());
				if(Mi2Cam != NULL || Mj2Cam != NULL)
				{
					double XUnitV[3] = {0};
					double angleCos;
					double angleRads;

					if (this->isShowingXpoints)
					{
						Facet* Fti;
						Facet* Ftj;
						Vector* LVij[2];
						static long NextDistI;
						static double distanceHistory[20];
						MTCollection::Collection* iIdFacets = new MTCollection::Collection(m1->identifiedFacets(this->pCurrCam));
						Fti = new Facet(iIdFacets->itemI(1));
						LVij[0] = (Fti->IdentifiedVectors())[0];
						MTCollection::Collection* jIdFacets = new MTCollection::Collection(m2->identifiedFacets(this->pCurrCam));
						Ftj = new Facet(jIdFacets->itemI(1));
						LVij[1] = (Fti->IdentifiedVectors())[0];
					} else	{
						double vec1[3];
						Mi2Cam->getShiftVector(vec1);
						//m1->marker2CameraXf(this->pCurrCam->Handle())->getShiftVector(vec1);
						double vec2[3];
						//m2->marker2CameraXf(this->pCurrCam->Handle())->getShiftVector(vec2);
						Mj2Cam->getShiftVector(vec2);
						double distance = this->findDistance( vec1, vec2 );
						
						char s[300];
						char buffer[100];
						sprintf(s, "%d", i);
						strcat(s, "-");
						sprintf(buffer, "%d", i+1);
						strcat(s, buffer);
						strcat(s, ": ");
						sprintf(buffer, "%.1f", distance);
						strcat(s, buffer);
						strcat(s, " mm / ");
						
						XUnitV[0] = 1;
						double XVect1[3];
						//m1->marker2CameraXf(this->pCurrCam->Handle())->getRotateVector(XVect1, XUnitV);
						Mi2Cam->getRotateVector(XVect1, XUnitV);
						double XVect2[3];
						//m2->marker2CameraXf(this->pCurrCam->Handle())->getRotateVector(XVect2, XUnitV);
						Mj2Cam->getRotateVector(XVect2, XUnitV);
						angleCos = this->dotProductVectors(XVect1, XVect2);
						angleRads = mtUtils::acos(angleCos);
						
						sprintf(buffer, "%.1f", angleRads * 180 / PI);
						strcat(s, buffer);
#ifdef WIN32
						//strcat(s, "?);
#else
						strcat(s, " deg");
#endif
						this->drawText(s, 10/(this->isShowingHalfSize ? 0.5 : 1), this->TextLine/(this->isShowingHalfSize ? 0.5 : 1) );
						this->TextLine += 20;

					}
				}
				delete Mj2Cam;
				free(m2);
			}
			delete Mi2Cam;
			free(m1);
		}
	}
	delete markersCollection; 

}

void GuiGenerator::drawText( char* text, int x, int y)
{
if (useOpenGL) {
#ifdef __APPLE__
    window->make_current();
    gl_start();
	glPushMatrix();

    glRasterPos2f(x * (this->isShowingHalfSize ? 0.5 : 1),  480 - y * (this->isShowingHalfSize ? 0.5 : 1));
    const char * message = (const char*)text;
    while (*message)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *message++);

    glColor3f(0.3, 0.1, 1.0);
		
	glEnd ();
    glPopAttrib ();
    glPopMatrix();
    glFinish();	
#endif

    } else {
    
	window->make_current();
	fl_font(FL_TIMES /*| FL_BOLD*/ , 20);

	fl_color(fl_rgb_color(15, 15, 255));
	fl_draw((const char*)(text), x * (this->isShowingHalfSize ? 0.5 : 1),  y * (this->isShowingHalfSize ? 0.5 : 1));
    
    }
}
/****************************/
/** Starts the main window and sets the timer*/
void GuiGenerator::startGrabingFrames()
{

	int width = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
	int height = pCurrCam->getYRes() * (this->isShowingHalfSize ? 0.5 : 1);
	int mult = 2.0;
	if ( isSingleWindow ) {
		mult = 1.0;
	}

	window = new Fl_Window(width*mult, height+MENU_HEIGHT);

	Fl::add_timeout(0.03, ctimer_cb, this);


	Fl_Menu_Bar* mainMenu = new Fl_Menu_Bar(0, 0, width*mult, MENU_HEIGHT, "");
	MarkersprocessingIndex = mainMenu->add("Activate/ Markers Processing", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE);
	XPointsprocessingIndex = mainMenu->add("Activate/ XPoints Processing", 0, 0, 0, FL_MENU_TOGGLE|0);
	HdrModeIndex = mainMenu->add("Activate/ HDR Mode", 0, 0, 0, FL_MENU_TOGGLE|0);
	HdrLockedMarkerIndex = mainMenu->add("Activate/ HDR Locked Markers", 0, 0, 0, FL_MENU_TOGGLE | 0);
	BackGroundProcessingIndex = mainMenu->add("Activate/ BackGround Processing", 0, 0, 0, FL_MENU_TOGGLE | 0);
	Cam2CamRegIndex = mainMenu->add("Activate/ Cam-Cam Registration", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE);
	mainMenu->add("Activate/ Marker Templates...", 0, (Fl_Callback*)activateMarkerTemplates, (void *)this);
	mainMenu->add("Activate/ Options...", 0, (Fl_Callback*)activateOptions, (void *)this);
	imageIndex = mainMenu->add("Display/ Image", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE);
	mainMenu->add("Display/ Equalize", 0, (Fl_Callback*)SetEqualize, (void *)this, FL_MENU_TOGGLE|FL_MENU_VALUE);
	vectorIndex = mainMenu->add("Display/ Vectors", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE);
	xpointsIndex = mainMenu->add("Display/ Xpoints", 0, 0, 0, FL_MENU_TOGGLE|FL_MENU_VALUE);
	char str[MT_MAX_STRING_LENGTH];

	int mIndex, i;
	void **cbuffer = 0;
	for ( i=0 ;i< this->pCameras->getCount();i++) {
		int width = this->pCameras->m_vCameras[i]->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
		int height = this->pCameras->m_vCameras[i]->getYRes() * (this->isShowingHalfSize ? 0.5 : 1);		
		sprintf(str,"Cameras/ %d  (%d*%d)",this->pCameras->m_vCameras[i]->getSerialNum(), width , height );
        cbuffer =  (void **)malloc(2*sizeof(void*));
		cbuffer[0] = (void*)this;
		cbuffer[1] = (void*) this->pCameras->m_vCameras[i];	
		mIndex =  mainMenu->add(str, 0, (Fl_Callback*)SetCurrCamera, (void *)cbuffer, FL_MENU_RADIO|FL_MENU_VALUE);
		CamIndexVector.push_back( mIndex);
	}

	menuArray = (Fl_Menu_Item *)mainMenu->menu();
	if (this->pCameras->getCount() > 1) menuArray[CamIndexVector[0]].setonly();
	window->end();
	window->show();
	Fl::run();
}

void GuiGenerator::ClearWindow( )
{
		int width = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
		int height = pCurrCam->getYRes() * (this->isShowingHalfSize ? 0.5 : 1);
		window->make_current();
		float mult = 2.0;
		if ( isSingleWindow ) {
		mult = 1.0;
		}
		fl_color(100, 100, 100);
		fl_rectf( 0,  MENU_HEIGHT, mult*width, height);

}
void GuiGenerator::drawImage( Fl_Window *window, int x, int y, int width, int height, unsigned char *pixels, bool firstRowAtBottom)
{
#ifdef WIN32
	if (useOpenGL) {
#endif
		// flip it
#ifdef OPENGL		
		int bmWidth, k;
		bmWidth = width;
		unsigned char *lbuffer;
		lbuffer = (unsigned char *)malloc(bmWidth*height);
		if (lbuffer == 0) return; // out of memory
		for (k=0; k<height; k++) {
			memcpy((void *)(lbuffer + (height - k -1)*bmWidth), (void*)(pixels+k*width), width);
		}
		
		window->make_current();
		gl_start();
		glPushMatrix();
		glRasterPos2i(x, 0); 
		glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, lbuffer);
		//Display left & right subsequently
		/*if (left_turn == true) {
			glDrawPixels(width, height, GL_BLUE, GL_UNSIGNED_BYTE, lbuffer);
			left_turn = false;
		} else {
			glDrawPixels(width, height, GL_RED, GL_UNSIGNED_BYTE, lbuffer);
			left_turn = true;
		}*/

		glPopMatrix();
		gl_finish();
		free (lbuffer);
#endif
#ifdef WIN32
	} else {
		int R;
		bmi.biSize = sizeof(bmi);
		bmi.biWidth = width;
		bmi.biHeight = height;
		bmi.biPlanes = 1;
		bmi.biBitCount = 8;
		bmi.biCompression = 0; //none
		bmi.biSizeImage = width * height;

		if (( width % 4 == 0) && firstRowAtBottom ) {
			// we can do it in one shot
			window->make_current();
			R = SetDIBitsToDevice(fl_gc, x, y, width, height, 0, 0, 0, height, pixels, (tagBITMAPINFO*)&bmi, DIB_RGB_COLORS);
		} else {
			int bmWidth, k;
			// we first need to copy the lines in a temporary buffer
			if ( width % 4 != 0) {
				bmWidth = 4 * (width /4 +1);
			} else {
				bmWidth = width;
			}
			unsigned char *lbuffer;
			lbuffer = (unsigned char *)malloc(bmWidth*height);
			if (lbuffer == 0) return; // out of memory
			// reset bitmap info to conform with the new bitmap size
			bmi.biWidth = bmWidth;
			bmi.biSizeImage = bmWidth * height;
			
			if ( firstRowAtBottom) {
				for (k=0; k<height;k++) {
					memcpy((void *)(lbuffer + k*bmWidth), (void*) (pixels+k*width), width);
				}
			} else {
				for (k=0; k<height;k++) {
					memcpy((void *)(lbuffer + (height - k -1)*bmWidth), (void*)(pixels+k*width), width);
				}
			}
			
			window->make_current();
			R = SetDIBitsToDevice(fl_gc, x, y, width, height, 0, 0, 0, height, lbuffer,(tagBITMAPINFO*)&bmi , DIB_RGB_COLORS);
			free (lbuffer);
		}
	}
#endif
}



void GuiGenerator::	drawImages(bool left, bool right, bool middle)
{
	unsigned char **laddr, **raddr, **maddr;
	if (this->pCurrCam->getSensorsNum() == 3) 
		if (this->isShowingHalfSize)
			pCurrCam->getHalfSizeImages3( &laddr, &raddr, &maddr, pCurrCam->getXRes(), pCurrCam->getYRes() );
		else
			pCurrCam->getImages3( &laddr, &raddr, &maddr);
	else
		if (this->isShowingHalfSize)
			pCurrCam->getHalfSizeImages( &laddr, &raddr, pCurrCam->getXRes(), pCurrCam->getYRes() );
		else
			pCurrCam->getImages( &laddr, &raddr);

	int width = pCurrCam->getXRes() * (this->isShowingHalfSize ? 0.5 : 1);
	int height = pCurrCam->getYRes() * (this->isShowingHalfSize ? 0.5 : 1);

	// switches left & right images subsequently	
	/*if (left_turn == true) 
		drawImage(window, 0, MENU_HEIGHT, width, height, (unsigned char *)laddr);
	else
		drawImage(window, 0, MENU_HEIGHT, width, height, (unsigned char *)raddr);
	*/
	
	
	if (left) {
		drawImage(window, 0, MENU_HEIGHT, width, height, (unsigned char *)laddr);
	}
	if (right) {
		drawImage(window, width, MENU_HEIGHT, width, height, (unsigned char *)raddr);
	}
	if (middle) {
		drawImage(window, 0, MENU_HEIGHT, width, height, (unsigned char *)maddr);
	}
}

void GuiGenerator::DisplayLRImages()
{
	if ( ! isSingleWindow)
		drawImages(true, true, false);
	else 
		drawImages(true, false, false);
}

void GuiGenerator::DisplayLRImages(int Side)
{
	if (Side == 0 ) {
	    drawImages(true, false, false);
	} else if (Side == 1) {
	    drawImages(false, true, false);
	} else if (Side == 2) {
	    drawImages(false, false, true);
	} else {
		if ( ! isSingleWindow) {
			drawImages(true, true, false);
		} else {
			drawImages(true, false, false);
		}
	 }
}

void GuiGenerator::disableUI()
{
		markersList->deactivate();
		renameButton->deactivate();
		deleteButton->deactivate();
		tmplName->deactivate();
		moreFacetCheckbox->deactivate();
		sampleFacetButton->deactivate();
		stopSampleButton->activate();
		saveButton->deactivate();
		cancelButton->deactivate();

}
void GuiGenerator::enableUI()
{
		markersList->activate();
		renameButton->activate();
		deleteButton->activate();
		tmplName->activate();
		sampleFacetButton->activate();
		stopSampleButton->deactivate();
		saveButton->activate();
		cancelButton->activate();

		//moreFacetCheckbox->deactivate(); // not functioning
		moreFacetCheckbox->activate(); // now functioning

}
void GuiGenerator::SetEqualize(Fl_Menu_Item* m, GuiGenerator* data)
{
	data->pCameras->setHistogramEqualizeImages(! data->pCameras->getHistogramEqualizeImages());
	
}

void GuiGenerator::SetCurrCamera(Fl_Menu_Item* m, int* cbuffer)
{

	GuiGenerator *data= (GuiGenerator *)cbuffer[0];
	MCamera *cam = (MCamera *)cbuffer[1];
	data->pCurrCam =  cam;
	int width = data->pCurrCam->getXRes() * (data->isShowingHalfSize ? 0.5 : 1);
	int height = data->pCurrCam->getYRes() * (data->isShowingHalfSize ? 0.5 : 1);
	data->window->size(width, height+ MENU_HEIGHT);

}


void GuiGenerator::onAutoCheckClicked(Fl_Button* button, GuiGenerator* data)
{
	data->pPers->setSection ("General");
	if ( button->value() == 0) {	
		data->pPers->saveInt("AutoCool", 0 );
	} else {
		data->pPers->saveInt("AutoCool", 1 );
	}
}

void GuiGenerator::onSMCheckClicked(Fl_Button* button, GuiGenerator* data)
{
	int value = button->value();
	data->pPers->setSection("General");
	data->pPers->saveInt("DetectSmallMarkers", value);
	data->pMarkers->setSmallerXPFootprint(value == 1);
}

void GuiGenerator::activateOptions(Fl_Menu_Item* m, GuiGenerator* data)
{
	if ( data->isOptionsInitialized) {
		delete data->optionsWindow;
		//delete data->lightCoolSlider;
		//delete data->autoCoolCheckbox;
	}
	data->isOptionsInitialized = true;
	data->optionsWindow = new Fl_Window(0,0,MARKER_WIN_WIDTH, MARKER_WIN_HEIGHT, "Options");

	data->lightCoolSlider = new Fl_Value_Slider(50, 15, 30, 400, "Light Coolness");
	data->lightCoolSlider->callback((Fl_Callback*)onSliderChange, data);
	data->lightCoolSlider->minimum(-0.2);
	data->lightCoolSlider->maximum(1.2);

	data->extrapFramesCounter = new Fl_Counter(130, 15, 80, 30, "Extrapolated Frames");
	data->extrapFramesCounter->type(FL_SIMPLE_COUNTER);
	data->extrapFramesCounter->callback((Fl_Callback*)onEFCounterChange, data);
	data->extrapFramesCounter->minimum(0);
	data->extrapFramesCounter->maximum(10);
	data->extrapFramesCounter->precision(0);
	data->extrapFramesCounter->value(data->pMarkers->getExtrapolatedFrames());

	data->autoCoolCheckbox = new Fl_Check_Button(50, 450, 100, 20, "Automatic Light Coolness");
	data->autoCoolCheckbox->callback((Fl_Callback*)onAutoCheckClicked, data);

	data->smallMarkersCheckbox = new Fl_Check_Button(50, 490, 100, 20, "Detect Small Markers");
	data->smallMarkersCheckbox->callback((Fl_Callback*)onSMCheckClicked, data);
	data->smallMarkersCheckbox->value(data->pMarkers->getSmallerXPFootprint());

	data->pPers->setSection ("General");
	int s = (data->pPers->retrieveInt("AutoCool", 1) );
	bool autoCool = (s == 0? false:true);
	if (autoCool) {
		// set the checkbox
		data->autoCoolCheckbox->set();
	}
	data->lightCoolSlider->value( data->pCurrCam->getLightCoolness());
	
	data->optionsWindow->end();
	data->optionsWindow->show();
	Fl::run();
}

/****************************/
/** */
void GuiGenerator::activateMarkerTemplates(Fl_Menu_Item* m, GuiGenerator* data)
{
	if ( data->isMarkerTemplatesInitialized) {
		delete data->markersList;
		delete data->renameButton;
		delete data->deleteButton;
		delete data->tmplName;
		delete data->moreFacetCheckbox;
		delete data->sampleFacetButton;
		delete data->stopSampleButton;
		delete data->collectingStatus;
		delete data->saveButton;
		delete data->markersListBuffer;
		delete data->cancelButton;
		delete data->markersTmplsWindow;

	}
	data->isMarkerTemplatesInitialized = true;
	data->markersTmplsWindow = new Fl_Window(0,0,MARKER_WIN_WIDTH, MARKER_WIN_HEIGHT, "Markers Template");
	data->markersList = new Fl_Text_Display(10, 10, MARKER_WIN_WIDTH-20, 100);

	data->renameButton = new Fl_Button(15, 120, 80, 20, "Rename");
	data->renameButton->callback((Fl_Callback*)onRenameTemplateClicked, data);

	data->deleteButton = new Fl_Button(150, 120, 80, 20, "Delete");
	data->deleteButton->callback((Fl_Callback*)onDeleteTemplateClicked, data);

	data->tmplName = new Fl_Input(115,170,100,20, "New template");

	data->moreFacetCheckbox = new Fl_Check_Button(25, 200, 100, 20, " Additional Facet");
	data->moreFacetCheckbox->callback((Fl_Callback*)onAdditionalFacetClicked, data);
	
	data->sampleFacetButton = new Fl_Button(22, 230, 100, 20, "Start Sampling");
	data->sampleFacetButton->callback((Fl_Callback*)onSampleFacetClicked, data);
	//data->sampleFacetButton->type(FL_RADIO_BUTTON);
	data->stopSampleButton = new Fl_Button(122,230, 100, 20, "Stop Sampling");
	data->stopSampleButton->callback((Fl_Callback*)onStopSamplingClicked, data);
	//data->stopSampleButton->type(FL_RADIO_BUTTON);

	data->collectingStatus = new Fl_Box(25,260, 200, 20,  "" );
	data->collectingStatus->color(FL_RED);


//	data->collectingNum = new Fl_Box(150, 300, 50, 20, "Collected: ");

	data->saveButton = new Fl_Button(22, 350, 120, 20, "Accept and Save");
	data->saveButton->callback((Fl_Callback*)onSaveSampleClicked, data);

	data->cancelButton = new Fl_Button(150, MARKER_WIN_HEIGHT-50, 80, 20, "Cancel");
	data->cancelButton->callback((Fl_Callback*)onCancelClicked, data);

	data->markersListBuffer = new Fl_Text_Buffer;
	data->enableUI();
	data->markersTmplsWindow->end();
	data->markersTmplsWindow->show();
	data->refreshTemplateList();
	Fl::run();
}

/****************************/
/** */

void GuiGenerator::onCancelClicked(Fl_Button* button, GuiGenerator* data)
{
	data->isCollectingSamples = false;
	data->isAdditionalFacetAdding = false;
	data->collectingStatus->label("");
	data->tmplName->value("");
	data->pCurrMarker = NULL;
}

/****************************/
/** */

void GuiGenerator::onAdditionalFacetClicked(Fl_Button* button, GuiGenerator* data)
{
	int start;
	int end;
	char* templateName;
	if (data->moreFacetCheckbox->value() == 0)
	{
		data->tmplName->value("");
		data->isAdditionalFacetAdding = false;
		return;
	}

	{
		data->markersListBuffer->selection_position(&start, &end);
	
		// Check to see if any line is selected from the list box at all.
		if (data->markersListBuffer->selected() != 0)
		{
			data->isAdditionalFacetAdding = true;
			templateName = data->markersListBuffer->line_text(start);
			data->tmplName->value(templateName);
		}
		else
		{
			fl_message("Please select a template name from the list and click the checkbox again!");
			data->moreFacetCheckbox->value(0);
			data->isAdditionalFacetAdding = false;
		}

	}
}

/****************************/
/** */
void GuiGenerator::onStopSamplingClicked(Fl_Button* button, GuiGenerator* data)
{  
	if (!data->isCollectingSamples)
		return;
	data->isCollectingSamples = false;
	string errorsString;
	Facet* f = new Facet();
	vector<Vector*> vectorPair;

	if (f->setVectorsFromSample(data->sampleVectors, errorsString))
	{
		if (data->isAdditionalFacetAdding)
		{
			vector<Xform3D*> facet1ToNewFacetXfs;
			int i;
			if ( data->sampleVectors.size() <1) return;
			Xform3D* facet1ToNewFacetXf;
			for (i=0; i<data->sampleVectors.size(); i++)
			{
				vectorPair.clear();
				vectorPair.push_back(new Vector( data->sampleVectors[i]->itemI(0) ));
				vectorPair.push_back(new Vector( data->sampleVectors[i]->itemI(1) ));
				if (f->identify(data->pCurrCam, vectorPair, 1)) // Then the sample matches the template
				{
					// Compute the xform between the first marker facet and the new one
					facet1ToNewFacetXf = data->facet1ToCameraXfs[i]->concatenate(f->getFacet2CameraXf(data->pCurrCam)->inverse());
					facet1ToNewFacetXfs.push_back(facet1ToNewFacetXf);
				}
			}
			// Combine the transforms accumulated to a new one and save it with the facet in the marker
			facet1ToNewFacetXf = facet1ToNewFacetXfs[0];
			for (i=1; i<facet1ToNewFacetXfs.size(); i++)
				facet1ToNewFacetXf->inBetween(facet1ToNewFacetXfs[i], 1);// will result in equal contribution by all faces
			data->pCurrMarker->addTemplateFacet(f, facet1ToNewFacetXf);
		}
		else
		{
			data->pCurrMarker = new MTMarker::Marker();
			Xform3D* Xf = new Xform3D();
			data->pCurrMarker->setName( (char*)data->tmplName->value());
			int addingResult = data->pCurrMarker->addTemplateFacet(f, Xf);
			if(addingResult != mtOK)
				data->collectingStatus->label("Error occured creating the new marker!");
		}
		data->collectingStatus->label("Sampling completed!");
	}
	else
		data->collectingStatus->label("Errors occured during sampling!");
	data->enableUI();
}

/****************************/
/** */
void GuiGenerator::onSampleFacetClicked(Fl_Button* button, GuiGenerator* data)
{
//	if (data->tmplName->value() == NULL)
//	{
//		fl_message("Please enter the name of the new template in the textbox!");
//		return;
//	}
	data->isCollectingSamples = true;
	if (data->isAdditionalFacetAdding)
		data->pCurrMarker = data->findMarkerInSet((char*)data->tmplName->value());
	data->disableUI();
}

/****************************/
/** */
void GuiGenerator::onSaveSampleClicked(Fl_Button*, GuiGenerator* data)
{
	string complString;
//	if (!data->pCurrMarker->validateTemplate(data->pMarkers->getTemplateMatchToleranceMM(), complString))
//		data->collectingStatus->label("Save failed!");
	data->saveMarkerTemplate( data->pCurrMarker->getName() );
	data->m_collectedSampleFrames = 0;
}

/****************************/
/** */
void GuiGenerator::saveMarkerTemplate(string name)
{
	char currDir[255];
	mtUtils::getCurrPath(currDir);
#ifdef WIN32
	strcat(currDir, "\\Markers\\");
#else
	strcat(currDir, "/Markers/");
#endif
	strcat(currDir, name.c_str());

	Persistence* newPersistence = new Persistence();
	newPersistence->setPath(currDir);

	int storeResult = this->pCurrMarker->storeTemplate(newPersistence, "");//(char*)(name.c_str()) );
	if (storeResult != mtOK)
		this->collectingStatus->label("Error in saving the current template");
	else
		this->collectingStatus->label("Template saved!");

	// Refresh the templates and the GUI list
	this->refreshMarkerTemplates();
	// this->refreshTemplateList();
	this->recreateTemplateList(); // force recreation
}


	
void GuiGenerator::onDeleteTemplateClicked(Fl_Button* button, GuiGenerator* data)
{	
	int start;
	int end;
	char* Name;
	data->markersListBuffer->selection_position(&start, &end);
	
	// Check to see if any line is selected from the list box at all.
	if (data->markersListBuffer->selected() != 0)
		Name = data->markersListBuffer->line_text(start);
	else
	{
		fl_message("Please select a file name from the list!");
		return;
	}
	if (Name != NULL) {
		data->deleteTemplate(Name);
	}
}


void GuiGenerator::onSliderChange(Fl_Value_Slider* slider, GuiGenerator* data)
{	
	data->pPers->setSection ("General");
	int s = (data->pPers->retrieveInt("AutoCool", 1) );
	bool autoCool = (s == 0? false:true);
	if (!autoCool) {
	data->lightCoolness = slider->value();
		data->pCurrCam->setLightCoolness(slider->value());
	data->pPers->saveDouble("LightCoolness", data->lightCoolness );
	}

}

void GuiGenerator::onEFCounterChange(Fl_Counter* counter, GuiGenerator* data)
{
	int numframes = counter->value();
	data->pMarkers->setExtrapolatedFrames(numframes);
	data->pPers->setSection("General");
	data->pPers->saveInt("ExtrapolatedFrames", data->pMarkers->getExtrapolatedFrames());
	counter->value(data->pMarkers->getExtrapolatedFrames()); //could have been clipped
}

/****************************/
/** */
void GuiGenerator::onRenameTemplateClicked(Fl_Button* button, GuiGenerator* data)
{	
	int start;
	int end;
	char* oldName;
	data->markersListBuffer->selection_position(&start, &end);
	
	// Check to see if any line is selected from the list box at all.
	if (data->markersListBuffer->selected() != 0)
		oldName = data->markersListBuffer->line_text(start);
	else
	{
		fl_message("Please select a file name from the list!");
		return;
	}

	
	const char* newNameInput = fl_input("Please enter the new name:", oldName );
	// If the user presses cancel the returned value if NULL
	if (newNameInput != NULL )
	{
		if (newNameInput != "") 
			data->changeTemplateName(data->markersListBuffer->count_lines(0,end), oldName, newNameInput);
	}
}

void GuiGenerator::deleteTemplate(string Name)
{
//	if (!this->isValid(Name)) // Check to see if the file exists
//	{
//		return;
//	}

	// delete the specified template file  
	char currPath[255];
	mtUtils::getCurrPath(currPath);
#ifdef WIN32
	strcat(currPath, "\\Markers\\");
#else
	strcat(currPath, "/Markers/");
#endif
	strcat(currPath, Name.c_str());

	int result = remove(currPath); 
	if (result != 0) {
		fl_message("The deletion was not successful! Try again.");
		return;
	}

	this->refreshMarkerTemplates();
	// this->refreshTemplateList();
	this->recreateTemplateList(); // force recreation
}
/****************************/
/** */
void GuiGenerator::changeTemplateName(int index, string oldName, string newName)
{
//	if (!this->isValid(newName)) // Check to see if this name already exists
//	{
//		fl_message("The name you entered already exists. Please select another name.");
//		return;
//	}

	char currDir[255];
	mtUtils::getCurrPath(currDir);
	char currDir2[255];
	mtUtils::getCurrPath(currDir2);

#ifdef WIN32
	strcat(currDir, "\\Markers\\");
#else
	strcat(currDir, "/Markers/");
#endif
	strcat(currDir, oldName.c_str());

#ifdef WIN32
	strcat(currDir2, "\\Markers\\");
#else
	strcat(currDir2, "/Markers/");
#endif
	strcat(currDir2, newName.c_str());


	// Change the file name in the markers 
	Persistence persistForLoadedMarker;

	// We have to save it back in the template file
	// This approach is trouble! We should hide the handles and keep arrays/list 
	// of C++ object. The higher level should not be aware of handles etc.

	persistForLoadedMarker.setPath( (char*)(currDir) );
	this->pMarkers->setTemplateItemName(index, (char*)(newName.c_str()) );
	this->pMarkers->storeTemplate(index, persistForLoadedMarker.getHandle(),NULL );

	int result = rename(currDir, currDir2); 
	if (result != 0) {// if name change is not succeeded
		fl_message("The renaming was not successful! Try again.");
		return;
	}

	this->refreshMarkerTemplates();
	// this->refreshTemplateList();
	this->recreateTemplateList(); // force recreation
}

/****************************/
/** */
void GuiGenerator::refreshTemplateList()
{

	markersListBuffer->text("");

	for (int i=1; i<= this->pMarkers->getTemplateCount(); i++)
	{
		std::string currName;
		this->pMarkers->getTemplateItemName(i, currName);
		markersListBuffer->append( currName.c_str() );
		markersListBuffer->append("\n");
	}
	markersList->buffer(markersListBuffer);

}

// the refresh of the markers list does not work properly.
// So we force destruction and recreation of the marker window.

void GuiGenerator::recreateTemplateList()
{
	markersTmplsWindow->hide();
	activateMarkerTemplates(NULL, this);

}
