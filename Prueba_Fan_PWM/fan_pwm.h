#ifndef FAN_PWM_H
#define	FAN_PWM_H

#include <xc.h>
#include <stdint.h>

// Definir la frecuencia si no está definida en el proyecto global
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

// Valor máximo del Duty Cycle para 25kHz @ 8MHz ( (PR2+1)*4 )
#define MAX_DUTY_VALUE 320

/**
 * @brief Inicializa los módulos CCP1 y CCP2 en modo PWM a 25kHz.
 * Configura los pines RC1 y RC2 como salidas.
 */
void FAN_PWM_Init(void);

/**
 * @brief Establece la velocidad del ventilador mediante un porcentaje.
 * @param percentage Valor de 0 a 100.
 */
void FAN_PWM_SetSpeed(uint8_t percentage);

/**
 * @brief Establece el Duty Cycle de forma precisa (valor bruto).
 * @param duty Valor entre 0 y 320.
 */
void FAN_PWM_SetRawDuty(uint16_t duty);

#endif	/* FAN_PWM_H */