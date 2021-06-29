#include "glTextureAccess.h"
//The PBO implementation part of this class is based on http://www.songho.ca/opengl/gl_pbo.html. More information about implementing and using PBOs: http://www.songho.ca/opengl/gl_pbo.html

///***********************************************************************************************/
DWORD WINAPI UploadThread_1(void* Param)
{
	GlTextureAccess* _THIS = (GlTextureAccess*)Param;
	while (_THIS->m_run)
	{
		if (_THIS->m_shouldWaitForGpuUpload_1)
		{
			const Spin_Plane* planeY = &_THIS->params.pic->asPlanes[0];
			const unsigned char *dataY = reinterpret_cast<unsigned char*>(planeY->pPlane);
			int ySize = planeY->iHeight * planeY->iWidth * 8;
			unsigned int offsetDiv = 1; //for 4:4:4 and 4:2:0 we upload the entire Ydata to the pbo
			
			if (_THIS->getChromaSubSampling() == _420) //chroma subsampling is not 422 or 444
			{
				offsetDiv = 2;
			}
			
			if (!_THIS->getPictureIsStrided())
			{
				std::memcpy(_THIS->params.ptr, dataY, ySize / offsetDiv);
			}
			else
			{
				GLubyte* ptrTmp = _THIS->params.ptr;
				for (unsigned int i = 0; i < planeY->iHeight / offsetDiv; i++)
				{
					std::memcpy(ptrTmp, dataY + i * planeY->iStride * 8, planeY->iWidth * 8);
					ptrTmp += planeY->iWidth * 8;
				}
			}
			_THIS->m_shouldWaitForGpuUpload_1 = false;
		}
	}
	return 0;
}

//
///***********************************************************************************************/
DWORD WINAPI UploadThread_2(void* Param)
{
	GlTextureAccess* _THIS = (GlTextureAccess*)Param;
	while (_THIS->m_run)
	{
		if (_THIS->m_shouldWaitForGpuUpload_2)
		{
			//this thread is only used for 4:2:0 and 4:4:4. For 4:2:2 we do not use this thread. Read more about it in the comments above.

			const Spin_Plane* planeY = &_THIS->params.pic->asPlanes[0];
			int ySize = planeY->iHeight * planeY->iWidth * 8;
			if (!_THIS->getPictureIsStrided())
			{
				if (_THIS->getChromaSubSampling() == _420)
				{
					const unsigned char *dataY = reinterpret_cast<unsigned char*>(planeY->pPlane);
					std::memcpy(_THIS->params.ptr + ySize / 2, dataY + ySize / 2, ySize / 2);
				}
				else if (_THIS->getChromaSubSampling() == _444) //if chroma subsampling is not 422, use this thread for the third componend (cb) of the frame
				{
					const Spin_Plane* planeCb = &_THIS->params.pic->asPlanes[1];
					const unsigned char *dataCb = reinterpret_cast<unsigned char*>(planeCb->pPlane);
					int cbSize = planeCb->iHeight * planeCb->iWidth * 8;
					std::memcpy(_THIS->params.ptr + ySize, dataCb, cbSize);
				}
			}
			else
			{
				if (_THIS->getChromaSubSampling() == _420)
				{
					const unsigned char *dataY = reinterpret_cast<unsigned char*>(planeY->pPlane);
					GLubyte* ptrTmp = _THIS->params.ptr + ySize / 2;
					for (int i = planeY->iHeight / 2; i < planeY->iHeight; i++)
					{
						std::memcpy(ptrTmp, dataY + i * planeY->iStride * 8, planeY->iWidth * 8);
						ptrTmp += planeY->iWidth * 8;
					}
				}
				else if (_THIS->getChromaSubSampling() == _444) //chroma subsampling is not 422 or 444
				{
					const Spin_Plane* planeCb = &_THIS->params.pic->asPlanes[1];
					const unsigned char *dataCb = reinterpret_cast<unsigned char*>(planeCb->pPlane);
					int cbSize = planeCb->iHeight * planeCb->iWidth * 8;
					GLubyte* ptrTmp = _THIS->params.ptr + ySize;
					for (int i = 0; i < planeCb->iHeight; i++)
					{
						std::memcpy(ptrTmp, dataCb + i * planeCb->iStride * 8, planeCb->iWidth * 8);
						ptrTmp += planeCb->iWidth * 8;
					}
				}
			}
			_THIS->m_shouldWaitForGpuUpload_2 = false;
		}
	}
	return 0;
}


/***********************************************************************************************/
GlTextureAccess::GlTextureAccess(uintptr_t texturePtr[], unsigned int width, unsigned int height, CHROMA_SUBSAMPLING chroma_subsampling) : BaseTextureAccess()
{
	m_width = width;
	m_height = height;
	m_chroma_subsampling = chroma_subsampling;
	m_ready = false;
	m_picIsStrided = true;
	m_shouldWaitForGpuUpload_1 = false;
	m_shouldWaitForGpuUpload_2 = false;
	m_glName[0] = (GLuint)texturePtr[0];
	m_glName[1] = (GLuint)texturePtr[1];
	m_glName[2] = (GLuint)texturePtr[2];
	m_size = (m_width * m_height) / 2; 
    if (m_chroma_subsampling == _420){
		m_size += (m_width / 2) * (m_height / 2);
	}
	else if (m_chroma_subsampling == _422)
	{
		m_size += m_width / 2 * m_height;
	}
	else if (m_chroma_subsampling == _444)
	{
		m_size += m_width * m_height;
	}
	m_curPBOIndex = 0;
	thandle_1 = CreateThread(NULL, 0, UploadThread_1, (void*)this, 0, &threadid_1);
	if (m_chroma_subsampling == _420 || getChromaSubSampling() == _444)
	{
		thandle_2 = CreateThread(NULL, 0, UploadThread_2, (void*)this, 0, &threadid_2);
	}

	//max wait before dropping the frame 5 sec, for avoiding dead loops? 
	setMaxWaitForGPUUpload(100);
}

/***********************************************************************************************/
GlTextureAccess::~GlTextureAccess()
{
	glDeleteBuffers(NUMBER_PBO, m_pboIds);
	for (int i = 0; i < NUMBER_PBO; i++)
	{
		m_pboIds[i] = NULL;
	}

	m_pboReady = false;
	m_run = false;
	WaitForSingleObject(thandle_1, INFINITE);
	CloseHandle(thandle_1);

	if (getChromaSubSampling() == _420 || getChromaSubSampling() == _444)
	{
		WaitForSingleObject(thandle_2, INFINITE);
		CloseHandle(thandle_2);
	}
}

/***********************************************************************************************/
void GlTextureAccess::enablePBO()
{
	if (m_pboReady)
		return;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glGenBuffers(NUMBER_PBO, m_pboIds);
	m_pboReady = true;
}

/***********************************************************************************************/
void GlTextureAccess::upladeDataToPBO(const Spin_Picture* pic, GLubyte* ptr)
{
	/*
	upload pixeldata to the GPU. The idea is to devide the data into equal parts (for 4:2:0 and 4:4:4 we use three parts, and for 4:2:2 two parts) and use three or two threads for those parts
	for 4:2:0 
	Ydata first half (thread 1)
	Ydata second half (thread 2)
	Cb +  Cr (main thread)

	for 4:2:2
	entire Ydata (thread 1)
	Cb + Cr (main thread)

	for 4:4:4
	ydata (thread 1)
	cbData (thread 2)
	crData (main thread)
	*/
	const Spin_Plane *planeY = &pic->asPlanes[0];
	const Spin_Plane *planeCb = &pic->asPlanes[1];
	const Spin_Plane *planeCr = &pic->asPlanes[2];
	const unsigned char *dataCb = reinterpret_cast<unsigned char*>(planeCb->pPlane);
	const unsigned char *dataCr = reinterpret_cast<unsigned char*>(planeCr->pPlane);
	//cout << planeY->iWidth << " " << planeCb->iWidth << " " << planeCr->iWidth << " " << planeY->iHeight << " " << planeCb->iHeight << " " << planeCr->iHeight << endl;

	int ySize = planeY->iHeight * planeY->iWidth * 8;

	if (m_shouldWaitForGpuUpload_1 || m_shouldWaitForGpuUpload_2)
	{
		cout << "frame dropped.\n";
		//frame dropping
		return;
	}
	params.pic = pic;
	params.ptr = ptr;
	m_shouldWaitForGpuUpload_1 = true;
	m_shouldWaitForGpuUpload_2 = getChromaSubSampling() == _420 || getChromaSubSampling() == _444;// || for 4:2:0 we use three threads. No need for waiting for this threat
	if (!m_picIsStrided)
	{
		if (m_chroma_subsampling == _420)
		{
			//_THIS->params.ptr, dataY, ySize/2 in thread 1
			int cbSize = planeCb->iHeight * planeCb->iWidth * 8;
			std::memcpy(ptr + ySize, dataCb, cbSize);
			std::memcpy(ptr + ySize + cbSize, dataCr, cbSize);
		}
		else if (m_chroma_subsampling == _422) {
			std::memcpy(ptr + ySize, dataCb, (ySize / 2));
			std::memcpy(ptr + ySize + ySize / 2, dataCr, ySize / 2);
		}
		else if (m_chroma_subsampling == _444) {
			int cbSize = planeCb->iHeight * planeCb->iWidth * 8;
			int crSize = planeCr->iHeight * planeCr->iWidth * 8;
			std::memcpy(ptr + ySize + cbSize, dataCr, crSize);
			
		}
	}
 	else
	{
		if (m_chroma_subsampling == _420)
		{
			GLubyte* ptrTmp = ptr + planeY->iWidth * 8 * planeY->iHeight;
			for (int i = 0; i < planeCb->iHeight; i++)
			{
				std::memcpy(ptrTmp, dataCb + i * planeCb->iStride * 8, planeCb->iWidth * 8);
				ptrTmp += planeCb->iWidth * 8;
			}

			for (int i = 0; i < planeCr->iHeight; i++)
			{
				std::memcpy(ptrTmp, dataCr + i * planeCr->iStride * 8, planeCr->iWidth * 8);
				ptrTmp += planeCr->iWidth * 8;
			}
		}
		else if (m_chroma_subsampling == _422)
		{
			GLubyte* ptrTmp = ptr + planeY->iWidth * 8 * planeY->iHeight;
			for (int i = 0; i < planeCb->iHeight; i++)
			{
				std::memcpy(ptrTmp, dataCb + i * planeCb->iStride * 8, planeCb->iWidth * 8);
				ptrTmp += planeCb->iWidth * 8;
			}

			for (int i = 0; i < planeCr->iHeight; i++)
			{
				std::memcpy(ptrTmp, dataCr + i * planeCr->iStride * 8, planeCr->iWidth * 8);
				ptrTmp += planeCr->iWidth * 8;
			}
		}
		else {

			GLubyte* ptrTmp = ptr + planeY->iWidth * 8 * planeY->iHeight + planeCb->iWidth * 8 * planeCb->iHeight;
			for (int i = 0; i < planeCr->iHeight; i++)
			{
				std::memcpy(ptrTmp, dataCr + i * planeCr->iStride * 8, planeCr->iWidth * 8);
				ptrTmp += planeCr->iWidth * 8;
			}
		}
	}
}


/***********************************************************************************************/
void GlTextureAccess::apply()
{
	if (!m_glName[0])
		return;
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[m_curPBOIndex]);
	glBindTexture(GL_TEXTURE_2D, m_glName[0]);
	glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_COMPRESSED_RED_RGTC1, (m_width * m_height) / 2, (GLvoid*)0);

	if (m_chroma_subsampling == _420)
	{
		glBindTexture(GL_TEXTURE_2D, m_glName[1]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_COMPRESSED_RED_RGTC1, ((m_width / 2) * (m_height / 2)) / 2, (GLvoid*)((m_width * m_height) / 2));

		glBindTexture(GL_TEXTURE_2D, m_glName[2]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_COMPRESSED_RED_RGTC1, ((m_width / 2) * (m_height / 2)) / 2, (GLvoid*)((m_width / 2) * (m_height / 2) / 2 + (m_width * m_height) / 2));
	}
	else if (m_chroma_subsampling == _422)
	{
		glBindTexture(GL_TEXTURE_2D, m_glName[1]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height, GL_COMPRESSED_RED_RGTC1, (m_width / 2 * m_height) / 2, (GLvoid*)((m_width * m_height) / 2));

		glBindTexture(GL_TEXTURE_2D, m_glName[2]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height, GL_COMPRESSED_RED_RGTC1, (m_width / 2 * m_height) / 2, (GLvoid*)((m_width / 2 * m_height) / 2 + (m_width * m_height) / 2));
	}
	else if (m_chroma_subsampling == _444)
	{
		glBindTexture(GL_TEXTURE_2D, m_glName[1]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_COMPRESSED_RED_RGTC1, (m_width * m_height) / 2, (GLvoid*)((m_width * m_height) / 2));

		glBindTexture(GL_TEXTURE_2D, m_glName[2]);
		glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_COMPRESSED_RED_RGTC1, (m_width * m_height) / 2, (GLvoid*)((m_width * m_height)));
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

/***********************************************************************************************/
bool GlTextureAccess::applyPictureData(const Spin_Picture* pic)
{
	enablePBO();  //if using Unity3d we have to initialize PBOs in this Gl-context.
	if (m_ready)
	{
		unsigned int counter = 0;
		while ((m_shouldWaitForGpuUpload_1 || m_shouldWaitForGpuUpload_2) && counter < m_maxWaitForGPUUploadSteps)
		{
			counter++;
			Sleep(0.1);
		}

		if (counter == m_maxWaitForGPUUploadSteps)
		{
			cout << "timeout for GPU upload reached! \n";
			return false;
		}
		apply();
	}
	unsigned int nextPBOIndex = (m_curPBOIndex + 1) % NUMBER_PBO;
	//prepare the next buffer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pboIds[nextPBOIndex]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_size, 0, GL_STREAM_DRAW);   //m_size => sum of both texture.
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

	if (ptr)
	{
		upladeDataToPBO(pic, ptr);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		m_curPBOIndex = nextPBOIndex;
		m_ready = true;
		return true;
	}

	//because of some unknown reason, sometimes we get a GLError at this place right after the start. Reinitializing the PBOs fixes the problme.
	glDeleteBuffers(NUMBER_PBO, m_pboIds);
	m_pboReady = false;
	cout << "Warning: Got a GL Problem. PBO will be reinitialized..." << endl;
	return false;
}

/***********************************************************************************************/
void GlTextureAccess::clearBuffer()
{
	m_ready = false;
}