
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "onewire.h"


typedef struct onewire_t{
  volatile uint16_t BitFrame;
  volatile uint8_t  BitCounter;
  
  volatile uint8_t  BusRstEvent;
  volatile uint8_t  AddrMatch;
  
  volatile uint16_t FrameCMD;
  volatile uint16_t FrameData;
  
  volatile uint16_t TempMode;
  volatile uint16_t SelectedMode;
  
  volatile uint16_t AddrReg;
  volatile uint16_t DataReg;

  volatile uint8_t  ACK;
  volatile uint8_t  DataReady;
  volatile uint8_t  SessionEnd;
  
  volatile uint8_t  DataBuf[ONEWIRE_DATA_BUF];
  volatile uint8_t  DataBufCounter;
}onewire_t;

onewire_t OneWire;

void OneWire_Struct_Init(void){
  OneWire.BitFrame = 0;
  OneWire.BitCounter = 0;
  
  OneWire.BusRstEvent = 0;
  OneWire.AddrMatch = 0;
  
  OneWire.FrameCMD = 0;
  OneWire.FrameData = 0;
  OneWire.ACK = 0;
  
  OneWire.TempMode = 0;
  OneWire.SelectedMode = 0;
  
  OneWire.AddrReg = 0;
  OneWire.DataReg = 0;
  
  
  OneWire.DataReady = 0;
  OneWire.SessionEnd = 1;
}

void OneWire_Flush_Bit_Frame(void){
  OneWire.BitFrame = 0;
  OneWire.BitCounter = 0;
}

void OneWire_Flush_Frame(void){
  OneWire.FrameCMD = 0;
  OneWire.FrameData = 0;
  OneWire.ACK = 0;
}

void OneWire_Flush_Data_Buf(void){
  for(uint8_t i=0; i<ONEWIRE_DATA_BUF; i++){
    OneWire.DataBuf[i] = 0;
  }
  OneWire.DataBufCounter = 0;
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
  //Open-Drain + Ext Pull-Up
  OneWire_TRX_Set_Port(0);
  OneWire_TRX_Set_DDR(0);
  OneWire_TRX_Set_Logic(1);
  
  #ifdef ONEWIRE_DBGTX_ENABLE
  ONEWIRE_DBGTX_DDR  |= (1<<ONEWIRE_DBGTX_BP);
  ONEWIRE_DBGTX_PORT &=~(1<<ONEWIRE_DBGTX_BP);
  #endif
  
  #ifdef ONEWIRE_DBGRX_ENABLE
  ONEWIRE_DBGRX_DDR  |= (1<<ONEWIRE_DBGRX_BP);
  ONEWIRE_DBGRX_PORT &=~(1<<ONEWIRE_DBGRX_BP);
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
  ONEWIRE_TRX_PCMSK &=~(1<<ONEWIRE_TRX_BP);
  
  sei();
}



void OneWire_TRX_Interrupt_Enable(void){
  ONEWIRE_TRX_PCMSK |= (1<<ONEWIRE_TRX_BP);
}

void OneWire_TRX_Interrupt_Disable(void){
  ONEWIRE_TRX_PCMSK &=~(1<<ONEWIRE_TRX_BP);
}

uint8_t OneWire_TRX_Interrupt_Status(void){
  if(ONEWIRE_TRX_PCMSK & (1<<ONEWIRE_TRX_BP)){
    return 1;
  }
  else{
    return 0;
  }
}

uint8_t OneWire_TRX_Falling_Edge_Interrupt(void){
  if(OneWire_TRX_Get_Input_State() == 0){
    return 1;
  }
  else{
    return 0;
  }
}






void OneWire_Debug_Tx_Pulse(uint8_t repeat){
  #ifdef ONEWIRE_DBGTX_ENABLE
  for(uint8_t i=0; i<repeat; i++){
    ONEWIRE_DBGTX_PORT |= (1<<ONEWIRE_DBGTX_BP);
    asm("nop");
	asm("nop");
    ONEWIRE_DBGTX_PORT &=~(1<<ONEWIRE_DBGTX_BP);
    asm("nop");
	asm("nop");
  }
  #endif
}

void OneWire_Debug_Rx_Pulse(uint8_t repeat){
  #ifdef ONEWIRE_DBGRX_ENABLE
  for(uint8_t i=0; i<repeat; i++){
    ONEWIRE_DBGRX_PORT |= (1<<ONEWIRE_DBGRX_BP);
    asm("nop");
	asm("nop");
    ONEWIRE_DBGRX_PORT &=~(1<<ONEWIRE_DBGRX_BP);
	asm("nop");
	asm("nop");
  }
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

void OneWire_Delay_Byte_Gap(void){
  _delay_us(ONEWIRE_BYTE_GAP);
}

void OneWire_Delay_Rx_Int(void){
  OneWire_TRX_Interrupt_Disable();
  OneWire_Delay_Clock_Low_Time();
  OneWire_Delay_Half_Bit_Time();
  OneWire_TRX_Interrupt_Enable();
}


uint8_t OneWire_Tx_Reset_Cmd(void){
  uint8_t ack_sts=0;
  OneWire_TRX_Interrupt_Disable();
  for(uint8_t i=0;i<(ONEWIRE_FRAME_LEN*2);i++){
    //Initial low logic
	OneWire_TRX_Set_Logic(0);
    OneWire_Delay_Clock_Low_Time();
	OneWire_TRX_Set_Logic(1);
	
	//leave for ack
	OneWire_Delay_Half_Bit_Time();
	OneWire_Debug_Tx_Pulse(1);
	ack_sts = OneWire_TRX_Get_Input_State();
	OneWire_Delay_Half_Bit_Time();
	OneWire_TRX_Set_Logic(1);
	OneWire_Delay_Clock_High_Time();
	if(ack_sts == 0){
	  ack_sts = 1;
	  break;
	}
  }
  OneWire_TRX_Interrupt_Enable();
  OneWire_Delay_Byte_Gap();
  return ack_sts;
}


uint16_t OneWire_TRX_Frame(uint16_t val){
  uint16_t rx_val=0;
  OneWire_TRX_Interrupt_Disable();
  for(uint8_t i=0;i<ONEWIRE_FRAME_LEN;i++){
	//Initial Logic Low
	OneWire_TRX_Set_Logic(0);
    OneWire_Delay_Clock_Low_Time();
	//Data Controlled By Master
	if(i < (ONEWIRE_FRAME_LEN-1)){
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
	OneWire_Debug_Tx_Pulse(1);
	rx_val <<= 1;
	rx_val |= OneWire_TRX_Get_Input_State();
	OneWire_Delay_Half_Bit_Time();
	//End Logic High
	OneWire_TRX_Set_Logic(1);
	OneWire_Delay_Clock_High_Time();
  }
  OneWire_TRX_Interrupt_Enable();
  OneWire_Delay_Byte_Gap();
  return rx_val;
}


uint16_t OneWire_TRX_Cmd_Data(uint16_t cmd, uint16_t data){
  uint16_t temp = cmd;
  temp  |= data;
  temp <<= 1;
  temp  |= 1;
  temp   = OneWire_TRX_Frame(temp);
  return temp;
}





void OneWire_Handle_Reset_Event(void){
  OneWire.AddrMatch = 0;
  OneWire.BusRstEvent = 1;
  OneWire.TempMode = 0;
  OneWire.SelectedMode = 0;
  OneWire.ACK = 1;
  OneWire_Debug_Rx_Pulse(6);
}


void OneWire_Handle_Start_Event(void){
  OneWire.BusRstEvent = 0;
  OneWire.AddrMatch = 1;
  OneWire.SessionEnd = 0;
  OneWire.ACK = 1;
  OneWire_Debug_Rx_Pulse(5);
}


void OneWire_Handle_Read_Event(void){
  OneWire.AddrReg = OneWire.FrameData;
  OneWire.TempMode = ONEWIRE_CMD_READ;
  OneWire.ACK = 1;
  OneWire_Debug_Rx_Pulse(4);
}

void OneWire_Handle_Write_Event(void){
  OneWire.AddrReg = OneWire.FrameData;
  OneWire.TempMode = ONEWIRE_CMD_WRITE;
  OneWire.ACK = 1;
  OneWire_Debug_Rx_Pulse(3);
}

void OneWire_Handle_Cont_Event(void){
  OneWire.ACK = 1;
  if(OneWire.TempMode == ONEWIRE_CMD_WRITE){
	OneWire.DataBuf[OneWire.DataBufCounter] = OneWire.FrameData;
	OneWire.DataBufCounter++;
	if(OneWire.DataBufCounter>=ONEWIRE_DATA_BUF){
	  OneWire.DataBufCounter = 0;
	}
  }
  OneWire_Debug_Rx_Pulse(2);
}

void OneWire_Handle_Stop_Event(void){
  OneWire.AddrMatch = 0;
  OneWire.BusRstEvent = 0;
  OneWire.SessionEnd = 1;
  OneWire.ACK = 1;
  OneWire_Debug_Rx_Pulse(1);
}



void OneWire_Handle_Cmd_Data(void){
  //reset request -> Send ack
  if(OneWire.BitFrame == ONEWIRE_CMD_RESET){
	OneWire_Handle_Reset_Event();
  }
  //Address matched -> Send ack
  else if((OneWire.FrameCMD == ONEWIRE_CMD_START) && (OneWire.FrameData == ONEWIRE_OWN_AADR)){
	if(OneWire.BusRstEvent == 1){
	  OneWire_Handle_Start_Event();
	}
  }
  //Read cmd -> Send ack
  else if((OneWire.FrameCMD == ONEWIRE_CMD_READ) && (OneWire.AddrMatch == 1)){
	OneWire_Handle_Read_Event();
  }
  //Write cmd -> Send ack
  else if((OneWire.FrameCMD == ONEWIRE_CMD_WRITE) && (OneWire.AddrMatch == 1)){
	OneWire_Handle_Write_Event();
  }
  //Continue cmd -> Send ack
  else if((OneWire.FrameCMD == ONEWIRE_CMD_CONT) && (OneWire.AddrMatch == 1)){
	OneWire_Handle_Cont_Event();
  }
  //Stop cmd -> Send ack
  else if((OneWire.FrameCMD == ONEWIRE_CMD_STOP) && (OneWire.AddrMatch == 1)){
	OneWire_Handle_Stop_Event();
  }
  //Nothing Matched, Send nack
  else{
	OneWire.ACK = 0;
    OneWire.AddrMatch = 0;
	OneWire.BusRstEvent = 0;
  }
}

void OneWire_Select_Mode(void){
  if(OneWire.TempMode == ONEWIRE_CMD_WRITE){
	OneWire.SelectedMode = ONEWIRE_CMD_WRITE;
  }
  else if(OneWire.TempMode == ONEWIRE_CMD_READ){
	OneWire.SelectedMode = ONEWIRE_CMD_READ;
	if(OneWire.DataReady == 0){
	  OneWire_Debug_Rx_Pulse(7);
	  OneWire_Set_Data_Reg(0x9C);
	  OneWire.DataReady = 1;
	}
  }
  else{
	OneWire.SelectedMode = 0;
  }
}


void OneWire_Master_Process_Cmd_Data(void){
  OneWire_Delay_Rx_Int();
  OneWire.BitFrame <<= 1;
  OneWire_Debug_Rx_Pulse(1);
  OneWire.BitFrame |= OneWire_TRX_Get_Input_State();
  if(OneWire.BitCounter == (ONEWIRE_FRAME_LEN-2)){
	OneWire.FrameCMD  = OneWire.BitFrame & ONEWIRE_CMD_MASK;
	OneWire.FrameData = OneWire.BitFrame & ONEWIRE_DATA_MASK;
	OneWire_Handle_Cmd_Data();
  }
}

void OneWire_Master_Process_Ack(void){
  //maybe need to disable interrupt
  OneWire_Debug_Rx_Pulse(1);
  if(OneWire.ACK == 1){
	OneWire.ACK = 0;
    OneWire_TRX_Set_Logic(0);
  }
  OneWire_Delay_Half_Bit_Time();
  OneWire_Delay_Half_Bit_Time();
  OneWire_TRX_Set_Logic(1);
  OneWire_Debug_Rx_Pulse(1);
  //maybe need to enable interrupt
}




void OneWire_Bit_Frame_Sample(void){

  if( (OneWire.SelectedMode == ONEWIRE_CMD_WRITE) || (OneWire.SelectedMode == 0) ){
  
    if(OneWire.BitCounter <= (ONEWIRE_FRAME_LEN-2)){
	  OneWire_Master_Process_Cmd_Data();
	}
	
	else if(OneWire.BitCounter == (ONEWIRE_FRAME_LEN-1)){
	  OneWire_Master_Process_Ack();
	}
  }
  else if(OneWire.SelectedMode == ONEWIRE_CMD_READ){
    OneWire_Read_Mode_Feedback();
  }
  
  
  OneWire.BitCounter++;
  if(OneWire.BitCounter >= ONEWIRE_FRAME_LEN){
    OneWire_Flush_Bit_Frame();
	OneWire_Select_Mode();
  }
}


void OneWire_Read_Mode_Feedback(void){
  OneWire_TRX_Interrupt_Disable();
  if(OneWire.DataReg & 0x800){
	OneWire_TRX_Set_Logic(1);
  }else{
	OneWire_TRX_Set_Logic(0);
  }
  OneWire.DataReg <<= 1;
  OneWire_Delay_Half_Bit_Time();
  OneWire_Debug_Rx_Pulse(1);
  OneWire_Delay_Half_Bit_Time();
  OneWire_TRX_Set_Logic(1);
  if(OneWire.BitCounter == (ONEWIRE_FRAME_LEN-1)){
    OneWire.TempMode = 0;
    OneWire.SelectedMode = 0;
	OneWire.DataReady = 0;
	OneWire_Debug_Rx_Pulse(2);
  }
  OneWire_TRX_Interrupt_Enable();
}


void OneWire_Master_Send_Data(uint8_t *data, uint8_t len){
  OneWire_TRX_Cmd_Data(ONEWIRE_CMD_START, ONEWIRE_OWN_AADR);
  OneWire_TRX_Cmd_Data(ONEWIRE_CMD_WRITE, 0x00);
  for(uint8_t i=0;i<len;i++){
    OneWire_TRX_Cmd_Data(ONEWIRE_CMD_CONT,  data[i]);
  }
  OneWire_TRX_Cmd_Data(ONEWIRE_CMD_STOP , 0x00);
}

uint8_t OneWire_Master_Receive_Data(uint8_t addr){
  OneWire_TRX_Cmd_Data(ONEWIRE_CMD_START, ONEWIRE_OWN_AADR);
  OneWire_TRX_Cmd_Data(ONEWIRE_CMD_READ , addr);
  uint16_t temp0 = OneWire_TRX_Cmd_Data(ONEWIRE_CMD_CONT , 0xFF);
  uint16_t temp1 = OneWire_TRX_Cmd_Data(ONEWIRE_CMD_STOP , 0x00);
  temp0 >>= 1;
  temp0 &= 0xFF;
  if((temp1 & 0x01) == 1){
    temp0 = 0;
  }
  return temp0;
}





ISR(ONEWIRE_TRX_PCINT_VECT){
  //TRX Interrupt Fired
  if( OneWire_TRX_Interrupt_Status() == 1){        
	//Falling Edge Interrupt
    if(OneWire_TRX_Falling_Edge_Interrupt() == 1){
	  //handle frame bits and ack
	  OneWire_Bit_Frame_Sample();
	}
  }
}


void OneWire_Set_Data_Reg(uint8_t val){
  OneWire.DataReg = val;
  OneWire.DataReg <<=1;
  OneWire.DataReg  |= (ONEWIRE_CMD_READ<<1) | 1;
}

uint8_t OneWire_Get_Session_End(void){
  return OneWire.SessionEnd;
}

uint8_t OneWire_Get_Data_Ready(void){
  if((OneWire_Get_Session_End() == 1) && (OneWire_Get_Data_Buf_Index() !=0) ){
    return 1;
  }
  else{
    return 0;
  }
}

uint8_t OneWire_Get_Data_Buf(uint8_t index){
  return OneWire.DataBuf[index];
}

uint8_t OneWire_Get_Data_Buf_Index(void){
  return OneWire.DataBufCounter;
}






void OneWire_Init_Master(void){
  OneWire_Struct_Init();
  OneWire_TRX_GPIO_Init();
  OneWire_TRX_Interrupt_Init();
  OneWire_TRX_Interrupt_Disable();
}

void OneWire_Init_Slave(void){
  OneWire_Struct_Init();
  OneWire_TRX_GPIO_Init();
  OneWire_TRX_Interrupt_Init();
  OneWire_TRX_Interrupt_Enable();
}