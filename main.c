#include "UART_Config.h"

int main(){
		
	Struct_USART USART_Init_Struct;
	USART_Init_Struct.BuadRate		=	Rate_9600;
	USART_Init_Struct.Operation_Mode	=	Asynchronous_Normal_Speed;
	USART_Init_Struct.Duplex_Select		=	TX_Mode;
	USART_Init_Struct.Character_Size	=	Char_8_Bits;
	USART_Init_Struct.Parity_Mode		=	No_Parity;
	USART_Init_Struct.Stop_Bit		=	One_Bit;
	USART_Init(USART_Init_Struct);
	
	USART_Transmit_String("Hello World!");
	
	while (1){
		
	}
}
