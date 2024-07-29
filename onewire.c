
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "onewire.h"


typedef struct onewire_t{
  volatile uint16_t BitFrame;
  volatile uint16_t RawBitFrame;
  volatile uint8_t  BitCounter;
  volatile uint8_t  BusRstEvent;
  volatile uint8_t  AddrMatch;
  
  volatile uint16_t CMD;
  volatile uint16_t RegAddr;
  volatile uint16_t RegData;

  volatile uint8_t  ACK;
  volatile uint8_t  NewDataAvailable;
  volatile uint8_t  DataBuf[ONEWIRE_DATA_BUF];
  volatile uint8_t  DataBufCounter;
}onewire_t;

onewire_t OneWire;

void OneWire_Struct_Init(void){
  OneWire.BitFrame = 0;
  OneWire.RawBitFrame = 0;
  OneWire.BitCounter = 0;
  OneWire.BusRstEvent = 0;
  OneWire.AddrMatch = 0;
  OneWire.RegData = 0;
  OneWire.CMD = 0;
  OneWire.ACK = 0;
  OneWire.NewDataAvailable = 0;
}

void OneWire_Flush_Bit_Frame(void){
  OneWire.BitFrame = 0;
  OneWire.RawBitFrame = 0;
  OneWire.BitCounter = 0;
}

void OneWire_Flush_Cmd_Reg(void){
  OneWire.RegData = 0;
  OneWire.CMD = 0;
  OneWire.ACK = 0;
  OneWire.NewDataAvailable = 0;
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






void OneWire_Debug_Tx_Pulse(void){
  #ifdef ONEWIRE_DBGTX_ENABLE
  ONEWIRE_DBGTX_PORT |= (1<<ONEWIRE_DBGTX_BP);
  _delay_us(1);
  ONEWIRE_DBGTX_PORT &=~(1<<ONEWIRE_DBGTX_BP);
  #endif
}

void OneWire_Debug_Rx_Pulse(void){
  #ifdef ONEWIRE_DBGRX_ENABLE
  ONEWIRE_DBGRX_PORT |= (1<<ONEWIRE_DBGRX_BP);
  _delay_us(1);
  ONEWIRE_DBGRX_PORT &=~(1<<ONEWIRE_DBGRX_BP);
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
	OneWire_Debug_Tx_Pulse();
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
	OneWire_Debug_Tx_Pulse();
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


void OneWire_Handle_Cmd_Data(void){
  //reset request -> Send ack
  if(OneWire.BitFrame == ONEWIRE_CMD_RESET){
	OneWire.AddrMatch = 0;
    OneWire.BusRstEvent = 1;
	OneWire.ACK = 1;
    OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
  }
  
  //Address matched -> Send ack
  else if((OneWire.CMD == ONEWIRE_CMD_START) && (OneWire.RegData == ONEWIRE_OWN_AADR)){
	if(OneWire.BusRstEvent == 1){
	  OneWire.BusRstEvent = 0;
	  OneWire.AddrMatch = 1;
	  OneWire.ACK = 1;
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	}
  }
  
  //Read cmd -> Send ack
  else if((OneWire.CMD == ONEWIRE_CMD_READ) && (OneWire.AddrMatch == 1)){
	OneWire.RegAddr = OneWire.RegData;
	OneWire.ACK = 1;
    OneWire_Debug_Rx_Pulse();
    OneWire_Debug_Rx_Pulse();
    OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
  }
  
  //Write cmd -> Send ack
  else if((OneWire.CMD == ONEWIRE_CMD_WRITE) && (OneWire.AddrMatch == 1)){
	OneWire.RegAddr = OneWire.RegData;
	OneWire.ACK = 1;
    OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
  }
  
  //Continue cmd -> Send ack
  else if((OneWire.CMD == ONEWIRE_CMD_CONT) && (OneWire.AddrMatch == 1)){
	OneWire.ACK = 1;
    OneWire_Debug_Rx_Pulse();
	OneWire_Debug_Rx_Pulse();
  }
 
  //Stop cmd -> Send ack
  else if((OneWire.CMD == ONEWIRE_CMD_STOP) && (OneWire.AddrMatch == 1)){
	OneWire.AddrMatch = 0;
    OneWire.BusRstEvent = 0;
	OneWire.ACK = 1;
    OneWire_Debug_Rx_Pulse();
  }

  //Nothing Matched, Send nack
  else{
	OneWire.ACK = 0;
    OneWire.AddrMatch = 0;
	OneWire.BusRstEvent = 0;
  }
}


void OneWire_Bit_Frame_Sample(void){
  //Last bit of the frame -> ACK bit
  if(OneWire.BitCounter == (ONEWIRE_FRAME_LEN-1)){
    //maybe need to disable interrupt
	OneWire_Debug_Rx_Pulse();
	if(OneWire.ACK == 1){
	  OneWire.ACK = 0;
      OneWire_TRX_Set_Logic(0);
	}
	OneWire_Delay_Half_Bit_Time();
	//OneWire.BitFrame <<= 1;
	//OneWire.BitFrame |= OneWire_TRX_Get_Input_State();
    OneWire_Delay_Half_Bit_Time();
	OneWire_TRX_Set_Logic(1);
	OneWire_Debug_Rx_Pulse();
	//maybe need to enable interrupt
  }
  //Regular bits to sample
  else{
    OneWire_Delay_Rx_Int();
    OneWire.BitFrame <<= 1;
    OneWire_Debug_Rx_Pulse();
    OneWire.BitFrame |= OneWire_TRX_Get_Input_State();
	
	if(OneWire.BitCounter == (ONEWIRE_FRAME_LEN-2)){
	  OneWire.CMD     = OneWire.BitFrame & ONEWIRE_CMD_MASK;
	  OneWire.RegData = OneWire.BitFrame & ONEWIRE_DATA_MASK;
	  OneWire_Handle_Cmd_Data();
	}
	
  }
  OneWire.BitCounter++;
}


uint8_t OneWire_Bit_Counter_Overflow(void){
  if(OneWire.BitCounter >= ONEWIRE_FRAME_LEN){
    OneWire.BitCounter = 0;
	
	OneWire.BitFrame = 0; ///////////////////////////////test
	
	return 1;
  }
  else{
    return 0;
  }
}


uint16_t OneWire_Build_Bit_Frame(void){
  uint16_t temp = 0xFFFF;
  OneWire_Bit_Frame_Sample();
  if(OneWire_Bit_Counter_Overflow() == 1){
    temp = OneWire.BitFrame;
	OneWire.BitFrame = 0;
  }
  return temp;
}

void OneWire_Extract_Cmd_Data(void){
  uint16_t temp = OneWire_Build_Bit_Frame();
  if(temp != 0xFFFF){
    OneWire.RawBitFrame = temp;
	OneWire.CMD = temp & ONEWIRE_CMD_MASK;
	OneWire.ACK = temp & ONEWIRE_ACK_MASK;
	temp >>= 1;
	temp &= 0xFF;
	OneWire.RegData = temp;
	OneWire.NewDataAvailable = 1;
  }
}

void OneWire_Handle_Data(void){
  OneWire_Extract_Cmd_Data();
  if(OneWire.NewDataAvailable == 1){
    if( (OneWire.RawBitFrame & 0xFFE)==0xFFE ){
      //reset request
	  OneWire_Flush_Bit_Frame();
	  OneWire_Flush_Cmd_Reg();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
    }
    else if(OneWire.CMD == ONEWIRE_CMD_START){
      //renew buf
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
    }
    else if(OneWire.CMD == ONEWIRE_CMD_CONT){
      //renew buf
	  OneWire_Debug_Rx_Pulse();
	  OneWire_Debug_Rx_Pulse();
    }
    else if(OneWire.CMD == ONEWIRE_CMD_STOP){
      //renew buf
	  OneWire_Debug_Rx_Pulse();
    }
	OneWire.NewDataAvailable = 0;
  }
}

void OneWire_Buf_Sample_And_Update(void){
/*  OneWire.FrameBuf[OneWire.FrameBufIndex] = OneWire.FrameVal;
  OneWire.FrameVal = 0;
  OneWire.FrameBufIndex++;*/
}

uint8_t OneWire_Buf_Counter_Overflow(void){
  /*if(OneWire.FrameBufIndex >= ONEWIRE_FRAME_BUF){
    OneWire.FrameBufIndex = 0;
	OneWire.FrameCounter++;
	return 1;
  }
  else{
    OneWire.FrameCounter++;
	return 0;
  }*/
}

void OneWire_Fill_Buf(void){
  /*OneWire_Buf_Sample_And_Update();
  OneWire_Buf_Counter_Overflow();
  
  if((OneWire.FrameBufIndex > 0) && ((OneWire.FrameBuf[OneWire.FrameBufIndex-1] & ONEWIRE_START_CMD) == ONEWIRE_START_CMD) ){
    OneWire.CmdReceived = 1;
	OneWire.CmdVal = OneWire.FrameBuf[OneWire.FrameBufIndex-1] & ONEWIRE_RW_MASK;
	OneWire.CmdValReg = (OneWire.FrameBuf[OneWire.FrameBufIndex-1] & ONEWIRE_REG_MASK)>>1;
	OneWire_Flush_Frame_Buf();
	OneWire_Debug_Rx_Pulse();
  }*/
}

void OneWire_Fill_Buf_Copy_Data(void){
  /*OneWire_Buf_Sample_And_Update();
  OneWire_Buf_Counter_Overflow();
  
  if( (OneWire.FrameBuf[OneWire.FrameBufIndex-1] & ONEWIRE_STOP_CMD) == ONEWIRE_STOP_CMD){
    //Copy data to DataBuf
    for(uint8_t i=0;i<OneWire.FrameBufIndex;i++){
	  OneWire.DataBuf[OneWire.DataBufIndex] = (OneWire.FrameBuf[i]>>1) & 0xFF;
	  OneWire.DataBufIndex++;
	  if(OneWire.DataBufIndex >= ONEWIRE_DATA_BUF){
	    OneWire.DataBufIndex = 0;
	  }
	}
    OneWire.CmdReceived = 0;
	OneWire_Flush_Frame_Buf();
	OneWire.CmdVal = 0;
	OneWire.CmdValReg = 0;
	OneWire.FeedbackDataLoaded = 0;
	OneWire.FeedbackCounter = 0;
	OneWire_Debug_Rx_Pulse();
  }*/
}


void OneWire_Read_Mode_Feedback(void){
  /*OneWire_TRX_Interrupt_Disable();
  if(OneWire.FeedbackData & 0x800){
	OneWire_TRX_Set_Logic(1);
  }else{
	OneWire_TRX_Set_Logic(0);
  }
  OneWire.FeedbackData <<= 1;
  OneWire_Delay_Half_Bit_Time();
  OneWire_Delay_Half_Bit_Time();
  OneWire_TRX_Set_Logic(1);
  OneWire_TRX_Interrupt_Enable();*/
}


void OneWire_Master_Send_Data(uint8_t *data, uint8_t len){
  /*uint16_t StartByte = ONEWIRE_START_CMD | ONEWIRE_WRITE_CMD | 1;
  uint16_t DataByte  = ONEWIRE_CONT_CMD  | ONEWIRE_WRITE_CMD | 1;
  uint16_t EndByte   = ONEWIRE_STOP_CMD  | ONEWIRE_WRITE_CMD | 1;
  uint16_t temp      = 0;
  OneWire_TRX_Byte(StartByte);
  _delay_us(100);
  for(uint8_t i=0;i<len;i++){
    if(i == (len-1)){
	  temp = EndByte  | (data[i]<<1);
	}
	else{
	  temp = DataByte | (data[i]<<1);
	}
	OneWire_TRX_Byte(temp);
  }
  _delay_us(200);*/
}

uint8_t OneWire_Master_Receive_Data(uint8_t addr){
  /*uint16_t StartByte = ONEWIRE_START_CMD | ONEWIRE_READ_CMD | 1;
  uint16_t EndByte   = ONEWIRE_STOP_CMD  | ONEWIRE_READ_CMD | 1;
  uint16_t temp      = 0;
  OneWire_TRX_Byte(StartByte | (addr<<1));
  _delay_ms(1);
  temp = OneWire_TRX_Byte(EndByte | (0xFF<<1));
  temp >>= 1;
  temp &= 0xFF;
  return (uint8_t)temp;*/
}





ISR(ONEWIRE_TRX_PCINT_VECT){
  //TRX Interrupt Fired
  if( OneWire_TRX_Interrupt_Status() == 1){        
	//Falling Edge Interrupt
    if(OneWire_TRX_Falling_Edge_Interrupt() == 1){
	  //handle frame bits and ack
	  OneWire_Bit_Frame_Sample();
	  OneWire_Bit_Counter_Overflow();
	}
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