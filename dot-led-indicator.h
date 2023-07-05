#define INDICATOR_HORIZONTAL_SIZE      16

#define PORT_HORIZONTAL_34567       GPIOC
#define PORT_HORIZONTAL_12          GPIOD

#define PIN_H1       1
#define BIT_H1                      (1<<PIN_H1 )
#define PIN_H2       2
#define BIT_H2                      (1<<PIN_H2 )
#define PIN_H3       3
#define BIT_H3                      (1<<PIN_H3 )
#define PIN_H4       4
#define BIT_H4                      (1<<PIN_H4 )
#define PIN_H5       5
#define BIT_H5                      (1<<PIN_H5 )
#define PIN_H6       6
#define BIT_H6                      (1<<PIN_H6 )
#define PIN_H7       7
#define BIT_H7                      (1<<PIN_H7 )

#define BIT_12_MASK      (BIT_H1 | BIT_H2)
#define BIT_34567_MASK   (BIT_H3 | BIT_H4 | BIT_H5 | BIT_H6 | BIT_H7)

#define PORT_SYNC          GPIOA

#define PIN_LED_CLK      1
#define BIT_LED_CLK                      (1<<PIN_LED_CLK)
#define PIN_LED_RST      2
#define BIT_LED_RST                      (1<<PIN_LED_RST)

#define BIT_SYNC_MASK  (BIT_LED_RST | BIT_LED_CLK)

#define PORT_COLON         GPIOB

#define PIN_COL1       4
#define BIT_COL1                      (1<<PIN_COL1 )
#define PIN_COL2       5
#define BIT_COL2                      (1<<PIN_COL2 )

#define BIT_COL_MASK  (BIT_COL1 | BIT_COL2)

typedef enum {MINUS, PLUS, DOT, SPACE} SpecChar;
typedef enum {NON_BLINKING, LEFT_BLINKING, RIGHT_BLINKING, ALL_BLINKING, COLON_BLINKING} BlinkingMode;

void printDigit(char digit, char position, char* dotIndicatorBuffer);
void printChar(char ch, char position, char* dotIndicatorBuffer);
void printSmallDigit(char digit, char position, char* dotIndicatorBuffer);

void printTime(char hours, char minutes, char* dotIndicatorBuffer);
void printYear(int year,   char* dotIndicatorBuffer) ;
void printDay(int day,    char* dotIndicatorBuffer);
void printMonth(int month, char* dotIndicatorBuffer);
void printSecCount(char sec, char* dotIndicatorBuffer);
void printIntAddition(int intAddition, char* dotIndicatorBuffer);
void printFractionAddition(int fractionAddition, char* dotIndicatorBuffer, char negativeZero); 

void initDotLeds(void);
void initAndTurnOnColon(void);
void enableEncoderAndTurnOffColon(void);
void refreshIndicator(char updCol, char* dotIndicatorBuffer, char idle);
void blankIndicator(void);