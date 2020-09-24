#include <TFT.h> // Arduino LCD library
#include <SD.h>
#include <Arduino.h>
#include "sillyGoose.h"

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
  this->horizontalDotCentrePosition =screenShort / 2;

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


void SillyGoose::structuredSelectableText(char *text, int height, bool selected, bool title, bool rightHandPushed)
{
 
  int multiplier = 1;
  if (title)
  {
    TFTscreen.setTextSize(2 * scaleText);
     multiplier = 2;
  }
  
  int textLength = multiplier * (textWidth * strlen(text));
  int textX = (screenShort - textLength);

  if (!rightHandPushed)
  {
    textX = textX / 2;
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
    colourText = White;
    colourUnderscore = Black;
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




