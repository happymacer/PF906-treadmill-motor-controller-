// This sketch assumes that there is a link across the relay so on power up the DC is immediately available at the motor terminals.  I did this by soldering a bridge across the relay contact terminals
//
// Make sure there is a delay between the DC powering up and the PWM being applied to the motor, and that the totem pole drivers are powered off.  I do this below by a 10s delay, but less time may be ok.
//
// This program sets up a PMW signal on pin 9 and 10 (in antiphase) - copied from Julian Lett video
// Connect a pot to  A3 to adjust the PWM signal
// Happymacer updates 13/12/20, 17/1/21, 1/2/21

int pot;
boolean startup = true;

void setup() {
  // select the mode for the output pins
  //                                                    TCCR1A Bit      7         6       5         4         3         2         1       0
  // register TCCR1A (timer/counter control register 1A) bit order is COM1A1 - COM1A0 - COM1B1 - COM1B0 - reserved - reserved - WGM11 - WGM10
  // b1011 0010 ( = 0xB2) sets COM1A1 to 1, COM1A0 to 0, COM1B1 to 1, and COM1B0 to 1, means clear OC1A/OC1B on compare match and set at bottom and channel A is normal, channel B is inverting.
  // also sets WGM11 to 1, WGM10 to 0
  TCCR1A = 0xB2;  
  //
  //                                                    TCCR1B Bit      7       6        5         4       3      2      1      0
  // register TCCR1B (timer/counter control register 1B) bit order is ICNC1 - ICES1 - reserved - WGM13 - WGM12 - CS12 - CS11 - CS10 (for this mode ICNC1 & ICES1 are disabled - ie ICR1 is used for top)
  // 0x11 hex =  b0001 0001 for phase correct (and 0x19 for fast) sets WGM13 to 1, WGM12 to 0, and cs12=0, cs11=0, cs10 to 1 - table 15-6 for CS12 to 10 says this is no prescaling
  // so the WGM bits set the mode to Mode 10 of table 15.5 in device datasheet - PMW phase correct top = ICR1, update OCR1x at top, TOV1 flag set on Bottom
  TCCR1B = 0x11;   
  
  // fclk = 16MHz
  // Pmw freq = fclk/(2*N*TOP) and in this case top = decimal 511 so PMW freq is about 15.6kHz page 104 datasheet sect 15.9.4
  
  ICR1 = 0x01FF; // = b0001 1111 1111
  //set the data direction for port B (ie outputs on the right pins)
  DDRB = 0x06; // 0b110  (1 is output, default is input or 0)
  // pin 13 will turn the totem pole driver off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // sets the digital pin 13 off
  
  // Not tested yet...
  // pin 14 will turn the RL2 on via FR7 
  pinMode(14, OUTPUT);
  digitalWrite(14, LOW); // sets the digital pin 14 off
}

void loop() {
  if (startup) {
    OCR1A = 0;
    OCR1B = 0;
//    digitalWrite(13, LOW);
    delay (10000); // wait for the system to be ready
    digitalWrite(14, HIGH); // turn on the RL2 relay
    }
  startup = false;
  digitalWrite(13, HIGH); // turn on the output power on the DC motor driver

  pot = analogRead(A3) / 2;
  OCR1A = pot;
  OCR1B = pot;

}
