#include "stm8s.h"
#include "dot-led-indicator.h"

void printDigit(char digit, char position, char* dotIndicatorBuffer) {
	char i;
  const char digitPatterns[10][4] = {{0xFE,0x82,0x82,0xFE}, {0x00,0x00,0x00,0xFE}, {0x9E,0x92,0x92,0xF2}, {0x92,0x92,0x92,0xFE}, {0xF0,0x10,0x10,0xFE},   //0-1-2-3-4
                                     {0xF2,0x92,0x92,0x9E}, {0xFE,0x92,0x92,0x9E}, {0x80,0x80,0x80,0xFE}, {0xFE,0x92,0x92,0xFE}, {0xF2,0x92,0x92,0xFE}};  //5-6-7-8-9
  for (i = position * 4; i < ((position * 4) + 4); i++) {
		*(dotIndicatorBuffer + i) = digitPatterns[digit][i - (position * 4)];
	}	
}

void printChar(SpecChar ch, char position, char* dotIndicatorBuffer) {
	char i;
  const char charPatterns[10][4] = {{0x10,0x10,0x10,0x00}, {0x10,0x38,0x10,0x00}, {0x00,0x00,0x06,0x06}, {0x00,0x00,0x00,0x00}, {0x10,0x10,0x10,0x10}};  //- + . ' ' -
  for (i = position * 4; i < ((position * 4) + 4); i++) {
		*(dotIndicatorBuffer + i) = charPatterns[ch][i - (position * 4)];
	}	
}

void printSmallDigit(char digit, char position, char* dotIndicatorBuffer) {
	char i;
  const char digitPatterns[10][4] = {{0x7C,0x44,0x7C,0x00}, {0x00,0x00,0x7C,0x00}, {0x5C,0x54,0x74,0x00}, {0x54,0x54,0x7C,0x00}, {0x70,0x10,0x7C,0x00},   //0-1-2-3-4
                                     {0x74,0x54,0x5C,0x00}, {0x7C,0x54,0x5C,0x00}, {0x40,0x40,0x7C,0x00}, {0x7C,0x54,0x7C,0x00}, {0x74,0x54,0x7C,0x00}};  //5-6-7-8-9
  for (i = position * 4; i < ((position * 4) + 4); i++) {
		*(dotIndicatorBuffer + i) = digitPatterns[digit][i - (position * 4)];
	}	
}

void printYear(int year, char* dotIndicatorBuffer) {
	printSmallDigit(year % 10, 3, dotIndicatorBuffer);
	year /= 10;
	printSmallDigit(year % 10, 2, dotIndicatorBuffer);
	year /= 10;
	printSmallDigit(year % 10, 1, dotIndicatorBuffer);
	year /= 10;
	printSmallDigit(year, 0, dotIndicatorBuffer);
}

void printDay(int day, char* dotIndicatorBuffer) {
	printSmallDigit(day % 10, 1, dotIndicatorBuffer);
	day /= 10;
	printSmallDigit(day, 0, dotIndicatorBuffer);
	printChar(SPACE, 2, dotIndicatorBuffer);
	printChar(SPACE, 3, dotIndicatorBuffer);
}

void printMonth(int month, char* dotIndicatorBuffer) {
	printSmallDigit(month % 10, 3, dotIndicatorBuffer);
	month /= 10;
	printSmallDigit(month, 2, dotIndicatorBuffer);
	printChar(SPACE, 0, dotIndicatorBuffer);
	printChar(SPACE, 1, dotIndicatorBuffer);
}

void printSecCount(char sec, char* dotIndicatorBuffer) {
	printDigit(sec % 10, 3, dotIndicatorBuffer);
	sec /= 10;
	printDigit(sec, 2, dotIndicatorBuffer);
	printChar(SPACE, 0, dotIndicatorBuffer);
	printChar(SPACE, 1, dotIndicatorBuffer);
}

void printIntAddition(int intAddition, char* dotIndicatorBuffer) {
	if (intAddition < 0) {
		intAddition = - intAddition;
		printChar(MINUS, 0, dotIndicatorBuffer);
	} else {
		printChar(PLUS, 0, dotIndicatorBuffer);
	}
	printChar(SPACE, 3, dotIndicatorBuffer);
	
	printDigit(intAddition % 10, 2, dotIndicatorBuffer);
	intAddition /= 10;
	printDigit(intAddition, 1, dotIndicatorBuffer);
}

void printFractionAddition(int fractionAddition, char* dotIndicatorBuffer, char negativeZero) {
	if (fractionAddition < 0) {
		fractionAddition = - fractionAddition;
		printChar(MINUS, 0, dotIndicatorBuffer);
	} else if (fractionAddition == 0 && negativeZero) {
		printChar(MINUS, 0, dotIndicatorBuffer);
  } else {
		printChar(PLUS, 0, dotIndicatorBuffer);
	}
	printChar(DOT, 1, dotIndicatorBuffer);
	
	printDigit(fractionAddition % 10, 3, dotIndicatorBuffer);
	fractionAddition /= 10;
	printDigit(fractionAddition, 2, dotIndicatorBuffer);
}


void printTime(char hours, char minutes, char* dotIndicatorBuffer) {
	printDigit(hours/10, 0, dotIndicatorBuffer);
	printDigit(hours%10, 1, dotIndicatorBuffer);
	printDigit(minutes/10, 2, dotIndicatorBuffer);
	printDigit(minutes%10, 3, dotIndicatorBuffer);
}

void initDotLeds(void){
	PORT_HORIZONTAL_12->CR1    &= ~BIT_12_MASK; 
	PORT_HORIZONTAL_34567->CR1 &= ~BIT_34567_MASK; 
	PORT_HORIZONTAL_12->CR2    |= BIT_12_MASK;
	PORT_HORIZONTAL_34567->CR2 |= BIT_34567_MASK; 
	PORT_HORIZONTAL_12->DDR    |= BIT_12_MASK;
	PORT_HORIZONTAL_34567->DDR |= BIT_34567_MASK; 
	PORT_HORIZONTAL_12->ODR    &= ~BIT_12_MASK;
	PORT_HORIZONTAL_34567->ODR &= ~BIT_34567_MASK; 
	
	PORT_SYNC->ODR     &= ~BIT_SYNC_MASK;
	PORT_SYNC->CR1     |= BIT_SYNC_MASK;
	PORT_SYNC->DDR     |= BIT_SYNC_MASK;
	PORT_SYNC->CR2     |= BIT_SYNC_MASK;

}

void initAndTurnOnColon(void){
	PORT_COLON->CR1    &= ~BIT_COL_MASK; 
	PORT_COLON->CR2    |= BIT_COL_MASK;
	PORT_COLON->DDR    |= BIT_COL_MASK;
	PORT_COLON->ODR    &= ~BIT_COL_MASK;
}

void enableEncoderAndTurnOffColon(void){
	PORT_COLON->CR2    &= ~BIT_COL_MASK;
	PORT_COLON->DDR    &= ~BIT_COL_MASK;
	PORT_COLON->CR1    |= BIT_COL_MASK; 
}

void blankIndicator() {
	PORT_HORIZONTAL_12->CR1    &= ~BIT_12_MASK; 
	PORT_HORIZONTAL_34567->CR1 &= ~BIT_34567_MASK;
  PORT_HORIZONTAL_12->ODR    |= BIT_12_MASK;
	PORT_HORIZONTAL_34567->ODR |= BIT_34567_MASK; 	
}

void refreshIndicator(char updCol, char* dotIndicatorBuffer, char idle) {
	char dl=0;
	if (updCol == 0) {
		PORT_SYNC     -> ODR |= BIT_LED_RST;
	} else {
		PORT_SYNC     -> ODR |= BIT_LED_CLK;
	}
	
	if (updCol & 1) { //1-3-5-7-9-11-13-15 col
					
    if (!idle) {
		  PORT_HORIZONTAL_12->CR1    &= ~BIT_12_MASK; 
		  PORT_HORIZONTAL_34567->CR1 &= ~BIT_34567_MASK; ; 
		  PORT_HORIZONTAL_12->ODR &= ~((*(dotIndicatorBuffer + updCol)) & BIT_12_MASK);
		  PORT_HORIZONTAL_34567->ODR &= ~((*(dotIndicatorBuffer + updCol)) & BIT_34567_MASK);
	  }
		
	} else {          //0-2-4-6-8-10-12-14 col
	
	  if (!idle) {
	    PORT_HORIZONTAL_12->CR1 |= ((*(dotIndicatorBuffer + updCol)) & BIT_12_MASK);
	  	PORT_HORIZONTAL_34567->CR1 |= ((*(dotIndicatorBuffer + updCol)) & BIT_34567_MASK);
	    PORT_HORIZONTAL_12->ODR |= BIT_12_MASK;
		  PORT_HORIZONTAL_34567->ODR |= BIT_34567_MASK;
		}
		
		PORT_SYNC     -> ODR &= ~BIT_LED_RST;
	}
	
	PORT_SYNC     -> ODR &= ~BIT_LED_CLK;
	nop();
}