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
  vTaskDelay(5);
  //UARTSend("With love\r\n",11);
  
  if(fromDirection == 'N') {
    UARTSend((unsigned char*)"From: North\r\n", 15);
    vTaskDelay(5);
  }
  if(fromDirection == 'E') {
    UARTSend((unsigned char*)"From: East\r\n", 14);
    vTaskDelay(5);
  }
  if(fromDirection == 'S') {
    UARTSend((unsigned char*)"From: South\r\n", 15);
    vTaskDelay(5);
  }
  if(fromDirection == 'W') {
    UARTSend((unsigned char*)"From: West\r\n", 14);
    vTaskDelay(5);
  }  
  if(fromDirection == 'X') {
    UARTSend((unsigned char*)"Tracks Idle\r\n", 13);
    vTaskDelay(5);
  }  
  
  if (!gridlock){
    if(north) {
      UARTSend((unsigned char*)"To: North\r\n", 11);
      vTaskDelay(5);
    }
    if(east) {
      UARTSend((unsigned char*)"To: East\r\n", 10);
      vTaskDelay(5);
    }
    if(south) {
      UARTSend((unsigned char*)"To: South\r\n", 11);
      vTaskDelay(5);
    }
    if(west) {
      UARTSend((unsigned char*)"To: West\r\n", 10);
      vTaskDelay(5);
    }
  } else {
    UARTSend((unsigned char*)"GRIDLOCK!!\r\n", 12);
    vTaskDelay(5);
  }
  
  if(trainSize) {
    unsigned char godDamnit;
    godDamnit = 48 + trainSize;
    vTaskDelay(5);
    UARTSend("Train Size: ", 12);
    vTaskDelay(5);
    UARTSend(&godDamnit, 1);
    vTaskDelay(5);
    UARTSend("\r\n",2);
    vTaskDelay(5);
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
    vTaskDelay(5);
    UARTSend(passCountArray, 4);
    vTaskDelay(5);
    UARTSend("\r\n",2);
    vTaskDelay(5);
  }  
  if(globalCount) {
    
    unsigned char globalCountArray[] = "          ";
    
    
    int justinCrazy = globalCount / 1000;
    
    int i = 9; 
  
  while(justinCrazy > 0) {
    globalCountArray[i] = (justinCrazy%10) + 48;
    justinCrazy = justinCrazy/10;
    i--;     
  }
    UARTSend("AHT Time: ", 10);
    vTaskDelay(5);
    UARTSend(globalCountArray, 10);
    vTaskDelay(5);
    UARTSend("\r\n",2);
    vTaskDelay(5);
  }  
  
  //PRINT TEMP STUFF TO UART
  
  char brakeTempArray[5] = "    ";
  unsigned int justinCrazy = (unsigned int) brakeTemp;
   int i = 2;
    while(justinCrazy > 0) {
      brakeTempArray[i] = (justinCrazy%10) + 48;
      justinCrazy = justinCrazy/10;
      i--;     
    }
    brakeTempArray[3] = '\0';
    
    UARTSend("Temp (C): ", 10);
    vTaskDelay(5);
    UARTSend(brakeTempArray, 10);
    vTaskDelay(5);
    UARTSend("\r\n",2);
    vTaskDelay(5);
    if (brakeTemp>200){
      UARTSend("FIRE!", 10);
    }
      
       
    
    
  
    
  
  UARTSend((unsigned char*)"\r\n", 2); //SPAAAAAAAAACE MAAAAAAAAAAAAAAAN AAAAASA!!! AAAAAAAAA!!! AAAAAOMGF!
   vTaskDelay(1000);
  }
  
}
