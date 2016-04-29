// Il n'est pas nécéssaire d'include SoftwareSerial.h déjà présent sur la Teensy pour la communication série

#define DEBUG true
#define esp8266 Serial1
#define WifiOn 14

//String NomduReseauWifi = "HugoSpot"
//String MotDePasse = "201195";

void setup()
{
  //Activation de la wifi sur l'ESP
  pinMode(WifiOn, OUTPUT); // Le PIN 14 permet de démarrer la wifi ou de la mettre en veille sur des applications de demandant pas un accès permanant
  digitalWrite(WifiOn, HIGH); //Démarrage des communications séries
  Serial.begin(115200); // On démarre la communication avec le PC
  esp8266.begin(9600); // On démare la communication en l'Arduino et l'ESP

  delay(1000); // Temps d'attente pour que l'initialisation se déroule correctement

  //Initialisation du serveur ESP :
  Serial.println( "**************************************************************");
  Serial.println( "*************** Initialisation AT de l'ESP *******************");
  Serial.println( "**************************************************************");

  //sendData("AT\r\n", DEBUG);
  sendData("AT+RST\r\n", DEBUG); // Reset du module
  sendData("AT+CWMODE=2\r\n", DEBUG); // 2 configure comme un point d'accès
  sendData("AT+CIPMUX=1\r\n", DEBUG); // // Permet les connexions multiples
  sendData("AT+CIPSERVER=1,80\r\n", DEBUG); // Mise en route du serveur en port 80
  sendData("AT+CWSAP?\r\n", DEBUG); // Retourne le SSID, le mot de passe, le cannal d'émission et le cryptage de l'AP produite
  sendData("AT+CIFSR\r\n",DEBUG); // Retourne l'adresse IP de la passerelle associée au ESP8266

}

void loop()
{
  //Serveur http :

  if(esp8266.available()) // On vérifie que l'ESP n'envoie pas de message
  {

    if(esp8266.find("+IPD,")) // Si on a une connexion
    {
      delay(1000);

      int connectionId = esp8266.read()-48; //On soustrait 48 car read() retourne la valeur décimal
      // en ASCII et 0 (le premier chiffre décimal) commence à 48.

      //ConnectionId contient l'Id de la connexion

      String webpage = "<h1>Hello<h1><h2>World!</h2><button>LED1</button>";

      // On défini la taille de la commande qui va être envoyée
      String cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend +=webpage.length();
      cipSend +="\r\n";

      sendData(cipSend,DEBUG);
      sendData(webpage,DEBUG);

      webpage="LED2";

      cipSend = "AT+CIPSEND=";
      cipSend += connectionId;
      cipSend += ",";
      cipSend +=webpage.length();
      cipSend +="\r\n";

      sendData(cipSend,DEBUG);
      sendData(webpage,DEBUG);

      //Fermeture de la connexion
      String closeCommand = "AT+CIPCLOSE=";
      closeCommand+=connectionId; // append connection id
      closeCommand+="\r\n";

      sendData(closeCommand,DEBUG);
    }
}

void sendData(String command, bool debug)
{
  esp8266.print(command);
  String r = esp8266.readString();
  Serial.print(r);
}
