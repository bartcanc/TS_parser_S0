#include "tsCommon.h"
#include "tsTransportStream.h"
#include <fstream>
#include <iostream>

//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{ 
  // TODO - open file   V
  std::fstream plik;
  plik.open("build/example_new.ts", std::ios::binary | std::ios::in);   //na laptopie dopisuj "build/" bo nie dziala inaczej
  if(!plik.good()){
    std::cout << "nie mozna odczytac pliku" << std::endl;
    return 0;
  }   
  
  // TODO - check if file if opened     V
  if(plik.good() and plik.is_open()){
      xTS_PacketHeader TS_PacketHeader;
      xTS_AdaptationField TS_AdaptationField;
      xPES_PacketHeader PES_PacketHeader;

      int32_t TS_PacketId = 0;
      while(/*not eof*/ !plik.eof())
      {
        // TODO - read from file          V
        char output[188] = {0};       
        plik.read(output, 188);         // 4 bajty - 4*8 bitow = 32 bity
        
        uint8_t* str = new uint8_t[188];
        str[0] = uint8_t(output[0]);     // przypisujemy pierwszy bajt   

        for(int i(1);i<188;i++){
          // unsigned char dummy;
          // dummy = output[i];
          //   str = str << 8;             // przesuwamy liczbe o 1 bajt
          //   str = (str | dummy);    // przypisujemy następny bajt
            //std::cout << "polaczenie nr " << i << " " << str << std::endl;
          str[i] = uint8_t(output[i]);
        }
        TS_PacketHeader.Reset();
        TS_AdaptationField.Reset();
        PES_PacketHeader.Reset();
//        uint32_t* streamPrt = &str; 
        
        if(output[0]==0x47 && /*TS_PacketHeader.getPID() == 136 &&*/ TS_PacketId <190){      // po kilku przestaje wypisywac ,i cant...
          // std::cout << int(output[0]) << std::endl;
          // std::cout << int(output[1]) << std::endl;
          // std::cout << int(output[2]) << std::endl;
          // std::cout << int(output[3]) << std::endl;
          // std::cout << str << std::endl;
          // std::cout << int(output[0]) << std::endl;        sprawdzalem ocb i wyszlo ze ujemne liczby psują input

          // plan jest taki
          // - zczytujemy 32 bity do zmiennej           V
          // - parsujemy te bity                        V
          // - przesuwamy strumień o 156 bitów (to moze nie zadzialac bo payload niekoniecznie moze miec 156 bitow dlugosci) (jednak jest w programie statycznie ustawione ze jest 188)         
          // - powtarzamy az do konca pliku

          //             !!!! wazne info !!!!
          // pakiet zawsze zaczyna sie od liczby 47!!! (47 w hex!)
          // trzeba dac warunek ze jezeli spotkamy 47 to rozpoczynamy zczytywanie 32 bitów
          // potem pomijamy wszystko co nie zaczyna sie na 47

          // dobra jednak nie trzeba
          // wystarczy w parserze dac warunek ze zaczyna sie od 47
          
          //std::cout << str << std::endl;

          //  Parsing
          TS_PacketHeader.Parse(str);
          TS_AdaptationField.Parse(str,TS_PacketHeader.getAFC()); //TODO: spojrzec na to jeszcze bo chyba jest zle
          if(TS_PacketHeader.getPID() == 136){
            // przetwarzamy PES (występuje od razu po AF albo po headerze TS)
            //
            // co musi sie zgadzac?
            //
            // wartości CC maja byc ciagłe
            //
            // jezeli istnieje AF
            // to bedzie poczatek PESa - bufor[5+TS_AdaptationField.getAdaptationFieldLength()] (albo 4+...)
            // jezeli AF nie ma
            // poczatek PESa to: bufor[4]
            // pierwsze co robimy to sprawdzamy czy pierwsze 3 bajty to odpowiednio 0,0,1
            // jezeli sie zgadza, to na 1 bajcie zczytujemy stream_id 
            // potem na 2 bajtach mamy długość pakietu PES (sprawdz co znaczy jak jest zero) (tak samo jak w AF, dlugosc tego co jest po tym polu)
            // potem opcjonalny pakiet
            PES_PacketHeader.Parse(str, TS_PacketHeader, TS_AdaptationField);
          }

          //  Printing
          //if((TS_PacketHeader.getAFC()==2 or TS_PacketHeader.getAFC()==3) and TS_AdaptationField.getPR()){
            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();
            TS_AdaptationField.Print();
            PES_PacketHeader.Print();
            printf("\n");
          //}
        }

      // xPES_Assembler::eResult Result = PES_Assembler.AbsorbPacket(str, &TS_PacketHeader, &TS_AdaptationField);
      //   switch(Result)
      //   {
      //     case xPES_Assembler::eResult::StreamPackedLost : printf("PcktLost "); break;
      //     case xPES_Assembler::eResult::AssemblingStarted : printf("Started "); PES_Assembler.PrintPESH(); break;
      //     case xPES_Assembler::eResult::AssemblingContinue: printf("Continue "); break;
      //     case xPES_Assembler::eResult::AssemblingFinished: printf("Finished "); printf("PES: Len=%d", PES_Assembler.getNumPacketBytes()); break;
      //     default: break;
      //   }
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