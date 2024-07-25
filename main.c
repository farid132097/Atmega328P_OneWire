

#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"


int main(void){
  

  OneWire_Init();
  
  uint16_t temp = ONEWIRE_START_CMD | ONEWIRE_READ_CMD | (0x55<<1);
  uint16_t return_val = 0;
  
  while(1){
      
	  
	  OneWire_TRX_Byte( temp );
	  OneWire_TRX_Byte( temp );
	  _delay_ms(500);
	  
    }
}