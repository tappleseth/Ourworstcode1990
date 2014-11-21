#include "headerThingy.h"

void UARTIntHandler(void)
{
    unsigned long ulStatus;

    //
    // Get the interrrupt status.
    //
    ulStatus = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ulStatus);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE));
    }
}

void UARTSend(const unsigned char *pucBuffer, unsigned long ulCount)
{
    //
    // Loop while there are more characters to send.
    //
    while(ulCount--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPutNonBlocking(UART0_BASE, *pucBuffer++);
    }
}

void SerialCom(void *vParameters){
  

  while(1){
  //for(int i=0; i<1600;i++);
  //UARTSend("With love\r\n",11);
  
  if(fromDirection == 'N') {
    UARTSend((unsigned char*)"From: North\r\n", 15);
    for(int i=0; i<1600;i++);
  }
  if(fromDirection == 'E') {
    UARTSend((unsigned char*)"From: East\r\n", 14);
    for(int i=0; i<1600;i++);
  }
  if(fromDirection == 'S') {
    UARTSend((unsigned char*)"From: South\r\n", 15);
    for(int i=0; i<1600;i++);
  }
  if(fromDirection == 'W') {
    UARTSend((unsigned char*)"From: West\r\n", 14);
    for(int i=0; i<1600;i++);
  }  
  if(fromDirection == 'X') {
    UARTSend((unsigned char*)"Tracks Idle\r\n", 13);
    for(int i=0; i<1600;i++);
  }  
  
  if (!gridlock){
    if(north) {
      UARTSend((unsigned char*)"To: North\r\n", 11);
      for(int i=0; i<1600;i++);
    }
    if(east) {
      UARTSend((unsigned char*)"To: East\r\n", 10);
      for(int i=0; i<1600;i++);
    }
    if(south) {
      UARTSend((unsigned char*)"To: South\r\n", 11);
      for(int i=0; i<1600;i++);
    }
    if(west) {
      UARTSend((unsigned char*)"To: West\r\n", 10);
      for(int i=0; i<1600;i++);
    }
  } else {
    UARTSend((unsigned char*)"GRIDLOCK!!\r\n", 12);
    for(int i=0; i<1600;i++);
  }
  
  if(trainSize) {
    unsigned char godDamnit;
    godDamnit = 48 + trainSize;
    for(int i=0; i<3000;i++);
    UARTSend("Train Size: ", 12);
    for(int i=0; i<1600;i++);
    UARTSend(&godDamnit, 1);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }
 if(passengerCount) {
   
   unsigned char passCountArray[] = "    ";
   
    
    int tempo = (int) passengerCount;
    
    int i = 3; 
  
  while(tempo > 0) {
    passCountArray[i] = (tempo%10) + 48;
    tempo = tempo/10;
    i--;     
  }
    UARTSend("Passengers: ", 12);
    for(int i=0; i<1600;i++);
    UARTSend(passCountArray, 4);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }  
  if(globalCount) {
    
    unsigned char globalCountArray[] = "          ";
    
    
    int justinCrazy = globalCount;
    
    int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
    UARTSend("AHT Time: ", 10);
    for(int i=0; i<1600;i++);
    UARTSend(globalCountArray, 10);
    for(int i=0; i<1600;i++);
    UARTSend("\r\n",2);
    for(int i=0; i<1600;i++);
  }  
  UARTSend((unsigned char*)"\r\n", 2); //SPAAAAAAAAACE MAAAAAAAAAAAAAAAN AAAAASA!!! AAAAAAAAA!!! AAAAAOMGF!
  for(int i=0; i<1600;i++);
   vTaskDelay(1000);
  }
  
}
