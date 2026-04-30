/* * Proyecto: Controlador Inteligente de Confort y Calidad de Aire
 * MCU: PIC18F4550
 */

#include "Configuracion.h"  // Config bits
#include "OLED_Libreria.h"  // Pantalla y Bus I2C
#include "Bme280_Libreria.h" // Sensor de Humedad
#include "adc.h"            // LM35 y LDR
#include "mq135.h"          // Calidad de Aire
#include <stdio.h>          

void main(void) {
    // --- Variables de almacenamiento ---
    float hum_bme;          // Humedad relativa (%)
    unsigned int t_lm35;    // Temp en décimas 
    unsigned int luz_ldr;   // Iluminación (0-100%)
    uint8_t aire_pct;       // Calidad de aire (0-100%)
    
    char buffer[20];        // Buffer para texto en la OLED

    // --- 1. Inicialización de Hardware ---
    OSCCON = 0x72;          // Configura oscilador interno a 8MHz
    ADCON1 = 0x0C;          // Configura pines analógicos (AN0, AN1, AN2)
    
    I2C_Init();             // Inicia bus I2C para OLED y BME280
    OLED_Init();            // Inicia pantalla
    ADC_Init();             // Inicia ADC para LM35 y LDR
    MQ135_Init();           // Inicia canal del sensor de gas

    // --- 2. Pantalla de Bienvenida ---
    OLED_Clear();
    OLED_String(0, 0, "SISTEMA DE CONTROL");
    OLED_String(1, 0, "   AMBIENTAL    ");
    __delay_ms(1500);

    // --- 3. Verificación de Sensores Inteligentes ---
    if (!BME280_Init()) {
        OLED_Clear();
        OLED_String(3, 0, "ERROR: BME280");
        while(1); // Bloqueo de seguridad si el I2C falla
    }

    OLED_Clear();
    // Encabezado fijo
    OLED_String(0, 0, "--- MONITOREO ---");

    while(1) {
        // --- 4. Adquisición de datos (Lectura) ---
        t_lm35 = ADC_LeerTemperatura();    // Promediado por filtro interno
        hum_bme = BME280_GetHumedad();     // Compensada por temperatura
        luz_ldr = ADC_LeerLuz();           // Porcentaje 0-100%
        aire_pct = MQ135_LeerPorcentaje(); // Modificado según nuestra charla

        // --- 5. Visualización en OLED ---
        
        // Fila 2: Temperatura (usando formato de décimas manual)
        sprintf(buffer, "Temp: %u.%u C   ", t_lm35 / 10, t_lm35 % 10);
        OLED_String(2, 0, buffer);

        // Fila 3: Humedad
        sprintf(buffer, "Hum:  %.1f %%   ", hum_bme);
        OLED_String(3, 0, buffer);

        // Fila 5: Iluminación
        sprintf(buffer, "Luz:  %u %%     ", luz_ldr);
        OLED_String(5, 0, buffer);

        // Fila 6: Calidad de Aire (Gas)
        sprintf(buffer, "Aire: %u %%     ", aire_pct);
        OLED_String(6, 0, buffer);

        __delay_ms(1000); // Frecuencia de muestreo (1 Hz)
    }
}