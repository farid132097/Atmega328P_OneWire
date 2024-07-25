
//Transceiver Pin Parameters
#define  ONEWIRE_TRX_PCINT_DDRB
#define  ONEWIRE_TRX_DDR     DDRB
#define  ONEWIRE_TRX_PORT    PORTB
#define  ONEWIRE_TRX_PIN     PINB
#define  ONEWIRE_TRX_BP      0U


//Only For Debugging Purpose
#define  ONEWIRE_DEBUG_ENABLE
#define  ONEWIRE_DEBUG_DDR   DDRC
#define  ONEWIRE_DEBUG_PORT  PORTC
#define  ONEWIRE_DEBUG_PIN   PINC
#define  ONEWIRE_DEBUG_BP    5U


//Timing Related Parameters
#define  ONEWIRE_CLK_LOW     10
#define  ONEWIRE_CLK_HIGH    10
#define  ONEWIRE_CLK_HBIT    20


#define  ONEWIRE_START_CMD   0x800
#define  ONEWIRE_CONT_CMD    0x000
#define  ONEWIRE_STOP_CMD    0x400

#define  ONEWIRE_WRITE_CMD   0x000
#define  ONEWIRE_READ_CMD    0x200


void     OneWire_Struct_Init(void);
void     OneWire_TRX_Set_DDR(uint8_t state);
void     OneWire_TRX_Set_Port(uint8_t state);
uint8_t  OneWire_TRX_Get_Input_State(void);
void     OneWire_TRX_Set_Logic(uint8_t state);
void     OneWire_TRX_GPIO_Init(void);

void     OneWire_TRX_Interrupt_Init(void);
void     OneWire_TRX_Interrupt_Enable(void);
void     OneWire_TRX_Interrupt_Disable(void);

void     OneWire_Debug_Pulse(void);

void     OneWire_Delay_Clock_Low_Time(void);
void     OneWire_Delay_Clock_High_Time(void);
void     OneWire_Delay_Half_Bit_Time(void);

uint16_t OneWire_TRX_Byte(uint16_t val);



void     OneWire_Init(void);