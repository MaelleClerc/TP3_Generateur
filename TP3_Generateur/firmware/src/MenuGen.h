#ifndef MenuGen_h
#define MenuGen_h

// Tp3  manipulation MenuGen avec PEC12
// C. HUBER  03.02.2016
// Fichier MenuGen.h
// Gestion du menu  du générateur
// Traitement cyclique à 1 ms du Pec12


#include <stdbool.h>
#include <stdint.h>
#include "DefMenuGen.h"

// *****************************************************************************
/* Application states

  Summary:
    Menu states enumeration

  Description:
    This enumeration defines the valid menu states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
	/* Menu's state machine's initial state. */
	MENU_STATE_FORME=0,
    MENU_STATE_FORME_VALUE=1,
    MENU_STATE_FREQUENCE=2,
    MENU_STATE_FREQUENCE_VALUE=3,
    MENU_STATE_AMPLITUDE=4,
    MENU_STATE_AMPLITUDE_VALUE=5,
    MENU_STATE_OFFSET=6,
    MENU_STATE_OFFSET_VALUE=7,
    MENU_STATE_SAVE=8
	
} MENU_STATES;


void MENU_Initialize(S_ParamGen *pParam);


void MENU_Execute(S_ParamGen *pParam);

#endif




  
   







