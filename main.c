
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"
#include "uart.h"

#define  TX_MODE
#define  RX_MODE

int main(void){
  
  #ifdef TX_MODE
  OneWire_Init_Master();
  _delay_ms(1000);
  #endif
  
  #ifdef RX_MODE
  OneWire_Init_Slave();
  #endif
  
  
  while(1){
      
	  
	  #ifdef TX_MODE
	  if(OneWire_Tx_Reset_Cmd() == 1){
	    _delay_us(100);
		OneWire_TRX_Cmd_Data(ONEWIRE_CMD_START, ONEWIRE_OWN_AADR);
	    OneWire_TRX_Cmd_Data(ONEWIRE_CMD_WRITE, 0x01);
		OneWire_TRX_Cmd_Data(ONEWIRE_CMD_STOP , 0x07);
	  }
	  _delay_ms(1000);
	  #endif
    }
}