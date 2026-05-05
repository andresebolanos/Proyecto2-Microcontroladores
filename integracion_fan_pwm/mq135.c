#include "mq135.h"

// Frecuencia del oscilador para el delay
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000 // Oscilador a 8MHz
#endif

void MQ135_Init(void) {
    TRISAbits.TRISA2 = 1; // RA2 como entrada
    ADCON1 = 0x0C;        // AN0-AN2 analogicos
    ADCON0 = 0x09;        // CHS=0010(AN2), ADON=1
}

uint16_t MQ135_LeerRaw(void) {
    ADCON0bits.CHS = 2;         /* Obligar al multiplexor a conectarse a AN2 */
    // ------------------------------------
    
    __delay_us(20);             /* Esperar que el capacitor interno se cargue */
    ADCON0bits.GO_DONE = 1;     /* Iniciar conversion */
    while (ADCON0bits.GO_DONE); /* Esperar fin */
    
    return ((uint16_t)(ADRESH & 0x03) << 8) | ADRESL;
}

uint8_t MQ135_LeerPorcentaje(void) {
    uint16_t raw = MQ135_LeerRaw();
    
    // Convierte el valor de 0-1023 a un porcentaje de 0-100%
    uint8_t porcentaje = (uint8_t)(((uint32_t)raw * 100) / 1023);
    
    return porcentaje;
}