#include "Configuracion.h"   // Config bits
#include "OLED_Libreria.h"   // Pantalla y Bus I2C
#include "Bme280_Libreria.h" // Sensor de Humedad
#include "adc.h"             // LM35 y LDR
#include "mq135.h"           // Calidad de Aire
#include "fan_pwm.h"         // Tu librería de ventilador
#include <stdio.h>

// --- DEFINICIONES PARA RELAY ---
#define RELAY_PIN  LATDbits.LATD0
#define RELAY_TRIS TRISDbits.TRISD0

// --- DEFINICIONES PARA ILUMINACIÓN Y ALARMA ---
// Usamos RD1 a RD5 para los LEDs (RD0 ya es del relé)
#define LED1 LATBbits.LATB2
#define LED2 LATBbits.LATB3
#define LED3 LATBbits.LATB4
#define LED4 LATBbits.LATB5
#define LED5 LATBbits.LATB6

#define TRIS_LED1 TRISBbits.TRISB2
#define TRIS_LED2 TRISBbits.TRISB3
#define TRIS_LED3 TRISBbits.TRISB4
#define TRIS_LED4 TRISBbits.TRISB5
#define TRIS_LED5 TRISBbits.TRISB6

// Usamos RC0 para el Buzzer
#define BUZZER LATCbits.LATC0
#define TRIS_BUZZER TRISCbits.TRISC0

// Umbrales de control
#define HUM_MAX 56.0      
#define HUM_MIN 54.0      
#define AIRE_CRITICO 60   

unsigned int temp_minima = 270;  // 27.0 C
unsigned int temp_maxima = 290;  // 29.0 C

void main(void) {
    // --- 1. DECLARACIÓN DE VARIABLES ---
    unsigned int t_lm35;
    unsigned int luz_ldr;
    uint8_t aire_pct;        
    float hum_bme;
    char buffer[20]; 

    // VARIABLES DE MEMORIA PARA LOS MENSAJES TEMPORALES
    unsigned char relay_encendido = 0; 
    uint8_t duty_ventilador = 0; 
    uint8_t duty_ventilador_ant = 255; // Empieza en 255 para forzar el primer aviso al encender
    unsigned char contador_aire = 0;
    
    // --- 2. INICIALIZACIÓN DE HARDWARE ---
    OSCCON = 0x72;          
    ADCON1 = 0x0C;          
    
    RELAY_TRIS = 0;         
    RELAY_PIN = 0;          

    // Inicializar pines de Iluminación y Alarma
    TRIS_LED1 = 0; TRIS_LED2 = 0; TRIS_LED3 = 0; TRIS_LED4 = 0; TRIS_LED5 = 0;
    LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0; LED5 = 0; // Inician apagados

    TRIS_BUZZER = 0;
    BUZZER = 0; // Inicia apagado

    I2C_Init();             
    OLED_Init();            
    ADC_Init();             
    MQ135_Init();           
    
    // --- 3. INICIALIZACIÓN PWM VENTILADOR ---
    FAN_PWM_Init();          
    FAN_PWM_SetSpeed(0);     

    // --- 4. BIENVENIDA ---
    OLED_Clear();
    OLED_String(0, 0, " SISTEMA DE CONTROL");
    OLED_String(1, 0, "    AMBIENTAL      ");
    __delay_ms(1500);

    if (!BME280_Init()) {
        OLED_Clear();
        OLED_String(3, 0, " ERROR: BME280");
        while(1); 
    }

    OLED_Clear();
    OLED_String(0, 0, " --- MONITOREO ---"); 

    // --- 5. BUCLE PRINCIPAL ---
    while(1) {
        // A. Adquisición de datos
        t_lm35   = ADC_LeerTemperatura();   
        hum_bme  = BME280_GetHumedad();     
        luz_ldr  = ADC_LeerLuz();           
        aire_pct = MQ135_LeerPorcentaje();  

        // ==========================================
        // B. LÓGICA DEL CALENTADOR
        // ==========================================
        if (t_lm35 < temp_minima && relay_encendido == 0) {
            RELAY_PIN = 1; 
            relay_encendido = 1;
            
            OLED_Clear();
            OLED_String(2, 2, "  CALENTADOR  ");
            OLED_String(3, 2, "   ENCENDIDO  ");
            __delay_ms(2000); 
            OLED_Clear();
            OLED_String(0, 0, " --- MONITOREO ---");
        } 
        else if (t_lm35 > temp_maxima && relay_encendido == 1) {
            RELAY_PIN = 0;
            relay_encendido = 0;
            
            OLED_Clear();
            OLED_String(2, 2, "  CALENTADOR  ");
            OLED_String(3, 2, "    APAGADO   ");
            __delay_ms(2000);
            OLED_Clear();
            OLED_String(0, 0, " --- MONITOREO ---");
        }

        // ==========================================
        // C. LÓGICA DEL VENTILADOR
        // ==========================================
        if (hum_bme > HUM_MAX || aire_pct > AIRE_CRITICO) duty_ventilador = 100;
        else if (hum_bme < HUM_MIN) duty_ventilador = 0;
        else duty_ventilador = 30;

        if (duty_ventilador != duty_ventilador_ant) {
            FAN_PWM_SetSpeed(duty_ventilador); 
            duty_ventilador_ant = duty_ventilador; 
            
            OLED_Clear();
            if (duty_ventilador == 100) {
                OLED_String(2, 0, "   VENTILADOR   ");
                OLED_String(3, 0, "   AL MAXIMO!   ");
            } else if (duty_ventilador == 30) {
                OLED_String(2, 0, "   VENTILADOR   ");
                OLED_String(3, 0, "  MODO CRUCERO  ");
            } else if (duty_ventilador == 0) {
                OLED_String(2, 0, "   VENTILADOR   ");
                OLED_String(3, 0, "    APAGADO     ");
            }
            __delay_ms(2000);
            OLED_Clear();
            OLED_String(0, 0, " --- MONITOREO ---");
        }

       // ==========================================
        // D. LÓGICA DE ILUMINACIÓN (LDR -> LEDs) - CORREGIDA
        // ==========================================
        // luz_ldr ahora viene en rango 0-100 gracias a ADC_LeerLuz()
        
        if (luz_ldr < 38) {
            LED1=1; LED2=1; LED3=1; LED4=1; LED5=1; // Muy oscuro (<15%): 5 LEDs
        } else if (luz_ldr < 48) {
            LED1=1; LED2=1; LED3=1; LED4=1; LED5=0; // 4 LEDs
        } else if (luz_ldr < 55) {
            LED1=1; LED2=1; LED3=1; LED4=0; LED5=0; // 3 LEDs
        } else if (luz_ldr < 75) {
            LED1=1; LED2=1; LED3=0; LED4=0; LED5=0; // 2 LEDs
        } else if (luz_ldr < 90) {
            LED1=1; LED2=0; LED3=0; LED4=0; LED5=0; // 1 LED
        } else {
            LED1=0; LED2=0; LED3=0; LED4=0; LED5=0; // Mucha luz (>90%): 0 LEDs
        }

        // ==========================================
        // E. PANTALLA PRINCIPAL Y ALARMA (BUZZER)
        // ==========================================
        sprintf(buffer, " Temp: %u.%u C   ", t_lm35 / 10, t_lm35 % 10);
        OLED_String(2, 0, buffer);

        sprintf(buffer, " Hum:  %.1f %%   ", hum_bme);
        OLED_String(3, 0, buffer);

        sprintf(buffer, " Aire: %u %%      ", aire_pct);
        OLED_String(4, 0, buffer);

        // Convertimos el valor crudo del LDR a un pseudo-porcentaje solo para visualización
        sprintf(buffer, " Luz:  %u %%      ", luz_ldr);
        OLED_String(5, 0, buffer);

        // Alerta de Aire Crítico
        if (aire_pct > AIRE_CRITICO) {
            contador_aire++; // Aumenta cada 500ms

            // Si el aire sigue crítico después de ~3 segundos (6 * 500ms)
            if (contador_aire >= 6) { 
                OLED_String(7, 0, "!! ALERTA AIRE !!");
                BUZZER = 1; // ACTIVAR ALARMA SONORA
            }
        } else {
            contador_aire = 0; 
            OLED_String(7, 0, "                "); 
            BUZZER = 0; // APAGAR ALARMA SONORA
        }

        __delay_ms(500); 
    }
}