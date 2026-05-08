#include "adc.h"

/* Filtros internos para cada canal */
static FiltroPromedio filtroTemp;
static FiltroPromedio filtroLuz;

static void Filtro_Init(FiltroPromedio *f){
    unsigned char i;
    for(i = 0; i < FILTRO_MUESTRAS; i++){
        f->muestras[i] = 0;
    }
    f->indice = 0;
    f->lleno  = 0;
}

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


void ADC_Init(void){
    /* Registro ADCON1: Configuracion de puertos
     * VCFG1=0, VCFG0=0 -> Voltajes de referencia internos (VSS = 0V y VDD = 5V).
     * PCFG = 0b1101 (0x0D) -> Configura AN0 (RA0) y AN1 (RA1) como entradas analogicas. */
    ADCON1 = 0x0D;

    /* Registro ADCON2: Formato de resultado y tiempos del ADC
     * ADFM = 1 -> Justificaci0n a la derecha (los 10 bits se guardan en ADRESL y los 2 bits m�s significativos en ADRESH).
     * ACQT = 101 -> Tiempo de adquisicion de 12 TAD (suficiente para que el capacitor interno se cargue).
     * ADCS = 010 -> Reloj de conversion Fosc/32 (ideal para 8 MHz). */
    ADCON2 = 0xAA;

    /* Configuracion de los pines fosicos como entradas (1 = Input) */
    TRISAbits.TRISA0 = 1; // Pin RA0 (LM35)
    TRISAbits.TRISA1 = 1; // Pin RA1 (LDR)

    /* Registro ADCON0: Encender el modulo
     * ADON = 1 -> Habilita el modulo convertidor A/D. */
    ADCON0bits.ADON = 1;
    
    Filtro_Init(&filtroTemp);
    Filtro_Init(&filtroLuz);
    
}

unsigned int ADC_Leer(unsigned char canal){
    // Selecciona el canal analogico a leer cargondolo en el registro ADCON0
    ADCON0bits.CHS = canal;
    
    // Espera un tiempo monimo para que el capacitor "Sample and Hold" se cargue con el voltaje del pin. 
    __delay_us(20);          

    // Inicia la conversion poniendo el bit GO/DONE
    ADCON0bits.GO = 1;       
    
    /* Bucle de espera. El PIC mantiene GO en 1 mientras convierte. 
     * Cuando termina la conversion, el hardware lo pone en 0 automoticamente. */
    while(ADCON0bits.GO);    

    /* Une los 2 registros de 8 bits (ADRESH y ADRESL) en un solo valor de 16 bits.
     * Desplaza ADRESH 8 posiciones a la izquierda y hace un OR (suma logica) con ADRESL. */
    return ((unsigned int)(ADRESH << 8) | ADRESL);
}

unsigned int ADC_LeerTemperatura(void){
    unsigned int adc = ADC_Leer(CANAL_LM35);
    unsigned int filtrado = Filtro_Agregar(&filtroTemp, adc);
    return (unsigned int)((filtrado * 5000UL) / 1023);
}

unsigned int ADC_LeerLuz(void){
    unsigned int adc = ADC_Leer(CANAL_LDR);
    unsigned int filtrado = Filtro_Agregar(&filtroLuz, adc);
    return (unsigned int)((filtrado * 100UL) / 1023);
}