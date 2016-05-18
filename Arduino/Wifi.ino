// Il n'est pas nécéssaire d'include SoftwareSerial.h déjà présent sur la Teensy pour la communication série

#include<String>

#include <TimerOne.h>
#include <time.h>
#include <Wire.h>


#define esp8266 Serial1
#define WifiOn 4
#define LedPin 13

#define DEBUG 0



const int ledPin = 13;  // Teensy 3.0 has the LED on pin 13
int dObstacle = 10;


// sorties  de controle des moteurs
const int M2Enable = 23;
const int M1Enable = 22;
const int M2Out1 = 16;
const int M2Out0 = 17;
const int M1Out1 = 14;
const int M1Out0 = 15;
// entr�e capteur
const int M1In1 = 5;
const int M1In0 = 6;
const int M2In1 = 8;  // sens inverse de M1
const int M2In0 = 7;


// interruption sur M1In0
//variables globales
volatile long int posM1 = 0;   // volatile pour modif dans l'interruption  multi processus
volatile long int posM2 = 0;   // position sur 32 bits
volatile unsigned long int timeout=0;   // au cas où
volatile long int posM1Anc = 0;   // volatile pour modif dans l'interruption  multi processus
volatile long int posM2Anc = 0;   // position pr�c�dente pour le calcul de la vitesse
volatile float vM1 = 0;   // vitesse en pas par seconde
volatile float vM2 = 0;
int intervalAsserv = 20000;  // 50000�s entre chaque interruption du timer 50ms
float intervalAsservSec = (float)intervalAsserv / 1.0E+6;
volatile float fAux1 = 0;  // variable de retour
volatile float fAux2 = 0;

// coeffiscient de correction de puissance
float kPM1 = 2.5;
float kPM2 = 2.5;

float consigneVitesseM1 = 0;  // vitesse demand�e
float consigneVitesseM2 = 0;
// liste des param�tres globaux
float paramVitesse = 1000;   // 


// interruption declench�e par la fourche optique du moteur 1
void ISRM1()
{   noInterrupts();  // zone ex

   // tester M1In0
    if (digitalRead( M1In0) == HIGH)
    posM1++;  // dans sens positif voir chronogramme
  else
    posM1--;  // dans sens n�gatif
  
  
  interrupts();  // zone ex
  
}
// interruption declench�e par la fourche optique du moteur 2
void ISRM2()
{
  noInterrupts();  // zone ex
  // tester M1In0
  if (digitalRead( M2In0) == HIGH)
    posM2++;  // dans sens positif voir chronogramme
  else
    posM2--;
  
  interrupts();  // zone ex
  
}
// fonction simple de calcul de la puissance en fonction de l'erreur sur la vitesse
int fonctionP( float v, float consigne, float k)
{
  if (fabs(consigne ) <10)
    return 0;  // c'est un arret

  float erreur = consigne - v;
  float puissance = k * erreur;
  fAux1 = erreur;
  fAux2 = puissance;

  if ( fabs(puissance) < 10)
    return 0;  // arret
  else if ( puissance < -255)
    return -255;
  else if ( puissance > 255)
    return 255;
  else
    return (int)puissance;
}
// fonctions d'asservissement
void regulV()
{
    
  MotorCC1( fonctionP( vM1, consigneVitesseM1, kPM1));  // calcul de puissance et application
  MotorCC2( fonctionP( vM2, consigneVitesseM2, kPM2));
}
// interruption du timer : calcul de la vitesse et asservissement
void ITimer1()
{
  if (timeout > 0)
    timeout--;
  // calcul de vitesse
  vM1 = (float)(posM1 - posM1Anc) / intervalAsservSec;
  vM2 = (float)(posM2 - posM2Anc) / intervalAsservSec;
  posM1Anc = posM1;
  posM2Anc = posM2;
  // appel fonctions d'asservissement
  regulV();

}

// fonction bas niveau de commande des moteurs
void MotorCC1( bool bEnable, bool bSens)
{
  if ( bSens)
  {
    digitalWrite( M1Out0, HIGH);    // rive gauche du pont � Vc
    digitalWrite( M1Out1, LOW);    // rive droite du pont � 0
  }
  else
  {
    digitalWrite( M1Out0, LOW);    // rive gauche du pont � 0
    digitalWrite( M1Out1, HIGH);    // rive droite du pont � Vc
  }

  
  digitalWrite( M1Enable, bEnable?HIGH:LOW);    // autoriser ou non
  
}
void MotorCC1( int iPuissance, bool bSens)
{
  if ( bSens)
  {
    digitalWrite( M1Out0, HIGH);    // rive gauche du pont � Vc
    digitalWrite( M1Out1, LOW);    // rive droite du pont � 0
  }
  else
  {
    digitalWrite( M1Out0, LOW);    // rive gauche du pont � 0
    digitalWrite( M1Out1, HIGH);    // rive droite du pont � Vc
  }

  analogWrite( M1Enable, iPuissance);

}
void MotorCC2( int iPuissance, bool bSens)
{
  if ( bSens)
  {
    digitalWrite( M2Out0, HIGH);    // rive gauche du pont � Vc
    digitalWrite( M2Out1, LOW);    // rive droite du pont � 0
  }
  else
  {
    digitalWrite( M2Out0, LOW);    // rive gauche du pont � 0
    digitalWrite( M2Out1, HIGH);    // rive droite du pont � Vc
  }

  analogWrite( M2Enable, iPuissance);
  
}
void MotorCC1( int iPuissance)
{
  if ( iPuissance>=0)
  {
    digitalWrite( M1Out0, HIGH);    // rive gauche du pont � Vc
    digitalWrite( M1Out1, LOW);    // rive droite du pont � 0
  }
  else
  {   // sens n�gatif
    iPuissance = -iPuissance;
    digitalWrite( M1Out0, LOW);    // rive gauche du pont � 0
    digitalWrite( M1Out1, HIGH);    // rive droite du pont � Vc
  }

  analogWrite( M1Enable, iPuissance);  // 
  
}
void MotorCC2( int iPuissance)
{
  if ( iPuissance>=0)
  {
    digitalWrite( M2Out0, HIGH);    // rive gauche du pont � Vc
    digitalWrite( M2Out1, LOW);    // rive droite du pont � 0
  }
  else
  {   // sens n�gatif
    iPuissance = -iPuissance;
    digitalWrite( M2Out0, LOW);    // rive gauche du pont � 0
    digitalWrite( M2Out1, HIGH);    // rive droite du pont � Vc
  }

  analogWrite( M2Enable, iPuissance);  

}
// divers
void Stop()
{
  MotorCC1( 0, true);  
  MotorCC2( 0, true);  
}

void posOrigine()
{
  posM1 = 0;
  posM2 = 0;
  
  digitalWrite(ledPin, LOW);    // no error
}

// liaison bluetooth
String readHC06( )
{
  String sRet;
  
  sRet = Serial1.readString( ); // echo + r�ponse
#ifdef DEBUG
  Serial.println( "r->" + sRet);    // debug
#endif
  return sRet;
}

String sendHC06( String as)
{
  String sRet;
  Serial1.print( as);
  
  sRet = Serial1.readString( ); // echo + r�ponse
#ifdef DEBUG
  Serial.println( "r->" + sRet);    // debug
#endif
  return sRet;
}

String sendHC06_ln( String as)
{
  String sRet;
  Serial1.println( as);
  
  sRet = Serial1.readString( ); // echo + r�ponse
#ifdef DEBUG
  Serial.println( "r->" + sRet);    // debug
#endif
  return sRet;
}


//String NomduReseauWifi = "HugoSpot"
//String MotDePasse = "201195";

void setup()
{
  //Activation de la wifi sur l'ESP
  pinMode(WifiOn, OUTPUT); // Le PIN 14 permet de démarrer la wifi ou de la mettre en veille sur des applications de demandant pas un accès permanant
  pinMode(LedPin, OUTPUT);
  digitalWrite(WifiOn, HIGH);
  
  // interruption
  pinMode( M1In1, INPUT); // inter
  pinMode( M1In0, INPUT); // quadrature
  attachInterrupt( M1In1, ISRM1, FALLING);  // li� l'�v�nement interruption sur le pin M1In1 avec la fonction ISRM1
  // moteur 2
  pinMode( M2In1, INPUT); // inter
  pinMode( M2In0, INPUT); // quadrature
  attachInterrupt( M2In1, ISRM2, FALLING);  // li� l'�v�nement interruption sur le pin M1In1 avec la fonction ISRM1

   pinMode( M1Enable, OUTPUT);  
  pinMode( M2Enable, OUTPUT);  
  pinMode( M1Out0, OUTPUT);  
  pinMode( M1Out1, OUTPUT);  
  pinMode( M2Out0, OUTPUT);  
  pinMode( M2Out1, OUTPUT);  

   // timer pour l'asservissement
  Timer1.initialize(intervalAsserv); // choix du delais
  Timer1.attachInterrupt( ITimer1);  // accroche de la routine d'interruption
  
  // liaisons s�ries

  

  // sortie
    pinMode( M1Enable, OUTPUT);   
  #ifdef DEBUG
    Serial.begin( 115200); // debug avec la console
    delay( 100);
    Serial.println( "init...");
  #endif
    
  //Démarrage des communications séries:
  Serial.begin(9600); // On démarre la communication avec le PC
  esp8266.begin(9600); // On démare la communication en l'Arduino et l'ESP

  digitalWrite(ledPin, HIGH); 
  
  delay(2000); // Temps d'attente pour que l'initialisation se déroule correctement

  //Initialisation du serveur ESP :
  Serial.println( "**************************************************************");
  Serial.println( "*************** Initialisation AT de l'ESP *******************");
  Serial.println( "**************************************************************");


  
  sendData("AT+RST\r\n",DEBUG); // reset du module
  sendData("AT+CWMODE=3\r\n",DEBUG); // 2 configure comme un point d'accès
  sendData("AT+CIFSR\r\n",DEBUG); // retourne l'adresse IP
  sendData("AT+CIPMUX=1\r\n",DEBUG); // Connexion multiples (Pas obligatoire pour nous)
  sendData("AT+CWSAP=\"Hugo\",\"1234test\",5,2\r\n",DEBUG); // Défini le nom du hotspot et le mdp
  sendData("AT+CIPSERVER=1,80\r\n",DEBUG); // mise en route du serveur en port 80
  //sendData("AT+CIPSTO=5000\r\n",DEBUG);
  //sendData("\r\n",DEBUG);

  esp8266.setTimeout(100);
  digitalWrite(ledPin, LOW); 
  delay(50);
  digitalWrite(ledPin, HIGH); 
  delay(50);
  digitalWrite(ledPin, LOW); 
}

void loop()
{
  //sendData("AT+IPD\r\n",DEBUG);


  
  //Serveur http :

  if(esp8266.available()) // On vérifie que l'ESP n'envoie pas de message
  {

    if(esp8266.find("+IPD,")) // Si on a une connexion
    {
      delay(1000);

      int connectionId = esp8266.read()-48; //On soustrait 48 car read() retourne la valeur décimal
      // en ASCII et 0 (le premier chiffre décimal) commence à 48.

      //ConnectionId contient l'Id de la connexion
      

     //String yo = esp8266.read();
     //Serial.println(yo);


      //haut : F80;;
      //bas : B80;;
      //Gauche : L80;;
      //Droite : R80;;
      
      String cmd;
      
      //String webpage = "<h1>Hello<h1><h2>World!</h2><button>LED1</button>";

      // On défini la taille de la commande qui va être envoyée
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      //cipSend += ",";
      //cipSend +=webpage.length();
      cipSend +="\r\n";

      
      cmd = sendDataRetour(cipSend,DEBUG);
      //sendData(webpage,DEBUG);

      cmd = cmd.substring(0,8);

      if(cmd == ",5:B80;;")
      {
        //Serial.println("#####BAS !!####"); 
        //BlindLed(100);

        //consigneVitesseM1 = -paramVitesse;
        //<consigneVitesseM2 = -paramVitesse; 

        consigneVitesseM1 = 0;
        consigneVitesseM2 = 0;
        
        //digitalWrite(ledPin, HIGH);
      }


      
      if(cmd == ",5:R80;;")
      {
        //Serial.println("####DROITE !!####"); 
        //BlindLed(200);

        consigneVitesseM1 = paramVitesse/2;
        consigneVitesseM2 = paramVitesse;
        
        
        //digitalWrite(ledPin, HIGH);
      }


      
      if(cmd == ",5:L80;;")
      {
        //Serial.println("####GAUCHE !!####"); 
        //BlindLed(300);

        consigneVitesseM1 = paramVitesse;
        consigneVitesseM2 = paramVitesse/2;
        
        //digitalWrite(ledPin, HIGH);
      }


      
      if(cmd == ",5:F80;;")
      {
        //Serial.println("####HAUT !!####"); 
        //BlindLed(400);
        consigneVitesseM1 = paramVitesse;
        consigneVitesseM2 = paramVitesse;
        //digitalWrite(ledPin, HIGH);
      }


      if(cmd == ",5:S80;;")
      {
        //Serial.println("####STOP !!####"); 
        //BlindLed(200);

        consigneVitesseM1 = 0;
        consigneVitesseM2 = 0;
        
        
        //digitalWrite(ledPin, HIGH);
      }



      
      /*webpage="LED2";

      cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend +=webpage.length();
      cipSend +="\r\n";

      //sendData(cipSend,DEBUG);
      //sendData(webpage,DEBUG);

      //Fermeture de la connexion
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";*/

      //sendData(closeCommand,DEBUG);

      //Serial.print("CONNEXION ! Voici l'ID :");
      //Serial.println(connectionId);

      


      
    }
}
}

void sendData(String command, bool debug)
{
  esp8266.print(command);
  String r = esp8266.readString();
  Serial.print(r);
}

String sendDataRetour(String command, bool debug)
{
  esp8266.print(command);
  String r = esp8266.readString();
  return(r);
}

void BlindLed(int time)
{

  for(int n = 0; n < 5 ; n++)
   { digitalWrite(LedPin, HIGH);
    delay(time);
    digitalWrite(LedPin, LOW);
    delay(time);
  }
}


