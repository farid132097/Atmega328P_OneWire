
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

#define  TX_MODE
//#define  RX_MODE

int main(void){
  
  #ifdef TX_MODE
  OneWire_Init_Master();
  uint8_t mode = 0;
  #endif
  
  #ifdef RX_MODE
  OneWire_Init_Slave();
  #endif
  
  
  while(1){
      
	  #ifdef TX_MODE
	  
	  if( mode == 0){
	    uint8_t buf[14]={0,1,2,3,4,5,6,7,8,9,10,11,12,13};
	    OneWire_Master_Send_Data(buf, 14);
		mode = 1;
	  }
	  else{
	    OneWire_Master_Receive_Data(0x00);
		OneWire_Master_Receive_Data(0x01);
		OneWire_Master_Receive_Data(0x02);
		OneWire_Master_Receive_Data(0x03);
		OneWire_Master_Receive_Data(0x04);
		OneWire_Master_Receive_Data(0x05);
		mode = 0;
	  }
	  
	  
	  _delay_ms(500);
	  #endif
    }
}