#include "tsCommon.h"
#include "tsTransportStream.h"
#include <fstream>
#include <iostream>

//=============================================================================================================================================================================
const bool print = false;
const bool printAF = false;
const bool printPES = true;

int main(int argc, char *argv[ ], char *envp[ ])
{ 
  // TODO - open file   V
  std::fstream plik;
  plik.open("build/example_new.ts", std::ios::binary | std::ios::in);   
  if(!plik.good()){
    std::cout << "nie mozna odczytac pliku" << std::endl;
    return 0;
  }   
  
  // TODO - check if file if opened     V
  if(plik.good() and plik.is_open()){
      xTS_PacketHeader TS_PacketHeader;
      xTS_AdaptationField TS_AdaptationField;
      xPES_PacketHeader PES_PacketHeader;
      xPES_Assembler PES_Assembler;

      int32_t TS_PacketId = 0;
      while(/*not eof*/ !plik.eof() 
      //&& TS_PacketId != 30
      )
      {
        // TODO - read from file          V
        char output[188] = {0};       
        plik.read(output, 188);         
        
        uint8_t* str = new uint8_t[188];
        str[0] = uint8_t(output[0]);      

        for(int i(1);i<188;i++){
          str[i] = uint8_t(output[i]);
        }
        TS_PacketHeader.Reset();
        TS_AdaptationField.Reset();
        PES_PacketHeader.Reset();

        if(output[0]==0x47){      
          //  Parsing
          TS_PacketHeader.Parse(str);
          TS_AdaptationField.Parse(str,TS_PacketHeader.getAFC()); 
          if(TS_PacketHeader.hasPayload() and TS_PacketHeader.getPID() == 136 and TS_PacketHeader.getS()){
            PES_PacketHeader.Reset();
            PES_PacketHeader.Parse(str, TS_PacketHeader, TS_AdaptationField);
          }
          //  Printing
          if(TS_PacketHeader.getPID() == 136){
            if(print){
              printf("%010d ", TS_PacketId);
              TS_PacketHeader.Print();
              if(printAF) TS_AdaptationField.Print();
            }
            if(printPES){
              if(print) PES_PacketHeader.Print();
              switch(PES_Assembler.AbsorbPacket(str, TS_PacketHeader, TS_AdaptationField)){
                case xPES_Assembler::eResult::StreamPackedLost:{
                  std::cout << " Packet lost! ";
                  break;  
                }
                case xPES_Assembler::eResult::UnexpectedPID:{
                  std::cout << " Unexpected PID! ";
                  break;
                } 
                case xPES_Assembler::eResult::AssemblingStarted:{
                  std::cout << " Started ";
                  std::cout << "PES: Len=" << int(PES_Assembler.getNumPacketBytes());
                  if(PES_PacketHeader.getPacketStartCodePrefix()) PES_Assembler.PrintPESH();
                  break;  
                }
                case xPES_Assembler::eResult::AssemblingContinue:{
                  std::cout << " Continue ";
                  std::cout << "PES: Len=" << int(PES_Assembler.getNumPacketBytes());
                  break; 
                }
                case xPES_Assembler::eResult::AssemblingFinished:{
                  std::cout << " Finished ";
                  std::cout << "PES: Len=" << int(PES_Assembler.getNumPacketBytes());
                  break;
                }
                default:{
                  break;
                }
              }
            }
            printf("\n");
          }
            
        }
        TS_PacketId++;
      }
      // TODO - close file    V
      plik.close();
      
      return EXIT_SUCCESS;
    }
    else{
      return EXIT_FAILURE;
    }
}
//=============================================================================================================================================================================
