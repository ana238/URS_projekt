#define F_CPU 7372800UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"

char buffer[255];

void USART_Init( unsigned int ubrr){

	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;

	UCSRB = (1<<RXEN)|(1<<TXEN);

	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}

unsigned char USART_Receive( void ){

	while ( !(UCSRA & (1<<RXC)) );

	return UDR;
}

void USART_Transmit( unsigned char data ){

	while ( !( UCSRA & (1<<UDRE)) );

	UDR = data;
}

int USART_ReceiveString(){
	
	int i = 0;
	int flag = 0;
	int cnt = 0;
	
	while(flag != 1){
		buffer[i] = USART_Receive();
		cnt++;
		if(buffer[i] == '\r'){
			buffer[i+1] = USART_Receive();
			cnt++;
			flag++;
			break;
		}
		i++;
	}
	
	return cnt;
	
}

int main(void){

	USART_Init(51);
	
	DDRD = _BV(4);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 65;

	int j = 0;
	int brojZnakova;

	lcd_init(LCD_DISP_ON);
	
    while (1){
		brojZnakova = USART_ReceiveString();
		for(j = 0; j < brojZnakova; j++){
			USART_Transmit(buffer[j]);
		}
		lcd_clrscr();
		if(brojZnakova == 84){
			lcd_puts("Odobreno");
			_delay_ms(3000);
			lcd_clrscr();
		}else{
			lcd_puts("Odbijeno");
			_delay_ms(3000);
			lcd_clrscr();
		}		
		_delay_ms(100);	

    }
	
}

