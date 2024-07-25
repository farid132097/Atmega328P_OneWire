
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "onewire.h"



void OneWire_Struct_Init(void){
  
  
  
}





void OneWire_TRX_Set_DDR(uint8_t state){
  if(state == 0){
    ONEWIRE_TRX_DDR  &=~ (1<<ONEWIRE_TRX_BP);
  }
  else{
    ONEWIRE_TRX_DDR  |=  (1<<ONEWIRE_TRX_BP);
  }
}

void OneWire_TRX_Set_Port(uint8_t state){
  if(state == 0){
    ONEWIRE_TRX_PORT &=~ (1<<ONEWIRE_TRX_BP);
  }
  else{
    ONEWIRE_TRX_PORT |=  (1<<ONEWIRE_TRX_BP);
  }
}

uint8_t OneWire_TRX_Get_Input_State(void){
  if(ONEWIRE_TRX_PIN & (1<<ONEWIRE_TRX_BP)){
    return 1;
  }
  else{
    return 0;
  }
}

void OneWire_TRX_Set_Logic(uint8_t state){
  if(state == 0){
    OneWire_TRX_Set_DDR(1);
  }
  else{
    OneWire_TRX_Set_DDR(0);
  }
}
    

  


void OneWire_TRX_GPIO_Init(void){
  //Open-Drain + Ext. Pull-Up
  OneWire_TRX_Set_Port(0);
  OneWire_TRX_Set_DDR(0);
  OneWire_TRX_Set_Logic(1);
  
  #ifdef ONEWIRE_DEBUG_ENABLE
  ONEWIRE_DEBUG_DDR  |= (1<<ONEWIRE_DEBUG_BP);
  ONEWIRE_DEBUG_PORT &=~(1<<ONEWIRE_DEBUG_BP);
  #endif
}






void OneWire_TRX_Interrupt_Init(void){
  #ifdef  ONEWIRE_TRX_PCINT_DDRB
  #define ONEWIRE_TRX_PCIE       PCIE0
  #define ONEWIRE_TRX_PCMSK      PCMSK0
  #define ONEWIRE_TRX_PCIF       PCIF0
  #define ONEWIRE_TRX_PCINT_VECT PCINT0_vect
  #endif
  
  #ifdef  ONEWIRE_TRX_PCINT_DDRC
  #define ONEWIRE_TRX_PCIE       PCIE1
  #define ONEWIRE_TRX_PCMSK      PCMSK1
  #define ONEWIRE_TRX_PCIF       PCIF1
  #define ONEWIRE_TRX_PCINT_VECT PCINT1_vect
  #endif
  
  #ifdef  ONEWIRE_TRX_PCINT_DDRD
  #define ONEWIRE_TRX_PCIE       PCIE2
  #define ONEWIRE_TRX_PCMSK      PCMSK2
  #define ONEWIRE_TRX_PCIF       PCIF2
  #define ONEWIRE_TRX_PCINT_VECT PCINT2_vect
  #endif
  
  PCICR             |= (1<<ONEWIRE_TRX_PCIE);
  PCIFR             |= (1<<ONEWIRE_TRX_PCIF);
  ONEWIRE_TRX_PCMSK |= (1<<ONEWIRE_TRX_BP);
  
  sei();
}



void OneWire_TRX_Interrupt_Enable(void){
  ONEWIRE_TRX_PCMSK |= (1<<ONEWIRE_TRX_BP);
}

void OneWire_TRX_Interrupt_Disable(void){
  ONEWIRE_TRX_PCMSK &=~(1<<ONEWIRE_TRX_BP);
}




void OneWire_Debug_Pulse(void){
  #ifdef ONEWIRE_DEBUG_ENABLE
  ONEWIRE_DEBUG_PORT |= (1<<ONEWIRE_DEBUG_BP);
  _delay_us(1);
  ONEWIRE_DEBUG_PORT &=~(1<<ONEWIRE_DEBUG_BP);
  #endif
}




void OneWire_Delay_Clock_Low_Time(void){
  _delay_us(ONEWIRE_CLK_LOW);
}

void OneWire_Delay_Clock_High_Time(void){
  _delay_us(ONEWIRE_CLK_HIGH);
}

void OneWire_Delay_Half_Bit_Time(void){
  _delay_us(ONEWIRE_CLK_HBIT);
}




uint16_t OneWire_TRX_Byte(uint16_t val){
  
  OneWire_TRX_Interrupt_Disable();
  
  uint16_t rx_val=0;
  
  for(uint8_t i=0;i<12;i++){
    
	//Initial Logic Low
	OneWire_TRX_Set_Logic(0);
    OneWire_Delay_Clock_Low_Time();
	
	//Data Controlled By Master
	if(i < 11){
      if(val & 0x800){
	    OneWire_TRX_Set_Logic(1);
	  }else{
	    OneWire_TRX_Set_Logic(0);
	  }
	}
	//Leave For Ack
	else{
	  OneWire_TRX_Set_Logic(1);
	}
	
	val <<= 1;
	OneWire_Delay_Half_Bit_Time();
	OneWire_Debug_Pulse();
	rx_val <<= 1;
	rx_val |= OneWire_TRX_Get_Input_State();
	OneWire_Delay_Half_Bit_Time();
	
	//End Logic High
	OneWire_TRX_Set_Logic(1);
	OneWire_Delay_Clock_High_Time();
	
  }
  
  OneWire_TRX_Interrupt_Enable();
  
  return rx_val;
}






void OneWire_Init(void){
  OneWire_Struct_Init();
  OneWire_TRX_GPIO_Init();
  OneWire_TRX_Interrupt_Init();
}