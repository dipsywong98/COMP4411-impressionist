#include "VideoProcessor.h"
#include "impressionistUI.h"
#include "impressionistDoc.h"
#include "bitmap.h"
#include <regex>

VideoProcessor* VideoProcessor::singletonPtr = nullptr;
ImpressionistUI* VideoProcessor::uiPtr = nullptr;
ImpressionistDoc* VideoProcessor::docPtr = nullptr;

std::function<void(void*)> VideoProcessor::methodAutoFill = [](void*)->void
{
	uiPtr->m_paintView->willAutoFill = true;
	uiPtr->m_paintView->draw();
	continueWriteStream();
};

std::function<void(void*)> VideoProcessor::methodPaintly = [](void*)->void
{
	uiPtr->m_paintView->willPainterly = true;
	uiPtr->m_paintView->draw();
	continueWriteStream();
};

VideoProcessor::VideoProcessor()
{
	AVIFileInit();  // Must be called to use AVI library
}

void VideoProcessor::saveImage()
{
	//Save image to stream
	auto *t = new unsigned char[byteLength];
	memcpy(t, docPtr->m_ucPainting, byteLength);

	reverseMapColor(t, bmpInfo.biWidth, bmpInfo.biHeight, cinepak_color_mapping);
	repackBmp(t, bmpInfo.biWidth, bmpInfo.biHeight, padding.pad);

	AVICOMPRESSOPTIONS compressionOptions = {
		streamtypeVIDEO,
		mmioFOURCC('c', 'v', 'i', 'd'),
		0,
		100,
		0,
		AVICOMPRESSF_VALID,
		&bmpInfo,
		sizeof(BITMAPINFOHEADER),
		nullptr,
		0,
		0
	};

	errorCode = AVIStreamWrite(aviCStreamPtr, streamStartingIndex + frameIndex, 1, t, byteLength, 0, nullptr, nullptr);

	delete[] t;

	if (errorCode == AVIERR_UNSUPPORTED)
	{
		fl_alert("Compression method unsupported. But this should never happen.");
		close();
	}
	else if (errorCode == AVIERR_NOCOMPRESSOR)
	{
		fl_alert("A suitable compressor cannot be found.");
		close();
	}
	else if (errorCode != AVIERR_OK)
	{
		fl_alert("Cannot write data to stream.");
		close();
	}

	frameIndex++;
	next();
}

void VideoProcessor::openVideoFromUser()
{
	// ICINFO info{};
	//
	// ICInfo(streamtypeVIDEO, mmioFOURCC('C', 'V', 'I', 'D'), &info);

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

void VideoProcessor::continueWriteStream()
{
	if (singletonPtr != nullptr && uiPtr && docPtr && !getInstancePtr()->isEnded())
	{
		getInstancePtr()->saveImage();
	}
}

VideoProcessor::~VideoProcessor()
{
	close();

	AVIFileExit();
}

bool VideoProcessor::openFile(const std::string& filename)
{
	const auto& newFilename = std::regex_replace(filename, std::regex("\\.avi"), "_new.avi");;

	// const bool copySucceeded = CopyFile(filename.c_str(), newFilename.c_str(), false);
	//
	// if (!copySucceeded)
	// {
	// 	fl_alert("Save file cannot be copied.");
	// }

	errorCode = AVIFileOpen(&aviReadPtr, filename.c_str(), OF_READ, nullptr);
	errorCode = AVIFileOpen(&aviPtr, newFilename.c_str(), OF_CREATE|OF_WRITE, nullptr);

	if (errorCode != AVIERR_OK)
	{
		AVIFileRelease(aviReadPtr);
		AVIFileRelease(aviPtr);
	}
	else
	{
		AVIFileInfo(aviReadPtr, &aviInfo, sizeof(AVIFILEINFO));
	}

	return errorCode;
}

bool VideoProcessor::startStream()
{
	errorCode = AVIFileGetStream(aviReadPtr, &aviReadStreamPtr, streamtypeVIDEO, 0);

	if (errorCode != AVIERR_OK)
	{
		fl_alert("Unable to get stream from video.");
		close();
	}

	streamStartingIndex = AVIStreamStart(aviReadStreamPtr);
	totalFrames = AVIStreamLength(aviReadStreamPtr);

	if (streamStartingIndex == -1 || totalFrames == -1)
	{
		fl_alert("Unable to start stream from video!");
		close();
		errorCode = -1;
	}

	AVISTREAMINFO info;

	AVIStreamInfo(aviReadStreamPtr, &info, sizeof(AVISTREAMINFO));

	info.dwQuality = 10000;
	info.fccHandler = mmioFOURCC('c','v','i','d');
	info.dwLength = 0;

	errorCode = AVIFileCreateStream(aviPtr, &aviStreamPtr, &info);

	AVICOMPRESSOPTIONS compressOptions{};

	ZeroMemory(&compressOptions, sizeof(AVICOMPRESSOPTIONS));
	compressOptions.fccType = streamtypeVIDEO;
	compressOptions.fccHandler = info.fccHandler;
	compressOptions.dwFlags = AVICOMPRESSF_KEYFRAMES | AVICOMPRESSF_VALID;
	compressOptions.dwKeyFrameEvery = 1;

	errorCode = AVIMakeCompressedStream(&aviCStreamPtr, aviStreamPtr, &compressOptions, nullptr);

	if (errorCode)
	{
		fl_alert("Codec unsupported.");
		return errorCode;
	}

	getFramePtr = AVIStreamGetFrameOpen(aviReadStreamPtr, nullptr); //Prepares to get frame

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
		destroy();
		return;
	}

	bitmapDibPtr = static_cast<BYTE*>(AVIStreamGetFrame(getFramePtr, streamStartingIndex + frameIndex));

	//Process bitmap DIB
	memcpy(&bmpInfo.biSize, bitmapDibPtr, sizeof(BITMAPINFOHEADER));

	if (!hasSetFormat)
	{
		AVIStreamSetFormat(aviCStreamPtr, 0, &bmpInfo, bmpInfo.biSize);
	}

	const auto offset = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER);
	padding = calculatePadding(bmpInfo.biWidth);
	byteLength = bmpInfo.biHeight * padding.padWidth;

	imageDataPtr = new unsigned char[padding.padWidth * bmpInfo.biHeight];  //Entrusts this array is deleted by ImpressionistDoc
	memcpy(imageDataPtr, bitmapDibPtr + offset, byteLength);

	mapColor(imageDataPtr, bmpInfo.biWidth, bmpInfo.biHeight, padding.pad, cinepak_color_mapping);

	if (docPtr != nullptr)
	{
		docPtr->loadImageFromData(imageDataPtr, bmpInfo.biWidth, bmpInfo.biHeight);
	}
	else
	{
		fl_alert("docPtr is nullptr but this should not happen.");
	}

	perImageFunction(nullptr);

	
}

bool VideoProcessor::isEnded()
{
	if (singletonPtr != nullptr)
	{
		const auto* selfPtr = getInstancePtr();
		if (selfPtr->aviReadStreamPtr != nullptr)
		{
			return selfPtr->frameIndex + selfPtr->streamStartingIndex >= selfPtr->totalFrames;
		}
	}

	return true;
}

void VideoProcessor::close()
{
	if (getFramePtr != nullptr)
	{
		AVIStreamGetFrameClose(getFramePtr);
	}
	if (aviCStreamPtr != nullptr)
	{
		AVIStreamRelease(aviCStreamPtr);
	}
	if (aviStreamPtr != nullptr)
	{
		AVIStreamRelease(aviStreamPtr);
	}
	if (aviPtr != nullptr)
	{
		AVIFileRelease(aviPtr);
	}
	if (aviReadStreamPtr != nullptr)
	{
		AVIStreamRelease(aviReadStreamPtr);
	}
	if (aviReadPtr != nullptr)
	{
		AVIFileRelease(aviReadPtr);
	}

	aviPtr = nullptr;
	aviCStreamPtr = nullptr;
	aviStreamPtr = nullptr;
	aviReadPtr = nullptr;
	aviReadStreamPtr = nullptr;
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
	singletonPtr = nullptr;
}
