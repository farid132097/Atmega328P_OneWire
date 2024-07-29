
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"
#include "uart.h"

//#define  TX_MODE
#define  RX_MODE

int main(void){
  
  #ifdef TX_MODE
  OneWire_Init_Master();
  UART_Init(38400);
  _delay_ms(1000);
  #endif
  
  #ifdef RX_MODE
  OneWire_Init_Slave();
  UART_Init(38400);
  #endif
  
  
  while(1){
      
	  
	  #ifdef TX_MODE
	  if(OneWire_Tx_Reset_Cmd() == 1){
	    _delay_us(100);
		uint8_t buf[4]={5,2,3,7};
		OneWire_Master_Send_Data(buf, 4);
	  }
	  
	  _delay_ms(10);
	  
	  if(OneWire_Tx_Reset_Cmd() == 1){
	    _delay_us(100);
		uint8_t temp = OneWire_Master_Receive_Data(0);
		UART_Transmit_Byte_Hex(temp);
		UART_Transmit_New_Line();
	  }
	  
	  _delay_ms(1000);
	  #endif
	  
	  
	  #ifdef RX_MODE
	  if(OneWire_Get_Data_Ready() == 1){
	    for(uint8_t i=0; i<OneWire_Get_Data_Buf_Index(); i++){
		  UART_Transmit_Byte_Hex(OneWire_Get_Data_Buf(i));
		  UART_Transmit_Space();
		}
		UART_Transmit_New_Line();
		OneWire_Flush_Data_Buf();
	  }
	  
	  _delay_ms(1);
	  #endif
	  
    }
}