#include <msp430f5529.h>

enum Trigger {Rising_Edge, Falling_Edge};
enum Trigger edge = Rising_Edge;

#define RED BIT2
#define GREEN BIT3
#define BLUE BIT4
#define BUTTON BIT1


float hex2duty(hex)
{
    float duty;
    duty = (hex/0xFF)*500;
    return duty;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    P1SEL |= RED + GREEN + BLUE;
    P1DIR |= RED + GREEN + BLUE;
    P1OUT &= ~(RED + GREEN + BLUE);

    P1IES |= BUTTON;

    P1REN |= BUTTON;
    P1IE |= BUTTON;               // Enable interrupt on P1.3 (Btn)
    P1OUT |= BUTTON;                  //pull up res
    P1IFG &= ~BUTTON;             // Clear Btn interrupt flag

    TA0CTL |=   TASSEL_2 |ID_1 | MC_3;

    TA0CCR0 = 500;              // Set clock period

    TA0CCR1 = 0;
    TA0CCTL1 |= OUTMOD_4;       // Set/Reset mode

    TA0CCR2 = 0;
    TA0CCTL2 |= OUTMOD_4;       // Set/Reset mode

    TA0CCR3 = 0;
    TA0CCTL3 |= OUTMOD_4;       // Set/Reset mode

    _BIS_SR(LPM0_bits+GIE);

    return 0;
}


// P1.3 (Button) Service routine
#pragma vector = PORT1_VECTOR
__interrupt void PORT_1(void)

{
        TA0CCR1 += 50;
        TA0CCR2 += 50;
        TA0CCR3 += 50;
        if (TA0CCR1 == 550)
            TA0CCR1 = 0;

        if (TA0CCR2 == 550)
            TA0CCR2 = 0;

        if (TA0CCR3 == 550)
            TA0CCR3 = 0;
    P1IFG &= ~BUTTON;
}
