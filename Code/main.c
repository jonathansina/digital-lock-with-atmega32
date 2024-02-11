/*******************************************************
This program was created by the
CodeWizardAVR V3.14 Advanced
Automatic Program Generator
� Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Digital-Lock
Version : 1.0.0
Date    : 2/10/2024
Author  : Jani, Heidari and Anvari
Company : 
Comments: 
This project shows how to design and simulate a digital lock using an atmega32 microcontroller and a 4x4 keypad as input.
The code is programmed in C language and tested in Proteus software.
The lock has a 16-digit or less password that can be entered by pressing the keys on the keypad.
The lock status is displayed on a 16x2 LCD module and a blue LED indicates when the lock is open and a red LED indicates when the lock is close.
The project also includes a DC motor that rotates clockwise when the lock is closed and rotates counter-clockwise when the lock is opened.
The project has a feature to change the default password by pushing * button for 2 seconds.

Default password: 1382

Contributors:
Sina Jani
Arman Heidari
Amir Mohammad Anvari

License:
MIT


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 8.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32.h>
#include <stdio.h>
#include <io.h>
#include <delay.h>
#include <string.h>

// Alphanumeric LCD functions
#include <alcd.h>

// Declare your global variables here

char text[16], password[16], pass[17] = "1382", key;
int i, counter, ret, flag, set_pass_counter, wrong_password = 0;

char read_key(int reset_mode) {
    set_pass_counter = 0;
    while(1) {
        PORTB=0b11111110;
        delay_ms(1);
        
        if (PINB.4 == 0) {
              return '1';
        }

        if (PINB.5 == 0) {
              return '2';
        }

        if (PINB.6 == 0) {
              return '3';
        }

        PORTB=0b11111101;
        delay_ms(1);
        
       if (PINB.4 == 0) {
              return '4';
        }

        if (PINB.5 == 0) {
              return '5';
        }

        if (PINB.6 == 0) {
              return '6';
        }

        PORTB=0b11111011;
        delay_ms(1);
        
        if (PINB.4 == 0) {
              return '7';
        }

        if (PINB.5 == 0) {
              return '8';
        }

        if (PINB.6 == 0) {
              return '9';
        }
        
        PORTB=0b11110111;
        delay_ms(1);
        
        if (PINB.4 == 0) {
            if (reset_mode) {
                while (PINB.4 == 0) {
                    set_pass_counter += 1;
                    delay_ms(25);
                    if (set_pass_counter >= 10) {
                        return 'r';
                    }
                }   
            }
            
            return '*';
        }

        if (PINB.5 == 0) {
              return '0';
        }

        if (PINB.6 == 0) {
              return '#';
        }
    }
}

void lock_door() {
    counter = 0;
    while (counter <= 9) {
        lcd_clear();
          
        lcd_gotoxy(0, 0);
        sprintf(text, "Lock after %d sec", 9 - counter);
        lcd_puts(text);
          
        lcd_gotoxy(0, 1);
        sprintf(text, "Press # to lock");
        lcd_puts(text);
     
        PORTB=0b11110111;
        
        i = 0;
        flag = 0;
        while (i < 5) {
            if (PINB.6 == 0) {
                flag = 1;
            }
            delay_ms(25);
            i += 1;
        }
        
        if (flag) {
            break;
        }
        
        counter += 1;
    }
      
    PORTD = 0x01;
    PIND.3 = 1;
    delay_ms(125);
    PORTD = 0x00;
    PIND.3 = 0;

    lcd_clear();
    lcd_gotoxy(0, 0);
    sprintf(text, "Locked !");
    lcd_puts(text);
    delay_ms(250);
}

void open_door() {
    lcd_clear();
    lcd_gotoxy(0, 0);
    sprintf(text, "Door Opened!");
    lcd_puts(text);
    
    PORTD = 0x02;
    PIND.2 = 1;
    delay_ms(125);
    PORTD = 0x00;
    PIND.2 = 0;
}

void reset_password() {
    wrong_password = 0;
    lcd_clear();
    lcd_gotoxy(0, 0);
    sprintf(text, "Reset Pass Mode");
    lcd_puts(text);
    counter = 0;
    while (counter < 20) {
        PIND.4 = 1;
        PIND.4 = 0; 
        delay_ms(25);
        counter += 1;
    }

    lcd_clear();
    lcd_gotoxy(0, 0);
    sprintf(text, "Old Password:");
    lcd_puts(text);

    for (i = 0; i < 16; i++) {
        key = read_key(0);
        
        delay_ms(50);

        if (key == '*') {
            break;
        }
        
        if (key == '#') {
            if (i == 0) {
                continue;
            }
            
            i -= 1;
            
            lcd_gotoxy(i, 1);

            lcd_putchar(' ');

            password[i] = '\0';

            i -= 1;
            continue;
        }

        lcd_gotoxy(i, 1);
        lcd_putchar(key);

        password[i] = key;
    }
      
    ret = strncmp(password, pass, 16);

    if (ret == 0) {
        for (i = 0; i < 16; i++) {
            pass[i] = '\0';
        }
        lcd_clear();
        lcd_gotoxy(0, 0);
        sprintf(text, "Correct Password!");
        lcd_puts(text);
        
        delay_ms(500);

        lcd_clear();
        lcd_gotoxy(0, 0);
        sprintf(text, "Enter New Pass");
        lcd_puts(text);

        for (i = 0; i < 16; i++) {
            key = read_key(0);
              
            delay_ms(50);

            if (key == '*') {
                break;
            }
            
            if (key == '#') {
                if (i == 0) {
                    continue;
                }
                i -= 1;
                
                lcd_gotoxy(i, 1);

                lcd_putchar(' ');

                pass[i] = '\0';

				i -= 1;
				continue;
			}

			lcd_gotoxy(i, 1);
			lcd_putchar(key);

			pass[i] = key;
		}
		
        pass[i] = '\0';

		lcd_clear();
		lcd_gotoxy(0, 0);
		sprintf(text, "Password Changed");
		lcd_puts(text);
		
        counter = 0;
		while (counter < 20) {
            PIND.4 = 1;
            PIND.4 = 0; 
            delay_ms(25);
            counter += 1;
		}
    } 
    else {
        lcd_clear();
        lcd_gotoxy(0, 0);
        sprintf(text, "Wrong Password!");
        lcd_puts(text);
        delay_ms(125);

        counter = 0;
        while (counter < 20) {
            PIND.2 = 1;
            PIND.2 = 0;
            PIND.3 = 1;
            PIND.3 = 0; 
            delay_ms(25);
            counter += 1;
        }
    }
}

void main(void)
{
    // Declare your local variables here

    // Input/Output Ports initialization
    // Port A initialization
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRA=(1<<DDA7) | (1<<DDA6) | (1<<DDA5) | (1<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (1<<DDA0);
    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

    // Port B initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=Out Bit0=Out
    DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTB=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=0xFF
    // OC0 output: Disconnected
    TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
    TCNT0=0x00;
    OCR0=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer1 Stopped
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer2 Stopped
    // Mode: Normal top=0xFF
    // OC2 output: Disconnected
    ASSR=0<<AS2;
    TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
    TCNT2=0x00;
    OCR2=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: Off
    MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
    MCUCSR=(0<<ISC2);

    // USART initialization
    // USART disabled
    UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

    // Analog Comparator initialization
    // Analog Comparator: Off
    // The Analog Comparator's positive input is
    // connected to the AIN0 pin
    // The Analog Comparator's negative input is
    // connected to the AIN1 pin
    ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
    SFIOR=(0<<ACME);

    // ADC initialization
    // ADC disabled
    ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

    // SPI initialization
    // SPI disabled
    SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

    // TWI initialization
    // TWI disabled
    TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

    // Alphanumeric LCD initialization
    // Connections are specified in the
    // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
    // RS - PORTA Bit 0
    // RD - PORTA Bit 1
    // EN - PORTA Bit 2
    // D4 - PORTA Bit 4
    // D5 - PORTA Bit 5
    // D6 - PORTA Bit 6
    // D7 - PORTA Bit 7
    // Characters/line: 16
    lcd_init(16);      

    while (1) {
        lcd_clear();
        lcd_gotoxy(0, 0);
        sprintf(text, "Enter Password:");
        lcd_puts(text);

        for (i = 0; i < 16; i++) {
            if (i == 0) {
                key = read_key(1);
            }
            else {
                key = read_key(0);
            }

            delay_ms(50);

            if (key == 'r') {
                reset_password();
                lcd_clear();
                lcd_gotoxy(0, 0);
                sprintf(text, "Enter Password:");
                lcd_puts(text);
                for (i = 0; i < 16; i++) {
                    password[i] = '\0';
                }
                i = -1;
                continue;
            }
            if (key == '*') {
                break;
            }
            if (key == '#') {
                if (i == 0) {
                    continue;
                }
                i -= 1;
                
                lcd_gotoxy(i, 1);

                lcd_putchar(' ');

                password[i] = '\0';

                i -= 1;
                continue;
            }

            lcd_gotoxy(i, 1);
            lcd_putchar(key);

            password[i] = key;
        }
        
        ret = strncmp(password, pass, 16);

        if (ret == 0) {
            wrong_password = 0;
            lcd_clear();
            lcd_gotoxy(0, 0);
            sprintf(text, "Correct Password!");
            lcd_puts(text);
                
            open_door();
            lock_door();
        } 
        else {
            wrong_password += 1;
            lcd_clear();
            lcd_gotoxy(0, 0);
            sprintf(text, "Wrong Password!");
            lcd_puts(text);
            delay_ms(125);

            counter = 0;
            while (counter < 20) {
                PIND.2 = 1;
                PIND.2 = 0;
                PIND.3 = 1;
                PIND.3 = 0; 
                delay_ms(25);
                counter += 1;
            }

            if (wrong_password == 3) {
                counter = 0;
                while (counter <= 9) {

                    PIND.2 = 0;
                    PIND.2 = 1;
                    PIND.3 = 0;
                    PIND.3 = 1;
                    PIND.4 = 0;
                    PIND.4 = 1;

                    lcd_clear();

                    lcd_gotoxy(0, 0);
                    sprintf(text, "3 Times Wrong!");
                    lcd_puts(text);

                    lcd_gotoxy(0, 1);
                    sprintf(text, "Wait %d Seconds", 9 - counter);
                    lcd_puts(text);

                    delay_ms(125);
                    wrong_password = 0; 
                    counter += 1;
                }
            }
        }
    }
}
