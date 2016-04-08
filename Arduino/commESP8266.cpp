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
  digitalWrite(WifiOn, HIGH) //Démarrage des communications séries
  Serial.begin(115200); // On démarre la communication avec le PC
  esp8266.begin(9600); // On démare la communication en l'Arduino et l'ESP

  delay(1000); // Temps d'attente pour que l'initialisation se déroule correctement

  //Initialisation du serveur ESP :
  Serial.println( "**************************************************************")
  Serial.println( "*************** Initialisation AT de l'ESP *******************");
  Serial.println( "**************************************************************");

  //sendData("AT\r\n", DEBUG);
  sendData("AT+RST\r\n", DEBUG); // Reset du module
  sendData("AT+CWMODE=2\r\n", DEBUG); // 2 configure comme un point d'accès
  sendData("AT+CIPMUX=1\r\n", DEBUG); // // Permet les connexions multiples
  sendData("AT+CIPSERVER=1,80\r\n", DEBUG); // Mise en route du serveur en port 80
  sendData("AT+CWSAP?\r\n", DEBUG); // Retourne le SSID, le mot de passe, le channel d'émission et le cryptage de l'AP produite
}

void loop()
{

}

void sendData(String command, bool debug)
{
  esp8266.print(command);
  String r = esp8266.readString();
  Serial.print(r);
}
