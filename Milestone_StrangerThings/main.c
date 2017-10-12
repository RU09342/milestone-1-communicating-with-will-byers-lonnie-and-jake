#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1SEL |= BIT1 | BIT2; 		// Select UART on P1.1 and P1.2
	P1SEL2 |= BIT1 | BIT2;		// P1.1 = RX; P1.2 = TX

	// For debug only
	P1DIR |= BIT6;
	P1OUT &= ~BIT6;

	UCA0CTL1 |= UCSSEL_2;
	UCA0BR0 = 104;				// Setting up clock dividers to
	UCA0BR1 = 0;				// achieve 9600 baud rate
	UCA0MCTL |= UCBRS0;			// Modulation UCBRSx = 1 (from datasheet)
	UCA0CTL1 &= ~UCSWRST;		// Initialize USCI state machine
	UC0IE |= UCA0RXIE;			// Enable receive (Rx) interrupt

	__BIS_SR(LPM1 + GIE);

	return 0;
}

#pragma vector = USCIABORX_VECTOR
__interrupt void USCIORX_ISR(void) {
	P1OIT ^= BIT6;
}
