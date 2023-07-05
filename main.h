#include "stm8s.h"

#define TASK_TIME_TABLE_SIZE 10

typedef enum {time} menu_t;
typedef enum {set_hr, set_min, set_year, set_month, set_day, set_int_corr, set_fract_positive_corr, set_fract_negative_corr} set_selector_t;

typedef struct {
	      char epochSecFirstPoint[4];
        char timeCorrSec;
        char timeCorrDecaMs;
        char positiveCorr;
} eeprom_data_t;


void init_tim2 (void);
void set_pwd_density(char level);

void menu_partition(menu_t menu);
void populate_timeAlignment_from_eeprom(void);
void saveTimeCorrectionToEeprom(void);
void saveFirstPointTimeToEeprom(void); 
void saveStrokeDataToEeprom(void); 
char menu_selector(void);
void printSeconds(void);
//int scan_value(set_selector_t set_selector, LocalTime presetTime, LocalDate presetDate) ;
void long_delay(void);
char get_ADC(char ch);
