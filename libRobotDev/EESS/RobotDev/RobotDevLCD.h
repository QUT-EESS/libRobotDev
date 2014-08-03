/* libRobotDev
 * RobotDevLCD.h
 * Purpose: Provides LCD functions
 * Created: 27/07/2014
 * Author(s): Jerry Luck
 * Status: UNTESTED
 */ 


#ifndef ROBOTDEVLCD_H_
#define ROBOTDEVLCD_H_


static void RDSPIInit( void ){
	set_bit(DDRB, SCLK);
	set_bit(DDRB, MOSI);
	set_bit(DDRC, SCE3);
	//Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

static void RDSPIWriteByte(unsigned char byte){
	
}

void RDLCDInit( void ){
	RDSPIInit();
}

static void RDLCDWrite(unsigned char data){
	
}

void RDLCDClear( void ){
	
}

void RDLCDCharacter(unsigned char character){
	
}

void RDLCDString(unsigned char* buffer){
	uint8_t idx = 0;
	char c_tmp = buffer[0];
	while(c_tmp != 0x00)
	{
		idx++;
		RDLCDCharacter(c_tmp);
		c_tmp = buffer[idx];
	}
}

void RDLCDPos(unsigned char x, unsigned char y){
	
}

#endif /* ROBOTDEVLCD_H_ */