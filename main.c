
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

//#define  TX_MODE
#define  RX_MODE

int main(void){
  
  #ifdef TX_MODE
  OneWire_Init_Master();
  uint16_t Reg_Addr   = ONEWIRE_START_CMD | ONEWIRE_READ_CMD | 1;
  uint16_t Act_Data   = ONEWIRE_STOP_CMD  | ONEWIRE_READ_CMD | 1;
  uint8_t  addr_inc   = 0;
  #endif
  
  #ifdef RX_MODE
  OneWire_Init_Slave();
  #endif
  
  
  while(1){
      
	  #ifdef TX_MODE
	  OneWire_TRX_Byte( Reg_Addr | (addr_inc<<1) );
	  OneWire_TRX_Byte( Act_Data | (0xFF<<1));
	  addr_inc++;
	  _delay_ms(500);
	  #endif
    }
}