/* 
 * libRobotDev
 * RDPinDefs.h
 * Purpose: Provides mappings between raw micro pins and desired use
 * Created: 25/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson
 */ 

#ifndef RDPINDEFS_H_
#define RDPINDEFS_H_

/* MICRO */
#define SCLK PB1
#define MOSI PB2
#define MISO PB3
#define OC1A PB5
#define OC1B PB6
#define SCE3 PC0
#define RESET3 PC1
#define DC PC3
#define MC_PWM1A OC1A
#define MC_PWM1B OC1B
#define MC_PWM3A OC3A
#define MC_PWM3B OC3B
#define ADC_VBATT ADC0
#define SERVO_CTRL1 PE0
#define SERVO_CTRL2 PE1
#define RESET1 PC7
#define SCL PD0
#define SDA PD1
#define RXD1 PD2
#define TXD1 PD3
#define OC3B PC5
#define OC3A PC6
#define ADC0 PF0
#define ADC1 PF1
#define ADC2 PF2
#define ADC3 PF3
#define ADC4 PF4
#define ADC5 PF5
#define ADC6 PF6
#define ADC7 PF7
#define LCD_LED_CTRL PB0

/* LEDS */
#define OB_LED1 PD2
#define OB_LED2 PC4

/* MOTOR CTRL */
#define MCTRL_IN1 MC_PWM1
#define MCTRL_IN2 MC_PWM2
#define MCTRL_IN3 MC_PWM3
#define MCTRL_IN4 MC_PWM4

/* LCD */
#define H_LCD6 SCE3
#define H_LCD5 RESET3
#define H_LCD4 DC
#define H_LCD3 MOSI
#define H_LCD2 SCLK
#define H_LCD1 LCD_LED_CTRL

/* ADC HEADER */
#define H_ADC1 ADC1
#define H_ADC2 ADC2
#define H_ADC3 ADC3
#define H_ADC4 ADC4
#define H_ADC5 ADC5
#define H_ADC6 ADC6
#define H_ADC7 ADC7

#endif // RDPINDEFS_H_

