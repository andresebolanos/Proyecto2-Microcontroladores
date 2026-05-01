/*
 * =============================================================================
 *  Ejemplo de uso de la libreria MQ-135
 *  MCU : PIC18F4550 @ 8 MHz
 *
 *  Archivos necesarios en el proyecto:
 *    mq135.h
 *    mq135.c
 *    main.c  
 * =============================================================================
 */

#pragma config FOSC   = INTOSC_HS
#pragma config WDT    = OFF
#pragma config PBADEN = OFF
#pragma config LVP    = OFF
#pragma config MCLRE  = ON
#pragma config PWRT   = ON
#pragma config BOR    = ON

#include <xc.h>
#include <stdint.h>
#include "mq135.h"

#define _XTAL_FREQ  8000000UL


void Init_Oscilador(void);

/* =============================================================================
   MAIN
   ============================================================================= */
void main(void) {
    uint16_t voltaje_mv;
    uint16_t raw;
    uint8_t  calidad;

    Init_Oscilador();

    /* Configurar pines de salida */
    TRISB = 0x00;    
    TRISD = 0x00;    /* RD0-RD2 = LEDs indicadores           */
    LATB  = 0x00;
    LATD  = 0x00;

    /* -------------------------------------------------------------------------
       Inicializar y precalentar el sensor
       ------------------------------------------------------------------------- */
    MQ135_Init();
    MQ135_Precalentar();   /* Bloquea ~20 s. LED en RD7 parpadea mientras espera */

    /* -------------------------------------------------------------------------
       Bucle principal
       ------------------------------------------------------------------------- */
    while (1) {

        /* Leer el sensor */
        raw       = MQ135_LeerRaw();          /* 0 - 1023        */
        calidad   = MQ135_LeerCalidad();      /* 0 - 4           */
        voltaje_mv= MQ135_LeerVoltaje_mV();   /* 0 - 5000 mV     */

        /* ---------------------------------------------------------------------
           USA LOS DATOS 
           --------------------------------------------------------------------- */
      
        LATBbits.LATB0 = (calidad >= MQ135_MODERADA) ? 1 : 0;
        LATBbits.LATB1 = (calidad == MQ135_MUY_MALA) ? 1 : 0;

        /* Tres LEDs en RD0 / RD1 / RD2 */
        LATD &= 0xF8;   /* Apagar RD0-RD2 sin tocar el resto */
        if      (calidad <= MQ135_BUENA)       LATDbits.LATD0 = 1;  
        else if (calidad == MQ135_MODERADA)    LATDbits.LATD1 = 1;  
        else                                   LATDbits.LATD2 = 1;  
      
        __delay_ms(500);
    }
}

/* =============================================================================
   OSCILADOR INTERNO 8 MHz
   ============================================================================= */
void Init_Oscilador(void) {
    OSCCONbits.IRCF2 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF0 = 1;   /* 111 = 8 MHz */
    OSCCONbits.SCS1  = 1;
    OSCCONbits.SCS0  = 0;
    while (!OSCCONbits.IOFS);
}
