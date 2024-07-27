
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"
#include "uart.h"

#define  TX_MODE
//#define  RX_MODE

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
	    _delay_us(1000);
	    OneWire_TRX_Frame( (ONEWIRE_CMD_START<<9) | (ONEWIRE_OWN_AADR<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_READ <<9) | (0x01<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_WRITE<<9) | (0x02<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_CONT <<9) | (0x03<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_STOP <<9) | (0x04<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_RSVD <<9) | (0x05<<1) | 1 );
		_delay_us(200);
		
		OneWire_TRX_Frame( (ONEWIRE_CMD_START<<9) | ((ONEWIRE_OWN_AADR+1)<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_READ <<9) | (0x01<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_WRITE<<9) | (0x02<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_CONT <<9) | (0x03<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_STOP <<9) | (0x04<<1) | 1 );
		_delay_us(100);
		OneWire_TRX_Frame( (ONEWIRE_CMD_RSVD <<9) | (0x05<<1) | 1 );
	  }
	  _delay_ms(1000);
	  #endif
    }
}