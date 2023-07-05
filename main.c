#include "stm8s.h"
#include "rtc.h"
#include "main.h"
#include "encoder.h"
#include "dot-led-indicator.h"

char dotIndicatorBuffer[INDICATOR_HORIZONTAL_SIZE];
char currentCol;
unsigned int globalTim2Counter;
BlinkingMode blinkingMode;

encoder_t              encoder;


AlignmentTimeCache     alignmentTimeCache;
TransferBody           transferBody; 
TimeTransferBodyCache  timeTransferBodyCache;
SecondsRtcUtcCache     secondsRtcUtcCache;
DateFromEpochDaysCache dateFromEpochDaysCache;

TimeAlignment          timeAlignment;
eeprom_data_t          eeprom_data @0x4100;         //store setting variables (in EEPROM)()

unsigned int resetCondition;

main() { 
  char h, iic_error;
	unsigned int brightness = 0;
	unsigned int local_cntr = 0;
	char ii;
  long l;
	
	LocalTime presetTime;
	LocalDate presetDate;
	presetTime.hr = 11;
	presetTime.min = 39;
	presetTime.sec = 5;
	
	presetDate.year = 2023;
	presetDate.month = 5;
	presetDate.dayOfMonth = 28;
	
	
	
	
	CLK->CKDIVR = 0x00; //~ 16 MHz   << 0x11 - 2MHz  0x10 - 4MHz  0x01 - 8MHz  0x00 - 16MHz >>
   
  init_rtc();
	init_encoder(&encoder);
	
	initDotLeds();
	init_tim2();
	
  populate_timeAlignment_from_eeprom();

  encoder.but_data_lim = 1;
  encoder_setter(0, 10, 5);
	blinkingMode = COLON_BLINKING;
  while(1) { 
	
  
	printTime(transferBody.hr, transferBody.min, dotIndicatorBuffer);

	  //set brightness
		if(!(local_cntr++ & 0x00ff)) {
			brightness += get_ADC(4);
			set_pwd_density(brightness / 100);
			brightness -= brightness / 100;
	  }
		
		refreshTimeTransferBody();
	
		
		
// fall into adjusting parameters
    if (encoder.but_data != 0 && (!encoder.cnt_button || encoder.but_data_long)) {
			
      encoder.but_data = 0;	
			if (encoder.but_data_long) {
				encoder.but_data_long = 0;
				printSeconds();
			} else {
        menu_selector();
		  }
			//encoder_setter(0, 10, i/5);
    }
  }
	
}

int scan_value(set_selector_t set_selector, LocalTime presetTime, LocalDate presetDate) {
	while(encoder.but_data == 0) {
		
		switch(set_selector) {
			case set_hr:
        blinkingMode = LEFT_BLINKING;			
				printTime(encoder.enc_data, presetTime.min, dotIndicatorBuffer);
				break;
			case set_min:
        blinkingMode = RIGHT_BLINKING;			
				printTime(presetTime.hr, encoder.enc_data, dotIndicatorBuffer);
				break;
			case set_year:
        blinkingMode = ALL_BLINKING;		
        printYear(encoder.enc_data, dotIndicatorBuffer);				
				break;
			case set_day:
        blinkingMode = LEFT_BLINKING;			
				printDay(encoder.enc_data, dotIndicatorBuffer);	
				break;
			case set_month:
        blinkingMode = RIGHT_BLINKING;			
				printMonth(encoder.enc_data, dotIndicatorBuffer);	
				break;
			case set_int_corr:
        blinkingMode = RIGHT_BLINKING;			
				printIntAddition(encoder.enc_data, dotIndicatorBuffer);	
				break;
			case set_fract_negative_corr:
        blinkingMode = RIGHT_BLINKING;			
				printFractionAddition(encoder.enc_data, dotIndicatorBuffer, 1);	
				break;
			case set_fract_positive_corr:
        blinkingMode = RIGHT_BLINKING;			
				printFractionAddition(encoder.enc_data, dotIndicatorBuffer, 0);	
				break;
		}

	}
	blinkingMode = NON_BLINKING;
	encoder.but_data = 0;
	return encoder.enc_data;
}


char menu_selector() {
	LocalTime presetTime;
	LocalDate presetDate;
	TimeAlignment presetCorrection;
	int i;
	
	//set time	
	presetTime.hr = transferBody.hr;
	presetTime.min = transferBody.min;
	
	encoder_setter(0, 23, presetTime.hr);
	presetTime.hr = scan_value(set_hr, presetTime, presetDate);
	
	encoder_setter(0, 59, presetTime.min);
	presetTime.min = scan_value(set_min, presetTime, presetDate);
	
	
	//set date
	presetDate.dayOfMonth = transferBody.dayOfMonth;
	presetDate.month = transferBody.month;
  presetDate.year = transferBody.year;	

	encoder_setter(2022, 3000, presetDate.year);
  presetDate.year = scan_value(set_year, presetTime, presetDate);

	encoder_setter(1, 31, presetDate.dayOfMonth);
	presetDate.dayOfMonth = scan_value(set_day, presetTime, presetDate);
	
	encoder_setter(1, 12, presetDate.month);
	presetDate.month = scan_value(set_month, presetTime, presetDate);

	

  
	//save date and time if changed
	if(presetDate.dayOfMonth != transferBody.dayOfMonth || presetDate.month != transferBody.month || presetDate.year != transferBody.year || presetTime.hr != transferBody.hr || presetTime.min != transferBody.min) {
		//print_save(); 
		rtc_set_time_date(presetTime, presetDate);
    secondsRtcUtcCache.cacheEneble = 0;
		timeAlignment.epochSecFirstPoint = receiveEpochSecondsRtcMoscow();
		saveFirstPointTimeToEeprom();
		
	}
	
	//set correction
	presetCorrection.timeCorrSec = timeAlignment.timeCorrSec;
	presetCorrection.timeCorrDecaMs = timeAlignment.timeCorrDecaMs;
	presetCorrection.positiveCorr = timeAlignment.positiveCorr;

	encoder_setter(-99, 99, presetCorrection.positiveCorr ? presetCorrection.timeCorrSec : -presetCorrection.timeCorrSec);
	i = scan_value(set_int_corr, presetTime, presetDate);
	presetCorrection.timeCorrSec = (i < 0) ? -i : i;
	presetCorrection.positiveCorr = (i < 0) ? 0 : 1;
	
	if (presetCorrection.positiveCorr) {
		encoder_setter(0, 99, presetCorrection.timeCorrDecaMs);
		presetCorrection.timeCorrDecaMs = scan_value(set_fract_positive_corr, presetTime, presetDate);
	} else {
		encoder_setter(-99, 0, -presetCorrection.timeCorrDecaMs);
		presetCorrection.timeCorrDecaMs = -scan_value(set_fract_negative_corr, presetTime, presetDate);
	}
	

	
  //save correction if changed
	if(presetCorrection.timeCorrSec != timeAlignment.timeCorrSec || presetCorrection.timeCorrDecaMs != timeAlignment.timeCorrDecaMs || presetCorrection.positiveCorr != timeAlignment.positiveCorr) {
		//print_save(); 
		timeAlignment.timeCorrSec = presetCorrection.timeCorrSec;
		timeAlignment.timeCorrDecaMs = presetCorrection.timeCorrDecaMs;
		timeAlignment.positiveCorr = presetCorrection.positiveCorr;
		saveTimeCorrectionToEeprom();		
		alignmentTimeCache.cacheEneble = 0;
	}
	
	//display days from first point	
	while(encoder.but_data == 0) {
	  i = ((receiveEpochSecondsRtcMoscow() - timeAlignment.epochSecFirstPoint) / 24 / 3600) % 10000;
		printYear(i, dotIndicatorBuffer);
  }
	blinkingMode = COLON_BLINKING;
	encoder.but_data = 0;
}

void printSeconds(void) {
	while(encoder.but_data == 0) {
		printSecCount(transferBody.sec, dotIndicatorBuffer);
		refreshTimeTransferBody();
	}
	encoder.but_data = 0;
}

void saveTimeCorrectionToEeprom() { 
  char i;
	sim();
  if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
  }
  rim();
  while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) ;
  
  eeprom_data.timeCorrSec = timeAlignment.timeCorrSec;
  eeprom_data.timeCorrDecaMs = timeAlignment.timeCorrDecaMs;
  eeprom_data.positiveCorr = timeAlignment.positiveCorr;
  
  FLASH->IAPSR &= ~(FLASH_IAPSR_DUL);  // lock EEPROM
}  

void saveFirstPointTimeToEeprom() { 
  char i;
	sim();
  if (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) {  // unlock EEPROM
       FLASH->DUKR = 0xAE;
       FLASH->DUKR = 0x56;
  }
  rim();
  while (!((FLASH->IAPSR) & (FLASH_IAPSR_DUL))) ;
  
  for(i =0; i<4; i++) {
    eeprom_data.epochSecFirstPoint[i] =  (timeAlignment.epochSecFirstPoint >> (i * 8)) & 0xFF;
  }
    
  FLASH->IAPSR &= ~(FLASH_IAPSR_DUL);  // lock EEPROM
}  


void populate_timeAlignment_from_eeprom() { 
  char i;
  for(i = 0; i<4; i++) {
     timeAlignment.epochSecFirstPoint |=  ((unsigned long) eeprom_data.epochSecFirstPoint[i]) << (i * 8);
  }
  timeAlignment.timeCorrSec = eeprom_data.timeCorrSec;
  timeAlignment.timeCorrDecaMs = eeprom_data.timeCorrDecaMs;
  timeAlignment.positiveCorr = eeprom_data.positiveCorr;
} 



void init_tim2 (void) {//encoder_poller
  TIM2->IER |= TIM2_IER_UIE;
	TIM2->IER |= TIM3_IER_CC1IE;
	//261 sec 50*65536=3 276 800 --> 12555 cycles / sec when 16 MHz -- CLK->CKDIVR = 0x00; //~ 16 MHz   << 0x11 - 2MHz  0x10 - 4MHz  0x01 - 8MHz  0x00 - 16MHz >>
  TIM2->PSCR=0;//0
	TIM2->ARRH=5; //2//5
  TIM2->ARRL=0;//20//0
	
	TIM2->CNTRH=0;
	TIM2->CNTRL=0;
	
	TIM2->CCR1H = 4;
	TIM2->CCR1L = 200;
	rim();
	TIM2->CR1|=TIM2_CR1_CEN;
}

void set_pwd_density(char level) {
	int val = (((int) level) * 48) / 5; //0..1224
	val = (val > 1224) ? 1224 : val;
	TIM2->CCR1H = val >> 8;
	TIM2->CCR1L = (char) val;
}


char get_ADC(char ch)
{
char h=0;
char l=0;
  ADC1->CSR=ADC1_CSR_CH&ch;
  ADC1->CR1|=ADC1_CR1_ADON; 
  ADC1->CR1|=ADC1_CR1_ADON;  //dubble 'ADON" for switch on ADC
while((ADC1->CSR&ADC1_CSR_EOC)==0)
  {nop();}

ADC1->CSR&=~ADC1_CSR_EOC;
h=ADC1->DRH;
l=ADC1->DRL;
return h;
//return ((h<<2)|l); unsigned int
}
