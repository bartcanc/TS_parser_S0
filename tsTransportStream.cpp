#include "tsTransportStream.h"
#include "tsCommon.h"
#include <iostream>
//=============================================================================================================================================================================
// xTS_PacketHeader
//=============================================================================================================================================================================


/// @brief Reset - reset all TS packet header fields
void xTS_PacketHeader::Reset()
{
  m_SB=0x0;       //8 bit           zawsze 71
  m_E=0x0;        //1 bit           0-1
  m_S=0x0;        //1 bit           0-1
  m_T=0x0;        //1 bit           0-1
  m_PID=0x0;      //13 bit (5+8)    0-8191
  m_TSC=0x0;      //2 bit           0-3
  m_AFC=0x0;      //2 bit           0-3
  m_CC=0x0;       //4 bit           0-15
}

/**
  @brief Parse all TS packet header fields
  @param Input is pointer to buffer containing TS packet
  @return Number of parsed bytes (4 on success, -1 on failure) 
 */
int32_t xTS_PacketHeader::Parse(const uint32_t* Input)    //funkcja przestała działac... (nie zmienilem w niej nic)
{
  //std::cout << *Input << std::endl;
  uint32_t* HP = (uint32_t*)Input;
  uint32_t Head = xSwapBytes32(*HP);
  //std::cout << Head << std::endl;
  if(Head){
    m_SB = m_SB|Head;     //SB          V
  }
  //std::cout << "w funkcji " << m_SB << std::endl;
  if(m_SB == 0x47){
    Head = Head >> 8;
    if(Head){
      m_PID = (m_PID|Head)&31;
      m_PID = m_PID << 8;
    }
    Head = Head >> 5;

    if(Head){
      m_T = (m_T|Head)&1;
    }
    Head = Head >> 1;

    if(Head){
      m_S = (m_S|Head)&1;
    }
    Head = Head >> 1;

    if(Head){
      m_E = (m_E|Head)&1;
    }
    Head = Head >> 1;

    uint8_t temp = 0;
    temp = temp|Head;
    if(Head){
      m_PID = m_PID|temp;
    }
    Head = Head >> 8;

    if(Head){
      m_CC = (m_CC|Head)&15;  //1111
    }
    Head = Head >> 4;

    if(Head){
      m_AFC = (m_AFC|Head)&3; //11
    }
    Head = Head >> 2;

    if(Head){
      m_TSC = (m_TSC|Head)&3; //11
    }
    Head = Head >> 2;

    return 4;             //SUCCESS
  }
    return -1;              //FAILURE
}

/// @brief Print all TS packet header fields
void xTS_PacketHeader::Print() const
{
  //print sth         V
  //if(m_SB == 255){
  //std::cout << "blad" << std::endl;
  //if(m_SB == 0x47){
    std::cout << " TS: SB=" << int(m_SB) << " E=" << int(m_E) << " S=" << int(m_S) << " T=" << int(m_T) << " PID=" << int(m_PID) << " TSC=" << int(m_TSC) << " AF=" << int(m_AFC) << " CC=" << int(m_CC);
  //}
  // trzeba rzutowac bo z jakiegos powodu nie mozna wypisac uint8_t normalnym sposobem
}



//=============================================================================================================================================================================
