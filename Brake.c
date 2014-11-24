#include "headerThingy.h"

void BrakeTemp(void *vParameters) {
  unsigned long inADC;
    
  while(1) {
    //trigger ADC to take measurement
    ADCProcessorTrigger(ADC0_BASE, 3);
    //wait for ADC to finish measurement
    while(!ADCIntStatus(ADC0_BASE, 3, false));
    //once finished, clear the interrupt flag
    ADCIntClear(ADC0_BASE, 3);
    //store value in global variable
    ADCSequenceDataGet(ADC0_BASE, 3, &inADC);
    
    brakeTemp = (unsigned long)((double)inADC / 1024 * 330);
    
    if(brakeTemp > 200)
      brakeHighTemp = TRUE;
    else
      brakeHighTemp = FALSE;
    
    vTaskDelay(1000);
  }      
}
