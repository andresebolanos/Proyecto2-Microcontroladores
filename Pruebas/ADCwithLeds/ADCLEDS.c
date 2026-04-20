#include "Configuracion.h"
#include "adc.h"

void main(void){
    /* Oscilador 8MHz interno */
    OSCCON = 0x72;
    
    /* RD0 y RD1 como salidas digitales para LEDs */
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    LATDbits.LATD0 = 0;
    LATDbits.LATD1 = 0;
    
    /* Inicializar ADC */
    ADC_Init();
    
    unsigned int temperatura, luz;
    
    while(1){
        temperatura = ADC_LeerTemperatura(); /* Decimas de grado */
        luz = ADC_LeerLuz();                 /* Porcentaje 0-100  */
        
        /* LED RD0: enciende si temperatura > 30.0°C (300 decimas) */
        if(temperatura > 300){
            LATDbits.LATD0 = 1;
        } else {
            LATDbits.LATD0 = 0;
        }
        
        /* LED RD1: enciende si luz < 30% */
        if(luz < 30){
            LATDbits.LATD1 = 1;
        } else {
            LATDbits.LATD1 = 0;
        }
        
        __delay_ms(500); /* Muestreo cada 500ms */
    }
}