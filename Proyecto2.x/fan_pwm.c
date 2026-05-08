#include "fan_pwm.h"

void FAN_PWM_Init(void) {
    // 1. Configurar Pines RC1 (CCP2) y RC2 (CCP1) como salidas
    TRISCbits.TRISC1 = 0; 
    TRISCbits.TRISC2 = 0;

    // 2. Configurar Frecuencia a 25kHz
    // PR2 = (Fosc / (Fpwm * 4 * Prescaler)) - 1
    // PR2 = (8,000,000 / (25,000 * 4 * 1)) - 1 = 79
    PR2 = 79; 

    // 3. Configurar módulos CCP en modo PWM
    CCP1CON = 0x0C; // Modo PWM para CCP1
    CCP2CON = 0x0C; // Modo PWM para CCP2

    // 4. Configurar Timer2
    T2CONbits.T2CKPS = 0;   // Prescaler 1:1
    T2CONbits.TMR2ON = 1;   // Activar Timer2
    
    // Iniciar al 0%
    FAN_PWM_SetRawDuty(0);
}

void FAN_PWM_SetRawDuty(uint16_t duty) {
    if(duty > MAX_DUTY_VALUE) duty = MAX_DUTY_VALUE;

    // Aplicar a CCP1 (RC2)
    CCP1CONbits.DC1B = duty & 0x03;      // 2 bits LSB
    CCPR1L = (uint8_t)(duty >> 2);       // 8 bits MSB

    // Aplicar a CCP2 (RC1)
    CCP2CONbits.DC2B = duty & 0x03;
    CCPR2L = (uint8_t)(duty >> 2);
}

void FAN_PWM_SetSpeed(uint8_t percentage) {
    if(percentage > 100) percentage = 100;
    
    // Mapeo lineal: (porcentaje * MAX_DUTY) / 100
    uint16_t duty = (uint16_t)(((uint32_t)percentage * MAX_DUTY_VALUE) / 100);
    FAN_PWM_SetRawDuty(duty);
}
