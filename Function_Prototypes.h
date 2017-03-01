
#ifndef FUNCTION_PROTOTYPES_H_ 
#define FUNCTION_PROTOTYPES_H_
#include "DataTypes.h"

u8 USART_Receive( void );
u16 USART_Receive_9bit( void );

void USART_Transmit( u8 Send_Data );
void USART_Transmit_String( char* Array_String );
void USART_Transmit_9Bit( u16 Data_9bit );

void USART_Disable( void );

#endif /* FUNCTION_PROTOTYPES_H_ */
