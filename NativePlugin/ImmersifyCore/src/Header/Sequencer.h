#pragma once

#ifndef __SEQUENCER__
#define __SEQUENCER__

#include<stdio.h>
#include <string>
#include <sstream>
#include <Decoder.h>
#include <Console.h>
#include <Utils.h>
#include "Timer.h"
#include "BaseTextureAccess.h"

enum PLAYER_STATE {
	PLAYING,
	PAUSED,
	STOPPED
};

class  Sequencer
{
	
public:
	Sequencer();
	~Sequencer();
	void setTextureAccess(BaseTextureAccess* textureAccess);
	BaseTextureAccess* getTextureAccess();
	void play(float framerate = 60.0f, bool shouldLoop = false);
	bool update();
	bool getAndApplyPictureData(const PictureContainer* out);
	void setPause(bool pause);
	void overrideTargetFPS(float fps);
	bool isPaused();
	bool isFinished();
	void stop();
	void setTargetPlayingTime(float targetPlayingTime);
	void pauseAfterFirstFrame(bool pauseAfterFirstFrame);
	double getTargetFrameDuration();
	double getTargetFPS();
	bool getSeekingIsSupported();
	bool isReady();
	double getCurrentPlayingTime();
	int getMaxQueueSize() const;
	VideoInformation loadMP4(const char* src_filename);
	PLAYER_STATE getPlayerState() { return m_state; }
	void setDecoderOptions(int numPictureBuffer, int decoderNumThreads, int maxQueueSize, bool writeLogs = false);
	const VideoInformation& getVideoInformation();
	int getCurrentErrorCode();
	void seekToMSec(int64_t seekForMSeconds);
	
private:
	Decoder *m_decoder = nullptr;
	BaseTextureAccess *m_textureAccess = nullptr;
	Timer m_timer;
	PLAYER_STATE m_state;
	bool m_pauseAfterFirstFrame;
	bool m_writeLogs;
	bool m_logFileOpened;
	float m_frameRate;
	float m_targetPlayingTime;
	bool m_isReady;
	int m_decoderNumThreads;
	int m_numOfPictureBuffer;
	int m_maxQueueSize;
	double m_frameDuration;
	double m_currentFrameDuration;
	double m_elapsedPlayingTime;
	VideoInformation m_videoInformation;
	CHROMA_SUBSAMPLING m_chroma_subsampling;
	std::ofstream ofs;
	void safeDelete(BaseTextureAccess *textureAccess);	
	void destroy();
	void writeToLogFile(const string log);
	void closeLogFile();
};

#endif