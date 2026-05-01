/*
 * =============================================================================
 *  Libreria : MQ-135 - Monitor de Calidad del Aire
 *  MCU      : PIC18F4550 @ 8 MHz (oscilador interno)
 *  Canal ADC: AN2 / RA2 (PIN 4)
 * ===========================================================================
 */

#ifndef MQ135_H
#define MQ135_H

#include <xc.h>
#include <stdint.h>

/* ---------------------------------------------------------------------------
   NIVELES DE CALIDAD DEL AIRE
   Retornados por MQ135_LeerCalidad()
   --------------------------------------------------------------------------- */
#define MQ135_EXCELENTE     0   /* ADC 0   - 200  */
#define MQ135_BUENA         1   /* ADC 201 - 400  */
#define MQ135_MODERADA      2   /* ADC 401 - 600  */
#define MQ135_MALA          3   /* ADC 601 - 800  */
#define MQ135_MUY_MALA      4   /* ADC 801 - 1023 */

/* ---------------------------------------------------------------------------
   UMBRALES ADC (0-1023)
   Modificar estos valores segun la calibracion del sensor 
   --------------------------------------------------------------------------- */
#define MQ135_UMBRAL_EXCELENTE   100
#define MQ135_UMBRAL_BUENA       200
#define MQ135_UMBRAL_MODERADA    300
#define MQ135_UMBRAL_MALA        400


/**
 * MQ135_Init()
 * Configura el ADC para usar AN2/RA2.
 * Llama esta funcion una sola vez al inicio, antes de cualquier lectura.
 */
void MQ135_Init(void);

/**
 * MQ135_Precalentar()
 * Espera el tiempo de precalentamiento obligatorio del sensor (~20 s).
 * Durante la espera parpadea el pin indicado (LED de estado)"opcional" .
 */
void MQ135_Precalentar(void);

/**
 * MQ135_LeerRaw()
 * Retorna la lectura cruda del ADC: 0 a 1023.
 */
uint16_t MQ135_LeerRaw(void);

/**
 * MQ135_LeerCalidad()
 * Retorna el nivel de calidad del aire:
 *   0 = MQ135_EXCELENTE
 *   1 = MQ135_BUENA
 *   2 = MQ135_MODERADA
 *   3 = MQ135_MALA
 *   4 = MQ135_MUY_MALA
 */
uint8_t MQ135_LeerCalidad(void);

/**
 * MQ135_LeerVoltaje_mV()
 * Retorna el voltaje en la salida AOUT del sensor en milivolts (0-5000).
 * Calculado como: (raw * 5000) / 1023
 */
uint16_t MQ135_LeerVoltaje_mV(void);

#endif /* MQ135_H */
