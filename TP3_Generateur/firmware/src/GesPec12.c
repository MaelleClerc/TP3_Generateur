// GesPec12.c  Canevas pour r�alisation  
// C. HUBER    09/02/2015

// Fonctions pour la gestion du Pec12
//
//
// Principe : Il est n�cessaire d'appeler cycliquement la fonction ScanPec12
//            avec un cycle de 1 ms
//
//  Pour la gestion du Pec12, il y a 9 fonctions � disposition :
//       Pec12IsPlus       true indique un nouveau incr�ment
//       Pec12IsMinus      true indique un nouveau d�cr�ment
//       Pec12IsOK         true indique action OK
//       Pec12IsESC        true indique action ESC
//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
//       Pec12ClearMinus   annule indication de d�cr�ment
//       Pec12ClearOK      annule indication action OK
//       Pec12ClearESC     annule indication action ESC
//
//
//---------------------------------------------------------------------------


// d�finitions des types qui seront utilis�s dans cette application

#include "GesPec12.h"
#include "Mc32Debounce.h"
#include "Mc32DriverLcd.h"

// Descripteur des sinaux
S_SwitchDescriptor DescrA;
S_SwitchDescriptor DescrB;
S_SwitchDescriptor DescrPB;

// Structure pour les traitement du Pec12
S_Pec12_Descriptor Pec12;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Principe utilisation des fonctions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
// ScanPec12 (bool ValA, bool ValB, bool ValPB)
//              Routine effectuant la gestion du Pec12
//              recoit la valeur des signaux et du boutons
//
// s'appuie sur le descripteur global.
// Apr�s l'appel le descripteur est mis � jour

// Comportement du PEC12
// =====================

// Attention 1 cran g�n�re une pulse compl�te (les 4 combinaisons)
// D'ou traitement uniquement au flanc descendand de B

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|                    

// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|        

void ScanPec12 (bool ValA, bool ValB, bool ValPB)
{
    // Variables locales
    static uint8_t Compteur_Pression = 0;
    static uint32_t Compteur_Inactivite = 0;
    
    // Traitement antirebond sur A, B et PB
    DoDebounce (&DescrA, ValA);
    DoDebounce (&DescrB, ValB);
    DoDebounce (&DescrPB, ValPB);
    

    // Detection increment / decrement
    // Rotation a droite
    if (((DescrB.bits.KeyPrevInputValue == 0) && (DescrB.bits.KeyValue == 1) && (DescrA.bits.KeyValue == 1)) || 
        ((DescrB.bits.KeyPrevInputValue == 1) && (DescrB.bits.KeyValue == 0) && (DescrA.bits.KeyValue == 0)))
    {
        Pec12.Inc = 1;
    }
    // Rotation a gauche
    else if (((DescrB.bits.KeyPrevInputValue == 0) && (DescrB.bits.KeyValue == 1) && (DescrA.bits.KeyValue == 0)) || 
             ((DescrB.bits.KeyPrevInputValue == 1) && (DescrB.bits.KeyValue == 0) && (DescrA.bits.KeyValue == 1)))
    {
        Pec12.Dec = 1;
    }
    else
    {
        Pec12.Inc = 0;
        Pec12.Dec = 0;
    }

    // Traitement du PushButton
    // Si le bouton est presse
    if (DescrPB.bits.KeyValue == 1)
    {
        Pec12.OK = 0;
        Pec12.ESC = 1;
        Compteur_Pression = 0;
    }
    else if (DescrPB.bits.KeyValue == 0)
    {
        // On incremente un compteur tant que le bouton est presse
        Compteur_Pression ++;
    }
    // Si le bouton est relache avant 500ms (cette fonction est appelee toutes les 10ms)
    if ((Compteur_Pression < 50) && (DescrPB.bits.KeyValue == 0))
    {
        Pec12.OK = 1;
        Pec12.ESC = 0;
        Compteur_Pression = 0;
    }
    // Si le bouton est relache a partir de 500ms
    else if ((Compteur_Pression >= 50) && (DescrPB.bits.KeyValue == 0))
    {
        Pec12.ESC = 1;
        Pec12.OK = 0;
        Compteur_Pression = 0;
    }

    // Gestion inactivite
    // Si rien n'a ete touche
    if ((DescrA.bits.KeyValue == 0) && (DescrB.bits.KeyValue == 0) && (DescrPB.bits.KeyValue == 1))
    {
        Compteur_Inactivite ++;
        
        // Si cela fait 5s que rien ne s'est passe (5s, appel toutes les 10ms => 5*60*100 = 30000)
        if(Compteur_Inactivite == 30000)
        {
            // On eteint le backlight du LCD
            lcd_bl_off();
        }
    }
    else
    {
        // On rallume le backlight du LCD
        lcd_bl_on();
        Compteur_Inactivite = 0;
    }
   
 } // ScanPec12


void Pec12Init (void)
{
   // Initialisation des descripteurs de touches Pec12
   DebounceInit(&DescrA);
   DebounceInit(&DescrB);
   DebounceInit(&DescrPB);
   
   // Init de la structure PEc12
    Pec12.Inc = 0;             // �v�nement incr�ment  
    Pec12.Dec = 0;             // �v�nement d�cr�ment 
    Pec12.OK = 0;              // �v�nement action OK
    Pec12.ESC = 0;             // �v�nement action ESC
    Pec12.NoActivity = 0;      // Indication d'activit�
    Pec12.PressDuration = 0;   // Pour dur�e pression du P.B.
    Pec12.InactivityDuration = 0; // Dur�e inactivit�
  
 } // Pec12Init





//       Pec12IsPlus       true indique un nouveau incr�ment
bool Pec12IsPlus    (void) {
   return (Pec12.Inc);
}

//       Pec12IsMinus      true indique un nouveau d�cr�ment
bool Pec12IsMinus    (void) {
   return (Pec12.Dec);
}

//       Pec12IsOK         true indique action OK
bool Pec12IsOK    (void) {
   return (Pec12.OK);
}

//       Pec12IsESC        true indique action ESC
bool Pec12IsESC    (void) {
   return (Pec12.ESC);
}

//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
bool Pec12NoActivity    (void) {
   return (Pec12.NoActivity);
}

//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
void Pec12ClearPlus   (void) {
   Pec12.Inc = 0;
}

//       Pec12ClearMinus   annule indication de d�cr�ment
void Pec12ClearMinus   (void) {
   Pec12.Dec = 0;
}

//       Pec12ClearOK      annule indication action OK
void Pec12ClearOK   (void) {
   Pec12.OK = 0;
}

//       Pec12ClearESC     annule indication action ESC
void Pec12ClearESC   (void) {
   Pec12.ESC = 0;
}

void Pec12ClearInactivity   (void) {
  Pec12.NoActivity = 0;
  Pec12.InactivityDuration = 0;
}


