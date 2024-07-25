

#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"


int main(void){
  

  OneWire_Init_Master();
  
  uint16_t Slave_Addr = ONEWIRE_START_CMD | ONEWIRE_READ_CMD | (0x3F<<1) | 1;
  uint16_t Reg_Addr   = ONEWIRE_CONT_CMD  | ONEWIRE_READ_CMD | (0x01<<1) | 1;
  uint16_t Data_Len   = ONEWIRE_CONT_CMD  | ONEWIRE_READ_CMD | (0xFF<<1) | 1;
  while(1){
      
	  
	  OneWire_TRX_Byte( Slave_Addr );
	  OneWire_TRX_Byte( Reg_Addr );
	  OneWire_TRX_Byte( Data_Len );
	  _delay_ms(500);
	  
    }
}