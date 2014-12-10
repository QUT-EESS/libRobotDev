/* 
 * libRobotDev
 * RDPinDefs.h
 * Purpose: Provides mappings between raw micro pins and desired use
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 */ 

#include <avr/io.h>

#ifndef RDPINDEFS_H_
/**
 * Robot Development Pin Definitions.
 */
#define RDPINDEFS_H_

/* MICRO */

/**
 * Serial Clock pin.
 */
#define SCLK PB1

/**
 * Master Out - Slave In (SPI) pin.
 */
#define MOSI PB2

/**
 * Master In - Slave Out (SPI) pin.
 */
#define MISO PB3

/**
 * Serial Chip Enable (SPI) pin.
 */
#define SCE PC0

/**
 * Data/Command for LCD Pinout pin.
 */
#define DC PC3

/**
 * Timer1/Counter1 Output Compare A pin.
 */
#define OC1A PB5

/**
 * Timer1/Counter1 Output Compare B pin.
 */
#define OC1B PB6

/**
 * Timer3/Counter3 Output Compare A pin.
 */
#define OC3A PC6

/**
 * Timer3/Counter3 Output Compare B pin.
 */
#define OC3B PC5

/**
 * Output Compare A and PWM Output A for Timer1 pin.
 */
#define MC_PWM1A OC1A

/**
 * Output Compare B and PWM Output B for Timer1 pin.
 */
#define MC_PWM1B OC1B

/**
 * Output Compare A and PWM Output A for Timer3 pin.
 */
#define MC_PWM3A OC3A

/**
 * Output Compare B and PWM Output B for Timer3 pin.
 */
#define MC_PWM3B OC3B

/**
 * Analog to Digital Converter Battery Voltage Sampling pin.
 */
#define ADC_VBATT ADC0

/**
 * Servo Motor1 Controller pin.
 */
#define SERVO_CTRL1 PE0

/**
 * Servo Motor2 Controller pin.
 */
#define SERVO_CTRL2 PE1

/**
 * Serial Clock Line (I2C) pin.
 */
#define SCL PD0

/**
 * Serial Data Line (I2C) pin.
 */
#define SDA PD1

/**
 * UART Receive pin.
 */
#define RXD1 PD2

/**
 * UART Transmit pin.
 */
#define TXD1 PD3

/**
 * Analog to Digital Converter Pin 0.
 */
#define ADC0 PF0

/**
 * Analog to Digital Converter Pin 1.
 */
#define ADC1 PF1

/**
 * Analog to Digital Converter Pin 2.
 */
#define ADC2 PF2

/**
 * Analog to Digital Converter Pin 3.
 */
#define ADC3 PF3

/**
 * Analog to Digital Converter Pin 4.
 */
#define ADC4 PF4

/**
 * Analog to Digital Converter Pin 5.
 */
#define ADC5 PF5

/**
 * Analog to Digital Converter Pin 6.
 */
#define ADC6 PF6

/**
 * Analog to Digital Converter Pin 7.
 */
#define ADC7 PF7

/**
 * LCD LED Control pin.
 */
#define LCD_LED_CTRL PB0

/* LEDS */

/**
 * On-Board LED1.
 */
#define OB_LED1 PD2

/**
 * On-Board LED1.
 */
#define OB_LED2 PC4

/* LCD */

/**
 * LCD Serial Chip Enable (SPI) pin.
 */
#define LCD6 SCE

/**
 * LCD Reset pin.
 */
#define LCD5 PC1

/**
 * LCD Data/Command for LCD Pinout pin.
 */
#define LCD4 DC

/**
 * LCD Master Out - Slave In for LCD Pinout pin.
 */
#define LCD3 MOSI

/**
 * LCD Serial Clock pin.
 */
#define LCD2 SCLK

/**
 * LCD LED Control pin.
 */
#define LCD1 LCD_LED_CTRL

#endif // RDPINDEFS_H_

