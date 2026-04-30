#include "Bme280_Libreria.h"
#include "OLED_Libreria.h"

BME_Calib c;
long t_fine;

// Función auxiliar para leer un solo registro
unsigned char BME_ReadReg(unsigned char reg) {
    unsigned char res;
    I2C_Start(BME280_ADDR_W);
    I2C_Write(reg);
    I2C_Restart();
    I2C_Start(BME280_ADDR_R);
    res = I2C_Read(0); 
    I2C_Stop();
    return res;
}

unsigned char BME280_Init(void) {
    unsigned char cal_h[7];
    
    // Verificar si el sensor responde (ID 0x60)
    if(BME_ReadReg(0xD0) != 0x60) return 0; 

    // 1. Leer calibración de temperatura (Registros 0x88 a 0x8D)
    I2C_Start(BME280_ADDR_W);
    I2C_Write(0x88);
    I2C_Restart();
    I2C_Start(BME280_ADDR_R);
    c.dig_T1 = I2C_Read(1) | (I2C_Read(1) << 8);
    c.dig_T2 = I2C_Read(1) | (I2C_Read(1) << 8);
    c.dig_T3 = I2C_Read(1) | (I2C_Read(0) << 8);
    I2C_Stop();

    // 2. Leer calibración de humedad (Registros 0xA1 y 0xE1 a 0xE7)
    c.dig_H1 = BME_ReadReg(0xA1);
    
    I2C_Start(BME280_ADDR_W);
    I2C_Write(0xE1);
    I2C_Restart();
    I2C_Start(BME280_ADDR_R);
    // Leemos los 7 bytes de calibración de humedad seguidos
    for(int i=0; i<6; i++) cal_h[i] = I2C_Read(1);
    cal_h[6] = I2C_Read(0); 
    I2C_Stop();

    // Reconstrucción CORRECTA de H2 a H6
    c.dig_H2 = (short)(cal_h[0] | (cal_h[1] << 8));
    c.dig_H3 = cal_h[2];
    // H4 y H5 comparten el byte cal_h[4] (Registro 0xE5)
    c.dig_H4 = (short)((cal_h[3] << 4) | (cal_h[4] & 0x0F));
    c.dig_H5 = (short)((cal_h[5] << 4) | (cal_h[4] >> 4));
    c.dig_H6 = (signed char)cal_h[6];

    // 3. Configuración: Humedad x1, Temp x1, Modo Normal
    I2C_Start(BME280_ADDR_W); I2C_Write(0xF2); I2C_Write(0x01); I2C_Stop();
    I2C_Start(BME280_ADDR_W); I2C_Write(0xF4); I2C_Write(0x27); I2C_Stop();
    
    return 1;
}

float BME280_GetHumedad(void) {
    unsigned char d[5];
    I2C_Start(BME280_ADDR_W);
    I2C_Write(0xFA); // Empezar a leer desde Temperatura
    I2C_Restart();
    I2C_Start(BME280_ADDR_R);
    for(int i=0; i<4; i++) d[i] = I2C_Read(1);
    d[4] = I2C_Read(0);
    I2C_Stop();

    long adc_T = ((long)d[0] << 12) | ((long)d[1] << 4) | (d[2] >> 4);
    long adc_H = ((long)d[3] << 8) | d[4];

    // Compensación de temperatura necesaria para la humedad
    long v1 = ((((adc_T >> 3) - ((long)c.dig_T1 << 1))) * ((long)c.dig_T2)) >> 11;
    long v2 = (((((adc_T >> 4) - ((long)c.dig_T1)) * ((adc_T >> 4) - ((long)c.dig_T1))) >> 12) * ((long)c.dig_T3)) >> 14;
    t_fine = v1 + v2;

    // Cálculo final de humedad
    float h = ((float)t_fine - 76800.0);
    h = (adc_H - (((float)c.dig_H4) * 64.0 + ((float)c.dig_H5) / 16384.0 * h)) *
        (((float)c.dig_H2) / 65536.0 * (1.0 + ((float)c.dig_H6) / 67108864.0 * h *
        (1.0 + ((float)c.dig_H3) / 67108864.0 * h)));
    h = h * (1.0 - ((float)c.dig_H1) * h / 524288.0);
    
    if (h > 100.0) h = 100.0; else if (h < 0.0) h = 0.0;
    return h;
}
