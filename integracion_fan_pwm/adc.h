#ifndef ADC_H
#define ADC_H

#include <xc.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000 // Frecuencia del oscilador interno (8 MHz)
#endif

/* =========================================
 * DEFINICI�N DE CANALES ANAL�GICOS
 * ========================================= */
/** Canal 0 (Pin RA0) asignado al sensor de temperatura LM35 */
#define CANAL_LM35  0  
/** Canal 1 (Pin RA1) asignado al sensor de luz (Fotorresistencia LDR) */
#define CANAL_LDR   1  

/* =========================================
 * PROTOTIPOS DE FUNCIONES
 * ========================================= */

void ADC_Init(void);

unsigned int ADC_Leer(unsigned char canal);

unsigned int ADC_LeerTemperatura(void);

unsigned int ADC_LeerLuz(void);

#define FILTRO_MUESTRAS 8

typedef struct {
    unsigned int muestras[FILTRO_MUESTRAS];
    unsigned char indice;
    unsigned char lleno;
} FiltroPromedio;

#endif /* ADC_H */