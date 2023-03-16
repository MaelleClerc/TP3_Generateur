// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  10/02/2015 pour SLO2 2014-2015
// Fichier MenuGen.c
// Gestion du menu  du générateur
// Traitement cyclique à 10 ms



#include <stdint.h>                   
#include <stdbool.h>
#include "MenuGen.h"
#include "GesPec12.h"
#include "Mc32DriverLcd.h"
#include "Generateur.h"
#include "bsp.h"

MENU_STATES MENU_DATA;

// Structure pour les traitement du Pec12
S_Pec12_Descriptor Pec12;

// Tableau constant pour l'affichage de la forme du signal
const char MenuFormes[4][21] = 
    {" Sinus", " Triangle", " DentDeScie", " Carre"};

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
    static uint8_t CompteurForme = 0;
    static bool Init_Affichage = 0;
    
    // Initialisation de l'affichage
    if (Init_Affichage == 0)
    {
            lcd_gotoxy(1, 1);
            printf_lcd(" Forme = Sinus");
            lcd_gotoxy(1, 2);
            printf_lcd(" Freq [Hz] = %4d", pParam->Frequence);
            lcd_gotoxy(1, 3);
            printf_lcd(" Ampl [mV] = %5d", pParam->Amplitude);
            lcd_gotoxy(1, 4);
            printf_lcd(" Offset [mV] = %5d", pParam->Offset);
            Init_Affichage = 1;
    }
    
    switch (MENU_DATA)
    {
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_FORME:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 1);
            switch (pParam->Forme)
            {
                case 0:
                    
                    printf_lcd("*Forme = Sinus");
                    
                    break;
                    
                case 1:
                    
                    printf_lcd("*Forme = Triangle");
                    
                    break;
                    
                case 2:
                    
                    printf_lcd("*Forme = DentDeScie");
                    
                    break;
                    
                case 3:
                    
                    printf_lcd("*Forme = Carre");
                    
                    break;
            }
            
            // Gestion de la sortie de l'etat
            if (Pec12IsPlus() == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE;
                lcd_gotoxy(1, 1);
                printf_lcd(" ");
            }
            else if (Pec12IsMinus() == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET;
                lcd_gotoxy(1, 1);
                printf_lcd(" ");
            }
            else if (Pec12IsOK() == 1)
            {
                MENU_DATA = MENU_STATE_FORME_VALUE;
                lcd_ClearLine(1);
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_FORME_VALUE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 1);
            switch (CompteurForme)
            {
                case 0:
                    
                    printf_lcd("#Forme = Sinus     ");
                    
                    break;
                    
                case 1:
                    
                    printf_lcd("#Forme = Triangle   ");
                    
                    break;
                    
                case 2:
                    
                    printf_lcd("#Forme = DentDeScie");
                    
                    break;
                    
                case 3:
                    
                    printf_lcd("#Forme = Carre     ");
                    
                    break;
            }
            
            if (Pec12IsPlus() == 1)
            {
                CompteurForme ++;
                
                if (CompteurForme == 4)
                {
                    CompteurForme = 0;
                }
            }
            else if (Pec12IsMinus() == 1)
            {
                if (CompteurForme == 0)
                {
                    CompteurForme = 4;
                }
                
                CompteurForme --;
            }
            
            // Gestion de la sortie de l'etat
            // Si la selection est OK
            if (Pec12IsOK() == 1)
            {
                // On valide la valeur choisie
                switch (CompteurForme)
                {
                    case 0:
                        
                        pParam->Forme = SignalSinus;
                        
                        break;
                        
                    case 1:
                        
                        pParam->Forme = SignalTriangle;
                        
                        break;
                        
                    case 2:
                        
                        pParam->Forme = SignalDentDeScie;
                        
                        break;
                        
                    case 3:
                        
                        pParam->Forme = SignalCarre;
                        
                        break;
                }
                
                // On envoie les nouvelles valeurs au generateur
                GENSIG_UpdateSignal(pParam);
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_FORME;
                lcd_ClearLine(1);
            }
            // Si la selection n'est pas OK
            else if (Pec12IsESC() == 1)
            {
                // On reprend l'ancienne valeur
                CompteurForme = pParam->Forme;
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_FORME; 
                lcd_ClearLine(1);
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_FREQUENCE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 2);
            printf_lcd("*Freq [Hz] = %4d", pParam->Frequence);
            
            // Gestion de la sortie de l'etat
            if (Pec12IsPlus() == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE;
                lcd_gotoxy(1, 2);
                printf_lcd(" Freq [Hz] = %4d", pParam->Frequence);
            }
            else if (Pec12IsMinus() == 1)
            {
                MENU_DATA = MENU_STATE_FORME;
                lcd_ClearLine(1);
                lcd_gotoxy(1, 2);
                printf_lcd(" Freq [Hz] = %4d", pParam->Frequence);
            }
            else if (Pec12IsOK() == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE_VALUE;
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_FREQUENCE_VALUE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 2);
            printf_lcd("#Freq [Hz] = %4d", Frequence_Selection);
            
            if (Pec12IsPlus() == 1)
            {
                Frequence_Selection += 20;
                
                if (Frequence_Selection == 2020)
                {
                    Frequence_Selection = 20;
                }
            }
            else if (Pec12IsMinus() == 1)
            {
                if (Frequence_Selection == 20)
                {
                    Frequence_Selection = 2020;
                }
                
                Frequence_Selection -= 20;
            }
            
            // Gestion de la sortie de l'etat
            // Si la selection est OK
            if (Pec12IsOK() == 1)
            {
                // On valide la valeur choisie
                pParam->Frequence = Frequence_Selection;
                
                // On envoie les nouvelles valeurs au generateur
                GENSIG_UpdateSignal(pParam);
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_FREQUENCE;
            }
            // Si la selection n'est pas OK
            else if (Pec12IsESC() == 1)
            {
                // On reprend l'ancienne valeur
                Frequence_Selection = pParam->Frequence;
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_FREQUENCE; 
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_AMPLITUDE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 3);
            printf_lcd("*Ampl [mV] = %5d", pParam->Amplitude);
            
            // Gestion de la sortie de l'etat
            if (Pec12IsPlus() == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET;
                lcd_gotoxy(1, 3);
                printf_lcd(" Ampl [mV] = %5d", pParam->Amplitude);
            }
            else if (Pec12IsMinus() == 1)
            {
                MENU_DATA = MENU_STATE_FREQUENCE;
                lcd_gotoxy(1, 3);
                printf_lcd(" Ampl [mV] = %5d", pParam->Amplitude);
            }
            else if (Pec12IsOK() == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE_VALUE;
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_AMPLITUDE_VALUE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 3);
            printf_lcd("#Ampl [mV] = %5d", Amplitude_Selection);
            
            if (Pec12IsPlus() == 1)
            {
                Amplitude_Selection += 100;
                
                if (Amplitude_Selection == 10100)
                {
                    Amplitude_Selection = 100;
                }
            }
            else if (Pec12IsMinus() == 1)
            {
                if (Amplitude_Selection == 100)
                {
                    Amplitude_Selection = 10100;
                }
                
                Amplitude_Selection -= 100;
            }
            
            // Gestion de la sortie de l'etat
            // Si la selection est OK
            if (Pec12IsOK() == 1)
            {
                // On valide la valeur choisie
                pParam->Amplitude = Amplitude_Selection;
                
                // On envoie les nouvelles valeurs au generateur
                GENSIG_UpdateSignal(pParam);
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_AMPLITUDE;
            }
            // Si la selection n'est pas OK
            else if (Pec12IsESC() == 1)
            {
                // On reprend l'ancienne valeur
                Amplitude_Selection = pParam->Amplitude;
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_AMPLITUDE; 
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;

////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_OFFSET:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 4);
            printf_lcd("*Offset [mV] = %5d", pParam->Offset);
            
            // Gestion de la sortie de l'etat
            if (Pec12IsPlus() == 1)
            {
                MENU_DATA = MENU_STATE_FORME;
                lcd_ClearLine(1);
                lcd_gotoxy(1, 4);
                printf_lcd(" Offset [mV] = %5d", pParam->Offset);
            }
            else if (Pec12IsMinus() == 1)
            {
                MENU_DATA = MENU_STATE_AMPLITUDE;
                lcd_gotoxy(1, 4);
                printf_lcd(" Offset [mV] = %5d", pParam->Offset);
            }
            else if (Pec12IsOK() == 1)
            {
                MENU_DATA = MENU_STATE_OFFSET_VALUE;
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_OFFSET_VALUE:
            
            // Gestion de l'affichage
            lcd_gotoxy(1, 4);
            printf_lcd("#Offset [mV] = %5d", Offset_Selection);
            
            if (Pec12IsPlus() == 1)
            {
                if (Offset_Selection < 5000)
                {
                    Offset_Selection += 100;
                }
            }
            else if (Pec12IsMinus() == 1)
            {
                if (Offset_Selection > -5000)
                {
                    Offset_Selection -= 100;
                }
            }
            
            // Gestion de la sortie de l'etat
            // Si la selection est OK
            if (Pec12IsOK() == 1)
            {
                // On valide la valeur choisie
                pParam->Offset = Offset_Selection;
                
                // On envoie les nouvelles valeurs au generateur
                GENSIG_UpdateSignal(pParam);
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_OFFSET;
            }
            // Si la selection n'est pas OK
            else if (Pec12IsESC() == 1)
            {
                // On reprend l'ancienne valeur
                Offset_Selection = pParam->Offset;
                
                // On retourne au menu precedent
                MENU_DATA = MENU_STATE_OFFSET; 
            }
            else if (PLIB_PORTS_PinGet(S_OK, PORT_CHANNEL_G, PORTS_BIT_POS_12) == 0)
            {
                MENU_DATA = MENU_STATE_SAVE;
                lcd_ClearLine(1);
                lcd_ClearLine(2);
                lcd_ClearLine(3);
                lcd_ClearLine(4);
            }
            
            break;
            
////////////////////////////////////////////////////////////////////////////////
        case MENU_STATE_SAVE:
            
            // Affichage specifique
            lcd_gotoxy(1, 2);
            printf_lcd("   Sauvegarde  ?");
            lcd_gotoxy(1, 3);
            printf_lcd("   (appuis long)");
            
            // Gestion de la sortie de l'etat
            /* if ()
             * {
             *     MENU_DATA = MENU_STATE_FORME;
             *     Init_Affichage = 1; 
             * }
            */
             
            break;
    }
    
    // On nettoie les flags du Pec12
    Pec12ClearESC();
    Pec12ClearMinus();
    Pec12ClearOK();
    Pec12ClearPlus();
}




