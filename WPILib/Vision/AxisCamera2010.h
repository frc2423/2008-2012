/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#ifndef AXIS_CAMERA_
#define AXIS_CAMERA_

#include <taskLib.h>
#include "nivision.h"
#include "vxWorks.h" 
#include "sockLib.h" 
#include "inetLib.h"
#include "ColorImage.h"

#include "ErrorBase.h"
#include "Task.h"

typedef enum ExposureT {automatic, hold, flickerfree50, flickerfree60};
typedef enum WhiteBalanceT {automaticwb, holdwb, fixed_outdoor1, fixed_outdoor2, fixed_indoor, fixed_fluor1, fixed_fluor2};
typedef enum ResolutionT { k640x480, k640x360, k320x240, k160x120};
typedef enum RotationT {k0, k180};


/** port for sending video to laptop */
#define VIDEO_TO_PC_PORT 1180

class AxisCamera: public ErrorBase {
public:
	void setNewImageSem(SEM_ID);
	static AxisCamera& getInstance();
	static bool isInstance(void);

	int GetImage(Image* image);
	ColorImage * GetImage();
	
	int GetJpegImageBlocking(char** imageData, int* numBytes);
	
	void deleteInstance();
	~AxisCamera();
	
	bool freshImage();
	
	// Mid-stream gets & writes
	void writeBrightness(int);
	int getBrightness();
	void writeWhiteBalance(WhiteBalanceT whiteBalance);
	WhiteBalanceT getWhiteBalance();
	void writeColorLevel(int);
	int getColorLevel();
	void writeExposureControl(ExposureT);
	ExposureT getExposureControl();
	void writeExposurePriority(int);
	int getExposurePriority();
	
	// New-Stream gets & writes
	void writeResolution(ResolutionT);
	ResolutionT getResolution();
	void writeCompression(int);
	int getCompression();
	void writeRotation(RotationT);
	RotationT getRotation();
	
	int taskID;
	
private:
	AxisCamera();
	AxisCamera(AxisCamera const&);
	AxisCamera& operator=(AxisCamera const&);
	
	void updateCamParam(const char*);
	void readCamParams();
	
	static int s_mjpegStream(char* cameraIP="192.168.0.90");
	int mjpegStream(char* cameraIP="192.168.0.90");
	void updateImage(char * imgBuffer, int bufLength);
	
	static AxisCamera* m_instance;
	static int cameraSocket;
	SEM_ID m_newImageSem;
	
	// Don't decode until user requests, so hold image as a char array
	char* m_protectedImageBuffer;
	int m_protectedImageBufferLength;
	static SEM_ID m_protectedImageSem;
	
	Task camTask;
	
	void searchForParam(char* pattern, char const* searchString, int searchStringLen, char *result);
	
	//Camera Properties
	RotationT m_rotation;
	int m_compression;
	ResolutionT m_resolution;
	int m_exposurePriority;
	ExposureT m_exposureControl;
	int m_colorLevel;
	WhiteBalanceT m_whiteBalance;
	int m_brightness;
	
	bool m_freshImage;
	bool m_freshBlockingImage;
	
};

extern "C" {
	void AxisCameraStart( void );
	int AxisCameraGetImage( Image* image );
	void AxisCameraDeleteInstance();
	int AxisCameraFreshImage();
	int AxisCameraGetJpegImageBlocking(char** imageData, int* numBytes);
	
	// Mid-stream gets & writes
	void AxisCameraWriteBrightness(int);
	int AxisCameraGetBrightness();
	void AxisCameraWriteWhiteBalance(WhiteBalanceT whiteBalance);
	WhiteBalanceT AxisCameraGetWhiteBalance();
	void AxisCameraWriteColorLevel(int);
	int AxisCameraGetColorLevel();
	void AxisCameraWriteExposureControl(ExposureT);
	ExposureT AxisCameraGetExposureControl();
	void AxisCameraWriteExposurePriority(int);
	int AxisCameraGetExposurePriority();
	
	// New-Stream gets & writes
	void AxisCameraWriteResolution(ResolutionT);
	ResolutionT AxisCameraGetResolution();
	void AxisCameraWriteCompression(int);
	int AxisCameraGetCompression();
	void AxisCameraWriteRotation(RotationT);
	RotationT AxisCameraGetRotation();
}


#endif
