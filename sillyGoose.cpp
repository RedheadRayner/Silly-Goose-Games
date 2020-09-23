#include <TFT.h> // Arduino LCD library
#include <SD.h>
#include <Arduino.h>
#include "sillyGoose.h"


void SillyGoose::initialise(int determinator)
{

    this->TFTscreen.begin();
    this->TFTscreen.setRotation(0);

    // the height of the screen (the shortest dimension of the screen)
    this->screenShort = this->TFTscreen.height();

    // the width of the screen (the longest dimension of the screen)
    this->screenLong = this->TFTscreen.width();
    
    this->scaleDisplay = 1;
    while (((float)this->screenLong / ((float)(determinator * (this->scaleDisplay + 1)))) > 1.0)
    {
        this->scaleDisplay++;
    }

    this->scaleText = this->scaleDisplay / 7;
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
