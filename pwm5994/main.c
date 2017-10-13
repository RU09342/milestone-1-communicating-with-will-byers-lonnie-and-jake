#include <msp430fr5994.h>

enum Trigger {Rising_Edge, Falling_Edge};
enum Trigger edge = Rising_Edge;

#define RED BIT5
#define GREEN BIT6
#define BLUE BIT7
#define BUTTON BIT5


float hex2duty(hex)
{
    float duty;
    duty = (hex/0xFF)*500;
    return duty;
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;  //ENABLE GPIO

    P3SEL0 |= RED + GREEN + BLUE;
    P3DIR |= RED + GREEN + BLUE;
    P3OUT &= ~(RED + GREEN + BLUE);

    P5IES |= BUTTON;

    P5REN |= BUTTON;
    P5IE |= BUTTON;               // Enable interrupt on P1.3 (Btn)
    P5OUT |= BUTTON;                  //pull up res
    P5IFG &= ~BUTTON;             // Clear Btn interrupt flag

    TB0CTL |=   TBSSEL_2 |ID_1 | MC_1;

    TB0CCR0 = 1000;              // Set clock period

    TB0CCR4 = 1000;
    TB0CCTL4 |= OUTMOD_3;       // Set/Reset mode

    TB0CCR5 = 250;
    TB0CCTL5 |= OUTMOD_3;       // Set/Reset mode

    TB0CCR6 = 0;
    TB0CCTL6 |= OUTMOD_3;       // Set/Reset mode

    _BIS_SR(LPM0_bits+GIE);

    return 0;
}

//Service routine
#pragma vector = PORT5_VECTOR
__interrupt void PORT_5(void)

{
        TB0CCR4 += 50;
        TB0CCR5 += 50;
        TB0CCR6 += 50;
        if (TB0CCR4 == 550)
            TB0CCR4 = 0;

        if (TB0CCR5 == 550)
            TB0CCR5 = 0;

        if (TB0CCR6 == 550)
            TB0CCR6 = 0;
    P5IFG &= ~BUTTON;
}
