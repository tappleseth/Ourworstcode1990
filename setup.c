#include "headerThingy.h"

void Startup(void) {
  
  //STEP 1: OLED and PWM setup
  unsigned long ulPeriod;
  //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  SysCtlPWMClockSet(SYSCTL_PWMDIV_1); 
  SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
  GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);
  ulPeriod = SysCtlClockGet() / 400;
  PWMGenConfigure(PWM0_BASE, PWM_GEN_0,PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
  PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod / 16);
  PWMGenEnable(PWM0_BASE, PWM_GEN_0); 
  
  
  //STEP 2: Timer setup
  //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  /*TimerState = 0;
  TimerIntUnregister(TIMER1_BASE, TIMER_A);
  TimerIntRegister(TIMER1_BASE, TIMER_A, IntTimer0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
  TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
  TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet());    
  IntEnable(INT_TIMER1A);
  TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
  TimerEnable(TIMER1_BASE, TIMER_A);*/
  
  // SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  
  //STEP 3: Button pad setup 
  TrainState = 0;
  GPIOPortIntUnregister(GPIO_PORTE_BASE);
  GPIOPortIntRegister(GPIO_PORTE_BASE,IntGPIOe);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 , GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
  GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 , GPIO_FALLING_EDGE);
  GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 );
  IntEnable(INT_GPIOE);
  
  IntPrioritySet( INT_GPIOE, configKERNEL_INTERRUPT_PRIORITY);
  
  //STEP 4: Frequency count setup
  tempCount = 0;
  frequencyCount = 0;
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);       
  GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_3);
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
  GPIOPortIntUnregister(GPIO_PORTF_BASE);
  GPIOPortIntRegister(GPIO_PORTF_BASE,IntGPIOf);
  GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_RISING_EDGE);
  GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_3);
  IntEnable(INT_GPIOF);
  
  //STEP 5: UART setup  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                       UART_CONFIG_PAR_NONE));
  IntEnable(INT_UART0); 
  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
  
  IntPrioritySet( INT_UART0, configKERNEL_INTERRUPT_PRIORITY);
  
  //STEP 6: pin setup  
  /*SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, PORT_DATA);*/
  
  //STEP 7: ADC SETUP
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
  
  ADCSequenceDisable(ADC0_BASE,0);
  ADCSequenceDisable(ADC0_BASE,1);
  ADCSequenceDisable(ADC0_BASE,2);
  ADCSequenceDisable(ADC0_BASE,3);
  
  GPIOPinTypeADC(ADC0_BASE, 0xF); 
  
  ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);   
  ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); 
  ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0); 
  ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
  
  ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);
  
  ADCSequenceEnable(ADC0_BASE, 0);
  ADCSequenceEnable(ADC0_BASE, 1);
  ADCSequenceEnable(ADC0_BASE, 2);
  ADCSequenceEnable(ADC0_BASE, 3);
  
   ADCIntClear(ADC0_BASE, 0);
   ADCIntClear(ADC0_BASE, 1);
    ADCIntClear(ADC0_BASE, 2);
  ADCIntClear(ADC0_BASE, 3);
  
  ADCIntEnable(ADC0_BASE, 0);
  ADCIntEnable(ADC0_BASE, 1);
  ADCIntEnable(ADC0_BASE, 2);
   ADCIntEnable(ADC0_BASE, 3);
  
 
  //IntEnable(INT_ADC0);
  //IntPrioritySet(INT_ADC, 50);
  
  //IntEnable(INT_ADC0);
  //ADCIntClear(ADC0_BASE, 0);
  
 
  
  return;
}
