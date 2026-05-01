/*
 * =============================================================================
 *  Libreria : MQ-135 - Monitor de Calidad del Aire
 *  Archivo  : mq135.c
 *  MCU      : PIC18F4550 @ 8 MHz (oscilador interno)
 *  Canal ADC: AN2 / RA2 (PIN 4)
 */

#include "mq135.h"

#ifndef _XTAL_FREQ
#define _XTAL_FREQ  8000000
#endif

/* ---------------------------------------------------------------------------
   Pin LED de estado durante el precalentamiento (RD7), opcional 
   --------------------------------------------------------------------------- */
#define LED_ESTADO_TRIS   TRISDbits.TRISD7
#define LED_ESTADO_LAT    LATDbits.LATD7

/* ---------------------------------------------------------------------------
   MQ135_Init
   --------------------------------------------------------------------------- */
void MQ135_Init(void) {
    /* RA2 como entrada analÃ³gica */
    TRISAbits.TRISA2 = 1;

    ADCON1 = 0x0C;   /* AN0-AN2 analalogicos, Vref = VDD/VSS */

    /*
     * ADCON0:
     *   CHS3:CHS0 = 0010  Canal AN2
     *   ADON = 1
     */
    ADCON0 = 0x09;   /* 0b00001001: CHS=0010(AN2), ADON=1 */

    /*
     * ADCON2:
     *   ADFM  = 1        Justificacion derecha (10 bits en ADRESH:ADRESL)
     *   ACQT2:ACQT0 = 010 Tiempo adquisicion = 4 Tad
     *   ADCS2:ADCS0 = 010  Fosc/32,  Tad = 4, @ 8 MHz (min requerido > 0.7 )
     */
    ADCON2 = 0b10010010;

    /* Deshabilitar comparadores analogicos */
    CMCON = 0x07;

    /* LED de estado como salida */
    LED_ESTADO_TRIS = 0;
    LED_ESTADO_LAT  = 0;
}

/* ---------------------------------------------------------------------------
   MQ135_Precalentar
   --------------------------------------------------------------------------- */
void MQ135_Precalentar(void) {
    uint8_t i;
    /* 20 ciclos de 1 s = 20 segundos */
    for (i = 0; i < 20; i++) {
        LED_ESTADO_LAT = 1;
        __delay_ms(500);
        LED_ESTADO_LAT = 0;
        __delay_ms(500);
    }
    LED_ESTADO_LAT = 1;   /* Fijo = sensor listo */
}

/* ---------------------------------------------------------------------------
   MQ135_LeerRaw  (funcion interna y publica)
   --------------------------------------------------------------------------- */
uint16_t MQ135_LeerRaw(void) {
    __delay_us(20);                     /* Tiempo de adquisicion adicional    */
    ADCON0bits.GO_DONE = 1;             /* Iniciar conversion                 */
    while (ADCON0bits.GO_DONE);         /* Esperar fin ( 13, Tad, 52 µs)    */
    return ((uint16_t)(ADRESH & 0x03) << 8) | ADRESL;
}

/* ---------------------------------------------------------------------------
   MQ135_LeerCalidad
   --------------------------------------------------------------------------- */
uint8_t MQ135_LeerCalidad(void) {
    uint16_t raw = MQ135_LeerRaw();

    if (raw <= MQ135_UMBRAL_EXCELENTE) return MQ135_EXCELENTE;
    if (raw <= MQ135_UMBRAL_BUENA)     return MQ135_BUENA;
    if (raw <= MQ135_UMBRAL_MODERADA)  return MQ135_MODERADA;
    if (raw <= MQ135_UMBRAL_MALA)      return MQ135_MALA;
    return MQ135_MUY_MALA;
}

/* ---------------------------------------------------------------------------
   MQ135_LeerVoltaje_mV
   --------------------------------------------------------------------------- */
uint16_t MQ135_LeerVoltaje_mV(void) {
    uint32_t raw = (uint32_t)MQ135_LeerRaw();
    return (uint16_t)((raw * 5000UL) / 1023UL);
}
