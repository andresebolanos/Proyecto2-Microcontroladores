/**
 * @file adc.c
 * @brief Implementación de las funciones del módulo ADC.
 * Se encarga de inicializar los registros ADCON0, ADCON1 y ADCON2,
 * realizar la lectura cruda de los pines y aplicar filtros de promedio
 * para evitar lecturas erráticas (cumpliendo el criterio de Robustez).
 */

#include "adc.h"

/* Filtros internos para cada canal */
static FiltroPromedio filtroTemp;
static FiltroPromedio filtroLuz;

/**
 * @brief Inicializa un filtro de promedio movil.
 * @param f Puntero a la estructura del filtro
 */
static void Filtro_Init(FiltroPromedio *f){
    unsigned char i;
    for(i = 0; i < FILTRO_MUESTRAS; i++){
        f->muestras[i] = 0;
    }
    f->indice = 0;
    f->lleno  = 0;
}

/**
 * @brief Agrega una muestra y retorna el promedio actual.
 * @param f Puntero a la estructura del filtro
 * @param nueva_muestra Valor a agregar
 * @return Promedio de las ultimas FILTRO_MUESTRAS muestras
 */
static unsigned int Filtro_Agregar(FiltroPromedio *f, unsigned int nueva_muestra){
    unsigned long suma = 0;
    unsigned char i, cantidad;

    f->muestras[f->indice] = nueva_muestra;
    f->indice = (f->indice + 1) % FILTRO_MUESTRAS;
    if(f->indice == 0) f->lleno = 1;

    cantidad = f->lleno ? FILTRO_MUESTRAS : f->indice;
    
    // Sumar todas las muestras almacenadas
    for(i = 0; i < cantidad; i++){
        suma += f->muestras[i];
    }

    return (unsigned int)(suma / cantidad); 
}
/**
 * @brief Inicializa el modulo ADC del PIC18F4550 y los filtros internos.
 * Configura AN0 y AN1 como entradas analogicas con Vref interno (5V),
 * justificacion a la derecha y reloj Fosc/32. Tambien inicializa los
 * filtros de promedio movil para cada canal.
 */
void ADC_Init(void){
    /* Registro ADCON1: Configuración de puertos
     * VCFG1=0, VCFG0=0 -> Voltajes de referencia internos (VSS = 0V y VDD = 5V).
     * PCFG = 0b1101 (0x0D) -> Configura AN0 (RA0) y AN1 (RA1) como entradas analógicas. */
    ADCON1 = 0x0D;

    /* Registro ADCON2: Formato de resultado y tiempos del ADC
     * ADFM = 1 -> Justificación a la derecha (los 10 bits se guardan en ADRESL y los 2 bits más significativos en ADRESH).
     * ACQT = 101 -> Tiempo de adquisición de 12 TAD (suficiente para que el capacitor interno se cargue).
     * ADCS = 010 -> Reloj de conversión Fosc/32 (ideal para 8 MHz). */
    ADCON2 = 0xAA;

    /* Configuración de los pines físicos como entradas (1 = Input) */
    TRISAbits.TRISA0 = 1; // Pin RA0 (LM35)
    TRISAbits.TRISA1 = 1; // Pin RA1 (LDR)

    /* Registro ADCON0: Encender el módulo
     * ADON = 1 -> Habilita el módulo convertidor A/D. */
    ADCON0bits.ADON = 1;
    
    Filtro_Init(&filtroTemp);
    Filtro_Init(&filtroLuz);
    
}

/**
 * @brief Lee un canal ADC y retorna el valor raw (0-1023).
 * @param canal Canal a leer. Usar CANAL_LM35 (0) o CANAL_LDR (1).
 * @return Valor ADC de 10 bits sin procesar (0-1023).
 */
unsigned int ADC_Leer(unsigned char canal){
    // Selecciona el canal analógico a leer cargándolo en el registro ADCON0
    ADCON0bits.CHS = canal;
    
    // Espera un tiempo mínimo para que el capacitor "Sample and Hold" se cargue con el voltaje del pin. 
    __delay_us(20);          

    // Inicia la conversión poniendo el bit GO/DONE
    ADCON0bits.GO = 1;       
    
    /* Bucle de espera. El PIC mantiene GO en 1 mientras convierte. 
     * Cuando termina la conversión, el hardware lo pone en 0 automáticamente. */
    while(ADCON0bits.GO);    

    /* Une los 2 registros de 8 bits (ADRESH y ADRESL) en un solo valor de 16 bits.
     * Desplaza ADRESH 8 posiciones a la izquierda y hace un OR (suma lógica) con ADRESL. */
    return ((unsigned int)(ADRESH << 8) | ADRESL);
}

/**
 * @brief Lee el LM35, aplica filtro y retorna temperatura
 * en decimas de grado. Ejemplo: 235 = 23.5C
 * @return Temperatura filtrada en decimas de grado Celsius
 */
unsigned int ADC_LeerTemperatura(void){
    unsigned int adc = ADC_Leer(CANAL_LM35);
    unsigned int filtrado = Filtro_Agregar(&filtroTemp, adc);
    return (unsigned int)((filtrado * 5000UL) / 1023);
}

/**
 * @brief Lee el LDR, aplica filtro y retorna nivel de luz
 * en porcentaje (0-100).
 * @return Porcentaje de luz filtrado
 */
unsigned int ADC_LeerLuz(void){
    unsigned int adc = ADC_Leer(CANAL_LDR);
    unsigned int filtrado = Filtro_Agregar(&filtroLuz, adc);
    return (unsigned int)((filtrado * 100UL) / 1023);
}