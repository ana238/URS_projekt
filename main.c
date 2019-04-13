#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"

char buffer[255];
volatile int cnt = 0, flag = 0, i =0;
int k = 0;

/*Inicijalizacija USART komunikacije. 
Zapisi ubrr vrijednost u UBRRH i UBRRL registre,
omogucimo primanje i slanje podataka, te prekid za primanje podataka (UCSRB registar).
Omogucimo pristup UCSRC registru, te podesimo na 8n1 (8 char size, no parity, 1 stop bit), UCSRC registar.
Koristimo asinkroni nacin rada.*/

void USART_Init( unsigned int ubrr){

	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;

	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);

	UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
	
	sei();
}

// unsigned char USART_Receive( void ){
// 
// 	while ( !(UCSRA & (1<<RXC)) );
// 
// 	return UDR;
// }

// void USART_ReceiveString(){
// 
// 	do{
// 		buffer[i++] = USART_Receive();
// 		cnt++;
// 		
// 	}while(buffer[i-1] != 0x0d);
// 	
// 	cnt++;
// 
// }


/*
Funkcija pomocu koje saljemo podatke sa kartice u UDR
registar te potom preko TX pina na plocici dalje na PC
*/
void USART_Transmit( unsigned char data ){

	while ( !( UCSRA & (1<<UDRE)) );

	UDR = data;
}

/*Interrupt pomocu kojega primamo podatke sa kartice (iz UDR registra), spremamo u buffer, te postavljamo flag (k)
ovisno da li je provucena odgovarajuca kartica */
ISR(USART_RXC_vect){
// 	flag = 1;
//	char a[20];
	
	buffer[i++] = UDR;
	if(buffer[i-1] == '\r'){
		//buffer[i++] = UDR;
		if(i == 83){
			
			k = 1;
		}else{

			k = 2;	
		}
	}
	
}

int main(void){

	USART_Init(51);
	
	DDRD = _BV(4);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 65;
	
	int j = 0;
//	char a[20],b[20];

	lcd_init(LCD_DISP_ON);
	
    while (1){
		//USART_ReceiveString();
		
		lcd_clrscr();
		
		/*Kada je k = 1, posalji podatke dalje u UDR registar, te na LCDu ispisi "Odobreno".
		Kada je k = 2, na ekranu ispisi "Odbijeno".
		Inace ispisi "Provuci karticu"*/
	
		if(k == 1){
			for(j = 0; j < 84; j++){
				USART_Transmit(buffer[j]);
			}
			lcd_puts("Odobreno");
			_delay_ms(1000);
		}else if(k == 2){
			lcd_puts("Odbijeno");
			_delay_ms(1000);
		}else{
			lcd_puts("Provuci karticu");
		}

		k = 0;
		i = 0;

		_delay_ms(1000);	

    }
	
}

