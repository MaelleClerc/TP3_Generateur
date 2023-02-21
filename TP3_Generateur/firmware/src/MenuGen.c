// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms



#include <stdint.h>                   
#include <stdbool.h>
#include "MenuGen.h"
#include "GesPec12.h"

MENU_STATES MENU_DATA;


// Initialisation du menu et des paramètres
void MENU_Initialize(S_ParamGen *pParam)
{
}


// Execution du menu, appel cyclique depuis l'application
void MENU_Execute(S_ParamGen *pParam)
{
    // Variable locales
    static int16_t Frequence_Selection = 20;
    static int16_t Amplitude_Selection = 100;
    static int16_t Offset_Selection = 0;
    
    switch (MENU_DATA)
    {
        case MENU_STATE_FORME:
            
            // Gestion de la sortie de l'etat
            if (Pec12.Inc == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE;
            }
            else if (Pec12.Dec == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET;
            }
            else if (Pec12.OK == 1)
            {
                MENU_DATA = MENU_STATE_FORME_VALUE;
            }
            
            break;
            
        case MENU_STATE_FORME_VALUE:
            
            break;
            
        case MENU_STATE_FREQUENCE:
            
            // Gestion de la sortie de l'etat
            if (Pec12.Inc == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE;
            }
            else if (Pec12.Dec == 1)
            {
                MENU_DATA = MENU_STATE_FORME;
            }
            else if (Pec12.OK == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE_VALUE;
            }
            
            break;
            
        case MENU_STATE_FREQUENCE_VALUE:
            
            if (Pec12.Inc == 1)
            {
                Frequence_Selection += 20;
                
                if (Frequence_Selection == 2020)
                {
                    Frequence_Selection = 20;
                }
            }
            else if (Pec12.Dec == 1)
            {
                if (Frequence_Selection == 20)
                {
                    Frequence_Selection = 2020;
                }
                
                Frequence_Selection -= 20;
            }
            
            // Gestion de la sortie de l'etat
            if (Pec12.OK == 1)
            {
                // On valide la valeur choisie
                pParam->Frequence = Frequence_Selection;
                
                MENU_DATA = MENU_STATE_FREQUENCE;
            }
            else if (Pec12.ESC == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE; 
            }
            
            break;
            
        case MENU_STATE_AMPLITUDE:
            
            // Gestion de la sortie de l'etat
            if (Pec12.Inc == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET;
            }
            else if (Pec12.Dec == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE;
            }
            else if (Pec12.OK == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE_VALUE;
            }
            
            break;
            
        case MENU_STATE_AMPLITUDE_VALUE:
            
            if (Pec12.Inc == 1)
            {
                Amplitude_Selection += 100;
                
                if (Amplitude_Selection == 10100)
                {
                    Amplitude_Selection = 100;
                }
            }
            else if (Pec12.Dec == 1)
            {
                if (Amplitude_Selection == 100)
                {
                    Amplitude_Selection = 10100;
                }
                
                Amplitude_Selection -= 100;
            }
            
            // Gestion de la sortie de l'etat
            if (Pec12.OK == 1)
            {
                // On valide la valeur choisie
                pParam->Amplitude = Amplitude_Selection;
                
                MENU_DATA = MENU_STATE_AMPLITUDE;
            }
            else if (Pec12.ESC == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE; 
            }
            
            break;
            
        case MENU_STATE_OFFSET:
            
            // Gestion de la sortie de l'etat
            if (Pec12.Inc == 1)
            {
                MENU_DATA = MENU_STATE_FORME;
            }
            else if (Pec12.Dec == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE;
            }
            else if (Pec12.OK == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET_VALUE;
            }
            
            break;
            
        case MENU_STATE_OFFSET_VALUE:
            
            if (Pec12.Inc == 1)
            {
                if (Offset_Selection < 5000)
                {
                    Offset_Selection += 100;
                }
            }
            else if (Pec12.Dec == 1)
            {
                if (Offset_Selection > -5000)
                {
                    Offset_Selection -= 100;
                }
            }
            
            // Gestion de la sortie de l'etat
            if (Pec12.OK == 1)
            {
                // On valide la valeur choisie
                pParam->Offset = Offset_Selection;
                
                MENU_DATA = MENU_STATE_OFFSET;
            }
            else if (Pec12.ESC == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET; 
            }
            
            break;
            
    }
}




