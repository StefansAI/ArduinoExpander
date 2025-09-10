// ================================================
//
// SPDX-FileCopyrightText: 2025 Stefan Warnke
//
// SPDX-License-Identifier: BeerWare
//
//=================================================
//
// File:  RAM_Tester
// 
// The code can be used to test a number of 
// different circuitries, like logic chips, RAMs, 
// ROMs etc with the Arduino ExpansionBoard
//
//=================================================

// General ExpansionBoard pin definitions
//---------------------------------------
// Definition of the D0 pin
const int DATA_PIN_D0 = 2;
// Output enable for AB outputs
const int OE_AB_PIN = 11;
// Latch enable for A register
const int LE_A_PIN = 13;
// Latch enable for B register
const int LE_B_PIN = 12;
// Output enable for D register
const int PIN1_D_PIN = A0;   
// Direction signal for 74HCT245 type chip
const int PIN1_C_PIN = A1;   
// Output enable for C register or 74HCT245 buffer
const int PIN19_C_PIN = A2;   
// Latch enable for D register
const int LE_D_PIN = 10; 
// J5 Header control line 0     
const int CTRL_0 = A3;   
// J5 Header control line 1    
const int CTRL_1 = A4;   
// J5 Header control line 2
const int CTRL_2 = A5;    

// RAM tester specific pin definitions
//---------------------------------------
// RAM Write Enable pin
const int WE_PIN = CTRL_0; 
// RAM Chip Select pin    
const int CS_PIN = CTRL_1;    
// RAM Output Enable pin 
const int OE_PIN = CTRL_2;      

// Masks to set or reset the control signals
const int RAM_CS_H = 1<<0;
const int RAM_OE_H = 1<<1;
const int RAM_WE_H = 1<<2;
const int RAM_CS_L = 0;
const int RAM_OE_L = 0;
const int RAM_WE_L = 0;

// Example max addr for a HM62256
const int MAX_ADDR = (1<<15)-1;

// Buffer for sprintf
char buff[100];
// Variable for address output
int addr = 0; //0x33A0;
// Variable for data output 
int wr_data = 0;
// Variable for data input
int rd_data = 0;
// Cycle counter
int cycle = 0;
// Control signals
int ctrl = 0;

// Set the 8 bit data output for any register
void set_data(int value)
{
  for (int i=0; i<8; i++)
  {
    if ((value & 1) == 0)
      digitalWrite(DATA_PIN+i, LOW);
    else
      digitalWrite(DATA_PIN+i, HIGH);
    value >>= 1;
  }
}

// Read data input
int get_data()
{
  int result=0;
  for (int i=0; i<8; i++)
  {
    int value = digitalRead(DATA_PIN+i);
    if (value == HIGH)
      result += 1<<i;    
  }
  return result;
}

// Set data to output mode
void set_data_out()
{
   for (int i=DATA_PIN; i<DATA_PIN+8; i++)
      pinMode(i, OUTPUT);  
}

// Set data to input mode
void set_data_in()
{
  for (int i=DATA_PIN; i<DATA_PIN+8; i++)
      pinMode(i, INPUT);  
}

// Set the pin output level
void set_pin_out(int pin, int level)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, level);
}

// Write a value to the A register
void set_A(int value)
{
  set_data_out();
  set_data(value);
  digitalWrite(LE_A_PIN, HIGH);
  digitalWrite(LE_A_PIN, LOW);
  digitalWrite(OE_AB_PIN, LOW);
}

// Write a value to the B register
void set_B(int value)
{
  set_data_out();
  set_data(value);
  digitalWrite(LE_B_PIN, HIGH);
  digitalWrite(LE_B_PIN, LOW);
  digitalWrite(OE_AB_PIN, LOW);
}

// Write a value to the D register
void set_D(int value)
{
  set_data_out();
  set_data(value);
  digitalWrite(LE_D_PIN, HIGH);
  digitalWrite(LE_D_PIN, LOW);
  digitalWrite(PIN1_D_PIN, LOW);
}

// Write a value to the C channel (DB)
void set_Cout(int value)
{
  set_data_out();
  set_data(value);
  digitalWrite(PIN1_C_PIN, HIGH);
  digitalWrite(PIN19_C_PIN, LOW);
}

// Read a value from the C channel (DB)
int get_Cin()
{
  set_data_in();
  digitalWrite(PIN1_C_PIN, LOW);
  digitalWrite(PIN19_C_PIN, LOW);
  int result = get_data();
  digitalWrite(PIN19_C_PIN, HIGH);
  return result;
}

// Setup the pins for the main loop
void setup() 
{
  Serial.begin(9600);
  set_data_out();
  set_pin_out(OE_AB_PIN, HIGH);
  set_pin_out(LE_A_PIN, HIGH);
  set_pin_out(LE_B_PIN, HIGH);
  set_pin_out(PIN1_D_PIN, HIGH);
  set_pin_out(PIN1_C_PIN, HIGH);
  set_pin_out(PIN19_C_PIN, HIGH);
  set_pin_out(LE_D_PIN, HIGH);
  set_pin_out(WE_PIN,HIGH);
  set_pin_out(CS_PIN,HIGH);
  set_pin_out(OE_PIN,HIGH);

/*WritePattern();

    digitalWrite(PIN19_C_PIN, HIGH);
    set_D(RAM_CS_H | RAM_OE_H | RAM_WE_H);
    set_A(addr & 0xFF);
    set_B(addr >> 8);
    set_Cout(wr_data);
*/
  Serial.println("Setup Done");
}

#define TIME 1000

// Write a pseudo random pattern
void WritePattern()
{
  sprintf(buff,"Writing cycle=%d", cycle); 
  Serial.println(buff);

  for (addr=0; addr<MAX_ADDR; addr++)
  {
     wr_data = ((addr & 0xFF)) ^ (addr >> 8) ^ cycle) & 0xFF;
    //wr_data = 1<<(i & 7);
 
    //sprintf(buff,"Writing cycle=%d  addr=%04X  wr_data=%02X", cycle, addr, wr_data); 
    //Serial.println(buff);

    digitalWrite(PIN19_C_PIN, HIGH);
    set_A(addr & 0xFF);
    set_B(addr >> 8);
    set_Cout(wr_data);
    digitalWrite(CS_PIN,LOW);
    digitalWrite(WE_PIN,LOW);
    digitalWrite(WE_PIN,HIGH);
    digitalWrite(CS_PIN,HIGH);
    digitalWrite(PIN19_C_PIN, HIGH);
    //delay(TIME);
  }
}

// Read and check the pseudo random pattern
void CheckPattern()
{
  Serial.println("Loop Reading");

  for (addr=0; addr<MAX_ADDR; addr++)
  {
    wr_data = ((addr & 0xFF)) ^ (addr >> 8) ^ cycle) &0xFF;
    //wr_data = 1<<(addr & 7);
 
    digitalWrite(PIN19_C_PIN, HIGH);
    set_A(addr & 0xFF);
    set_B(addr >> 8);
    digitalWrite(CS_PIN,LOW);
    digitalWrite(OE_PIN,LOW);
    rd_data = get_Cin();
    digitalWrite(OE_PIN,HIGH);
    digitalWrite(CS_PIN,HIGH);
    if (rd_data != wr_data)
    {
      sprintf(buff,"Reading cycle=%d  addr=%04X  wr_data=%02X  rd_data=%02X", cycle, addr, wr_data, rd_data); 
      Serial.println(buff);
      delay(TIME);
    }
  }
}


// Another way of testing
void Test1()
{
  sprintf(buff,"addr=%04X", addr); 
  Serial.println(buff);
  set_A(addr & 0xFF);
  set_B(addr >> 8);
  set_D(ctrl);
  set_Cout(wr_data);
  delay(500);
  ctrl = ctrl >> 1;
  if ((ctrl & 0xFF) == 0)
    ctrl = 0x80;
  wr_data = wr_data << 1;
  if ((wr_data & 0xFF) == 0)
    wr_data = 1;
  addr=addr+1;
}

// Another possible Test run
void Test1()
{

  //set_D(RAM_CS_H | RAM_OE_H | RAM_WE_H);
  //rd_data = get_Cin();

  for (i=0; i<8; i++)
  {
    wr_data = 1<<(i & 7);

      sprintf(buff,"Writing cycle=%d  wr_data=%02X", cycle, wr_data); 
      Serial.println(buff);

    set_Cout(wr_data);
    delay(TIME);
    
    rd_data = get_Cin();

      sprintf(buff,"Reading cycle=%d  rd_data=%02X", cycle, rd_data); 
      Serial.println(buff);

    delay(TIME);
  }
}

// Main loop
void loop() 
{
  WritePattern();
  CheckPattern();
   cycle++;
}
