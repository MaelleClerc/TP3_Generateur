// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "driver/tmr/drv_tmr.h"
#include <math.h>

// T.P. 2016 100 echantillons
#define MAX_ECH     100
#define MAX_AMP     65535

//Valeur d initialisation
#define FREQ_INIT   20
#define OFFSET_INIT 0
#define SIGNAL_MIDDLE 32767
#define AMPLITUDE_INIT 10000

//Facteur de conversion 32767 / 10000
#define FACTEUR_CONVERSION 3.2767

//Test Offset
#define MAX_VAL        65535
#define MIN_VAL        0

//Gestion de la frequence
#define F_SYS          80000000
#define PRESCALER      64

//Tableaux valeur du signal
uint32_t tabSignalValues[MAX_ECH] = {0};

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    pParam->Amplitude = AMPLITUDE_INIT;
    pParam->Forme = DENTSCIE;
    pParam->Frequence = FREQ_INIT;
    pParam->Offset = OFFSET_INIT;  
}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    // Variable locale
    float Val_Periode = 0;
    
    //Calcul du nb Tic avec la nouvelle frequence
    Val_Periode = (float)F_SYS/(float)(pParam -> Frequence * MAX_ECH * PRESCALER) - 1;
    
    //Changer la valeur de la periode
    PLIB_TMR_Period16BitSet(TMR_ID_3, Val_Periode); 
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{   
    //Variables Locale
    int32_t TestAmplitudeMaxMin = 0;
    int i;
    float OldValue = 0;
    float signal;
    
    //
    for (i = 0; i < MAX_ECH; i++)
    {
        switch (pParam->Forme)
        {
            case CARRE:
                if(i < (MAX_ECH / 2))
                {
                    signal = pParam->Offset + pParam->Amplitude;                  
                    tabSignalValues[i] = SIGNAL_MIDDLE + signal * FACTEUR_CONVERSION + 0.5;                                 
                }
                else
                {                    
                    signal = pParam->Offset - pParam->Amplitude;
                    tabSignalValues[i] = SIGNAL_MIDDLE + signal * FACTEUR_CONVERSION + 0.5;
                }              
                break;
                
            case TRIANGLE: 
                
                tabSignalValues[i] = OldValue + (pParam->Offset *(FACTEUR_CONVERSION));
                if(i < (MAX_ECH / 2))
                {                    
                    OldValue = OldValue + ((pParam->Amplitude * FACTEUR_CONVERSION) * 2 + 1)/ (MAX_ECH / 2);
                }
                else
                {
                    OldValue = OldValue - ((pParam->Amplitude * FACTEUR_CONVERSION) * 2 + 1)/ (MAX_ECH / 2);
                }   
                break;
                
            case DENTSCIE:
                
                tabSignalValues[i] = ((((pParam->Amplitude *FACTEUR_CONVERSION) * 2) / (MAX_ECH-1)) * i)+ (pParam->Offset *FACTEUR_CONVERSION); 

                break;
                
            case SINUS:
                tabSignalValues[i] = (pParam->Amplitude *FACTEUR_CONVERSION) * sin(2* M_PI * i / (MAX_ECH ))+ SIGNAL_MIDDLE + (pParam->Offset *FACTEUR_CONVERSION);
                break;
        }

    }
    for (i = 0; i < MAX_ECH; i++)
    {
        TestAmplitudeMaxMin = (int32_t)tabSignalValues[i] ; 
        if(TestAmplitudeMaxMin >= MAX_VAL)
        {
            TestAmplitudeMaxMin = MAX_VAL;
        }
        else if(TestAmplitudeMaxMin <= MIN_VAL)
        {
            TestAmplitudeMaxMin = MIN_VAL;
        }
        tabSignalValues[i] = (uint16_t)TestAmplitudeMaxMin;
    }   
    
   
}


// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
    
    static uint16_t EchNb = 0;
    
    SPI_WriteToDac(0,tabSignalValues[EchNb]);
    EchNb++;
    EchNb = EchNb % MAX_ECH;
}


    

