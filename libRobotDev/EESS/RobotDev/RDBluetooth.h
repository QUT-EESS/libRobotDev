/* 
 * libRobotDev
 * RDBluetooth.h
 * Purpose: Abstracts all bluetooth functions
 * Created: 29/07/2014
 * Author(s): Jerry Luck, Jeremy Pearson, Arda Yilmaz, Lachlan Cesca, Sam Cunningham-Nelson
 * Status: UNTESTED
 */ 


//Data Sheet: http://www.techbitar.com/uploads/2/0/3/1/20316977/hc-05_at_commands.pdf

#include "RDUSART.h"

#ifndef RDBLUETOOTH_H_
#define RDBLUETOOTH_H_


void RDBluetoothInit(){
  RDUSARTInit();
}


void RDBluetoothSend(char* buffer, uint16_t length){
  RDUSARTSend(buffer, length);
}

char* RDBluetoothRecieve(){
  return RDUSARTRecieve();
}

void* RDBluetoothPacket(char header, char* buffer, uint16_t length){
  /* Dynamic Packet struct
   * char   header
   * uint16_t   data_len
   * char*  data
   */
  //Allocate memory
  void* packet = (void*)malloc(sizeof(header)+length);
  //Copy data into malloc
  memcpy(packet, header, sizeof(header));           // header
  memcpy(packet+sizeof(header), length, sizeof(length));    // data_len
  memcpy(packet+sizeof(header)+sizeof(length), buffer, length); // data

  return packet;
}




/*
 * Check to see if bluetooth module returns "OK" confimration
 * 
 * @return int
 *     Whether the function was successfull or not
 */
int RDBluetoothCheckOk(){
   char okCheckStr[2];
   char okStr = "OK";
   
    int j = 0;
    for(j = 0; j<2; j++){
        okCheckStr[j] = RDBluetoothRecieve();
        if(okStr[j] != okCheckStr[j]){
            return 0; // Failed
        }
    }
    return 1; // Successfull
}


/*
 * Tests bluetooth connection, should recieve an "OK" respsonse
 * 
 */
void RDBluetoothTest(){
    char buffer = "AT"; //Caps
    uint16_t length = Sizeof(buffer)/Sizeof(char);
    RDBluetoothSend(buffer, length);
    
    int cnt = 0;
    if(RDBluetoothCheckOK() == 0 && cnt<2){
        RDBluetoothTest();
        cnt++; 
    }
    
}

/*
 * Resets bluetooth module
 * 
 */
 
void RDBluetoothReset(){
    char buffer = "AT+RESET"; //Caps
    uint16_t length = Sizeof(buffer)/Sizeof(char);
    RDBluetoothSend(buffer, length);
    
    int cnt = 0;
    if(RDBluetoothCheckOK() == 0 && cnt<2){
        RDBluetoothReset();
        cnt++;
    }
}


/*
 * Restores the bluetooth module to default status/settings
 * 
 */
void RDBluetoothRestoreDefault(){
    char buffer = "AT+ORGL"; //Caps
    uint16_t length = Sizeof(buffer)/Sizeof(char);
    RDBluetoothSend(buffer, length);
    
    int cnt = 0;
    if(RDBluetoothCheckOK() == 0 && cnt<2){
        RDBluetoothReset();
        cnt++;
    }
}



/*
 * Restores the bluetooth module to default status/settings
 * 
 */
void RDBluetoothGetSoft(){
    char buffer = "AT+VERSION?"; //Caps
    char softVersion[30];
    uint16_t length = Sizeof(buffer)/Sizeof(char);
    
    RDBluetoothSend(buffer, length);
    char recievedChar =  RDBluetoothRecieve();
    
    //Waits for reply
       
    while(recievedChar != '+'){
        //Do nothing
        recievedChar =  RDBluetoothRecieve();
    }
    
    int i = 0;
    if(recievedChar == '+'){
        while(recievedChar != '\n'){
            recievedChar =  RDBluetoothRecieve();
            softVersion[i] = recievedChar;
            i++;
        }
        
        //If message end confirmation is not recieved, send the request again
        //Up to a maximum of 2 more times
        int cnt = 0;
        if(RDBluetoothCheckOK() == 0 && cnt<2){
            RDBluetoothGetSoft();
            cnt++;
        }
    }
    
    //PRINT OR DO SOMETHING WITH IT HERE (INPUT PARAMETER?)
    
}


#endif // RDBLUETOOTH_H_
