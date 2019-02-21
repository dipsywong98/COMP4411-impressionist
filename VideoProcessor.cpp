#include "VideoProcessor.h"
#include "impressionistUI.h"
#include "impressionistDoc.h"
#include "bitmap.h"

VideoProcessor* VideoProcessor::singletonPtr = nullptr;
ImpressionistUI* VideoProcessor::uiPtr = nullptr;
ImpressionistDoc* VideoProcessor::docPtr = nullptr;

std::function<void(void*)> VideoProcessor::methodAutoFill = [](void*)->void
{
	docPtr->autoFill();
};

std::function<void(void*)> VideoProcessor::methodPaintly = [](void*)->void
{
	
};

VideoProcessor::VideoProcessor()
{
	AVIFileInit();  // Must be called to use AVI library
}

void VideoProcessor::openVideoFromUser()
{
	auto* path = fl_file_chooser("Open AVI", "*.avi", "");

	if (path == nullptr)
	{
		fl_alert("Path is not chosen!");
		return;
	}

	openFile(path);
	if (errorCode)
	{
		fl_alert("Error opening file.");
		return;
	}

	startStream();
	if (errorCode) return;

	next();
}

void VideoProcessor::cbVideoAutoFill(Fl_Widget* o, void* v)
{
	getInstancePtr()->setManipulationMethod(methodAutoFill); 
	cbPreparation(o);
}

void VideoProcessor::cbVideoPaintly(Fl_Widget* o, void* v)
{
	getInstancePtr()->setManipulationMethod(methodPaintly);
	cbPreparation(o);
}

void VideoProcessor::cbPreparation(Fl_Widget* o)
{
	uiPtr = static_cast<ImpressionistUI*>(o->parent()->user_data());
	docPtr = uiPtr->getDocument();

	auto selfPtr = getInstancePtr();
	selfPtr->openVideoFromUser();
}

VideoProcessor::~VideoProcessor()
{
	close();

	AVIFileExit();
}

bool VideoProcessor::openFile(const std::string& filename)
{
	errorCode = AVIFileOpen(&aviPtr, filename.c_str(), OF_READ, nullptr);

	if (errorCode != AVIERR_OK)
	{
		AVIFileRelease(aviPtr);
	}
	else
	{
		AVIFileInfo(aviPtr, &aviInfo, sizeof(AVIFILEINFO));
	}

	return errorCode;
}

bool VideoProcessor::startStream()
{
	errorCode = AVIFileGetStream(aviPtr, &aviStreamPtr, streamtypeVIDEO, 0);

	if (errorCode != AVIERR_OK)
	{
		close();
	}

	streamStartingIndex = AVIStreamStart(aviStreamPtr);
	totalFrames = AVIStreamLength(aviStreamPtr);

	if (streamStartingIndex == -1 || totalFrames == -1)
	{
		fl_alert("Unable to start stream from video!");
		close();
		errorCode = -1;
	}

	getFramePtr = AVIStreamGetFrameOpen(aviStreamPtr, nullptr); //Prepares to get frame

	if (getFramePtr == nullptr)
	{
		fl_alert("Video codec is too new for VFW!");
		close();
		errorCode = -1;
	}

	return errorCode;
}

bool VideoProcessor::hasError() const
{
	return errorCode != 0;
}

void VideoProcessor::setManipulationMethod(const std::function<void(void*)>& callbackFunction)
{
	this->perImageFunction = callbackFunction;
}

void VideoProcessor::next()
{
	if (isEnded())
	{
		close();
		return;
	}

	bitmapDibPtr = static_cast<BYTE*>(AVIStreamGetFrame(getFramePtr, streamStartingIndex + frameIndex));

	frameIndex++;

	//Process bitmap DIB
	BITMAPINFOHEADER bitmapInfoHeader{};
	memcpy(&bitmapInfoHeader.biSize, bitmapDibPtr, sizeof(BITMAPINFOHEADER));

	const auto offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	const auto padding = calculatePadding(bitmapInfoHeader.biWidth);
	const auto byteLength = bitmapInfoHeader.biHeight * padding.padWidth;

	delete[] imageDataPtr;
	imageDataPtr = new unsigned char[padding.padWidth * bitmapInfoHeader.biHeight];
	memcpy(imageDataPtr, bitmapDibPtr + offset, byteLength);

	mapColor(imageDataPtr, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, padding.pad, cinepak_color_mapping);

	if (docPtr != nullptr)
	{
		docPtr->loadImageFromData(imageDataPtr, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight);
	}
	else
	{
		fl_alert("docPtr is nullptr but this should not happen.");
	}

	perImageFunction(nullptr);

	//Save image to stream
	reverseMapColor(imageDataPtr, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, cinepak_color_mapping);
	repackBmp(imageDataPtr, bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, padding.pad);

	memcpy(bitmapDibPtr + offset, imageDataPtr, byteLength);


}

bool VideoProcessor::isEnded() const
{
	if (aviStreamPtr != nullptr)
	{
		return frameIndex + streamStartingIndex >= totalFrames;
	}

	return true;
}

void VideoProcessor::close()
{
	if (getFramePtr != nullptr)
	{
		AVIStreamGetFrameClose(getFramePtr);
	}
	if (aviStreamPtr != nullptr)
	{
		AVIStreamRelease(aviStreamPtr);
	}
	if (aviPtr != nullptr)
	{
		AVIFileRelease(aviPtr);
	}

	aviPtr = nullptr;
	aviStreamPtr = nullptr;
	getFramePtr = nullptr;
}

VideoProcessor* VideoProcessor::getInstancePtr()
{
	if (singletonPtr == nullptr)
	{
		singletonPtr = new VideoProcessor();
	}

	return singletonPtr;
}

void VideoProcessor::destroy()
{
	delete singletonPtr;
}
