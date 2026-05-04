#include "Configuracion.h"   // Config bits
#include "OLED_Libreria.h"   // Pantalla y Bus I2C
#include "Bme280_Libreria.h" // Sensor de Humedad
#include "adc.h"             // LM35 y LDR
#include "mq135.h"           // Calidad de Aire
#include <stdio.h>


// --- DEFINICIONES PARA RELAY ---
#define RELAY_PIN  LATDbits.LATD0
#define RELAY_TRIS TRISDbits.TRISD0

// Definicion de límites
unsigned int temp_minima = 270;  // Prender si baja de 27.0 C
unsigned int temp_maxima = 290; // Apagar si sube de 29.0 C

void main(void) {
    // --- 1. DECLARACIÓN DE VARIABLES ---
    unsigned int t_lm35;
    unsigned int luz_ldr;
    unsigned int aire_pct;
    float hum_bme;
    char buffer[20]; // Buffer para formatear el texto de la OLED
    unsigned char relay_encendido = 0; // 0 = apagado, 1 = encendido

    // --- 2. INICIALIZACIÓN DE HARDWARE ---
    OSCCON = 0x72;          // Configura oscilador interno a 8MHz
    ADCON1 = 0x0C;          // Configura pines analógicos (AN0, AN1, AN2)
    
    // Inicialización de pines del relay
    RELAY_TRIS = 0;         // Configura RD0 como salida
    RELAY_PIN = 0;          // Empieza con el relay (calentador) apagado

    // Inicialización de Módulos
    I2C_Init();             // Inicia bus I2C para OLED y BME280
    OLED_Init();            // Inicia pantalla
    ADC_Init();             // Inicia ADC para LM35 y LDR
    MQ135_Init();           // Inicia canal del sensor de gas

    // --- 3. PANTALLA DE BIENVENIDA ---
    OLED_Clear();
    OLED_String(0, 0, " SISTEMA DE CONTROL");
    OLED_String(1, 0, "    AMBIENTAL    ");
    __delay_ms(1500);

    // --- 4. VERIFICACIÓN DE SENSORES INTELIGENTES ---
    if (!BME280_Init()) {
        OLED_Clear();
        OLED_String(3, 0, " ERROR: BME280");
        while(1); // Bloqueo de seguridad si el I2C falla
    }

    OLED_Clear();
    OLED_String(0, 0, " --- MONITOREO ---"); // Encabezado fijo

    // --- 5. BUCLE PRINCIPAL ---
    while(1) {
        // A. Adquisición de datos
        t_lm35   = ADC_LeerTemperatura();   // Temperatura
        hum_bme  = BME280_GetHumedad();     // Humedad
        luz_ldr  = ADC_LeerLuz();           // Porcentaje de luz
        aire_pct = MQ135_LeerPorcentaje();  // Calidad del aire

        // B. Tu lógica de control del Relay
        if (t_lm35 < temp_minima && relay_encendido == 0) {
            RELAY_PIN = 1; // Prende el bombillo
            relay_encendido = 1; 
            
            // Mensaje temporal de encendido
            OLED_Clear();
            OLED_String(2, 2, "  CALENTADOR  ");
            OLED_String(3, 2, "   ENCENDIDO  ");
            __delay_ms(2000); // Mostrar por 2 seg
            OLED_Clear();
            OLED_String(0, 0, " --- MONITOREO ---");
        } 
        else if (t_lm35 > temp_maxima && relay_encendido == 1) {
            RELAY_PIN = 0; // Apaga el bombillo
            relay_encendido = 0; 
            
            // Mensaje temporal de apagado
            OLED_Clear();
            OLED_String(2, 2, "  CALENTADOR  ");
            OLED_String(3, 2, "    APAGADO   ");
            __delay_ms(2000); // Mostrar por 2 segundos
            OLED_Clear();
            OLED_String(0, 0, " --- MONITOREO ---");
        }

        // C. Visualización en OLED
        // Fila 2: Temperatura 
        sprintf(buffer, " Temp: %u.%u C   ", t_lm35 / 10, t_lm35 % 10);
        OLED_String(2, 0, buffer);

        // Fila 3: Humedad
        sprintf(buffer, " Hum:  %.1f %%   ", hum_bme);
        OLED_String(3, 0, buffer);

        // Fila 5: Iluminación
        sprintf(buffer, " Luz:  %u %%     ", luz_ldr);
        OLED_String(5, 0, buffer);

        // Fila 6: Calidad de Aire (Gas)
        sprintf(buffer, " Aire: %u %%     ", aire_pct);
        OLED_String(6, 0, buffer);

        __delay_ms(1000); // 1 segundo de actualización
    }
}