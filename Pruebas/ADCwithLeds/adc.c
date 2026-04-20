/**
 * @file adc.c
 * @brief Implementación de las funciones del módulo ADC.
 * Se encarga de inicializar los registros ADCON0, ADCON1 y ADCON2,
 * realizar la lectura cruda de los pines y aplicar filtros de promedio
 * para evitar lecturas erráticas (cumpliendo el criterio de Robustez).
 */

#include "adc.h"

/**
 * @brief Inicializa los registros del ADC para usar AN0 y AN1.
 */
void ADC_Init(void){
    /* Registro ADCON1: Configuración de puertos
     * VCFG1=0, VCFG0=0 -> Voltajes de referencia internos (VSS = 0V y VDD = 5V).
     * PCFG = 0b1101 (0x0D) -> Configura AN0 (RA0) y AN1 (RA1) como entradas analógicas. */
    ADCON1 = 0x0D;

    /* Registro ADCON2: Formato de resultado y tiempos del ADC
     * ADFM = 1 -> Justificación a la derecha (los 10 bits se guardan en ADRESL y los 2 bits más significativos en ADRESH).
     * ACQT = 101 -> Tiempo de adquisición de 12 TAD (suficiente para que el capacitor interno se cargue).
     * ADCS = 010 -> Reloj de conversión Fosc/32 (ideal para 8 MHz). */
    ADCON2 = 0xAA;

    /* Configuración de los pines físicos como entradas (1 = Input) */
    TRISAbits.TRISA0 = 1; // Pin RA0 (LM35)
    TRISAbits.TRISA1 = 1; // Pin RA1 (LDR)

    /* Registro ADCON0: Encender el módulo
     * ADON = 1 -> Habilita el módulo convertidor A/D. */
    ADCON0bits.ADON = 1;
}

/**
 * @brief Ejecuta una lectura cruda del ADC.
 */
unsigned int ADC_Leer(unsigned char canal){
    // Selecciona el canal analógico a leer cargándolo en el registro ADCON0
    ADCON0bits.CHS = canal;
    
    // Espera un tiempo mínimo para que el capacitor "Sample and Hold" se cargue con el voltaje del pin. 
    
    __delay_us(20);          

    // Inicia la conversión poniendo el bit GO/DONE
    ADCON0bits.GO = 1;       
    
    /* Bucle de espera. El PIC mantiene GO en 1 mientras convierte. 
     * Cuando termina la conversión, el hardware lo pone en 0 automáticamente. */
    while(ADCON0bits.GO);    

    /* Une los 2 registros de 8 bits (ADRESH y ADRESL) en un solo valor de 16 bits.
     * Desplaza ADRESH 8 posiciones a la izquierda y hace un OR (suma lógica) con ADRESL. */
    return ((unsigned int)(ADRESH << 8) | ADRESL);
}

/**
 * @brief Lee el LM35, aplica filtro de promedio y convierte a temperatura.
 */
unsigned int ADC_LeerTemperatura(void){
    unsigned long suma = 0;
    
    // Filtro de Promedio Móvil: Tomamos 10 muestras continuas para eliminar el ruido eléctrico
    for(int i = 0; i < 10; i++){
        suma += ADC_Leer(CANAL_LM35);
        __delay_ms(2); // Pequeña pausa entre muestras para estabilizar el canal
    }
    unsigned int promedio = suma / 10;

    /* Conversión LM35: 
     * El LM35 entrega 10mV por cada °C. Con Vref de 5V, el ADC tiene 1024 pasos.
     * Resolución = 5000mV / 1023 = 4.887 mV por paso del ADC.
     * Multiplicamos por 5000 y dividimos entre 1023. El resultado incluye un decimal 
     * implícito para mayor precisión al mostrar en pantalla. */
    return (unsigned int)((promedio * 5000UL) / 1023UL);
}

/**
 * @brief Lee el LDR, aplica filtro y lo convierte a porcentaje.
 */
unsigned int ADC_LeerLuz(void){
    unsigned long suma = 0;
    
    /* Filtro de Promedio Móvil para estabilizar la lectura de luz */
    for(int i = 0; i < 10; i++){
        suma += ADC_Leer(CANAL_LDR);
        __delay_ms(2);
    }
    unsigned int promedio = suma / 10;

    /* Convertir el valor crudo del ADC (0-1023) a un porcentaje de iluminación (0-100%)
     * Formula: (Valor_ADC * 100) / 1023 */
    return (unsigned int)((promedio * 100UL) / 1023UL);
}
