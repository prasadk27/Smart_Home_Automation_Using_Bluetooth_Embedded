#include <msp430.h> 

unsigned int i = 0;                            	//Counter
unsigned int sensor_value = 0;					//Sensor Value variable

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    // GPIO Set-up
    P3SEL = BIT3 + BIT4;                      // P3.3,4 = USCI_A0 TXD/RXD
    P1DIR |= BIT2 + BIT0;					  // P1.0 and P1.2 set as output
    P1OUT &= ~(BIT2 + BIT0);				  // P1.0 and P1.2 set low

    // ADC Set up
    ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP;                     // Use sampling timer
    ADC12IE = 0x01;                           // Enable interrupt
    ADC12CTL0 |= ADC12ENC;					  // Enable Conversion
    P6SEL |= 0x01;                            // P6.0 ADC option select for sensor data
    P1DIR |= BIT5;                            // P1.5 output
    P1OUT &= ~BIT5;							  // P1.5 set low

    //USCI_A0 Set-up
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 109;                            // 1MHz 9600 - Check for compatibility with HC06
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL |= UCBRS_1 + UCBRF_0;            // Modulation UCBRSx=1, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

  while(1)
  	  {
	  	  ADC12CTL0 |= ADC12SC;                   	// Start sampling/conversion

	      __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
	      __no_operation();                         // For debugger

  	  }
}

// USCI_A0 ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
	unsigned char Rx_Data = 0;
	unsigned int i = 0;
	Rx_Data = UCA0RXBUF;						// Store received data in variable
	switch (Rx_Data)
		  	  {
		  	  	  case 'A':									// LED 1 On
		  	  		P1SEL &= ~BIT0;							// P1.0 selected as GPIO
		  	  		P1OUT |= BIT0;			        		// P1.0 set high
		  	  		i = 0;
		  	  		const char *string1 = "LED 1 is ON";
		  	  		UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
		  	  		while (*string1 != '\0')
		  	  		{
		  	  			while (!(UCA0IFG&UCTXIFG));     	// USCI_A0 TX buffer ready?
		  	  			UCA0TXBUF = *string1;				// TX string, character by character
		  	  			string1++;
		  	  		}
		  	  		UCA0IE &= ~UCTXIE;						// Disable USCI_A0 TX interrupt
		  	  		break;

		  	  	  case 'B':									// LED 1 Off
		  	  		P1SEL &= ~BIT0;							// P1.0 selected as GPIO
		  	  		P1OUT &= ~BIT0;			        		// P1.0 set low
		  	  		i = 0;
		  	  		const char *string2 = "LED 1 is OFF";
		  	  		UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
		  	  		while (*string2 != '\0')
		  	  		{
		  	  			while (!(UCA0IFG&UCTXIFG));     	// USCI_A0 TX buffer ready?
		  	  			UCA0TXBUF = *string2;				// TX string, character by character
		  	  			string2++;
		  	  		}
		  	  		UCA0IE &= ~UCTXIE;						// Disable USCI_A0 TX interrupt
		  	 	  	break;

		  	  	  case 'C':									// LED 2 On
			  	  	P1SEL &= ~BIT2;							// P1.2 selected as GPIO
			  	  	P1OUT |= BIT2;			        		// P1.2 set high
		  	  		i = 0;
		  	  		const char *string3 = "LED 2 is ON";
		  	  		UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
		  	  		while (*string3 != '\0')
		  	  		{
		  	  			while (!(UCA0IFG&UCTXIFG));     	// USCI_A0 TX buffer ready?
		  	  			UCA0TXBUF = *string3;				// TX string, character by character
		  	  			string3++;
		  	  		}
		  	  		UCA0IE &= ~UCTXIE;						// Disable USCI_A0 TX interrupt
			  	  	break;

		  	  	  case 'D':									// LED 2 Off
				  	P1SEL &= ~BIT2;							// P1.2 selected as GPIO
				  	P1OUT &= ~BIT2;			        		// P1.2 set low
		  	  		i = 0;
		  	  		const char *string4 = "LED 2 is OFF";
		  	  		UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
		  	  		while (*string4 != '\0')
		  	  		{
		  	  			while (!(UCA0IFG&UCTXIFG));     	// USCI_A0 TX buffer ready?
		  	  			UCA0TXBUF = *string4;				// TX string, character by character
		  	  			string4++;
		  	  		}
		  	  		UCA0IE &= ~UCTXIE;						// Disable USCI_A0 TX interrupt
				  	break;

		  	  	  default: break;
		  	  }
	//__bic_SR_register_on_exit(LPM0_bits);		// Wake-up CPU
}

//ADC12_ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    if (ADC12MEM0 >= 0x7ff)              	// ADC12MEM = A0 > 0.5AVcc, Threshold value for firing alarm
    {
      P1OUT |= BIT5;                        // P1.5 = 1
      UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
      while (!(UCA0IFG&UCTXIFG));     		// USCI_A0 TX buffer ready?
      UCA0TXBUF = 'X';						// TX character indicating emergency
      UCA0IE &= ~UCTXIE;					// Disable USCI_A0 TX interrupt
    }
    //else {								// For Testing
    	//P1OUT &= ~BIT5;							// P1.5 set low (for testing)
    	//UCA0IE |= UCTXIE;               		// Enable USCI_A0 TX interrupt
    	//while (!(UCA0IFG&UCTXIFG));     		// USCI_A0 TX buffer ready?
    	//UCA0TXBUF = 'Y';						// TX character indicating safety
    	//UCA0IE &= ~UCTXIE;					// Disable USCI_A0 TX interrupt
    //}
      __bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
      break;
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
