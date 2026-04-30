#ifndef BME280_LIBRERIA_H
#define	BME280_LIBRERIA_H

#include <xc.h>

/* Frecuencia del oscilador para los delays (si no está definida en otro lado) */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

/* =========================================
 * DEFINICIONES I2C PARA BME280
 * ========================================= */
// Dirección estándar 0x76 (Desplazada para escritura/lectura)
#define BME280_ADDR_W 0xEC
#define BME280_ADDR_R 0xED

/* =========================================
 * ESTRUCTURA DE CALIBRACIÓN
 * (Necesaria para las fórmulas de Bosch)
 * ========================================= */
typedef struct {
    // Parámetros de Temperatura (necesarios para compensar humedad)
    unsigned short dig_T1;
    short          dig_T2;
    short          dig_T3;
    
    // Parámetros de Humedad
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

/**
 * @brief Inicializa el sensor, verifica el ID y carga parámetros de calibración.
 * @return 1 si tuvo éxito, 0 si falló.
 */
unsigned char BME280_Init(void);

/**
 * @brief Lee los registros del sensor y calcula la humedad relativa.
 * @return Valor de humedad en porcentaje (0.0% a 100.0%).
 */
float BME280_GetHumedad(void);

/**
 * @brief Función interna para leer registros específicos (opcional en el .h).
 */
unsigned char BME_ReadReg(unsigned char reg);

#endif	/* BME280_HUMEDAD_H */