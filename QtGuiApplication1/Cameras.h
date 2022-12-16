/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Shi Sherebrin , Robarts Research Institute - London- Ontario , www.robarts.ca
*			Shahram Izadyar, Robarts Research Institute - London- Ontario , www.robarts.ca
*			Claudio Gatti, Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2013
*
***************************************************************/


#ifndef __CAMERAS_H__
#define __CAMERAS_H__
#include <vector>
#include "MCamera.h"

//#define MaxCameras 5

using namespace std;

class Cameras
{
public:
	Cameras();
	~Cameras();
	inline mtHandle getHandle() {return m_handle;};
	inline int	getCount() {return m_attachedCamNums;};
	int setHistogramEqualizeImages(bool on_off);
	bool getHistogramEqualizeImages();
	MCamera* getCamera(int index);


	int getShutterPreference();
	int setShutterPreference(int val);
	int getGainPreference();
	int setGainPreference(int val);

	int AttachAvailableCameras();
	void Detach();

	inline void setMarkersHandle(mtHandle markersHandle){m_markersHandle = markersHandle;};

	bool grabFrame(MCamera *cam = NULL); // returns true for success

	// need to lear how to hide it, and keep same access syntax
	vector<MCamera *> m_vCameras;


private:
	int getMTHome( char *, int);
	mtHandle m_handle;
	bool ownedByMe;
	MCamera* mCurrCam;
	MCamera*	mFailedCam;


	int m_attachedCamNums;
	mtHandle m_markersHandle;

};

#endif
