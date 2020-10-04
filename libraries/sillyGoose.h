#include <TFT.h> // Arduino LCD library
#include <SD.h>

/* TFT screen pin definitions */
// TFT screen CS pin definition
#define TFT_CS PB10

// TFT screen DC pin definition
#define TFT_DC PB0

// TFT screen RESET pin definition
#define TFT_RST PB1

/* SD card pin definitions */
// SD MOSI pin definition
#define SD_MOSI PB15

// SD MISO pin definition
#define SD_MISO PB14

// SD SCK pin definition
#define SD_SCK PB13

// SD CS pin definition
#define SD_CS PB12

/* push button pin definitions */
// down button pin definition
#define DOWN_BUTTON PA2
// up button pin definition
#define UP_BUTTON PA4

// right button pin definition
#define RIGHT_BUTTON PA6

// left button pin definition
#define LEFT_BUTTON PA3

// b button pin definition
#define B_BUTTON PA1

// a button pin definition
#define A_BUTTON PA0

// Color definitions
/* some RGB color definitions*/
#define Black 0x0000
#define Green 0x03E0
#define Yellow 0x03EF
#define DarkBlue 0x7800
#define Pink 0x780F
#define LightGrey 0xC618
#define DarkGrey 0x7BEF
#define Red 0x001F
#define Orange 0x02EF
#define Green 0x07E0
#define White 0xFFFF
#define Blue 0xFD20
#define GreenYellow 0xAFE5

class SillyGoose
{

public:
    void initialise(int determinator);

    // the multiplier to scale the gameplay depending on the screen resolution
    int scaleDisplay;

    // the scale for the text depending on the screen resolution
    int scaleText;

    // variable for reading the down button status
    int downButtonState;

    // variable for reading the left button status
    int leftButtonState;

    // variable for reading the right button status
    int rightButtonState;

    // variable for reading the up button status
    int upButtonState;

    // variable for reading the b button status
    int bButtonState;

    // variable for reading the a button status
    int aButtonState;

    // the height of the screen (the shortest dimension of the screen)
    int16_t screenShort;

    // the width of the screen (the longest dimension of the screen)
    int16_t screenLong;

    // pixel height of text on the screen at text size 1
    int textHeight;

    // pixel width of text on the screen at text size 1
    int textWidth;

    // y position of the point in the middle of the screen
    int verticalDotCentrePosition;

    // x position of the point in the middle of the screen
    int horizontalDotCentrePosition;

    // create an instance of the library for the TFT screen
    TFT TFTscreen = TFT(TFT_CS, TFT_DC, TFT_RST);

    // creates an error screen with a message... (only one line of message, so must be short)
    void errorScreen(char *message);

    // creates selectable centred or right hand pushed text at a given height (doubles size for titles). When selected, it changes green and is underlined.
    void structuredSelectableText(char *text, int height, bool selected, bool title, bool rightHandPushed);

    // wait for the user to press the B button.
    void waitForB();

    // wait for the user to press the A button.
    void waitForA();

// wait for the user to press the right button.
void waitForRight();

// wait for the user to press the left button.
void waitForLeft();

    // read the states of all buttons on silly goose.
    void readButtonStates();
};
