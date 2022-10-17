//Pin Declarations
int latch=51;
int clk=49;
int data=50;
int dig4=31;
int dig3=30;
int buzzer=27;
int R0 = 47;
int Y0 = 45;
int G0 = 43;
int R1 = 41;
int Y1 = 39;
int G1 = 37;
int BTN = 21;

//State Declarations
#define RST 0
#define R0R1 1
#define G0R1 2
#define G0R1B 3
#define Y0R1 4
#define R1R0 5
#define G1R0 6
#define G1R0B 7
#define Y1R0 8

//Global Variables
//unsigned char count =5;
  //This table allows easy conversion from digit to 7-segment
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};
volatile int state = RST;
volatile int next_state = 1;
volatile int change;
volatile int count=3;
int dummy;

//Function Prototypes
void Display(unsigned char num);
void Display2(unsigned char num);
void ClearLEDs(void);
void toggle_reset();

void setup() {
  pinMode(latch,OUTPUT);
  pinMode(clk,OUTPUT);
  pinMode(data,OUTPUT);
  pinMode(dig4,OUTPUT);
  pinMode(dig3,OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(BTN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN), toggle_reset, FALLING);

  digitalWrite(dig4, LOW);
  digitalWrite(dig3, LOW);

  //set pins as outputs
  //pinMode(13, OUTPUT);

  cli();//stop interrupts

  //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}

ISR(TIMER1_COMPA_vect){//timer1 interrupt 1Hz toggles pin 13 (LED)
//generates pulse wave of frequency 1Hz/2 = 0.5kHz (takes two cycles for full wave- toggle high then toggle low)
  change = !change;
  state = next_state;
  count -=1;
}

void loop() {
  //toggle the bit that controls display loop
  change = 1;
  dummy=0;
  
  //Main State Machine for LEDs
  switch(state){   
    case(R0R1):
      ClearLEDs();
      digitalWrite(R0, HIGH);
      digitalWrite(R1, HIGH);
      
      if(count<1){
        ClearLEDs();
        digitalWrite(G0, HIGH);
        digitalWrite(R1, HIGH);
        next_state = G0R1;
        count = 20;
      }
      else {
        next_state = R0R1;
        digitalWrite(buzzer, HIGH);
        delay(10);
        digitalWrite(buzzer, LOW);
        //count -=1;
      }
      break;
    
    case(Y0R1):
      ClearLEDs();
      digitalWrite(Y0, HIGH);
      digitalWrite(R1, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(10);
      digitalWrite(buzzer, LOW);
      
      if(count<1){
        ClearLEDs();
        digitalWrite(R0, HIGH);
        digitalWrite(R1, HIGH);
        next_state = R1R0;
        count = 3;
      }
      else {
        next_state = Y0R1;
        //count -=1;
      }
      break;
    
    case(G0R1):
      ClearLEDs();
      digitalWrite(G0, HIGH);
      digitalWrite(R1, HIGH);
      if(count==4){
        next_state = G0R1B;
      }
      else next_state = G0R1;
      //count -=1;
      break;
    
    case(G0R1B):
      ClearLEDs();
      digitalWrite(G0, HIGH);
      digitalWrite(R1, HIGH);
      
      digitalWrite(buzzer, HIGH);
      delay(10);
      digitalWrite(buzzer, LOW);
      
      if(count<1){
        ClearLEDs();
        digitalWrite(Y0, HIGH);
        digitalWrite(R1, HIGH);
        next_state = Y0R1;
        count = 3;
      }
      else {
        next_state = G0R1B;
        //count -=1;
      }
      break;
    
    case(R1R0):
      ClearLEDs();
      digitalWrite(R0, HIGH);
      digitalWrite(R1, HIGH);
      
      if(count<1){
        ClearLEDs();
        digitalWrite(G1, HIGH);
        digitalWrite(R0, HIGH);
        next_state = G1R0;
        count = 20;
      }
      else {
        digitalWrite(buzzer, HIGH);
        delay(10);
        digitalWrite(buzzer, LOW);
        next_state = R1R0;
        //count -=1;
      }
      break;
    
    case(Y1R0):
      ClearLEDs();
      digitalWrite(Y1, HIGH);
      digitalWrite(R0, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(10);
      digitalWrite(buzzer, LOW);
      
      if(count<1){
        ClearLEDs();
        digitalWrite(R0, HIGH);
        digitalWrite(R1, HIGH);
        next_state = R0R1;
        count = 3;
      }
      else {
        next_state = Y1R0;
        //count-=1;
      }
      break;
    
    case(G1R0):
      ClearLEDs();
      digitalWrite(G1, HIGH);
      digitalWrite(R0, HIGH);
      if(count==4){
        next_state = G1R0B;
      }
      else {
        next_state = G1R0;
      }
      //count-=1;
      break;
    
    case(G1R0B):
      ClearLEDs();
      digitalWrite(G1, HIGH);
      digitalWrite(R0, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(10);
      digitalWrite(buzzer, LOW);
      //count-=1;
      if(count<1){
        ClearLEDs();
        digitalWrite(Y1, HIGH);
        digitalWrite(R0, HIGH);
        next_state = Y1R0;
        count = 3;
      }
      else {
        next_state = G1R0B;
        //count-=1;
      }
      break;
    
    case(RST):
    //Mask ISR PCINT for Button pressed 
      next_state = RST;
      count = 4;
      while(state==RST){
        ClearLEDs();
        delay(500);
        digitalWrite(R0, HIGH);
        digitalWrite(R1, HIGH);
        digitalWrite(buzzer, HIGH);
        delay(10);
        digitalWrite(buzzer, LOW);
        delay(490);
      }
  }

    
//  for(int i=0; i<50; i++){
//    Display2(count);
//  }
//  if(count>1){
//    count-=1;
//    if(count<4){
//      digitalWrite(buzzer, HIGH);
//      delay(10);
//      digitalWrite(buzzer, LOW);
//    }
//  }
//  else count=20;
  
  while(change){
    Display2(count);
  }


}


void Display(unsigned char num)
{

  digitalWrite(latch,LOW);
  shiftOut(data,clk,MSBFIRST,table[num]);
  digitalWrite(latch,HIGH);
  
}

void ClearLEDs(void){
  digitalWrite(R0, LOW);
  digitalWrite(R1, LOW);
  digitalWrite(Y0, LOW);
  digitalWrite(Y1, LOW);
  digitalWrite(G0, LOW);
  digitalWrite(G1, LOW);
}

void Display2(unsigned char num){
  digitalWrite(dig3, LOW);
  digitalWrite(dig4, HIGH);
  Display(count%10);
  delay(10);
  digitalWrite(dig4, LOW);
  if(count/10>0){ 
    digitalWrite(dig3, HIGH);
    Display(count/10);
  }
  delay(10);
}

void toggle_reset(){
  if(state==RST){
    count = 0;
    next_state=R0R1;
  }
  else next_state = RST;
}
