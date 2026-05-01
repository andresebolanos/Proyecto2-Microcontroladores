#ifndef MQ135_H
#define MQ135_H

#include <xc.h>
#include <stdint.h>

/* Frecuencia del oscilador para los delays (si no está definida en otro lado) */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

// Inicializa el pin del sensor
void MQ135_Init(void);

// Lectura cruda del ADC (0 a 1023)
uint16_t MQ135_LeerRaw(void);

// Devuelve el nivel de contaminación en porcentaje i(0 a 100%)
uint8_t MQ135_LeerPorcentaje(void);

#endif