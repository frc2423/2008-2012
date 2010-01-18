/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008. All Rights Reserved.							  */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in $(WIND_BASE)/WPILib.  */
/*----------------------------------------------------------------------------*/

#include <string.h>

#include "AxisCamera2010.h"
#include "VisionAPI.h"
#include "pcre.h"
#include "PCVideoServer.h"

#define DEFAULT_PACKET_SIZE 1536
static const int READ_PARAM_LENGTH = 65;

AxisCamera* AxisCamera::m_instance= NULL;
SEM_ID AxisCamera::m_protectedImageSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE | SEM_DELETE_SAFE);
int AxisCamera::cameraSocket = 0;

/**
 * Get an image from the camera and store it in the provided image.
 * @param image the image to store the result in. This must be an HSL or RGB image
 * @return 1 upon success, zero on a failure
 */
int AxisCamera::GetImage(Image* image) {
	if (!m_protectedImageBuffer)
		return 0;
	semTake(m_protectedImageSem, -1);
	{
		Priv_ReadJPEGString_C(image,(unsigned char*)m_protectedImageBuffer,m_protectedImageBufferLength);
		m_freshImage = false;
	}
	semGive(m_protectedImageSem);
	return 1;
}

/**
 * Get an image from the camera and store it in the provided image.
 * @return a pointer to a ColorImage object
 */
ColorImage * AxisCamera::GetImage() {
	ColorImage *colorImage = new ColorImage(IMAQ_IMAGE_HSL);
	if (!m_protectedImageBuffer)
		return NULL;
	semTake(m_protectedImageSem, -1);
	{
		Priv_ReadJPEGString_C(colorImage->image,(unsigned char*)m_protectedImageBuffer,m_protectedImageBufferLength);
		m_freshImage = false;
	}
	semGive(m_protectedImageSem);
	return colorImage;
}


int AxisCamera::GetJpegImageBlocking(char** imageData, int* numBytes) {
	int success = 1;
	while (1) {
		semTake(m_protectedImageSem, -1);
		{
			if (!m_protectedImageBuffer)
				success = 0;
			char* newImageData = new char[m_protectedImageBufferLength];
			if (newImageData == NULL)
				success = 0;
	
			if (success) {
				memcpy(newImageData, m_protectedImageBuffer, m_protectedImageBufferLength);
				*imageData = newImageData;
				*numBytes = m_protectedImageBufferLength;
				if (m_freshBlockingImage) {
					semGive(m_protectedImageSem);
					return 1;
				}
				m_freshBlockingImage = false;
			}
			delete newImageData;
		}
		semGive(m_protectedImageSem);
		if (!success)
			return 0;
	}
}

/**
 * Return true if the latest image from the camera has not been retrieved yet.
 * @return true if the image has not been retrieved yet.
 */
bool AxisCamera::freshImage() {
	return m_freshImage;
}

/**
 * Static interface that will cause an instantiation if necessary.
 */
int AxisCamera::s_mjpegStream(char* cameraIP) {
	return AxisCamera::getInstance().mjpegStream(cameraIP);
}

/**
 * Task spawned by AxisCamera constructor to receive images from cam
 * If setNewImageSem has been called, this function does a semGive on each new image
 * Images can be accessed by calling getImage()
 */
int AxisCamera::mjpegStream(char* cameraIP) {
	if(NULL == cameraIP)
		cameraIP = "192.168.0.90";
	int sockAddrSize;
	struct sockaddr_in serverAddr;
	char *imgBuffer = NULL;
	int imgBufferLength = 0;
	/* create socket */
	if ((cameraSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
		return (ERROR);
	}

	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(80);

	if ((serverAddr.sin_addr.s_addr = inet_addr(cameraIP)) == (u_long)ERROR) {
		perror("invalid IP");
		close(cameraSocket);
		return (ERROR);
	}

	/* connect to server */
	if (connect(cameraSocket, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) {
		perror("connect");
		close(cameraSocket);
		return (ERROR);
	}
	/*send mjpeg */
	char * requestString = "GET /mjpg/video.mjpg HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";
	// Send request
	int sent = send(cameraSocket, requestString, strlen(requestString), 0);
	if (sent == ERROR) {
		perror("send");
		close(cameraSocket);
		return(ERROR);
	}
	int counter = 2;
	//Infinite loop, task deletion handled by taskDeleteHook
	// Socket cleanup handled by destructor
	while (1) {
		char initialReadBuffer[DEFAULT_PACKET_SIZE] = "";
		char intermediateBuffer[1];
		char *trailingPtr = initialReadBuffer;
		int trailingCounter = 0;
		int i = 0;
		/* Find content-length, then read that many bytes */
		while (counter) {
			if(recv(cameraSocket, intermediateBuffer,1,0) == ERROR){
				perror ("Failed to read image header");
				close (cameraSocket);
				return (ERROR);
			}
			strncat(initialReadBuffer, intermediateBuffer, 1);
			if (NULL != strstr(trailingPtr, "\r\n\r\n")) {
				--counter;
			}
			if (++trailingCounter >= 4) {
				trailingPtr++;
			}
			i++;
			if(i == 1500){
			}
		}
		counter = 1;
		char *contentLength = strstr(initialReadBuffer, "Content-Length: ");
		if (contentLength == NULL) {
			perror("No content-length token found in packet");
			close(cameraSocket);
			return(ERROR);
		}
		contentLength = contentLength + 16;

		int readLength = atol(contentLength);
		if (imgBufferLength < readLength)
		{
           delete[] imgBuffer;
           imgBufferLength = readLength + 0x100;
           imgBuffer = new char[imgBufferLength];
		}
		int bytesRead = 0;
		int remaining = readLength;
		while(bytesRead < readLength){
			 int bytesThisRecv= recv(cameraSocket, &imgBuffer[bytesRead], remaining, 0);
			 bytesRead += bytesThisRecv;
			 remaining -= bytesThisRecv;
		}
		//update image
		getInstance().updateImage(imgBuffer, readLength);
	}
}

void AxisCamera::updateImage(char * imgBuffer, int bufLength) {
	semTake(m_protectedImageSem, -1);
	{
		if (NULL != m_protectedImageBuffer) {
			delete[] m_protectedImageBuffer;
		}
		m_protectedImageBufferLength = bufLength;
		m_protectedImageBuffer = new char[bufLength];
		memcpy(m_protectedImageBuffer, imgBuffer, bufLength);
		m_freshImage = true;
		m_freshBlockingImage = true;
	}
	semGive(m_protectedImageSem);
}

/**
 * Get a pointer to the AxisCamera object, if the object does not exist, create it
 * @return reference to AxisCamera object
 */
AxisCamera& AxisCamera::getInstance() {
	if (NULL == m_instance) {
		m_instance = new AxisCamera();
		m_instance->readCamParams();
		m_instance->camTask.Start();
	}
	return *m_instance;
}

/**
 * Does the global instance exist?
 */
bool AxisCamera::isInstance(void) {
	return m_instance != NULL;
}

void AxisCamera::deleteInstance() {
	delete m_instance;
}

/**
 * Set the semaphore to be used to synchronize image access with camera acquisition
 * @param semaphore to notify when new image is received
 */
void AxisCamera::setNewImageSem(SEM_ID pImageSem) {
	m_newImageSem = pImageSem;
}

AxisCamera::AxisCamera() :
	m_protectedImageBuffer(NULL), 
	camTask("cameraTask", (FUNCPTR)s_mjpegStream)
	{
		m_protectedImageBufferLength = 0;
		new PCVideoServer();
	}

AxisCamera::~AxisCamera() {
	camTask.Stop();
	close(cameraSocket);

	semDelete(m_protectedImageSem);
	semDelete(m_newImageSem);
	m_instance = NULL;
}

/**
 * @param valid values 0 .. 100
 */
void AxisCamera::writeBrightness(int brightness) {
	char param[15];
	char* baseString = "Source.I0.Sensor.Brightness=%i";
	sprintf(param, baseString, brightness);
	updateCamParam(param);
}

int AxisCamera::getBrightness() {
	return m_brightness;
}

/**
 * @param valid values automatic, hold, fixed_outdoor1, fixed_outdoor2, fixed_indoor,fixed_flour1, fixed_flour2
 */
void AxisCamera::writeWhiteBalance(WhiteBalanceT whiteBalance) {
	static const char* paramStrings[] = {"automatic", "hold", "fixed_outdoor1", "fixed_outdoor2", "fixed_indoor", "fixed_fluor1", "fixed_fluor2"};
	char param[50];
	char* baseString = "Source.I0.Sensor.WhiteBalance=%i";
	sprintf(param, baseString, paramStrings[whiteBalance]);
	updateCamParam(param);
}

WhiteBalanceT AxisCamera::getWhiteBalance() {
	return m_whiteBalance;
}

/**
 * @param valid values are 0 .. 100
 */
void AxisCamera::writeColorLevel(int colorLevel) {
	char param[50];
	char* baseString = "Source.I0.Sensor.ColorLevel=%i";
	sprintf(param, baseString, colorLevel);
	updateCamParam(param);
}

int AxisCamera::getColorLevel() {
	return m_colorLevel;
}

/**
 * @param valid values are automatic, hold, flickerfree50, flickerfree60
 */
void AxisCamera::writeExposureControl(ExposureT exposureControl) {
	static const char* paramStrings[] = {"automatic", "hold", "flickerfree50", "flickerfree60"};
	char param[50];
	char* baseString = "Source.I0.Sensor.Exposure=%i";
	sprintf(param, baseString, paramStrings[exposureControl]);
	updateCamParam(param);
}

ExposureT AxisCamera::getExposureControl() {
	return m_exposureControl;
}

/**
 * @param Valid values are 0, 50, 100.
 * 0 = Prioritize image quality
 * 50 = None
 * 100 = Prioritize frame rate
 */
void AxisCamera::writeExposurePriority(int exposurePriority) {
	char param[50];
	char* baseString = "Source.I0.Sensor.exposurePriority=%i";
	sprintf(param, baseString, exposurePriority);
	updateCamParam(param);
}

int AxisCamera::getExposurePriority() {
	return m_exposurePriority;
}

ResolutionT AxisCamera::getResolution() {
	return m_resolution;
}

int AxisCamera::getCompression() {
	return m_compression;
}

RotationT AxisCamera::getRotation() {
	return m_rotation;
}

/**
 * @param valid values defined in ResolutionT enum
 */
void AxisCamera::writeResolution(ResolutionT resolution){
	static const char* paramStrings[] = {"640x480", "640x360", "320x240", "160x120"};
	char param[50];
	char* baseString = ".I0.Appearance.Resolution=%s";
	sprintf(param, baseString, paramStrings[resolution]);
	updateCamParam(param);
	camTask.Stop();
	camTask.Start();	
}

void AxisCamera::writeCompression(int compression){
	char param[50];
	char* baseString = ".I0.Appearance.Compression=%i";
	sprintf(param, baseString, compression);
	updateCamParam(param);
	camTask.Stop();
	camTask.Start();
}

/**
 * @param k0 or k180
 */
void AxisCamera::writeRotation(RotationT rotation){
	static const char* paramStrings[] = {"0", "180"};
	char param[50];
	char* baseString = ".I0.Appearance.Rotation=%s";
	sprintf(param, baseString, paramStrings[rotation]);
	updateCamParam(param);
	camTask.Stop();
	camTask.Start();
}

	
	
	//TODO errors
void AxisCamera::updateCamParam(const char* param) {
	char* cameraIP = "192.168.0.90";
	int sockAddrSize;
	struct sockaddr_in serverAddr;
	int camSocket;
	/* create socket */
	if ((camSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
	}

	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(80);

	if ((serverAddr.sin_addr.s_addr = inet_addr(cameraIP)) == (u_long)ERROR) {
		perror("invalid IP");
		close(camSocket);
	}

	/* connect to server */
	if (connect(camSocket, (struct sockaddr *) &serverAddr, sockAddrSize)
			== ERROR) {
		perror("connect");
		close(camSocket);
	}
	/*send update */
	char
			* requestString =
					"GET /axis-cgi/admin/param.cgi?action=update&Image%s HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";
	char completedRequest[1024];
	sprintf(completedRequest, requestString, param);
	// Send request
	int sent = send(camSocket, completedRequest, strlen(completedRequest), 0);
	if (sent == ERROR) {
		perror("send");
	}
	close(camSocket);
}

/**
 * Read the full param list from camera, use regular expressions to find the bits we care about
 * assign values to member variables.
 */
void AxisCamera::readCamParams() {
	char* cameraIP = "192.168.0.90";
	int sockAddrSize;
	struct sockaddr_in serverAddr;
	int camSocket;
	/* create socket */
	if ((camSocket = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		perror("socket");
	}

	sockAddrSize = sizeof(struct sockaddr_in);
	bzero((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_port = htons(80);

	if ((serverAddr.sin_addr.s_addr = inet_addr(cameraIP)) == (u_long)ERROR) {
		perror("invalid IP");
		close(camSocket);
	}

	/* connect to server */
	if (connect(camSocket, (struct sockaddr *) &serverAddr, sockAddrSize)
			== ERROR) {
		perror("connect");
		close(camSocket);
	}
	/*send update */
	char
			* requestString =
					"GET /axis-cgi/admin/param.cgi?action=list HTTP/1.1\n\
User-Agent: HTTPStreamClient\n\
Connection: Keep-Alive\n\
Cache-Control: no-cache\n\
Authorization: Basic RlJDOkZSQw==\n\n";
	// Send request
	int sent = send(camSocket, requestString, strlen(requestString), 0);
	if (sent == ERROR) {
		perror("send");
		close(camSocket);
	}
	//Read responses
	char readBuffer[7000];
	int totalRead = 0;
	int bytesRead = recv(camSocket, &readBuffer[totalRead], 1000, 0);
	while (bytesRead> 0) {
		totalRead += bytesRead;
		if (bytesRead == ERROR) {
			perror("Failed to read image header");
			close(camSocket);
		}
		bytesRead = recv(camSocket, &readBuffer[totalRead-1], 1000, 0);
	}
	readBuffer[totalRead--] = '\0';

	char resultString[50];
	searchForParam("root.ImageSource.I0.Sensor.ColorLevel=(.*)",
			readBuffer,
			totalRead,
			resultString);
	m_colorLevel = atoi(resultString);
	
	searchForParam("root.Image.I0.Appearance.Compression=(.*)",
				readBuffer,
				totalRead,
				resultString);
	m_compression = atoi(resultString);
	
	searchForParam("root.ImageSource.I0.Sensor.ExposurePriority=(.*)",
					readBuffer,
					totalRead,
					resultString);
	m_exposurePriority = atoi(resultString);
	
	searchForParam("root.ImageSource.I0.Sensor.Brightness=(.*)",
					readBuffer,
					totalRead,
					resultString);
	m_brightness = atoi(resultString);
	
	searchForParam("root.Image.I0.Appearance.Resolution=(.*)", readBuffer,
			totalRead, resultString);
	const char *resStrings[] = { "640x480", "640x360", "320x240", "160x120" };
	for (int i=0; i<4; i++) {
		if (strcmp(resultString, resStrings[i]) == 0)
			m_resolution = (ResolutionT) i;
	}

	searchForParam("root.ImageSource.I0.Sensor.WhiteBalance=(.*)", readBuffer,
			totalRead, resultString);
	const char *wbStrings[] = { "auto", "holdwb", "fixed_outdoor1",
			"fixed_outdoor2", "fixed_indoor", "fixed_fluor1", "fixed_fluor2" };
	for (int i=0; i<7; i++) {
		if (strcmp(resultString, wbStrings[i]) == 0)
			m_whiteBalance = (WhiteBalanceT) i;
	}

	searchForParam("root.ImageSource.I0.Sensor.Exposure=(.*)", readBuffer,
			totalRead, resultString);
	const char *expConStrings[] = { "automatic", "hold", "flickerfree50",
			"flickerfree60" };
	for (int i=0; i<4; i++) {
		if (strcmp(resultString, expConStrings[i]) == 0)
			m_exposureControl = (ExposureT) i;
	}
	
	searchForParam("root.Image.I0.Appearance.Rotation=(.*)", readBuffer,
			totalRead, resultString);
	const char *rotStrings[] = { "0", "180"};
	for (int i=0; i<2; i++) {
		if (strcmp(resultString, rotStrings[i]) == 0)
			m_rotation = (RotationT) i;
	}
}

/**
 * @param pattern: the regular expression
 * @param searchString the text to search
 * @param searchStringLen the length of searchString
 * @param result buffer to put resulting text into, must be pre-allocated
 */
void AxisCamera::searchForParam(char* pattern, char const* searchString, int searchStringLen, char *result) {
	int vectorLen = 10;
	int resultVector[vectorLen];
	const char *error;
	int erroffset;
	pcre *compiledPattern = pcre_compile(
			pattern, //"root.Image.I0.Appearance.Resolution=(.*)", 
			PCRE_CASELESS, 
			&error, // for error message 
			&erroffset, // for error offset 
			NULL); // use default character tables 
	int rc;
	rc = pcre_exec(compiledPattern, 
					NULL, 
					searchString, 
					searchStringLen, 
					0, 
					0, 
					resultVector,		//locations of submatches 
					vectorLen);			//size of ovector
	int length = resultVector[3] - resultVector[2];
	memcpy(result, &searchString[resultVector[2]], length);
	result[length] = '\0';
}

void AxisCameraStart() {
	AxisCamera::getInstance();
}

int AxisCameraGetImage (Image* image) {
	return AxisCamera::getInstance().GetImage(image);
}

void AxisCameraWriteBrightness(int brightness) {
	AxisCamera::getInstance().writeBrightness(brightness);
}

int AxisCameraGetBrightness() {
	return AxisCamera::getInstance().getBrightness();
}

void AxisCameraWriteWhiteBalance(WhiteBalanceT whiteBalance) {
	AxisCamera::getInstance().writeWhiteBalance(whiteBalance);
}

WhiteBalanceT AxisCameraGetWhiteBalance() {
	return AxisCamera::getInstance().getWhiteBalance();
}

void AxisCameraWriteColorLevel(int colorLevel)  {
	AxisCamera::getInstance().writeColorLevel(colorLevel);
}

int AxisCameraGetColorLevel() {
	return AxisCamera::getInstance().getColorLevel();
}

void AxisCameraWriteExposureControl(ExposureT exposure) {
	AxisCamera::getInstance().writeExposureControl(exposure);
}

ExposureT AxisCameraGetExposureControl() {
	return AxisCamera::getInstance().getExposureControl();
}

void AxisCameraWriteExposurePriority(int exposure) {
	AxisCamera::getInstance().writeExposurePriority(exposure);
}

int AxisCameraGetExposurePriority() {
	return AxisCamera::getInstance().getExposurePriority();
}

void AxisCameraWriteResolution(ResolutionT resolution) {
	AxisCamera::getInstance().writeResolution(resolution);
}

ResolutionT AxisCameraGetResolution() {
	return AxisCamera::getInstance().getResolution();
}

void AxisCameraWriteCompression(int compression) {
	AxisCamera::getInstance().writeCompression(compression);
}

int AxisCameraGetCompression() {
	return AxisCamera::getInstance().getCompression();
}

void AxisCameraWriteRotation(RotationT rotation) {
	AxisCamera::getInstance().writeRotation(rotation);
}

RotationT AxisCameraGetRotation() {
	return AxisCamera::getInstance().getRotation();
}

void AxisCameraDeleteInstance() {
	AxisCamera::getInstance().deleteInstance();
}

int AxisCameraFreshImage() {
	return AxisCamera::getInstance().freshImage();
}

int AxisCameraGetJpegImageBlocking(char** imageData, int* numBytes) {
	return AxisCamera::getInstance().GetJpegImageBlocking(imageData, numBytes);
}
