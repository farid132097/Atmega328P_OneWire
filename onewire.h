
#define  MASTER_TX_MODE
//#define  SLAVE_RX_MODE


#define  ONEWIRE_OWN_AADR    0x68

//Transceiver Pin Parameters
#define  ONEWIRE_TRX_PCINT_DDRD
#define  ONEWIRE_TRX_DDR     DDRD
#define  ONEWIRE_TRX_PORT    PORTD
#define  ONEWIRE_TRX_PIN     PIND
#define  ONEWIRE_TRX_BP      6U


//Only For Debugging Purpose TX
//#define  ONEWIRE_DBGTX_ENABLE
#define  ONEWIRE_DBGTX_DDR   DDRC
#define  ONEWIRE_DBGTX_PORT  PORTC
#define  ONEWIRE_DBGTX_PIN   PINC
#define  ONEWIRE_DBGTX_BP    5U

//Only For Debugging Purpose RX
#define  ONEWIRE_DBGRX_ENABLE
#define  ONEWIRE_DBGRX_DDR   DDRD
#define  ONEWIRE_DBGRX_PORT  PORTD
#define  ONEWIRE_DBGRX_PIN   PIND
#define  ONEWIRE_DBGRX_BP    4U



//Timing Related Parameters
#define  ONEWIRE_CLK_LOW     20
#define  ONEWIRE_CLK_HIGH    20
#define  ONEWIRE_CLK_HBIT    40
#define  ONEWIRE_BYTE_GAP    200

//Frame Len in bits (Max 15)
#define  ONEWIRE_FRAME_LEN   12
//Frame Buf Len
#define  ONEWIRE_FRAME_BUF   50
//Data Buf Len
#define  ONEWIRE_DATA_BUF    150

#define  ONEWIRE_CMD_RESET   0x7FF
#define  ONEWIRE_CMD_MASK    0x700
#define  ONEWIRE_DATA_MASK   0x0FF
#define  ONEWIRE_ACK_MASK    0x001

#define  ONEWIRE_CMD_ERROR   0x700
#define  ONEWIRE_CMD_START   0x600
#define  ONEWIRE_CMD_READ    0x400
#define  ONEWIRE_CMD_STOP    0x300
#define  ONEWIRE_CMD_RSVD    0x200
#define  ONEWIRE_CMD_WRITE   0x100
#define  ONEWIRE_CMD_CONT    0x000




void     OneWire_Struct_Init(void);

void     OneWire_Flush_Bit_Frame(void);
void     OneWire_Flush_Cmd_Reg(void);
void     OneWire_Flush_Data_Buf(void);

void     OneWire_TRX_Set_DDR(uint8_t state);
void     OneWire_TRX_Set_Port(uint8_t state);
uint8_t  OneWire_TRX_Get_Input_State(void);
void     OneWire_TRX_Set_Logic(uint8_t state);
void     OneWire_TRX_GPIO_Init(void);

void     OneWire_TRX_Interrupt_Init(void);
void     OneWire_TRX_Interrupt_Enable(void);
void     OneWire_TRX_Interrupt_Disable(void);
uint8_t  OneWire_TRX_Interrupt_Status(void);
uint8_t  OneWire_TRX_Falling_Edge_Interrupt(void);

void     OneWire_Debug_Tx_Pulse(void);
void     OneWire_Debug_Rx_Pulse(void);

void     OneWire_Delay_Clock_Low_Time(void);
void     OneWire_Delay_Clock_High_Time(void);
void     OneWire_Delay_Half_Bit_Time(void);
void     OneWire_Delay_Byte_Gap(void);
void     OneWire_Delay_Rx_Int(void);

uint8_t  OneWire_Tx_Reset_Cmd(void);
uint16_t OneWire_TRX_Frame(uint16_t val);
uint16_t OneWire_TRX_Cmd_Data(uint16_t cmd, uint16_t data);

void     OneWire_Handle_Cmd_Data(void);
void     OneWire_Bit_Frame_Sample(void);
uint8_t  OneWire_Bit_Counter_Overflow(void);
uint16_t OneWire_Build_Bit_Frame(void);
void     OneWire_Extract_Cmd_Data(void);
void     OneWire_Handle_Data(void);

void     OneWire_Buf_Sample_And_Update(void);
uint8_t  OneWire_Buf_Counter_Overflow(void);
void     OneWire_Fill_Buf(void);
void     OneWire_Fill_Buf_Copy_Data(void);
void     OneWire_Read_Mode_Feedback(void);

void     OneWire_Master_Send_Data(uint8_t *data, uint8_t len);
uint8_t  OneWire_Master_Receive_Data(uint8_t addr);

uint8_t  OneWire_Get_Data_Buf(uint8_t index);
uint8_t  OneWire_Get_Data_Buf_Index(void);

void     OneWire_Init_Master(void);
void     OneWire_Init_Slave(void);