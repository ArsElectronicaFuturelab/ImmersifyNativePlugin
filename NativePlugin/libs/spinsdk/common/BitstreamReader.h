/**
 * \file    BitstreamReader.h
 * \brief
 * \author  chi@spin-digital.com
 * \date    29/06/2016
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2016-2017, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */


#ifndef BitstreamReader_H
#define BitstreamReader_H

#include <cstring>
#include <future>

// only transport streams with a fixed packet size of 188 are supported
#define TS_PACKET_SIZE 188

// the buffersize has to be a multiple of the TS packet size for proper TS parsing
#ifdef _WIN32
//5MB buffer needed in windows to get good enough file reading bandwidth
#define BUFSIZE 27000 * TS_PACKET_SIZE
#else
#define BUFSIZE 5000 * TS_PACKET_SIZE
#endif

#define PADDING 0

class BitstreamReader
{

private:
  std::istream& m_Input; /* Input stream to read from */  
  uint8_t* m_aBuffer[2];
  int m_iBufSize;
  bool m_bReadAhead;
  int m_curBufIdx;
  int m_ReadSize[2];
  uint8_t* m_pBuffer;
  int m_BufferIdx;
  int m_State;
  uint64_t m_streamOffset;
  std::future<int> m_future;
  bool m_bReadInflight;
public:
  /**
   * Create a bytestream reader that will extract bytes from
   * istream.
   *
   * NB, it isn't safe to access istream while in use by a
   * BitstreamReader.
   *
   * Side-effects: the exception mask of istream is set to eofbit
   */
  BitstreamReader(std::istream& istream, int bufSize = BUFSIZE, bool bReadAhead = true)
    : m_Input(istream)
    , m_iBufSize (bufSize)
    , m_bReadAhead (bReadAhead)
    , m_BufferIdx(0)
    , m_State(0)
    , m_streamOffset(0)
    , m_bReadInflight(false)
  {
	  m_aBuffer[0] = new uint8_t[bufSize + PADDING];
	  m_aBuffer[1] = new uint8_t[bufSize + PADDING];
    ::memset(m_aBuffer[0], 0, bufSize +PADDING);
	  ::memset(m_aBuffer[1], 0, bufSize + PADDING);

    m_ReadSize[0] = 0;
    m_ReadSize[1] = 0;

    readChunk(0);
    if ( bReadAhead ){
      if (!m_Input.eof()) {
        m_future = std::async (std::launch::async, &BitstreamReader::readChunk, this, 1 );
        m_bReadInflight = true;
      }
      else {
        m_ReadSize[1] = 0;
      }
	  }
    m_curBufIdx = 0;
	  m_pBuffer = m_aBuffer[m_curBufIdx];    
  }

  ~BitstreamReader()
  {
    if (m_bReadInflight) {
      m_future.wait();
      m_bReadInflight = false;
    }
	  if (m_aBuffer[0] != NULL) {
	    delete[] m_aBuffer[0];
	  }
	  if (m_aBuffer[1] != NULL) {
      delete[] m_aBuffer[1];
    }
	  m_aBuffer[0] = NULL;
	  m_aBuffer[1] = NULL;
	  m_pBuffer = NULL;
  }

  int readChunk(int i)
  {
    m_Input.read((char*)m_aBuffer[i], m_iBufSize);

    m_ReadSize[i] = (int)m_Input.gcount();
    return m_ReadSize[i];
  }

  void readNextChunk()
  {
    if ( m_bReadAhead ) {
      m_streamOffset += m_ReadSize[m_curBufIdx];
      if (m_bReadInflight) {
        m_future.wait();
        m_bReadInflight = false;
      }
      if (!m_Input.eof()) {
        m_future = std::async(std::launch::async, &BitstreamReader::readChunk, this, m_curBufIdx);
        m_bReadInflight = true;
      } else {
        m_ReadSize[m_curBufIdx] = 0;
      }

      m_curBufIdx = (m_curBufIdx + 1) % 2;
      m_pBuffer = m_aBuffer[m_curBufIdx];
      m_BufferIdx = 0;
    } else {
      m_streamOffset += m_ReadSize[m_curBufIdx];
      if (!m_Input.eof()) {
        readChunk(0);
        m_BufferIdx = 0;
      }
    }
  }

  bool isEOF()
  {
    if ( m_bReadAhead ){
      return m_Input.eof() && (m_ReadSize[m_curBufIdx] ==0);
    } else {
      return m_Input.eof();
    }
  }

public:      

  bool byteStreamNALUnit(uint8_t*& nalUnit, int& size, uint64_t& streamOffset);

  bool tsStreamAU(int pid, uint8_t*& tsbuf, int& size, uint64_t& streamOffset, bool& isRandomAccessPoint);

  uint64_t getAbsOffset() { return m_streamOffset + m_BufferIdx; }  

};



#endif
