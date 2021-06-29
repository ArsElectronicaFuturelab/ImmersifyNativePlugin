/**
 * \file    BitstreamReader.cpp
 * \brief   
 * \author  chi@spin-digital.com
 * \date    29/06/2016
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2016-2017, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */


#include <iostream>
#include <cassert>
#include "BitstreamReader.h"

using namespace std;

/**
 * Parse an AnnexB Bytestream bs to extract a single nalUnit
 * while accumulating bytestream statistics into stats.
 *
 * Returns false if EOF was reached (NB, nalunit data may be valid),
 *         otherwise true.
 */
bool BitstreamReader::byteStreamNALUnit( uint8_t*& nalbuf, int& size, uint64_t& streamOffset)
{
  int readsize = m_ReadSize[m_curBufIdx];
  streamOffset = m_streamOffset + m_BufferIdx;
  nalbuf = (uint8_t*)realloc(nalbuf, readsize - m_BufferIdx+8);
  int nalbufidx=0;
  if (m_BufferIdx>=0){
    m_State=0;
  }else {
    m_State = -m_BufferIdx;
    m_BufferIdx = 0;
  }
  while (1){
    if ( isEOF() && m_BufferIdx == readsize){
      size=nalbufidx;
      nalbuf[nalbufidx]   = 0; //allow 3 byte overread without valgrind complaint
      nalbuf[nalbufidx+1] = 0;
      nalbuf[nalbufidx+2] = 0;
      return true;
    } else if (m_BufferIdx == readsize){
      readNextChunk();
      readsize = m_ReadSize[m_curBufIdx];
      nalbuf = (uint8_t*)realloc(nalbuf, nalbufidx+readsize+8);
    }

    while (m_BufferIdx < readsize ){
      switch (m_State){
      case 0:
        if (m_pBuffer[m_BufferIdx] == 0)
          m_State =1;
        m_BufferIdx++;
        break;
      case 1:
        if (m_pBuffer[m_BufferIdx] == 0)
          m_State =2;
        else{
          m_State =0;
        }
        m_BufferIdx++;
        break;
      case 2:
        if (m_pBuffer[m_BufferIdx] == 1)
          m_State =3;
        else if (m_pBuffer[m_BufferIdx] > 1){
          m_State = 0;
        }
        m_BufferIdx++;
        break;
      case 3:
        while(m_BufferIdx < readsize -8){
          if (!((~*(const uint64_t*)(m_pBuffer+ m_BufferIdx) &
              (*(const uint64_t*)(m_pBuffer+ m_BufferIdx) - 0x0101010101010101ULL)) & 0x8080808080808080ULL)){
            *(uint64_t*)(nalbuf+nalbufidx) = *(const uint64_t*)(m_pBuffer+ m_BufferIdx);            
            nalbufidx+=8;
            m_BufferIdx+=8;

          } else {
            break;
          }
        }
        for (;m_BufferIdx < readsize;){
          if (m_pBuffer[m_BufferIdx]!=0){
            nalbuf[nalbufidx++] = m_pBuffer[m_BufferIdx++];
          }
          else{
            m_State= 4;
            m_BufferIdx++;
            break;
          }
        }
        break;
      case 4:
        if (m_pBuffer[m_BufferIdx]!=0){
          nalbuf[nalbufidx++] = 0;
          nalbuf[nalbufidx++] = m_pBuffer[m_BufferIdx++];
          m_State= 3;
        }else {
          m_BufferIdx++;
          m_State= 5;
        }
        break;
      case 5:
        if (m_pBuffer[m_BufferIdx]==1){
          m_BufferIdx-=2;
          nalbuf[nalbufidx]   = 0; //allow 3 byte overread without valgrind complaint
          nalbuf[nalbufidx+1] = 0;
          nalbuf[nalbufidx+2] = 0;
          size=nalbufidx;
          return false;
        }else if(m_pBuffer[m_BufferIdx] > 1){
          nalbuf[nalbufidx++] = 0;
          nalbuf[nalbufidx++] = 0;
          nalbuf[nalbufidx++] = m_pBuffer[m_BufferIdx];
          m_State= 3;
        }
        m_BufferIdx++;
        break;
      }
    }
  }
  assert(0);
  return false;
}

/**
* Parse transport stream to extract an access unit based on a given TS packet identifier (PID).
*
* Returns true if EOF was reached, otherwise false.
*/
bool BitstreamReader::tsStreamAU(int pid, uint8_t*& aubuf, int& size, uint64_t& streamOffset, bool& isRandomAccessPoint)
{
  int tsPayloadLength;
  int currPesPayloadLength;;
  int pesHeaderNeeded = true;

  int readsize = m_ReadSize[m_curBufIdx];
  streamOffset = m_streamOffset + m_BufferIdx;
  isRandomAccessPoint = false;
  aubuf = (uint8_t*)realloc(aubuf, readsize - m_BufferIdx + 8);
  int aubufidx = 0;

  while (1) {
    if (isEOF() && (m_BufferIdx + TS_PACKET_SIZE) > readsize) {
      // file ended and no packets remaining, return parsed AU
      size = aubufidx;
      return true;
    }
    else if ((m_BufferIdx + TS_PACKET_SIZE) > readsize) {
      // not enough data remaining in buffer, read next chunk
      readNextChunk();
      readsize = m_ReadSize[m_curBufIdx];
      aubuf = (uint8_t*)realloc(aubuf, aubufidx + readsize + 8);      
    }

    while ((m_BufferIdx + TS_PACKET_SIZE) <= readsize) {

      if (m_pBuffer[m_BufferIdx] != 0x47)
      {
        // this is not the start of a TS packet, continue search
        printf("Error reading TS packet: starts with %02x instead of %02x\n", m_pBuffer[m_BufferIdx], 0x47);
        m_BufferIdx++;
        continue;
      }

      // drop everything that is not the desired stream
      int currPid = ((m_pBuffer[m_BufferIdx + 1] & 0x1f) << 8) | m_pBuffer[m_BufferIdx + 2];
      if (currPid != pid) {
        m_BufferIdx += TS_PACKET_SIZE;
        continue;
      }

      int payloadUnitStartIndicator = (m_pBuffer[m_BufferIdx + 1] & 0x40) >> 6;
      int adaptationFieldControl = (m_pBuffer[m_BufferIdx + 3] & 0x30) >> 4;
      int adaptLength;

      if (payloadUnitStartIndicator && !pesHeaderNeeded) {
        // old unit ended and next unit would start, return parsed AU
        size = aubufidx;
        return false;
      }

      switch (adaptationFieldControl) {
      case 1:
        // contains only payload
        tsPayloadLength = TS_PACKET_SIZE - 4;
        m_BufferIdx += 4;
        break;
      case 2:
        // contains only adaption field        
        if (m_pBuffer[m_BufferIdx + 5] & 0x40) // randomAccessIndicator 
          isRandomAccessPoint = true;
        m_BufferIdx += TS_PACKET_SIZE;
        continue;
        break;
      case 3:
        // contains payload and adaption field
        adaptLength = m_pBuffer[m_BufferIdx + 4];
        if (m_pBuffer[m_BufferIdx + 5] & 0x40) //randomAccessIndicator
          isRandomAccessPoint = true;
        tsPayloadLength = TS_PACKET_SIZE - 5 - adaptLength;
        m_BufferIdx += 5 + adaptLength;
        break;
      default:
        // RESERVED
        m_BufferIdx += TS_PACKET_SIZE;
        continue;
      }

      if (tsPayloadLength <= 0) {
        printf("WARNING: empty TS payload \n");
        continue;
      }

      if (payloadUnitStartIndicator) {
        // new unit has started, now the PES header has to be handled
        pesHeaderNeeded = false;        
        int offset = m_pBuffer[m_BufferIdx + 8] + 9;
        m_BufferIdx += offset;
        currPesPayloadLength = tsPayloadLength - offset;
      }
      else
      {
        if (pesHeaderNeeded) {
          // no new unit, but PES header is still missing, so ignore incomplete data
          printf("TS Reader warning: skipping data without PES header \n");
          m_BufferIdx += tsPayloadLength;
          continue;
        }
        else {
          // still same unit, data contains only PES payload data without header
          currPesPayloadLength = tsPayloadLength;
        }
      }

      // copy the received part of access unit
      memcpy(&(aubuf[aubufidx]), &(m_pBuffer[m_BufferIdx]), currPesPayloadLength);
      aubufidx += currPesPayloadLength;
      m_BufferIdx += currPesPayloadLength;
    }

  }
  assert(0);
  return false;
}