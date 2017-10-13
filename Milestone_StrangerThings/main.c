#include <msp430.h>

#define RED BIT5
#define GREEN BIT6
#define BLUE BIT7
#define BUTTON BIT5


int hex2duty(int hex)
{
    float duty;
    duty = 1000-(hex/0xFF)*1000;
    return (int)duty;
}

unsigned int RxCount = 0;					// Keeps track of how many bytes have been received
unsigned int nBytes = 0;					// Stores number of bytes in current packet

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop Watchdog

    P3SEL0 |= RED + GREEN + BLUE;
    P3DIR |= RED + GREEN + BLUE;
    P3OUT &= ~(RED + GREEN + BLUE);

    TB0CTL |=   TBSSEL_2 |ID_1 | MC_1;

    TB0CCR0 = 1000;              // Set clock period

    TB0CCR4 = 1000;
    TB0CCTL4 |= OUTMOD_3;       // Set/Reset mode

    TB0CCR5 = 1000;
    TB0CCTL5 |= OUTMOD_3;       // Set/Reset mode

    TB0CCR6 = 1000;
    TB0CCTL6 |= OUTMOD_3;       // Set/Reset mode

    // Configure GPIO
    P6SEL0 &= ~(BIT0 | BIT1);
    P6SEL1 |= BIT0 | BIT1;                  // USCI_A0 UART operation

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA3CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA3CTLW0 |= UCSSEL__SMCLK;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA3BRW = 52;                           // 8000000/16/9600
    UCA3MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA3CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
    UCA3IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

    __bis_SR_register(LPM0_bits | GIE);     // Enter LPM3, interrupts enabled
    //__no_operation();                       // For debugger
}


#pragma vector=EUSCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{

    switch(__even_in_range(UCA3IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE: break;
        case USCI_UART_UCRXIFG:
            while(!(UCA3IFG & UCTXIFG));
            	if (RxCount == 0 ) {
            		nBytes = UCA3RXBUF;
            		UCA3TXBUF = nBytes - 3;
            		RxCount ++;
            	}
            	else if (RxCount == 1) {	// Red info
            		RxCount ++;
            		TB0CCR4 = hex2duty(UCA3RXBUF);
            	}

            	else if (RxCount == 2) {	// Green info
            		RxCount ++;
            		TB0CCR5 = hex2duty(UCA3RXBUF);
            	}
            	else if (RxCount == 3) {	// Blue info
            		RxCount ++;
            		TB0CCR6 = hex2duty(UCA3RXBUF);
            	}
            	else if (RxCount > 2) {		// Pass these along
            		UCA3TXBUF = UCA3RXBUF;
            		RxCount ++;
            		if (RxCount == nBytes)	// EOM
            			RxCount = 0;
            	}
            __no_operation();
            break;
        case USCI_UART_UCTXIFG: break;
        case USCI_UART_UCSTTIFG: break;
        case USCI_UART_UCTXCPTIFG: break;
        default: break;
    }
}
