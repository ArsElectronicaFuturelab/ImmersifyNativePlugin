#pragma once

#ifndef __baseTextureAccess_H__
#define __baseTextureAccess_H__

#include <spindec.h>
enum CHROMA_SUBSAMPLING {
	_420 = 0,
	_422 = 1,
	_444 = 2
};

class BaseTextureAccess
{
public:
	void setPicIsStrided(bool strided) { m_picIsStrided = strided; }
	virtual ~BaseTextureAccess() {} // Nothing to do here, but it needs to be virtual.
	bool isReady() const { return m_ready; }
	unsigned int width() const { return m_width; }
	unsigned int height() const { return m_height; }
	CHROMA_SUBSAMPLING getChromaSubSampling() { return m_chroma_subsampling; }
	virtual bool applyPictureData(const Spin_Picture* pic) = 0;

protected:	
	unsigned int m_width;
	unsigned int m_height;
	CHROMA_SUBSAMPLING m_chroma_subsampling;

	bool m_ready;
	bool m_picIsStrided;
};

#endif