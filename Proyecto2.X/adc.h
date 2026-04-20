/**
 * @file adc.h
 * @brief Librería para la configuración y lectura del módulo ADC del PIC18F4550.
 * @author Andres Bolaños
 * * Este archivo contiene las definiciones de los canales analógicos y los
 * prototipos de las funciones necesarias para adquirir y filtrar los datos
 * de los sensores (LM35 y LDR) requeridos para el sistema de control ambiental.
 */

#ifndef ADC_H
#define ADC_H

#include <xc.h>

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000 // Frecuencia del oscilador interno (8 MHz)
#endif

/* =========================================
 * DEFINICIÓN DE CANALES ANALÓGICOS
 * ========================================= */
/** Canal 0 (Pin RA0) asignado al sensor de temperatura LM35 */
#define CANAL_LM35  0  
/** Canal 1 (Pin RA1) asignado al sensor de luz (Fotorresistencia LDR) */
#define CANAL_LDR   1  

/* =========================================
 * PROTOTIPOS DE FUNCIONES
 * ========================================= */

/**
 * @brief Configura el módulo ADC (Pines, voltajes de referencia y reloj).
 */
void ADC_Init(void);

/**
 * @brief Realiza la conversión analógica a digital de un canal específico.
 * @param canal Número de canal a leer (0 a 12).
 * @return Valor digital crudo de la conversión (resolución de 10 bits: 0 a 1023).
 */
unsigned int ADC_Leer(unsigned char canal);

/**
 * @brief Lee el sensor LM35, aplica un filtro de promediado y calcula la temperatura.
 * @return Temperatura en décimas de grado Celsius (Ejemplo: 235 equivale a 23.5 °C).
 */
unsigned int ADC_LeerTemperatura(void);

/**
 * @brief Lee el sensor LDR, aplica un filtro de promediado y calcula el porcentaje de luz.
 * @return Porcentaje de iluminación (0% a 100%).
 */
unsigned int ADC_LeerLuz(void);

/* Numero de muestras para el filtro */
#define FILTRO_MUESTRAS 8

/* Estructura del filtro promedio movil */
typedef struct {
    unsigned int muestras[FILTRO_MUESTRAS];
    unsigned char indice;
    unsigned char lleno;
} FiltroPromedio;

#endif /* ADC_H */