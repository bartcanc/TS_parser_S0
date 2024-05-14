#include "tsTransportStream.h"
#include "tsCommon.h"
#include <iostream>
#include <bitset>
#include <iomanip>
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
int32_t xTS_PacketHeader::Parse(const uint8_t* Input)    //funkcja przestała działac... (nie zmienilem w niej nic)
{
  //std::cout << *Input << std::endl;
  // uint32_t* HP = (uint32_t*)Input;
  // uint32_t Head = xSwapBytes32(*HP);
  uint32_t Head = 0;
  for(int i(0);i<4;i++){
    //std::cout << "input " << i << " " << int(Input[i]) << std::endl;
    Head = Head | Input[i];
    if(i!=3){
      Head = Head << 8;
    }
    //std::cout << "Head = " << Head << std::endl;
  }
  Head = xSwapBytes32(Head);

  //std::cout << Head << std::endl;
  if(Head){
    m_SB = m_SB|Head;     //SB          V
  }
  //std::cout << "w funkcji " << int(m_SB) << std::endl;
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

// ---------AF------------------------------------------------

/// @brief Reset - reset all TS packet header fields
void xTS_AdaptationField::Reset()
{
//setup
  m_AdaptationFieldControl=0;
  m_AdaptationFieldLength=0;
  DC=0; 
  RA=0; 
  SP=0; 
  PR=0; 
  OR=0; 
  SF=0; 
  TP=0; 
  EX=0;

  PCRB=0;                    
  R=0;                      
  PCRE=0;                       
  //OR = 1
  OPCRB=0;                     
  R=0;                           
  OPCRE=0;                       
  Stuffing=0;
}
/**
@brief Parse adaptation field
@param PacketBuffer is pointer to buffer containing TS packet
@param AdaptationFieldControl is value of Adaptation Field Control field of
corresponding TS packet header
@return Number of parsed bytes (length of AF or -1 on failure)
*/
int32_t xTS_AdaptationField::Parse(const uint8_t* PacketBuffer, uint8_t AdaptationFieldControl)
{
//parsing
  m_AdaptationFieldControl=AdaptationFieldControl;
  if(m_AdaptationFieldControl == 2 or m_AdaptationFieldControl == 3){
    m_AdaptationFieldLength = PacketBuffer[4];
    //std::cout << std::endl;
    //std::cout <<  int(PacketBuffer[4]) << std::endl;
    uint8_t mask = 0b10000000;
    //std::cout << std::endl;
    DC = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(DC) << std::endl;

    RA = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(RA) << std::endl;

    SP = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(SP) << std::endl;

    PR = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(PR) << std::endl;

    OR = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(OR) << std::endl;

    SF = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(SF) << std::endl;

    TP = PacketBuffer[5] & mask;
    mask = mask >> 1;
    //std::cout << int(TP) << std::endl;

    EX |= PacketBuffer[5] & mask;
    mask = mask >> 1;
    //td::cout << int(EX) << std::endl;

    Stuffing = m_AdaptationFieldLength - 1;

    if(PR){   // nastepne 48 bit - 6 bajt
      //wyliczenie pól
      //PCRB = PCRB | (0b10000000 & PacketBuffer[10]);
      for(int i=0;i<4;i++){
        // std::bitset<8> x(PacketBuffer[6+i]);
        PCRB = PCRB | PacketBuffer[6+i];
        // std::cout << "PacketBuffer[" << 6+i << "] = " << x << std::endl;
        if(i!=3){
          PCRB = PCRB << 8;
        }
        else{
          PCRB = PCRB << 1;
        }
      }
      PCRB = PCRB | ((0b10000000 & PacketBuffer[10])?1:0);
      // std::bitset<8> x(PacketBuffer[10]);
      // std::cout << "PacketBuffer[" << 10 << "] = " << x << std::endl;
      //std::cout << int(PacketBuffer[10]) << std::endl;
      R = R | ((0b01111110 & PacketBuffer[10]) >> 1);
      //std::cout << int(R) << std::endl;
      PCRE = PCRE | (1 & PacketBuffer[10]);
      PCRE = PCRE << 8;
      PCRE = PCRE | PacketBuffer[11];
      // std::bitset<8> y(PacketBuffer[11]);
      // std::cout << "PacketBuffer[" << 11 << "] = " << y << std::endl;
      Stuffing=0;
    }
    else if(OR){
      //wyliczenie pól
      for(int i=0;i<4;i++){
        OPCRB = OPCRB | PacketBuffer[6+i];
        if(i!=3){
          OPCRB = OPCRB << 8;
        }
        else{
          OPCRB = OPCRB << 1;
        }
      }
      OPCRB = OPCRB | (((0b10000000 & PacketBuffer[10])>0)?1:0);
      R2 = R2 | (0b01111110 & PacketBuffer[10]);
      
      OPCRE = OPCRE | (1 & PacketBuffer[10]);
      OPCRE = OPCRE << 8;
      OPCRE = OPCRE | PacketBuffer[11];
      

      Stuffing=0;
    }
    return 4;
  }
  return -1;
}
/// @brief Print all TS packet header fields
void xTS_AdaptationField::Print() const
{
//print print print
if(m_AdaptationFieldControl == 2 or m_AdaptationFieldControl == 3){
  std::cout << " AF: L=" << int(m_AdaptationFieldLength) << " DC=" << int(DC) << " RA=" << int(RA) << " SP=" << int(SP) << " PR=" << int(PR) << " OR=" << int(OR) << " SF=" << int(SF) << " TP=" << int(TP) << " EX=" << int(EX);
  if(PR == 1){
    float time = (float)((PCRB*300)+PCRE)/27000000;
    std::cout << " PCR=" << (PCRB*300)+PCRE << " (Time=" << time << "s)";
  }
  else if (OR == 1){
    std::cout << " OPCR=" /*<< obliczone cos tam*/;
  }
  std::cout << " Stuffing=" << int(Stuffing);
}
}

// xPES_PacketHeader----------------------------------------------
void xPES_PacketHeader::Reset(){
  m_PacketStartCodePrefix=0;           // 24 bit__________3 bytes
  m_StreamId=0;                        // 8 bit___________1 byte
  m_PacketLength=0;                    // 16 bit_________2 bytes
                                       // 2 bit
  m_PES_Scrambling_Control = 0;        // 2 bit
  m_PES_Priority = 0;                  // 1 bit
  m_Data_Alignment_Indicator = 0;      // 1 bit
  m_Copyright = 0;                     // 1 bit
  m_Original_Or_Copy = 0;              // 1 bit___________1 byte
  m_PTS_DTS_Flags = 0;                 // 2 bit
  m_ESCR_Flag = 0;                     // 1 bit
  m_ES_Rate_Flag = 0;                  // 1 bit
  m_DSM_Trick_Mode_Flag = 0;           // 1 bit
  m_Additional_Copy_Info_Flag = 0;     // 1 bit
  m_PES_CRC_Flag = 0;                  // 1 bit
  m_PES_Extension_Flag = 0;            // 1 bit___________1 byte
  m_PES_Header_Data_Length = 0;        // 8 bit___________1 byte
}

int32_t xPES_PacketHeader::Parse(const uint8_t* Input, xTS_PacketHeader TS_PacketHeader, xTS_AdaptationField TS_AdaptationField){
  whereToStart=4;
  if(TS_PacketHeader.getAFC()){
    whereToStart+=1;
    whereToStart+=TS_AdaptationField.getAdaptationFieldLength();
    //std::cout << "WTD = " << whereToStart << std::endl;
  }
  // zaczynamy parsowanie
  // header ma zawsze 6 bajtów
  // pierwsze 3 bajty - start code (0000 0000 | 0000 0000 | 0000 0001)
  // 4 bajt - stream ID ()
  // 5 i 6 bajt - długość pakietu PES (analogicznie jak w Adaptation Field)
  if(Input[whereToStart] == 0 and Input[whereToStart+1] == 0 and Input[whereToStart+2] == 1){
    m_PacketStartCodePrefix=1;
    m_StreamId = Input[whereToStart+3];
    m_PacketLength += Input[whereToStart+4];
    m_PacketLength = m_PacketLength << 8;
    m_PacketLength += Input[whereToStart+5];

    switch(m_StreamId){
      case eStreamId_program_stream_map: break;
      case eStreamId_padding_stream: break;
      case eStreamId_private_stream_2: break;
      case eStreamId_ECM: break;
      case eStreamId_EMM : break;
      case eStreamId_program_stream_directory: break;
      case eStreamId_DSMCC_stream: break;
      case eStreamId_ITUT_H222_1_type_E: break;
      default:{
        // zczytujemy opcjonalne pola
        if((Input[whereToStart+6]&0b10000000) and !(Input[whereToStart+6]&0b01000000)){
          m_PES_Scrambling_Control = (m_PES_Scrambling_Control | (Input[whereToStart+6]&0b00110000)) >> 4;     // 2 bit
          m_PES_Priority = (Input[whereToStart+6]&0b00001000);                  // 1 bit
          m_Data_Alignment_Indicator = (Input[whereToStart+6]&0b00000100);      // 1 bit
          m_Copyright = (Input[whereToStart+6]&0b00000010);                     // 1 bit
          m_Original_Or_Copy = (Input[whereToStart+6]&0b00000001);              // 1 bit
          m_PTS_DTS_Flags = (m_PTS_DTS_Flags | (Input[whereToStart+7]&0b11000000)) >> 6;              // 2 bit
          m_ESCR_Flag = (Input[whereToStart+7]&0b00100000);                     // 1 bit
          m_ES_Rate_Flag = (Input[whereToStart+7]&0b00010000);                  // 1 bit
          m_DSM_Trick_Mode_Flag = (Input[whereToStart+7]&0b00001000);           // 1 bit
          m_Additional_Copy_Info_Flag = (Input[whereToStart+7]&0b00000100);     // 1 bit
          m_PES_CRC_Flag = (Input[whereToStart+7]&0b00000010);                  // 1 bit
          m_PES_Extension_Flag = (Input[whereToStart+7]&0b00000001);            // 1 bit
          m_PES_Header_Data_Length = Input[whereToStart+8];     // 8 bit
        }
        break;
      }
    } 
  }
  else{
    return -1;
  }
  return 1;
}

void xPES_PacketHeader::Print() const{
  std::cout << " PES: PSCP=" << int(m_PacketStartCodePrefix) << " SID=" << int(m_StreamId) << " L=" << m_PacketLength;
}


// xPES_Assembler-------------------------------------------------
void xPES_Assembler::Init (int32_t PID){
  m_PID = PID;
  m_LastContinuityCounter = 0;
  m_BufferSize = 0;
  m_DataOffset = 0;
}
xPES_Assembler::eResult xPES_Assembler::AbsorbPacket(const uint8_t* TransportStreamPacket, xTS_PacketHeader PacketHeader, xTS_AdaptationField AdaptationField){
  if(PacketHeader.getS()) Init(PacketHeader.getPID()); 
  m_PESH.Parse(TransportStreamPacket, PacketHeader, AdaptationField);
  switch(m_PID){
      case xPES_PacketHeader::eStreamId::eStreamId_program_stream_map: return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_padding_stream: return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_private_stream_2: return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_ECM: return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_EMM : return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_program_stream_directory: return xPES_Assembler::eResult::UnexpectedPID; 
      case xPES_PacketHeader::eStreamId::eStreamId_DSMCC_stream: return xPES_Assembler::eResult::UnexpectedPID;
      case xPES_PacketHeader::eStreamId::eStreamId_ITUT_H222_1_type_E: return xPES_Assembler::eResult::UnexpectedPID;
      default: break;
  }

  if(PacketHeader.getCC() == 0){
    m_LastContinuityCounter = PacketHeader.getCC();
    xBufferReset();
    // std::cout <<std::endl;
    // std::cout << "size = " << m_PESH.getWTS() << " header length = " << int(m_PESH.getPES_HDL()) << std::endl;
    xBufferAppend(TransportStreamPacket, m_PESH.getWTS());
    return xPES_Assembler::eResult::AssemblingStarted;
  }
  else if(PacketHeader.getCC() == 15){
    // std::cout <<std::endl;
    // std::cout << "size = " << m_PESH.getWTS() << " header length = " << int(m_PESH.getPES_HDL()) << std::endl;
    xBufferAppend(TransportStreamPacket, m_PESH.getWTS());
    m_LastContinuityCounter = PacketHeader.getCC();
    return xPES_Assembler::eResult::AssemblingFinished;
  }
  else if(m_LastContinuityCounter == (PacketHeader.getCC()-1)){
    // std::cout <<std::endl;
    // std::cout << "size = " << m_PESH.getWTS() << " header length = " << int(m_PESH.getPES_HDL()) << std::endl;
    xBufferAppend(TransportStreamPacket, m_PESH.getWTS()-1);
    m_LastContinuityCounter = PacketHeader.getCC();
    return xPES_Assembler::eResult::AssemblingContinue;
  }
  else{
    return xPES_Assembler::eResult::StreamPackedLost;
  }
}

void xPES_Assembler::xBufferReset (){
  m_Buffer = {0};
}
void xPES_Assembler::xBufferAppend(const uint8_t* Data, int32_t Size){
  /*TODO: popraw to bo chyba jest zle (zle data offset jest)*/
  if(m_LastContinuityCounter == 0){
    m_BufferSize = m_PESH.getPacketLength();
    if(m_BufferSize) m_Buffer = new uint8_t[m_BufferSize+6];
  }
  if(m_BufferSize){
for(int i=Size;i<188;i++){
    //std::cout << m_DataOffset << " " << Size << std::endl;
    m_Buffer[m_DataOffset++] = Data[i];
  } 
  }
  
}

//=============================================================================================================================================================================