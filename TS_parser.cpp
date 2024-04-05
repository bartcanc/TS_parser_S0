#include "tsCommon.h"
#include "tsTransportStream.h"
#include <fstream>
#include <iostream>

//=============================================================================================================================================================================

int main(int argc, char *argv[ ], char *envp[ ])
{ 
  // TODO - open file   V
  std::fstream plik;
  plik.open("build/example_new.ts", std::ios::binary | std::ios::in);
  
  // TODO - check if file if opened     V
  if(plik.good() and plik.is_open()){
      xTS_PacketHeader TS_PacketHeader;
      xTS_AdaptationField TS_AdaptationField;

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
//        uint32_t* streamPrt = &str; 
        
        if(output[0]==0x47 && TS_PacketId <34){      // po kilku przestaje wypisywac ,i cant...
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

          //  Printing
          //if((TS_PacketHeader.getAFC()==2 or TS_PacketHeader.getAFC()==3) and TS_AdaptationField.getPR()){
            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();
            TS_AdaptationField.Print();
            printf("\n");
          //}
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