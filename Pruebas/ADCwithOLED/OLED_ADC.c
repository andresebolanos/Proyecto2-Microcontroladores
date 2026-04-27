#include "Configuracion.h"
#include "adc.h"
#include "OLED_Libreria.h"
#include <stdio.h>

void main(void){
    /* Oscilador 8MHz interno */
    OSCCON = 0x72;
    
    /* RD0 y RD1 como salidas digitales para LEDs */
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;
    LATDbits.LATD0 = 0;
    LATDbits.LATD1 = 0;
    
    /* Inicializar Módulos */
    ADC_Init();
    I2C_Init();   /* Inicializa la comunicación I2C */
    OLED_Init();  /* Inicializa la pantalla OLED */
    
    unsigned int temperatura, luz;
    char buffer[20]; /* Arreglo para guardar el texto formateado */
    
    /* Mensaje estático de bienvenida/título */
    OLED_String(0, 0, "Monitoreo Ambiental");
    
    while(1){
        temperatura = ADC_LeerTemperatura(); /* Décimas de grado */
        luz = ADC_LeerLuz();                 /* Porcentaje 0-100  */
        luz = 100 - luz;
        
        /* LED RD0: enciende si temperatura > 30.0°C (300 décimas) */
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
        
        /* =========================================
         * ACTUALIZACIÓN DE PANTALLA OLED
         * ========================================= */
        
        /* Formatear Temperatura: 
         * Como está en décimas, dividimos por 10 para los enteros 
         * y usamos el módulo 10 para el decimal para mostrar "23.5 C" */
        sprintf(buffer, "Temp: %u.%u C   ", (temperatura / 10), (temperatura % 10));
        OLED_String(2, 0, buffer); /* Imprime en la Fila 2 */
        
        /* Formatear Luz:
         * Imprimimos el porcentaje directo. Usamos %% para que se imprima el símbolo % */
        sprintf(buffer, "Luz : %u %%     ", luz);
        OLED_String(4, 0, buffer); /* Imprime en la Fila 4 */
        
        __delay_ms(500); /* Muestreo cada 500ms */
    }
}