#pragma once

#include <Windows.h>
#include <Vfw.h>
#include <string>
#include <functional>
#include "impressionistUI.h"
#include "bitmap.h"

class Fl_Widget;

/**
 * Class done with help from: https://www.codeproject.com/Articles/8739/Extracting-AVI-Frames
 */
class VideoProcessor
{
public:
	/**
	 * Retrieves a VideoProcessor singleton instance.
	 * @returns a VideoProcessor singleton instance.
	 */
	static VideoProcessor* getInstancePtr();

	/**
	 * Destroys the singleton instance.
	 */
	static void destroy();

	/**
	 * Initialize the user interaction of opening the dialog and prepare to read the video.
	 */
	void openVideoFromUser();

	/**
	 * Opens a video file in read-only mode.
	 * @param filename the path to read the file.
	 * @returns if the file was successfully opened.
	 */
	bool openFile(const std::string & filename);

	/**
	 * Retrieves a stream pointer to read the opened file.
	 */
	bool startStream();

	/**
	 * Returns whether the previous operation has generated an error.
	 * @returns if the previous operation has generated an error.
	 */
	bool hasError() const;

	/**
	 * Sets the full image auto-run manipulation function that modifies the input image to a stylized output image.
	 * @param callbackFunction The image manipulation function.
	 */
	void setManipulationMethod(const std::function<void(void*)>& callbackFunction);

	/**
	 * Initialize the process of reading the video: processing frame by frame and saves the results to a local buffer.
	 */
	void process() const;

	/**
	 * Continues with the processing when one image is done.
	 */
	void next();

	/**
	 * Returns whether the file is read completely
	 */
	bool isEnded() const;

	/**
	 * Close all opened file and stream.
	 */
	void close();

	/**
	 * Information regarding the opened AVI.
	 */
	AVIFILEINFO aviInfo{};

	/**
	 * AVI buffer pointer from the opened file.
	 */
	PAVIFILE aviReadPtr = nullptr;

	/**
	 * AVI stream pointer within the opened file.
	 */
	PAVISTREAM aviReadStreamPtr = nullptr;

	/**
	 * AVI buffer pointer from the opened file.
	 */
	PAVIFILE aviPtr = nullptr;

	/**
	 * AVI stream pointer within the opened file.
	 */
	PAVISTREAM aviStreamPtr = nullptr;
	/**
	 * AVI stream pointer within the opened file.
	 */
	PAVISTREAM aviCStreamPtr = nullptr;

	bool hasSetFormat = false;

	/**
	 * Reading position of the stream
	 */
	long int frameIndex = 0L;

	static std::function<void(void*)> methodAutoFill;
	static std::function<void(void*)> methodPaintly;

	static Fl_Callback cbVideoAutoFill;
	static Fl_Callback cbVideoPaintly;

	static void cbPreparation(Fl_Widget* o);

	static ImpressionistUI* uiPtr;
	static ImpressionistDoc* docPtr;

	static void continueWriteStream();

	~VideoProcessor();

private:
	explicit VideoProcessor();

	static VideoProcessor* singletonPtr;

	BITMAPINFOHEADER bmpInfo{};
	PADDING padding;
	unsigned long byteLength;
	std::function<void(void*)> perImageFunction = [&](void*){};
	HRESULT errorCode = 0;
	long int streamStartingIndex = 0;
	long int totalFrames = 0;
	PGETFRAME getFramePtr = nullptr;
	BYTE* bitmapDibPtr = nullptr;
	unsigned char* imageDataPtr = nullptr;

	void saveImage();
};

