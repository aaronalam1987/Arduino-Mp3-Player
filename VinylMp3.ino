#include "SoftwareSerial.h"
#include <LiquidCrystal_I2C.h>

SoftwareSerial playerSerial(10, 11); //Define dfplayer module.
LiquidCrystal_I2C lcd (0x26, 16, 2); //Define 16x2 LCD.

# define Start_Byte 0x7E
# define Version_Byte 0xFF
# define Command_Length 0x06
# define End_Byte 0xEF
# define Acknowledge 0x00
# define ACTIVATED LOW

//ints to keep track of the touch counts and current track.
int touchCount = 0;
int currTrack = 0;

//Initial lcd text (Album title).
String lcdText = "Enter the Wu-tang";
//Track array
String trackName[11] = {"Bring Da Ruckus", "Shame On a Nigga", "Clan In Da Front", "Wu-Tang: 7th Chamber", "Can It Be All So Simple", "Da Mystery of Chessboxin'", "Wu-Tang Clain Ain't Nuthin ta F' Wit", "C.R.E.A.M", "Method Man", "Protect Ya Neck", "Tearz"};

void setup()
{ 
  //Start serial and player serial.
  Serial.begin(9600);
  playerSerial.begin(9600);

  //Set pinmode.
  pinMode(8, INPUT);

  //Initilise LCD.
  lcd.init();
  lcd.clear(); 
  lcd.backlight();

  //Configure dfplayer.
  //Repeat Play.
  execute_CMD(0x11, 0, 0);
  delay(500);
  //Set volume (to max).
  execute_CMD(0x06, 0, 30);
  delay(500);
}

void doAudio(){
  if(touchCount > 5){
    //Reset module.
    execute_CMD(0x0C, 0, 0);
    touchCount = 0;
    currTrack = 0;
    lcdText = "Enter the Wu-tang";
  }

  else{
    switch (touchCount){
    case 1:
      //Play (first track).
      execute_CMD(0x0D,0,1); 
      delay(1000);
      //Reset touch counter.
      touchCount = 0;
      //Clear lcd and set text based on current track.
      lcd.clear();
      lcdText = trackName[currTrack];
      break;
    
    case 2:
    //Send next track.
      execute_CMD(0x01, 0, 0);
      delay(1000);
      touchCount = 0;
      currTrack++;
      lcd.clear();
      lcdText = trackName[currTrack];
      break;

    case 3:
    //Send previous track.
      execute_CMD(0x02, 0, 0);
      delay(1000);
      touchCount = 0;
      currTrack--;
      lcd.clear();
      lcdText = trackName[currTrack];
      break;
    }
  }
  
}

void loop()
{
  
  //Set and print lcd text.
  lcd.setCursor(0, 1);
  //Print lcdText (Current track pulled from array).
  lcd.print(lcdText);
  //Scroll text left.
  lcd.command(0x18);
  //Delay which is needed to stop fast scrolling & fast button incrementing.
  delay(150);

  //Current track is out of range of our array, reset.
  if(currTrack > 11){
    currTrack = 0;
  }

    //Check for touch/button press.
  if(digitalRead(8) == HIGH) {
    touchCount++;
  }
  else{
    //No touch, do audio commands.
    doAudio();
  }  

  //Debug stuff.
  Serial.println(touchCount);
}


//Not my code, credit goes to Nick Koumaris @ https://www.electronics-lab.com/project/mp3-player-using-arduino-dfplayer-mini/
void execute_CMD(byte CMD, byte Par1, byte Par2)
// Excecute the command and parameters
{
// Calculate the checksum (2 bytes)
word checksum = -(Version_Byte + Command_Length + CMD + Acknowledge + Par1 + Par2);
// Build the command line
byte Command_line[10] = { Start_Byte, Version_Byte, Command_Length, CMD, Acknowledge,
Par1, Par2, highByte(checksum), lowByte(checksum), End_Byte};
//Send the command line to the module
for (byte k=0; k<10; k++)
{
playerSerial.write( Command_line[k]);
}

}