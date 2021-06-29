#ifndef __Decoder__
#define __Decoder__


#include <spindec.h>
#include <stdint.h>
#include <fstream>
#include <Windows.h>
#include <algorithm>
#include <stdint.h>
#include <map>
#include <list>
#include <cstring>
#include <future>
#include <queue>
#include "BaseTextureAccess.h"

static const char* strChromaFmt[] = { "400", "420", "422", "444", "Undefined" };

extern "C" {
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
}


class BitstreamReader;
class LicenseCheckException : public std::exception
{
};


typedef struct VideoInformation {
	int width = -1;
	int height = -1;
	double fps = 0.0;
	int64_t durationMS = 0.0;
	bool isInitialized = false;
	CHROMA_SUBSAMPLING chroma_subsampling;
	std::string videoPath;
} VideoInformation;

typedef struct PictureContainer {
	SpinDec_Picture* pHEVCPic; // actual picture
	bool needoutput; // true if the picture still needs to be displayed / written to output
	double decodingTime;
	int decodingSteps;
	int frameNumber;
} PictureContainer;


class Decoder
{
	std::list<PictureContainer*> m_lDecPicPool;
	std::map<void*, PictureContainer*> m_mExtPic;
	std::queue<PictureContainer*> frameQueue;
	
	mutable std::mutex m_mutex;
	std::condition_variable m_cv;

public:
  Decoder();
  ~Decoder();
  bool isActive;
  void  createDecoder(int iNumOutPictureBuffer = -1, int iNumThreads = -1, int maxQueueSize = -1, bool writeLogs =  false);
  VideoInformation loadMP4(const char* src_filename);
  void  destroy();  
  int   decode(AVPacket pkt);
  void   run(bool shouldLoop);
  void   stop();
  const PictureContainer* getPic();
  bool isFinished();
  bool getPicIsStrided();
  bool getSeekingIsSupported();
  bool isVideoFileLoaded();
  int getCurrentFrameNumber();
  int getCurrentErrorCode();
  void seekToMSecond(int64_t seekForMSeconds);

private:
	SpinDec_Param m_sDecParam;
	Spin_LicenseConfig m_sLicenseConfig;
	SpinDecLib_Handle m_hHEVCDecoder;
	bool m_bMp4Markers;
	bool m_bDescriptInitialized;
	bool m_outPicIsStrided;
	bool m_shouldLoop;
	bool m_writeLogs;
	bool m_AV_EndOfFile;
	bool m_bVideoIsSeekable;
	SpinDec_Descript m_hDescript;
	int m_iOutframes;
	int m_bufferQueueMaxSize;
	int m_streamIndex;
	int m_currentErrorCode = 0;
	int64_t m_seekToMSecond = -1;
	double m_decodingTime;
	uint8_t *m_pNalUnit;
	SpinDec_Picture* m_picOut;
	SpinDec_Picture* m_picIn;
	HANDLE thandle;
	DWORD threadid;
	SpinDec_Picture* getNewPictureBuffer();
	bool fileIsSeekable();
	
	void   allocPictureBuffer(PictureContainer* pPicCon);         
	void  xPrintPicInfo(const SpinDec_Picture* pPic);           
	void  xPrintVideoInfo(const SpinDec_Descript & rDecDescript);
	double getDecoderTime();
	AVFormatContext *m_avformatContext;
};

#endif
