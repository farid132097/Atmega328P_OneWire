
#define  MASTER_TX_MODE
//#define  SLAVE_RX_MODE





#ifdef   MASTER_TX_MODE
//Transceiver Pin Parameters
#define  ONEWIRE_TRX_PCINT_DDRB
#define  ONEWIRE_TRX_DDR     DDRB
#define  ONEWIRE_TRX_PORT    PORTB
#define  ONEWIRE_TRX_PIN     PINB
#define  ONEWIRE_TRX_BP      0U


//Only For Debugging Purpose TX
//#define  ONEWIRE_DBGTX_ENABLE
#define  ONEWIRE_DBGTX_DDR   DDRC
#define  ONEWIRE_DBGTX_PORT  PORTC
#define  ONEWIRE_DBGTX_PIN   PINC
#define  ONEWIRE_DBGTX_BP    5U

//Only For Debugging Purpose RX
//#define  ONEWIRE_DBGRX_ENABLE
#define  ONEWIRE_DBGRX_DDR   DDRD
#define  ONEWIRE_DBGRX_PORT  PORTD
#define  ONEWIRE_DBGRX_PIN   PIND
#define  ONEWIRE_DBGRX_BP    7U

#endif


#ifdef   SLAVE_RX_MODE

//Transceiver Pin Parameters
#define  ONEWIRE_TRX_PCINT_DDRC
#define  ONEWIRE_TRX_DDR     DDRC
#define  ONEWIRE_TRX_PORT    PORTC
#define  ONEWIRE_TRX_PIN     PINC
#define  ONEWIRE_TRX_BP      5U


//Only For Debugging Purpose TX
//#define  ONEWIRE_DBGTX_ENABLE
#define  ONEWIRE_DBGTX_DDR   DDRC
#define  ONEWIRE_DBGTX_PORT  PORTC
#define  ONEWIRE_DBGTX_PIN   PINC
#define  ONEWIRE_DBGTX_BP    1U

//Only For Debugging Purpose RX
#define  ONEWIRE_DBGRX_ENABLE
#define  ONEWIRE_DBGRX_DDR   DDRC
#define  ONEWIRE_DBGRX_PORT  PORTC
#define  ONEWIRE_DBGRX_PIN   PINC
#define  ONEWIRE_DBGRX_BP    4U

#endif


//Timing Related Parameters
#define  ONEWIRE_CLK_LOW     20
#define  ONEWIRE_CLK_HIGH    20
#define  ONEWIRE_CLK_HBIT    40
#define  ONEWIRE_BYTE_GAP    100

//Frame Len in bits
#define  ONEWIRE_FRAME_LEN   12
//Frame Buf Len
#define  ONEWIRE_FRAME_BUF   50
//Data Buf Len
#define  ONEWIRE_DATA_BUF    150

#define  ONEWIRE_START_CMD   0x800
#define  ONEWIRE_CONT_CMD    0x000
#define  ONEWIRE_STOP_CMD    0x400

#define  ONEWIRE_WRITE_CMD   0x000
#define  ONEWIRE_READ_CMD    0x200
#define  ONEWIRE_RW_MASK     0x200
#define  ONEWIRE_REG_MASK    0x1FE


void     OneWire_Struct_Init(void);

void     OneWire_Flush_Frame(void);
void     OneWire_Flush_Frame_Buf(void);

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

uint16_t OneWire_TRX_Byte(uint16_t val);

void     OneWire_Bit_Sample_And_Update(void);
uint8_t  OneWire_Bit_Counter_Overflow(void);
void     OneWire_Buf_Sample_And_Update(void);
uint8_t  OneWire_Buf_Counter_Overflow(void);
void     OneWire_Fill_Buf(void);
void     OneWire_Read_Mode_Feedback(void);

void     OneWire_Master_Send_Data(uint8_t *data, uint8_t len);
uint8_t  OneWire_Master_Receive_Data(uint8_t addr);

void     OneWire_Init_Master(void);
void     OneWire_Init_Slave(void);