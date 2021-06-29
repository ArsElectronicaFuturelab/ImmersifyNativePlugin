#pragma once

#ifndef __glTextureAccess_H__
#define __glTextureAccess_H__

#include <gl/glew.h>
#include <Console.h>
#include <Utils.h>
#include <windows.h>
#include "BaseTextureAccess.h"

const int NUMBER_PBO = 2;
struct YPARAMETERS
{
	const Spin_Picture* pic;
	GLubyte* ptr;
};

class GlTextureAccess : public BaseTextureAccess
{
public:
	GlTextureAccess(uintptr_t texturePtr[], unsigned int width, unsigned int height, CHROMA_SUBSAMPLING chroma_subsampling);
	~GlTextureAccess() override;

	void clearBuffer();

	virtual bool applyPictureData(const Spin_Picture* pic) override;
	bool getPictureIsStrided() { return m_picIsStrided; }
	void setMaxWaitForGPUUpload(int ms) { m_maxWaitForGPUUploadSteps = (float)(ms) / 0.1; }

	YPARAMETERS params;
	HANDLE thandle_1;
	DWORD threadid_1;

	HANDLE thandle_2;
	DWORD threadid_2;

	bool m_run = true;
	bool m_shouldWaitForGpuUpload_1 = false;
	bool m_shouldWaitForGpuUpload_2 = false;

private:
	void apply();
	bool m_pboReady;
	unsigned int m_size;
	unsigned int m_curPBOIndex;
	unsigned int m_maxWaitForGPUUploadSteps;
	void upladeDataToPBO(const Spin_Picture* pic, GLubyte* ptr);
	GLuint m_glName[3];
	GLuint m_pboIds[NUMBER_PBO];
	void enablePBO();
};

#endif