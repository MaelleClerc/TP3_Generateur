// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber

#include <stdlib.h>
#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"
#include "driver/tmr/drv_tmr.h"
#include "Mc32NVMUtil.h"
#include <math.h>

// T.P. 2016 100 echantillons
#define MAX_ECH     100
#define MAX_AMP     65535
#define FREQ_INIT   20
#define OFFSET_INIT 0
#define CONVERSION  10000
#define FACTEUR_CONVERSION 3.2767
#define SIGNAL_MIDDLE 32767
#define AMPLITUDE_INIT 10000
#define MAX_VAL        65535
#define MIN_VAL        0

#define F_SYS          80000000
#define PRESCALER      64

//Tableaux
uint32_t tabSignalValues[MAX_ECH] = {0};

// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
    //Lecture du bloc sauvegradé et met à jour la structure
    uint32_t test = sizeof(S_ParamGen); 
    NVM_ReadBlock((uint32_t*)pParam, sizeof(S_ParamGen));
    
    //test de la sauvegarde
    if (pParam->Magic != 0x12345678)
    {
        //Mettre le parametre par defaut
        pParam->Amplitude = AMPLITUDE_INIT;
        pParam->Forme = DENTSCIE;
        pParam->Frequence = FREQ_INIT;
        pParam->Offset = OFFSET_INIT;  
        pParam->Magic = 0x12345678;
    }
}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
    // Variable locale
    float Val_Periode = 0;
    
    Val_Periode = (float)F_SYS/(float)(pParam -> Frequence * MAX_ECH * PRESCALER) - 1;
    PLIB_TMR_Period16BitSet(TMR_ID_3, Val_Periode); 
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{   
    int32_t TestAmplitudeMaxMin = 0;
    int i;
    float OldValue = 0;
    float signal;
    
    for (i = 0; i < MAX_ECH; i++)
    {
        switch (pParam->Forme)
        {
            case CARRE:
                if(i < (MAX_ECH / 2))
                {
                    signal = pParam->Offset + pParam->Amplitude;                  
                    tabSignalValues[i] = SIGNAL_MIDDLE + signal * FACTEUR_CONVERSION + 0.5;                   
                    //tabSignalValues[i] = (float)(SIGNAL_MIDDLE + (pParam->Amplitude *((float)SIGNAL_MIDDLE/(float)CONVERSION)) + 1) + (float)(pParam->Offset *((float)SIGNAL_MIDDLE/(float)CONVERSION));
                }
                else
                {
                    //tabSignalValues[i] = (float)(SIGNAL_MIDDLE - (pParam->Amplitude *((float)SIGNAL_MIDDLE/(float)CONVERSION)))+ (float)(pParam->Offset *((float)SIGNAL_MIDDLE/(float)CONVERSION)));
                    signal = pParam->Offset - pParam->Amplitude;

                    tabSignalValues[i] = SIGNAL_MIDDLE + signal * FACTEUR_CONVERSION + 0.5;
                }              
                break;
                
            case TRIANGLE: 
                tabSignalValues[i] = OldValue + (pParam->Offset *(FACTEUR_CONVERSION));
                //tabSignalValues[i] = SIGNAL_MIDDLE + signal * FACTEUR_CONVERSION + 0.5;
                if(i < (MAX_ECH / 2))
                {
                    //signal = signal + pParam->Offset + (pParam->Amplitude * 2 + 0.5)/(MAX_ECH / 2);
                    OldValue = OldValue + ((pParam->Amplitude * FACTEUR_CONVERSION) * 2 + 1)/ (MAX_ECH / 2);
                }
                else
                {
                    //signal = signal - pParam->Offset + (pParam->Amplitude * 2 + 0.5)/(MAX_ECH / 2);
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
        TestAmplitudeMaxMin = (int32_t)tabSignalValues[i] ; //- (int32_t)pParam->Offset;
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