#include <cassert>
#include <cstring>
#include <Decoder.h>
#include <BitstreamReader.h>
#include <Console.h>
#include <Utils.h>
#include <bitset> 
#include <AppUtils.h>

using namespace std;
const int DEFAULT_BUFFER_QUEUE_MAX_SIZE = 16;
/***********************************************************************************************/
void printErrorCode(int err)
{
	if (err >= 0)
		return;
	switch (err)
	{
	case SD_E_LICENSECONTAINER_NOT_FOUND:
		cout << "LICENSECONTAINER_NOT_FOUND" << endl;
		break;
	case SD_E_LICENSE_NOT_FOUND:
		cout << "LICENSE_NOT_FOUND" << endl;
		break;
	case SD_E_FEATURE_NOT_AVAILABLE:
		cout << "FEATURE_NOT_AVAILABLE" << endl;
		break;
	default:
		cout << "Error code not found: " << err << endl;
		return;
	}
}

/***********************************************************************************************/
DWORD WINAPI decodeThread(void* Param)
{
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;
	Decoder* This = (Decoder*)Param;
	while (This->isActive)
	{
		if (This->decode(pkt) < 0)
		{
			cout << "decoder error!" << endl;
			return -1;
		}
	}
	return 0;
}

/***********************************************************************************************/
Decoder::Decoder()
{
	m_bufferQueueMaxSize = DEFAULT_BUFFER_QUEUE_MAX_SIZE;
	m_bDescriptInitialized = false;
	m_bVideoIsSeekable = false;
	m_iOutframes = 0;
	m_hHEVCDecoder = NULL;
	m_pNalUnit = NULL;
	m_picOut = NULL;
	m_picIn = NULL;
	m_streamIndex = -1;
	m_decodingTime = 0;
	m_avformatContext = 0;
}

/***********************************************************************************************/
Decoder::~Decoder()
{
	destroy();
}

/***********************************************************************************************/
void Decoder::stop()
{
	isActive = false;
}

/***********************************************************************************************/
void Decoder::createDecoder(int iNumOutPictureBuffer, int iNumThreads, int maxQueueSize, bool writeLogs)
{
	//cout << "Using SpinDecLib v.: " << SpinDecLib_GetVersionString() << endl;
	SpinDecLib_GetDefaultParams(&m_sDecParam);
	m_sDecParam.bTraceHLS = false;
	m_sDecParam.ePicBufferMode = SE_PBM_ExtInt; // use internal/external pictures
	m_sDecParam.bCalcHash = 0;
	m_writeLogs = writeLogs;

	if (iNumThreads > -1)
	{
		m_sDecParam.asThreadPoolConfig->iNumThreads = iNumThreads;

	}
	if (iNumOutPictureBuffer > -1)
	{
		m_sDecParam.asThreadPoolConfig->iNumConcurrentPics = iNumOutPictureBuffer;
	}

	if (maxQueueSize > -1)
	{
		m_bufferQueueMaxSize = maxQueueSize;
	}

	Spin_LicenseConfig sLicConfig;
	int res = readLicenseConfig(sLicConfig);  //read liceense configuration from config file (licenseconfig.txt)
	if (res != 0) {
		//if reading license configuration failed, set pLicConfg to NULL pointer,
		printf("Warning: read license configuration failed\n");
	}
	// initialize license
	if (SpinLib_InitLicense(&sLicConfig)) {
		throw LicenseCheckException();
	}

	m_sDecParam.bCalcHash = 0;
	m_sDecParam.ePixFmtMeth = SE_PFCAT_BC4;
	m_currentErrorCode = SpinDecLib_Open(&m_hHEVCDecoder, &m_sDecParam);
	printErrorCode(m_currentErrorCode);
	m_iOutframes = 0;
}

/***********************************************************************************************/
void Decoder::destroy()
{
	isActive = false;
	WaitForSingleObject(thandle, INFINITE);
	CloseHandle(thandle);

	if (m_hHEVCDecoder)
	{
		if (SpinDecLib_Close(&m_hHEVCDecoder))
		{
			assert(0);
		}
		// empty picture buffers
		std::list<PictureContainer*>::iterator ppic = m_lDecPicPool.begin();
		for (; ppic != m_lDecPicPool.end(); ppic++) {
			SpinLib_FreeFrame(&(*ppic)->pHEVCPic->sPic);
			delete (*ppic)->pHEVCPic;
			delete *ppic;
		}
		m_lDecPicPool.clear();

		memset(&m_sDecParam, 0, sizeof(SpinDec_Param));
		m_hHEVCDecoder = NULL;
	}
	m_bDescriptInitialized = false;
	m_bMp4Markers = false;
	memset(&m_hDescript, 0, sizeof(SpinDec_Descript));

	if (m_pNalUnit) {
		free(m_pNalUnit);
		m_pNalUnit = NULL;
	}

	if (m_avformatContext)
	{
		avformat_close_input(&m_avformatContext);
	}
	memset(&m_sLicenseConfig, 0, sizeof(m_sLicenseConfig));
	SpinLib_DeInitLicense();
}

/***********************************************************************************************/
VideoInformation Decoder::loadMP4(const char* src_filename)
{
	if (m_avformatContext)
	{
		avformat_close_input(&m_avformatContext);
		m_avformatContext = NULL;
	}

	/* open input file, and allocate format context */
	if (avformat_open_input(&m_avformatContext, src_filename, NULL, NULL) < 0) {
		fprintf(stderr, "Could not open source file %s\n", src_filename);
		//exit(1);
		m_currentErrorCode = -5001;
	}
	m_avformatContext->probesize = 5000000 * 20; //5000000 is the default size that doesn't seem to be enough for hight resolution hevc pictures
	cout << "probsize: " << m_avformatContext->probesize << endl;

	/* retrieve stream information */
	if (avformat_find_stream_info(m_avformatContext, NULL) < 0) {
		fprintf(stderr, "Could not find stream information\n");
		//exit(1);
		m_currentErrorCode = -5002;
	}

	for (int i = 0; i < m_avformatContext->nb_streams; i++)
		if (m_avformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			m_streamIndex = i;
			break;
		}
	//AVPixelFormat
	if (m_streamIndex == -1)
	{
		cout << "no video stream could be found in the file..." << endl;
		//exit(1); // Didn't find a video stream
		m_currentErrorCode = -5003;
	}
	AVCodecParameters* pCodecCtx;
	pCodecCtx = m_avformatContext->streams[m_streamIndex]->codecpar;
	assert(pCodecCtx != NULL);
	VideoInformation videoInformation;
	videoInformation.fps = (double)m_avformatContext->streams[m_streamIndex]->r_frame_rate.num / (double)m_avformatContext->streams[m_streamIndex]->r_frame_rate.den;
	videoInformation.videoPath = string(src_filename);
	double time_base = (double)m_avformatContext->streams[m_streamIndex]->time_base.num / (double)m_avformatContext->streams[m_streamIndex]->time_base.den;
	videoInformation.durationMS = max(-1.0 ,(double)m_avformatContext->streams[m_streamIndex]->duration * time_base * 1000.0);
	m_bVideoIsSeekable = videoInformation.durationMS > 0;
	AVPacket pkt;
	av_init_packet(&pkt);
	pkt.data = NULL;
	pkt.size = 0;
	unsigned int timeoutCounter = 0;

	if (m_currentErrorCode != 0)
	{
		cout << "An error occurred. Error code:" << m_currentErrorCode << endl;
		return videoInformation;
	}

	while (!m_bDescriptInitialized && timeoutCounter < 500) //get slices of a single frame until it is complete and return. If after 500 steps (max number of trials) nothing happend, break.
	{
		if (av_read_frame(m_avformatContext, &pkt) < 0) {
			av_packet_unref(&pkt);
		}

		if (pkt.stream_index != m_streamIndex) {
			av_packet_unref(&pkt);
			continue;
		}
		if (pkt.stream_index != m_streamIndex) {
			av_packet_unref(&pkt);
			continue;
		}
		unsigned int consumedBytes = 0;
		unsigned int consumedBytesDesc = 0;

		int ret = SpinDecLib_DecodeAU(m_hHEVCDecoder, pkt.data, pkt.size, pkt.pts, m_bMp4Markers, &consumedBytes, NULL, NULL, NULL, NULL, NULL);

		int hr = SpinDecLib_GetDescription(m_hHEVCDecoder, &m_hDescript);
		if (hr < 0) {
			//try with mp4 markers
			ret = SpinDecLib_DecodeAU(m_hHEVCDecoder, pkt.data, pkt.size, pkt.pts, !m_bMp4Markers, &consumedBytes, NULL, NULL, NULL, NULL, NULL);
			hr = SpinDecLib_GetDescription(m_hHEVCDecoder, &m_hDescript);
			if (hr == SD_OK) {
				m_bMp4Markers = !m_bMp4Markers;
			}
		}
		av_packet_unref(&pkt);
		if (hr == SD_OK) {
			m_bDescriptInitialized = true;
			videoInformation.width = m_hDescript.sPicDesc.asPlanes[0].iWidth * 4; //mul with 4 because of BC4
			videoInformation.height = m_hDescript.sPicDesc.asPlanes[0].iHeight * 4;
			cout << "vieo width:" << videoInformation.width << " video height:" << videoInformation.height << " framerate: " << videoInformation.fps << " pixelformat:" << pCodecCtx->format << " duration in MS:" << videoInformation.durationMS << endl;
			xPrintVideoInfo(m_hDescript);
		}
		timeoutCounter++;
	}

	if (timeoutCounter == 500)
	{
		//break after max number of n (e.g. 500) trials, obviously the file cannot be read by SPIN-Decoder
		cout << "The video stream is not supported by SPIN-Decoder, please check the format." << endl;
		return videoInformation;
	}


	if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar420)
	{
		videoInformation.chroma_subsampling = _420;
		cout << "pixelformat: 420" << endl;
	}
	else if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar422) {
		videoInformation.chroma_subsampling = _422;
		cout << "pixelformat: 422" << endl;
	}
	else if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar444) {
		videoInformation.chroma_subsampling = _444;
		cout << "pixelformat: 444" << endl;
	}
	else {
		cout << "pixelformat: " << SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) << " is not supported." << endl;
	}

	if (fileIsSeekable())
	{
		//SEEK to begin of the file: !Experimental!
		av_seek_frame(m_avformatContext, m_streamIndex, 0, AVSEEK_FLAG_BACKWARD);
		cout << "file is seekable" << endl;
	}
	else 
	{
		// reopen context after format is found
		avformat_close_input(&m_avformatContext);
		m_avformatContext = NULL;
		if (avformat_open_input(&m_avformatContext, src_filename, NULL, NULL) < 0) {
			fprintf(stderr, "Could not open source file %s\n", src_filename);
			m_currentErrorCode = -5004;
		}
		cout << "file is not seekable" << endl;
	}
	videoInformation.isInitialized = true;
	m_AV_EndOfFile = false;
	return videoInformation;
}

/***********************************************************************************************/
bool  Decoder::fileIsSeekable() {
	if (m_avformatContext->pb->seekable == 0) {
		return false;
	}
	if (m_avformatContext->duration == AV_NOPTS_VALUE) {
		return false;
	}
	return true;
};

/***********************************************************************************************/
double Decoder::getDecoderTime()
{
	//Get the time of the last decoded frame based on media information
	
	double dMediaTime = std::nan("0");
	AVStream* pStream = m_avformatContext->streams[m_streamIndex];
	if (pStream->cur_dts >= 0) {
		dMediaTime = (double)pStream->cur_dts / pStream->time_base.den;
	}
	return dMediaTime;
}

/***********************************************************************************************/
const PictureContainer* Decoder::getPic()
{
	if (frameQueue.size() > 1)
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		PictureContainer* pc = frameQueue.front();//free previous used pic
		pc->needoutput = false;
		frameQueue.pop();
		pc = frameQueue.front(); //return the front of the queue
		m_iOutframes = pc->frameNumber;
		m_cv.notify_one();
		return pc;
	}

	if (!isActive && frameQueue.size() == 1) //get the last pic
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		PictureContainer* pc = frameQueue.front();
		pc->needoutput = false;
		m_iOutframes = pc->frameNumber;
		frameQueue.pop();
		m_cv.notify_one();
		return pc;
	}
	return NULL;
}

/***********************************************************************************************/
int Decoder::decode(AVPacket pkt)
{
	if (m_currentErrorCode != 0)
	{
		cout << "An error occurred... Error code:" << m_currentErrorCode << endl;
		return m_currentErrorCode;
	}

	int decodingSteps = 0;
	SpinDec_Picture* picOut = NULL;
	SpinDec_Picture* picIn = NULL;
	bool _endOfFile = false;
	double _fps = (double)m_avformatContext->streams[m_streamIndex]->r_frame_rate.num / (double)m_avformatContext->streams[m_streamIndex]->r_frame_rate.den;
	while (isActive) //get slices of a single frame until it is complete and return
	{
		if (m_seekToMSecond >= 0) {
			int avret;
			int64_t targetDTS = max(0, m_seekToMSecond/1000.0) * m_avformatContext->streams[m_streamIndex]->time_base.den;
			int64_t diffDTS = min(m_avformatContext->streams[m_streamIndex]->duration, targetDTS) - m_avformatContext->streams[m_streamIndex]->cur_dts;
			if (diffDTS < 0) {
				avret = av_seek_frame(m_avformatContext, m_streamIndex, targetDTS, AVSEEK_FLAG_FRAME | AVSEEK_FLAG_BACKWARD);
			}
			else {
				avret = av_seek_frame(m_avformatContext, m_streamIndex, targetDTS, AVSEEK_FLAG_FRAME);
			}
			if (avret >= 0) {
				SpinDecLib_InvalidateInFlightPictures(m_hHEVCDecoder);
				m_seekToMSecond = -1;
			}
			else {
				cout << "ERROR: could not seek. Please check if seeking is supported for the video format." << endl;
			}
		}

		if (frameQueue.size() >= m_bufferQueueMaxSize) {
			continue;
		}
		int avret = av_read_frame(m_avformatContext, &pkt);
		if (avret < 0) {
			_endOfFile = avret == (int)AVERROR_EOF;
			cout << "The end of file reached, check if loop is active\n";
			break;
		}
		if (pkt.stream_index != m_streamIndex) {
			av_packet_unref(&pkt);
			continue;
		}

		unsigned int consumedBytes = 0;
		unsigned int consumedBytesDesc = 0;
		bool hasPicOut = false;
		bool usedPicIn = false;

		if (picIn == NULL) {
			picIn = getNewPictureBuffer();
		}
		double start;
		if (m_writeLogs)
		{
			start = SpinLib_GetRealTime();
		}
		m_currentErrorCode = SpinDecLib_DecodeAU(m_hHEVCDecoder, pkt.data, pkt.size, pkt.pts, m_bMp4Markers, &consumedBytes, picIn, &usedPicIn, &picOut, &hasPicOut, NULL);
		av_packet_unref(&pkt);
		if (m_writeLogs)
		{
			m_decodingTime += (SpinLib_GetRealTime() - start);
			decodingSteps++;
		}
		if (m_currentErrorCode >= 0)
		{
			if (hasPicOut)
			{
				PictureContainer* picOutCon = m_mExtPic[picOut->sPic.pPlanesData];
				if (m_writeLogs)
				{
					picOutCon->decodingTime = m_decodingTime * 1000.0;
					picOutCon->decodingSteps = frameQueue.size() + 1;
					m_decodingTime = 0;
					decodingSteps = 0;
				}
				std::lock_guard<std::mutex> lock(m_mutex);
				frameQueue.push(picOutCon);
				m_cv.notify_one();
				picOutCon->frameNumber = getDecoderTime() * _fps;	
			}

			if (usedPicIn) {
				picIn = NULL;
			}
		}
	}
	// flush all pictures inside the decoder library
	SpinDecLib_FlushInFlightPictures(m_hHEVCDecoder);
	while (SpinDecLib_GetDecPicture(m_hHEVCDecoder, &picOut, true)) {
		PictureContainer* picOutCon = m_mExtPic[picOut->sPic.pPlanesData];
		std::lock_guard<std::mutex> lock(m_mutex);
		frameQueue.push(picOutCon);
		m_cv.notify_one();
		cout << "flushing rest pictures... " << endl;
	}

	if (m_shouldLoop && _endOfFile)
	{
		if (fileIsSeekable())
		{
			auto stream = m_avformatContext->streams[m_streamIndex];
			avio_seek(m_avformatContext->pb, 0, SEEK_SET);
			avformat_seek_file(m_avformatContext, m_streamIndex, 0, 0, stream->duration, 0);
		}
		else {
			avformat_close_input(&m_avformatContext);
			const char* src_filename = m_avformatContext->url;
			m_avformatContext = NULL;
			if (avformat_open_input(&m_avformatContext, src_filename, NULL, NULL) < 0) {
				fprintf(stderr, "Could not open source file %s\n", src_filename);
				m_currentErrorCode = -5004;
			}
		}
	}
	else if (_endOfFile) {
		m_AV_EndOfFile = true;
		isActive = false;
	}
	return m_currentErrorCode;
}

/***********************************************************************************************/
void Decoder::allocPictureBuffer(PictureContainer* pPicCon)
{
	SpinDec_Picture* pic = new SpinDec_Picture;
	memset(pic, 0, sizeof(SpinDec_Picture));

	pic->sPic = m_hDescript.sPicDesc;

	//problem with special pixel types?
	pic->sPic.asPlanes[0].iAlign = 64;

	if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar420)
	{
		pic->sPic.ePixFormat = SE_PF_BC4_420;
	}
	else if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar422) {
		pic->sPic.ePixFormat = SE_PF_BC4_422;
	}
	else if (SD_CastToPlanar(m_hDescript.sPicDesc.ePixFormat) == SE_PF_Planar444) {
		pic->sPic.ePixFormat = SE_PF_BC4_444;
	}

	if (SpinLib_AllocFrame(&pic->sPic)) {
		fprintf(stderr, "Failed to allocate frame buffer! \n");
		throw exception();
	}
	// store the picture container with the picture as a reference for easier access
	m_mExtPic[pic->sPic.pPlanesData] = pPicCon;
	// store picture in picture container and flag as "needs output" 
	pPicCon->pHEVCPic = pic;
	pPicCon->needoutput = true;
}

/***********************************************************************************************/
SpinDec_Picture* Decoder::getNewPictureBuffer()
{
	// initialize picture description if necessary
	if (!m_bDescriptInitialized) {
		if (SpinDecLib_GetDescription(m_hHEVCDecoder, &m_hDescript)) {
			//cannot alloc picture without description, wait for more NAL units
			return NULL;
		}
		m_bDescriptInitialized = true;
		xPrintVideoInfo(m_hDescript);
	}

	std::list<PictureContainer*>::iterator pic = m_lDecPicPool.begin();
	for (; pic != m_lDecPicPool.end(); pic++) {
		if (!(*pic)->needoutput) {
			// unused picture found, reuse it
			(*pic)->needoutput = true;
			//TODO get if picture is strided.
			SpinDec_Picture *spic = (*pic)->pHEVCPic;
			const Spin_Plane *planeY = &spic->sPic.asPlanes[0];
			const Spin_Plane *planeCb = &spic->sPic.asPlanes[1];
			const Spin_Plane *planeCr = &spic->sPic.asPlanes[2];
			m_outPicIsStrided = planeY->iWidth != planeY->iStride || planeCb->iWidth != planeCb->iStride || planeCr->iWidth != planeCr->iStride;
			return spic;
		}
	}
	PictureContainer* pPicCon = new PictureContainer();
	allocPictureBuffer(pPicCon);
	// store new picture in picture pool for future reuse
	m_lDecPicPool.push_back(pPicCon);
	return pPicCon->pHEVCPic;
}

/***********************************************************************************************/
void   Decoder::run(bool shouldLoop)
{
	m_shouldLoop = shouldLoop;
	isActive = true;
	thandle = CreateThread(NULL, 0, decodeThread, (void*)this, 0, &threadid);
}

/***********************************************************************************************/
void  Decoder::xPrintPicInfo(const SpinDec_Picture* picOut) {

	char c = (picOut->ePicType == SE_FT_IFrame ? 'I' : picOut->ePicType == SE_FT_PFrame ? 'P' : 'B');
	if (!(picOut->uiNalType & 0x1)) c += 32;
	//cout << "PicInfo:" << "POC " << picOut->uiPOC << " TID: " << picOut->uiTLayer << " NAL: " << picOut->uiNalType << " ( " << c << "-SLICE, QP" << picOut->uiQp << " ) [DT " << 1000 * picOut->dDecodingTime << " ms] [FL " << 1000 * picOut->dFrameLatency << " ms] [CPS " << picOut->uiCodedPicSize << " B]";
}

/***********************************************************************************************/
void  Decoder::xPrintVideoInfo(const SpinDec_Descript & decDescript) {
	stringstream ss;
	ss << "*** Decoding information ***";
	ss << "* Video resolution : " << decDescript.sPicDesc.asPlanes[0].iWidth << "x" << decDescript.sPicDesc.asPlanes[0].iHeight;
	const char* cspaces[SE_NUM_ColorSpaces] = { "bt601", "bt709", "bt2020" };
	const char* transfer[SE_NUM_TransferFuncs] = {
		"sdr",
		"linear",
		"st2084"
	};
	string cm;
	const char* cmstr[2] = { "rgba", "yuva" };
	const char* cm_cstr = cm.c_str();
	ss << "* Chroma format    : " << cm_cstr << " " << strChromaFmt[SD_CastToPlanar(decDescript.sPicDesc.ePixFormat)];
	ss << "* Color space      : " << cspaces[decDescript.sPicDesc.eColorSpace] << (decDescript.sPicDesc.eColorRange == SE_CR_Limited ? 'l' : 'f') << " " << transfer[decDescript.sPicDesc.eTransferFunc];

	string pixmeth;
	switch (SD_GetPixFmtCat(decDescript.sPicDesc.ePixFormat)) {
	case SE_PFCAT_Planar:
		pixmeth = "planar";
		break;
	case SE_PFCAT_SemiPlanar:
		pixmeth = "semipl";
		break;
	case SE_PFCAT_Packed32b:
		pixmeth = "packed";
		break;
	case SE_PFCAT_BitPlanar:
		pixmeth = "bitpla";
		break;
	case SE_PFCAT_BitSemiPl:
		pixmeth = "bitsem";
		break;
	default:
		pixmeth = "unknown";
		break;
	}

	ss << "* Format type      : " << pixmeth.c_str();
	ss << "* Luma bitdepth    : " << decDescript.sPicDesc.aiBitdepth[0];
	ss << "* Chroma bitdepth  : " << decDescript.sPicDesc.aiBitdepth[1];
	cout << ss.str() << endl;
}


/***********************************************************************************************/
bool Decoder::isFinished()
{
	if (!m_avformatContext)
	{
		return true;
	}
	return m_AV_EndOfFile && frameQueue.size() == 0;
}

/***********************************************************************************************/
bool Decoder::isVideoFileLoaded() {
	return m_avformatContext != 0;
}

/***********************************************************************************************/
int Decoder::getCurrentFrameNumber() {
	return m_iOutframes;
};

/***********************************************************************************************/
int Decoder::getCurrentErrorCode() {
	return m_currentErrorCode;
}

/***********************************************************************************************/
bool Decoder::getPicIsStrided() {
	return m_outPicIsStrided; 
}

/***********************************************************************************************/
void Decoder::seekToMSecond(int64_t seekToMSecond) {
	m_seekToMSecond = seekToMSecond; 
}

/***********************************************************************************************/
bool Decoder::getSeekingIsSupported()
{
	return m_bVideoIsSeekable;
}

