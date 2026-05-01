#include <xc.h>
#include <stdint.h>
#include "fan_pwm.h"

// Configuracion de bits para PIC18F4550
#pragma config FOSC = INTOSCIO_EC // Oscilador interno, RA6 como pin de E/S
#pragma config WDT = OFF          // Watchdog Timer apagado
#pragma config MCLRE = ON         // Master Clear habilitado
#pragma config LVP = OFF          // Programación de bajo voltaje apagada

#define _XTAL_FREQ 8000000

// Función para inicializar el ADC
void ADC_Init(void) {
    // Configurar RA0 como entrada analógica
    TRISAbits.TRISA0 = 1;
    ADCON1 = 0x0E;      // VREF+ = VDD, VREF- = VSS, AN0 es analogico
    
    // Configuracion del ADC
    // ADCON2: Right justified, 12 TAD, Fosc/8 para 8MHz
    ADCON2 = 0xAA;      
    
    // ADCON0: Canal 0 (AN0), ADC On
    ADCON0 = 0x01;      
}

// Funcion para leer el canal ADC
uint16_t ADC_Read(void) {
    ADCON0bits.GO_DONE = 1;         // Iniciar conversi0n
    while(ADCON0bits.GO_DONE);      // Esperar a que termine
    return ((uint16_t)((ADRESH << 8) + ADRESL)); // Retornar valor de 10 bits (0-1023)
}

void main(void) {
    uint16_t adc_value = 0;
    uint8_t fan_speed = 0;

    // 1. Configurar Oscilador Interno a 8MHz
    OSCCON = 0x72; 
    while(!OSCCONbits.IOFS);

    // 2. Inicializar Módulos
    ADC_Init();
    FAN_PWM_Init();

    while(1) {
        // 3. Leer el potenciometro (0 a 1023)
        adc_value = ADC_Read();

        // 4. Mapear el valor del ADC (0-1023) a Porcentaje (0-100)
        // fan_speed = (adc_value * 100) / 1023
        fan_speed = (uint8_t)(((uint32_t)adc_value * 100) / 1023);

        // 5. Aplicar la velocidad al ventilador
        FAN_PWM_SetSpeed(fan_speed);

        // Pequeño retardo para estabilidad
        __delay_ms(50);
    }
}
