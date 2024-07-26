
#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"
#include "uart.h"

#define  TX_MODE
//#define  RX_MODE

int main(void){
  
  UART_Init(19200);
  
  
  #ifdef TX_MODE
  UART_Transmit_Text("TX Mode\r\n");
  OneWire_Init_Master();
  uint8_t mode = 0;
  _delay_ms(1000);
  #endif
  
  #ifdef RX_MODE
  UART_Transmit_Text("RX Mode\r\n");
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
	    UART_Transmit_Text("Tx data :");
		for(uint8_t i=0;i<6; i++){
		  UART_Transmit_Number(OneWire_Master_Receive_Data(i));
		  UART_Transmit_Space();
		  _delay_ms(1);
		}
	    UART_Transmit_New_Line();
		mode = 0;
	  }
	  
	  
	  _delay_ms(100);
	  #endif
	  
	  
	  #ifdef RX_MODE
	  if(OneWire_Get_Data_Buf_Index()>=14){
	    UART_Transmit_Text("Rx data :");
		for(uint8_t i=0;i<OneWire_Get_Data_Buf_Index(); i++){
		  UART_Transmit_Number(OneWire_Get_Data_Buf(i));
		  UART_Transmit_Space();
		}
		UART_Transmit_New_Line();
		OneWire_Flush_Data_Buf();
	  }
	  #endif
	  
	  
    }
}