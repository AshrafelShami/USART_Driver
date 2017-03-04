#include "UART_Config.h"

/********************************************************************************************
  Name:     USART_Init.
  Purpose:  Initialize USART registers for ATmega32.
  Entry:    ( Struct_USART USART_Config ) a STRUCT that setup the following.
							1) Baud Rate.
							2) Operation Mode(Normal and Double speed).
							3) Duplex selection.
							4) Character size.
							5) Stop bit set.
							6) Parity Mode.
							7) Interrupts Selection if required.
							8) Clock Polarity.
  Exit:     no parameters
********************************************************************************************/
void USART_Init(Struct_USART USART_Config){
	u16 UBRR;
	u8 Mode;
	if (USART_Config.Operation_Mode == Asynchronous_Double_Speed){Mode = 8;}
	else {Mode = 16;}
	UBRRH &=~(1U << URSEL);
	UBRR  = (F_CPU/Mode/USART_Config.BuadRate) -1;
	UBRRL = (u8)UBRR;
	UBRRH = (u8)(UBRR >> 8);
	
	UCSRC &=~(1 << URSEL);
	
	switch(USART_Config.Operation_Mode){
		case Synchronous		: UCSRC &=~(1U << UMSEL); break;
		case Asynchronous_Normal_Speed	: UCSRC |= (1U << UMSEL); UCSRA &=~(1U << U2X); break;
		case Asynchronous_Double_Speed	: UCSRC |= (1U << UMSEL); UCSRA |= (1U << U2X); break;
		default				: UCSRC &=~(1U << UMSEL); break;
	}
	
	switch(USART_Config.Duplex_Select){
		case TX_Mode	: UCSRB |= (1U << TXEN); break;
		case RX_Mode	: UCSRB |= (1U << RXEN); break;
		case TX_RX_Mode : UCSRB |= (1U << TXEN)|(1U << RXEN); break;
		default		: UCSRB |= (1U << TXEN); break; 
	}
	
	switch(USART_Config.Character_Size){
		case Char_5_Bits : UCSRB &=~(1U << UCSZ2); UCSRC &=~(1U << UCSZ1); UCSRC &=~(1U << UCSZ0); break;
		case Char_6_Bits : UCSRB &=~(1U << UCSZ2); UCSRC &=~(1U << UCSZ1); UCSRC |= (1U << UCSZ0); break;
		case Char_7_Bits : UCSRB &=~(1U << UCSZ2); UCSRC |= (1U << UCSZ1); UCSRC &=~(1U << UCSZ0); break;
		case Char_8_Bits : UCSRB &=~(1U << UCSZ2); UCSRC |= (1U << UCSZ1); UCSRC |= (1U << UCSZ0); break;
		case Char_9_Bits : UCSRB |= (1U << UCSZ2); UCSRC |= (1U << UCSZ1); UCSRC |= (1U << UCSZ0); break;
		default		 : UCSRB &=~(1U << UCSZ2); UCSRC |= (1U << UCSZ1); UCSRC |= (1U << UCSZ0); break;
	}
	
	switch(USART_Config.Parity_Mode){
		case No_Parity	: UCSRC &=~(1U << UPM1); UCSRC &=~(1U << UPM0); break;
		case Even_Parity: UCSRC |= (1U << UPM1); UCSRC &=~(1U << UPM0); break;
		case Odd_Parity	: UCSRC |= (1U << UPM1); UCSRC |= (1U << UPM0); break;
		default		: UCSRC &=~(1U << UPM1); UCSRC &=~(1U << UPM0); break;
	}
	
	switch(USART_Config.Stop_Bit){
		case One_Bit	: UCSRC &=~(1U << USBS); break;
		case Two_Bits	: UCSRC |= (1U << USBS); break;
		default		: UCSRC &=~(1U << USBS); break;
	}

	switch(USART_Config.Clock_Polirity){
		case TX_Falling_Edge	: UCSRC |= (1U << UCPOL); break;
		case TX_Rising_Edge	: UCSRC &=~(1U << UCPOL); break;
		default			: UCSRC &=~(1U << UCPOL); break;
	}
	
	switch(USART_Config.interrupt_Select){
		case RX_Complete_Int_Enable	: UCSRB |= (1U << RXCIE); break;
		case TX_Complete_Int_Enable	: UCSRB |= (1U << TXCIE); break;
		case Data_Reg_Empty_Int_Enable	: UCSRB |= (1U << UDRIE); break;
		default				: UCSRB &=~((1U << RXCIE)|(1U << TXCIE)|(1U << UDRIE)); break;
	}
}

/********************************************************************************************
  Name:     USART_Rx_Error_Detection.
  Purpose:  Function receive the data of the USART and check if there is possible errors or not..
  Entry:    no parameters
  Exit:     ( Struct_Status Data_Status ) a STRUCT to check the following errors and returns the received Data.
						  1) Frame Error.
						  2) Data OverFlow Error.
						  3) Polarity Error.
********************************************************************************************/
Struct_Status USART_Rx_Error_Detection( void ){
	Struct_Status Data_Error;
	while (!(UCSRA & (1U << RXC)));		/*	Wait for the data to be received	*/	
	
	if(((UCSRA & (1U << FE)) == 0) && ((UCSRA & (1U << DOR)) == 0) && ((UCSRA & (1U << PE)) == 0))
		Data_Error.Detection = Correct;

	else if ((UCSRA & (1U << FE)) != 0)
		Data_Error.Detection = Frame_Error;

	else if ((UCSRA & (1U << DOR)) != 0)
		Data_Error.Detection = Data_OverFlow;

	else if ((UCSRA & (1U << PE)) != 0)
		Data_Error.Detection = Parity_Error;
	Data_Error.Data = UDR;
	return Data_Error;
}

/********************************************************************************************
  Name:     USART_Receive.
  Purpose:  Function receive the data of the UDR register.
  Entry:    no parameters
  Exit:     ( return UDR ) Return Data in unsigned char.
********************************************************************************************/
u8 USART_Receive( void ){
	while (!(UCSRA & (1U << RXC)));		/*	Wait for the data to be received			*/
	return UDR;				/*	Return the received data in the UDR			*/
}

/********************************************************************************************
  Name:     USART_Receive_9bit.
  Purpose:  Function receive the data of the UDR register if the frame structure is 9-bit.
  Entry:    no parameters
  Exit:     ( return Data_9bit; ) Return Data in u16.
********************************************************************************************/
u16 USART_Receive_9bit( void ){
	u16 Data_9bit = 0;
	while (!(UCSRA & (1U << RXC)));
	Data_9bit |= ((UCSRB & (1U << RXB8)) << 8);
	Data_9bit |= UDR;
	return Data_9bit;
}

/********************************************************************************************
  Name:     USART_Transmit.
  Purpose:  Function Transmit the data to the UDR register.
  Entry:    ( u8 Data ) The required data to be sent.
  Exit:     no parameters
********************************************************************************************/
void USART_Transmit( u8 Data ){
	while (!(UCSRA & (1U << UDRE)));	/*	Wait for the Buffer register to be Free.(1)	*/	
	UDR = Data;				/*	Send the Data to the Data register.			*/
}

/********************************************************************************************
  Name:     USART_Transmit_9Bit.
  Purpose:  Function Transmit the data to the UDR register if the frame structure is 9-bit.
  Entry:    ( u16 Data_9bit ) The required data to be sent.
  Exit:     no parameters
********************************************************************************************/
void USART_Transmit_9Bit( u16 Data_9bit ){
	while(!(UCSRA & (1U << UDRE)));
	UCSRB |= ((Data_9bit & (1U << 8)) << TXB8);
	UDR = Data_9bit;
}

/********************************************************************************************
  Name:     USART_Transmit_String.
  Purpose:  Function Transmit the data in the form of array of char to the UDR register.
  Entry:    ( char* String ) The required data to be sent.
  Exit:     no parameters
********************************************************************************************/
void USART_Transmit_String( char* String ){
	u8 Index = 0;
	while (String[Index] != '\0')
		USART_Transmit(String[Index++]);
}

/********************************************************************************************
  Name:     USART_Disable.
  Purpose:  Function used to disable the USART and override the USART pins to the default state.
  Entry:    no parameters
  Exit:     no parameters
********************************************************************************************/
void USART_Disable( void ){
	UCSRB &=~((1 << TXEN)|(1 << RXEN));	/*	Disable Transmitter and Receiver.			*/
}
