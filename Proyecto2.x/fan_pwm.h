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

void FAN_PWM_Init(void);

void FAN_PWM_SetSpeed(uint8_t percentage);

void FAN_PWM_SetRawDuty(uint16_t duty);

#endif	/* FAN_PWM_H */