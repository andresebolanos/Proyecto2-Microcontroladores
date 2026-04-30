#ifndef BME280_LIBRERIA_H
#define	BME280_LIBRERIA_H

#include <xc.h>

/* Frecuencia del oscilador para los delays (si no est� definida en otro lado) */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

/* =========================================
 * DEFINICIONES I2C PARA BME280
 * ========================================= */
// Direcci�n est�ndar 0x76 (Desplazada para escritura/lectura)
#define BME280_ADDR_W 0xEC
#define BME280_ADDR_R 0xED

/* =========================================
 * ESTRUCTURA DE CALIBRACI�N
 * (Necesaria para las f�rmulas de Bosch)
 * ========================================= */
typedef struct {
    // Par�metros de Temperatura (necesarios para compensar humedad)
    unsigned short dig_T1;
    short          dig_T2;
    short          dig_T3;
    
    // Par�metros de Humedad
    unsigned char  dig_H1;
    short          dig_H2;
    unsigned char  dig_H3;
    short          dig_H4;
    short          dig_H5;
    signed char    dig_H6;
} BME_Calib;

/* =========================================
 * PROTOTIPOS DE FUNCIONES
 * ========================================= */

unsigned char BME280_Init(void);

float BME280_GetHumedad(void);

unsigned char BME_ReadReg(unsigned char reg);

#endif	/* BME280_HUMEDAD_H */