/*	BASIC INTERRUPT VECTOR TABLE FOR STM8 devices
 *	Copyright (c) 2007 STMicroelectronics
 */
 
#include "stm8s.h"
#include "dot-led-indicator.h"
#include "encoder.h"
#include "main.h"


typedef void @far (*interrupt_handler_t)(void);

struct interrupt_vector {
	unsigned char interrupt_instruction;
	interrupt_handler_t interrupt_handler;
};

@far @interrupt void NonHandledInterrupt (void)
{
	/* in order to detect unexpected events during development, 
	   it is recommended to set a breakpoint on the following instruction
	*/
	return;
}

@far @interrupt void TIM2InterruptCC (void) {
	
	TIM2->SR1 &= ~TIM3_SR1_CC1IF;//flag "0"
	blankIndicator();
	enableEncoderAndTurnOffColon();
	nop();
	
}

@far @interrupt void TIM2Interrupt (void)
{
	extern char dotIndicatorBuffer[INDICATOR_HORIZONTAL_SIZE];
	extern char currentCol;
	extern unsigned int globalTim2Counter;
	extern BlinkingMode blinkingMode;
	extern encoder_t encoder;
	char i;
	
	TIM2->SR1 &= ~TIM2_SR1_UIF;//flag "0"
	
	if (encoder.transient_counter && 8) {
	  encoder_handler(&encoder);
  }
	encoder.transient_counter++;
	globalTim2Counter++;
	
	if (currentCol == INDICATOR_HORIZONTAL_SIZE) currentCol = 0;
	
	if (globalTim2Counter & 0x2000 && blinkingMode == COLON_BLINKING && !currentCol) {
		initAndTurnOnColon();
	}
		
	if (globalTim2Counter & 0x1000) {

		
	  if (blinkingMode == ALL_BLINKING) {
			refreshIndicator(currentCol++, dotIndicatorBuffer, 1);
	    return;
    } else if (blinkingMode == RIGHT_BLINKING && currentCol > 7) {
			refreshIndicator(currentCol++, dotIndicatorBuffer, 1);
	    return;
	  } else if (blinkingMode == LEFT_BLINKING && currentCol < 8) {
			refreshIndicator(currentCol++, dotIndicatorBuffer, 1);
			return;
		}
  }
	
	refreshIndicator(currentCol++, dotIndicatorBuffer, 0);
	return;
}

extern void _stext();     /* startup routine */

struct interrupt_vector const _vectab[] = {
	{0x82, (interrupt_handler_t)_stext}, /* reset */
	{0x82, NonHandledInterrupt}, /* trap  */
	{0x82, NonHandledInterrupt}, /* irq0  */
	{0x82, NonHandledInterrupt}, /* irq1  */
	{0x82, NonHandledInterrupt}, /* irq2  */
	{0x82, NonHandledInterrupt}, /* irq3  */
	{0x82, NonHandledInterrupt}, /* irq4  */
	{0x82, NonHandledInterrupt}, /* irq5  */
	{0x82, NonHandledInterrupt}, /* irq6  */
	{0x82, NonHandledInterrupt}, /* irq7  */
	{0x82, NonHandledInterrupt}, /* irq8  */
	{0x82, NonHandledInterrupt}, /* irq9  */
	{0x82, NonHandledInterrupt}, /* irq10 */
	{0x82, NonHandledInterrupt}, /* irq11 */
	{0x82, NonHandledInterrupt}, /* irq12 */
	{0x82, TIM2Interrupt}, /* irq13 */
	{0x82, TIM2InterruptCC}, /* irq14 */
	{0x82, NonHandledInterrupt}, /* irq15 */
	{0x82, NonHandledInterrupt}, /* irq16 */
	{0x82, NonHandledInterrupt}, /* irq17 */
	{0x82, NonHandledInterrupt}, /* irq18 */
	{0x82, NonHandledInterrupt}, /* irq19 */
	{0x82, NonHandledInterrupt}, /* irq20 */
	{0x82, NonHandledInterrupt}, /* irq21 */
	{0x82, NonHandledInterrupt}, /* irq22 */
	{0x82, NonHandledInterrupt}, /* irq23 */
	{0x82, NonHandledInterrupt}, /* irq24 */
	{0x82, NonHandledInterrupt}, /* irq25 */
	{0x82, NonHandledInterrupt}, /* irq26 */
	{0x82, NonHandledInterrupt}, /* irq27 */
	{0x82, NonHandledInterrupt}, /* irq28 */
	{0x82, NonHandledInterrupt}, /* irq29 */
};
