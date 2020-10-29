#include <TFT.h> // Arduino LCD library
#include <SD.h>
#include <Arduino.h>
#include "SillyGoose.h"
#include <tuple>

using namespace std;

void SillyGoose::initialise(int determinator)
{

  this->TFTscreen.begin();

  // the height of the screen (the shortest dimension of the screen)
  this->screenShort = this->TFTscreen.height();

  // the width of the screen (the longest dimension of the screen)
  this->screenLong = this->TFTscreen.width();

  this->TFTscreen.setRotation(0);

  int scaleDisplayInt = 1;
  while (((float)screenLong / ((float)(determinator * (scaleDisplayInt + 1)))) > 1.0)
  {
    scaleDisplayInt++;
  }
  this->scaleDisplay = scaleDisplayInt;
  this->scaleText = scaleDisplayInt / 7;

  this->textHeight = scaleText * 8;
  this->textWidth = scaleText * 6;

  this->TFTscreen.setTextSize(this->scaleText);

  // variable for reading the down button status
  this->downButtonState = 0;

  // variable for reading the left button status
  this->leftButtonState = 0;

  // variable for reading the right button status
  this->rightButtonState = 0;

  // variable for reading the up button status
  this->upButtonState = 0;

  // variable for reading the b button status
  this->bButtonState = 0;

  // variable for reading the a button status
  this->aButtonState = 0;

  pinMode(PB11, OUTPUT);
  analogWrite(PB11, 100);
  pinMode(DOWN_BUTTON, INPUT);
  pinMode(UP_BUTTON, INPUT);
  pinMode(LEFT_BUTTON, INPUT);
  pinMode(RIGHT_BUTTON, INPUT);
  pinMode(B_BUTTON, INPUT);
  pinMode(A_BUTTON, INPUT);

  SD.begin(SD_CS, SD_MOSI, SD_MISO, SD_SCK);

  this->verticalDotCentrePosition = screenLong / 2;
  this->horizontalDotCentrePosition = screenShort / 2;
}

void SillyGoose::readButtonStates()
{
  this->downButtonState = digitalRead(DOWN_BUTTON);
  this->upButtonState = digitalRead(UP_BUTTON);
  this->leftButtonState = digitalRead(LEFT_BUTTON);
  this->rightButtonState = digitalRead(RIGHT_BUTTON);
  this->bButtonState = digitalRead(B_BUTTON);
  this->aButtonState = digitalRead(A_BUTTON);
}

void SillyGoose::errorScreen(char *message)
{
  this->TFTscreen.background(Red);
  this->TFTscreen.stroke(White);
  this->TFTscreen.text("OT OH :(", textHeight, 0);
  this->TFTscreen.text(message, 0, textHeight * 3);
  this->TFTscreen.text("PLEASE RESET", 0, textHeight * 5);

  while (true)
  {
  }
}

void SillyGoose::structuredSelectableText(const char *text, int height, bool selected, bool title, bool rightHandPushed, bool leftHandPushed, bool invisible)
{

  int multiplier = 1;
  if (title)
  {
    TFTscreen.setTextSize(2 * scaleText);
    multiplier = 2;
  }

  int textLength = multiplier * (textWidth * strlen(text));
  int textX ;

  if (!rightHandPushed && !leftHandPushed)
  {
    textX = textX / 2;
  }
    else if (leftHandPushed)
  {
    textX = 5;
  } else if( rightHandPushed){
textX = (screenShort - textLength);
  
  }else{
    errorScreen("INVALID TEXT FORMAT BOOLEAN");
  }

  int16_t colourText;
  int16_t colourUnderscore;
  if (selected)
  {
    colourText = Green;
    colourUnderscore = Green;
  }
  else
  {
    if(invisible){
      colourText = Black;
    } else{
    colourText = White;
    }colourUnderscore = Black;
  }
  TFTscreen.stroke(colourText);
  TFTscreen.text(text, textX, height);
  if (!title)
  {
    TFTscreen.drawRect(textX, height + (multiplier * textHeight), multiplier * textLength, 1, colourUnderscore);
  }

  TFTscreen.setTextSize(scaleText);
}

void SillyGoose::waitForB()
{
  while (true)
  {
    bButtonState = digitalRead(B_BUTTON);

    if (bButtonState == HIGH)
    {
      return;
    }
  }
}

void SillyGoose::waitForA()
{
  while (true)
  {
    aButtonState = digitalRead(A_BUTTON);

    if (aButtonState == HIGH)
    {
      return;
    }
  }
}

void SillyGoose::waitForRight()
{
  while (true)
  {
    rightButtonState = digitalRead(RIGHT_BUTTON);

    if (rightButtonState == HIGH)
    {
      return;
    }
  }
}

void SillyGoose::waitForLeft()
{
  while (true)
  {
    leftButtonState = digitalRead(LEFT_BUTTON);

    if (leftButtonState == HIGH)
    {
      return;
    }
  }
}

// menuStartPosition = sillyGoose.verticalDotCentrePosition - (4 * sillyGoose.textHeight);
int SillyGoose::menuSetup( char *title, int menuStartPosition, vector<string> items, int gap, bool invisible)
{
  TFTscreen.background(Black);
  structuredSelectableText(title, menuStartPosition, false, true, false, false,false);

  int selectionsStart = menuStartPosition + 2*(textHeight) + gap;
return menuOptions(   selectionsStart, items,  gap, invisible);
  
}

int SillyGoose::menuOptions( int selectionsStart, vector<string> items, int gap, bool invisible){
  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;

  initialiseMenu(   selectionsStart, items,  gap, invisible);

  while (true)
  {
    readButtonStates();

    if (downButtonState == HIGH && letGoOfDown)
    {
      if (selected < items.size() -1)
      {
         structuredSelectableText(items[selected].c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), false, false, false, false,invisible);
        selected++;
         structuredSelectableText(items[selected].c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), true, false,false , false,invisible);
      }
      letGoOfDown = false;
    }
    if (upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
         structuredSelectableText(items[selected].c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), false, false, false,false,invisible);
        selected--;
         structuredSelectableText(items[selected].c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), true, false, false,false,invisible);
      }
      letGoOfUp = false;
    }

    if (aButtonState == HIGH)
    {
      return selected;
    }
    if (upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (downButtonState == LOW)
    {
      letGoOfDown = true;
    }

    delay(50);
  }
}


int SillyGoose::menuOptionsTuple( int selectionsStart, vector<tuple <string, bool, string>> items, int gap){
  bool letGoOfUp = true;
  bool letGoOfDown = true;

  int selected = 0;

  initialiseMenuTuple(   selectionsStart, items,  gap);
  while ( get<1>(items[selected])){
   
    selected ++;
     if(  selected == items.size() ){
       // we have selected all of them...
       break;
     }

  }

  structuredSelectableText(get<0>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), true, false, true,false,false);
  
    structuredSelectableText(get<2>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)) , true, false, false,false,false);


  while (true)
  {
    readButtonStates();

    if (downButtonState == HIGH && letGoOfDown)
    {
      if (selected < items.size() -1)
      {
        int prevSel = selected;
         structuredSelectableText(get<0>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), false, false, true,false,false);
         structuredSelectableText(get<2>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)) , false, false, false,false,true);
        selected ++;
 while ( get<1>(items[selected])){
   
    selected ++;
     if(  selected == items.size() ){
       // going off the bottom of the menu... go back to the top...
       selected = 0;
     }
     if (prevSel == selected){
       // there's only one left to select.
       break;
     }

  }


         structuredSelectableText(get<0>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), true, false, true,false,false);
         structuredSelectableText(get<2>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)) , true, false, false,false,false);
      }
      letGoOfDown = false;
    }
    if (upButtonState == HIGH && letGoOfUp)
    {
      if (selected > 0)
      {
        int prevSel = selected;
         structuredSelectableText(get<0>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), false, false, true,false,false);
         structuredSelectableText(get<2>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)) , false, false, false,false,true);
       
    selected --;
        while ( get<1>(items[selected])){
   
    selected --;
     if(  selected == items.size() ){
       // going off the bottom of the menu... go back to the top...
       selected = 0;
     }
     if (prevSel == selected){
       // there's only one left to select.
       break;
     }

  }


         structuredSelectableText(get<0>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)), true, false, true,false,false);
         structuredSelectableText(get<2>(items[selected]).c_str(), selectionsStart + ((selected + 1) *( textHeight + gap)) , true, false, false,false,false);
      }
      letGoOfUp = false;
    }

    if (aButtonState == HIGH)
    {
      return selected;
    }
    if (upButtonState == LOW)
    {
      letGoOfUp = true;
    }

    if (downButtonState == LOW)
    {
      letGoOfDown = true;
    }

    delay(50);
  }
}

void SillyGoose::initialiseMenu( int selectionsStartPosition, vector<string> items, int gap, bool invisible)
{
  structuredSelectableText(items[0].c_str(), selectionsStartPosition + textHeight + gap, true, false, false,false,invisible);
  for (int i = 1; i < items.size(); i++)
  {
    structuredSelectableText(items[i].c_str(),  selectionsStartPosition + i*( textHeight + gap) , false, false, false,false, invisible);
  }
}

void SillyGoose::initialiseMenuTuple( int selectionsStartPosition, vector<tuple <string, bool, string>> items, int gap)
{
  // structuredSelectableText(items[0].c_str(), selectionsStartPosition + textHeight + gap, true, false, false,invisible);
  for (int i = 0; i < items.size(); i++)
  {
    structuredSelectableText(get<0>(items[i]).c_str(),  selectionsStartPosition + i*( textHeight + gap) , false, false, false,true,false);
    structuredSelectableText(get<2>(items[i]).c_str(),  selectionsStartPosition + i*( textHeight + gap) , false, false, false,false, get<1>(items[i]));
  }
}

int SillyGoose::startMenu( char *title)
{

  vector<string> items;
  items.push_back("START");
  items.push_back("SCORES");
  items.push_back("SETTINGS");
 return menuSetup( title, verticalDotCentrePosition - (4 * textHeight), items, textHeight, false);
  
 
}
