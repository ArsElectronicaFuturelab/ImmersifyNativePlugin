#include "Sequencer.h"

/***********************************************************************************************/
Sequencer::Sequencer()
{
	m_state = PAUSED;
	m_pauseAfterFirstFrame = false;
	m_isReady = false;
	m_decoderNumThreads = -1;
	m_numOfPictureBuffer = -1;
	m_maxQueueSize = -1;
	m_logFileOpened = false;
	m_decoder = new Decoder();
	m_decoder->createDecoder(m_numOfPictureBuffer, m_decoderNumThreads, m_maxQueueSize, m_writeLogs);
}

/***********************************************************************************************/
Sequencer::~Sequencer()
{
	destroy();
	closeLogFile();
}

/***********************************************************************************************/
void Sequencer::safeDelete(BaseTextureAccess *textureAccess)
{
	if (textureAccess) {
		delete textureAccess;
		textureAccess = NULL;
	}
}

/***********************************************************************************************/
void Sequencer::setTextureAccess(BaseTextureAccess* textureAccess)
{
	m_textureAccess = textureAccess;
}

/***********************************************************************************************/
BaseTextureAccess * Sequencer::getTextureAccess()
{
	return m_textureAccess;
}

/***********************************************************************************************/
VideoInformation Sequencer::loadMP4(const char* src_filename)
{
	m_videoInformation = m_decoder->loadMP4(src_filename);
	return m_videoInformation;
}


/***********************************************************************************************/
void Sequencer::play(float frameRate, bool shouldLoop)
{
	if (!m_decoder)
	{
		cout << " the file cannot be played since a decoder is not initialized!" << endl;
		return;
	}

	if (!m_decoder->isVideoFileLoaded())
	{
		cout << "No video file is loaded in the deocder. Please call loadMP4 before this method" << endl;
	}

	m_decoder->run(shouldLoop);
	m_timer.stop();
	m_timer.start();
	m_frameRate = frameRate;
	m_frameDuration = 1000.0 / frameRate;
	m_currentFrameDuration = m_frameDuration;
	m_elapsedPlayingTime = 0;
	m_state = PLAYING;
	m_isReady = false;
	//stringstream ss;
	//ss << "Play() was called for video " << m_videoInformation.videoPath;
	//writeToLogFile(ss.str());
}

/***********************************************************************************************/
void Sequencer::overrideTargetFPS(float fps)
{
	if (fps == 0)
	{
		m_currentFrameDuration = 0;
	}
	else {
		m_currentFrameDuration = 1000.0 / fps;
	}
}

/***********************************************************************************************/
double Sequencer::getCurrentPlayingTime()
{ 
	//cout << "time from decoder" << m_decoder->getMediaTime() << endl;
	return m_frameDuration * m_decoder->getCurrentFrameNumber();
}

/***********************************************************************************************/
int Sequencer::getMaxQueueSize() const
{
	return m_maxQueueSize;
}

/***********************************************************************************************/
void Sequencer::setTargetPlayingTime(float targetPlaybackTime)
{
	if (m_state == PAUSED || m_state == STOPPED)
	{
		return;
	}
	float delta = targetPlaybackTime - ((float)m_frameDuration * m_decoder->getCurrentFrameNumber()); //(m_decoder->getMediaTime() * 1000.0);
	m_currentFrameDuration = m_frameDuration / ((delta + 1000.0f) / 1000.0f); //adjust the frame duration based on current playback time. Using a smoothing function for better playback experience.
}

/***********************************************************************************************/
bool Sequencer::update()
{
	if (m_state == PAUSED || m_state == STOPPED)
	{
		return false;
	}

	double currentTime = m_timer.getElapsedTimeInMilliSec();
	if (currentTime - m_elapsedPlayingTime < m_currentFrameDuration)
	{
		return false;
	}
	
	const PictureContainer* out = m_decoder->getPic();
	bool success = false;
	if (out)
	{
		if (m_textureAccess->isReady())
		{
			m_isReady = true;
			m_textureAccess->setPicIsStrided(m_decoder->getPicIsStrided());
			if (m_pauseAfterFirstFrame)
			{
				if (m_decoder->getPicIsStrided())
				{
					cout << "picture is strided!! this can influence the performance of the playback.\n";
				}
				success = getAndApplyPictureData(out);
				if (success)
				{
					m_state = PAUSED;
					pauseAfterFirstFrame(false);
				}

				m_elapsedPlayingTime = currentTime;
				
				return success;
			}
		}

		success = getAndApplyPictureData(out);
		m_elapsedPlayingTime = currentTime;
	}
	return success;
}

/***********************************************************************************************/
bool Sequencer::getAndApplyPictureData(const PictureContainer* out)
{
	const Spin_Picture *pic = &out->pHEVCPic->sPic;
	
	double currentTime = 0;
	double start = 0;

	if (m_writeLogs)
	{
		currentTime = m_timer.getElapsedTimeInMilliSec();
		start = SpinLib_GetRealTime();
	}

	bool success = m_textureAccess->applyPictureData(pic);
	if (m_writeLogs)
	{
		float uploadTime = (float)(SpinLib_GetRealTime() - start);
		stringstream ss;
		ss << uploadTime * 1000.0 << "\t" << out->decodingTime << "\t" << out->pHEVCPic->dDecodingTime*1000.0 << "\t" << out->pHEVCPic->dFrameLatency*1000.0 << "\t" << currentTime - m_elapsedPlayingTime << "\t" << out->decodingSteps;
		writeToLogFile(ss.str());
	}
	return success;
}

/***********************************************************************************************/
void Sequencer::destroy()
{
	m_state = STOPPED;
	//stringstream ss;
	//ss << "[" << m_videoInformation.videoPath << "] Play got stopped because Sequencer is destroyed.";
	//writeToLogFile(ss.str());
	if (m_decoder)
	{
		m_decoder->stop();
		delete(m_decoder);
		m_decoder = NULL;
	}
	safeDelete(m_textureAccess);
}

/***********************************************************************************************/
void Sequencer::setPause(bool pause)
{
	if (m_state != STOPPED)
	{
		if (pause)
		{
			m_state = PAUSED;
		}
		else {
			m_state = PLAYING;
		}
	}
}

/***********************************************************************************************/
void Sequencer::stop()
{
	delete m_decoder;
	m_decoder = 0;
	m_state = STOPPED;
}

/***********************************************************************************************/
bool Sequencer::isPaused()
{
	return m_state == PAUSED;
}

/***********************************************************************************************/
bool Sequencer::isFinished()
{
	return m_decoder->isFinished();
}

/***********************************************************************************************/
bool Sequencer::isReady()
{
	return m_isReady;
}

/***********************************************************************************************/
void Sequencer::pauseAfterFirstFrame(bool pauseAfterFirstFrame)
{
	m_pauseAfterFirstFrame = pauseAfterFirstFrame;
}

/***********************************************************************************************/
static string getCurrentDateTime(string s) {
	time_t now = time(0);
	struct tm  tstruct;
	char  buf[80];
	tstruct = *localtime(&now);
	if (s == "now")
		strftime(buf, sizeof(buf), "%Y-%m-%d_%H%M%S", &tstruct);
	else if (s == "date")
		strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);
	return string(buf);
};


/***********************************************************************************************/
void Sequencer::writeToLogFile(const string log)
{
	if (!m_logFileOpened)
	{
		string filePath = getCurrentDateTime("now") + ".txt";
		ofs.open(filePath.c_str(), std::ios_base::out | std::ios_base::app);
		m_logFileOpened = true;
		cout << filePath;
	}

	//string now = getCurrentDateTime("now");
	ofs << log << '\n';
	ofs.flush();
}

/***********************************************************************************************/
void Sequencer::closeLogFile()
{
	if (!m_logFileOpened)
	{
		ofs << "Logfile got closed correctly." << '\n';

		ofs.flush();
		ofs.close();

		m_logFileOpened = false;
	}
}

/***********************************************************************************************/
int Sequencer::getCurrentErrorCode() {
	return m_decoder->getCurrentErrorCode();
}

/***********************************************************************************************/
void Sequencer::setDecoderOptions(int numPictureBuffer, int decoderNumThreads, int maxQueueSize, bool writeLogs)
{
	m_numOfPictureBuffer = numPictureBuffer;
	m_decoderNumThreads = decoderNumThreads;
	m_maxQueueSize = maxQueueSize;
	m_writeLogs = writeLogs;
}

/***********************************************************************************************/
const VideoInformation& Sequencer::getVideoInformation()
{
	return m_videoInformation;
}

/***********************************************************************************************/
void Sequencer::seekToMSec(int64_t seekToMSeconds) {
	if (m_decoder->getSeekingIsSupported())
	{
		cout << m_frameRate << " " << seekToMSeconds << endl;
		m_decoder->seekToMSecond(seekToMSeconds);
	}
	else {
		cout << "seeking is not supported for this video." << endl;
	}
}

/***********************************************************************************************/
bool Sequencer::getSeekingIsSupported()
{
	return m_decoder->getSeekingIsSupported();
}

/***********************************************************************************************/
double Sequencer::getTargetFrameDuration()
{ 
	return m_currentFrameDuration; 
}

/***********************************************************************************************/
double Sequencer::getTargetFPS()
{ 
	return m_currentFrameDuration*1000.0; 
}