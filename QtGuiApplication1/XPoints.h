/**************************************************************
*
*     Micron Tracker: Example C++ wrapper and Multi-platform demo
*   
*     Written by: 
*			Ahmad Kolahi, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*	  Modified by:
*			Claudio Gatti, Claron Technology - Toronto -Ontario, www.clarontech.com
*
*     Copyright Claron Technology 2000-2012
*
***************************************************************/
#ifndef __XPOINTS_H__
#define __XPOINTS_H__

#include "MTC.h"
#include "UtilityFunctions.h"
#include "Collection.h"
#include "MTXPoint.h"
#include "MCamera.h"

#include <string>

class XPoints
{
public:
	XPoints();
	~XPoints();
	inline mtHandle getHandle(){ return m_handle; };

	mtHandle detectedXPoints(MCamera *cam);
	int countXPoints(MCamera *cam);
	int processFrame(MCamera *cam);

private:
	mtHandle m_handle;
	bool ownedByMe;

};

#endif
