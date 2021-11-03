//Déclaration des bibliothèques
//Il faut avoir installé les bibliothèques GSM (SIM800L), DHT sensor library et Arduino_DY_IRDaikin (https://github.com/danny-source/Arduino_DY_IRDaikin)

#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 4 //Pin du capteur DHT
#define DHTTYPE DHT22      // DHT 22 ou DHT11
DHT dht(DHTPIN, DHTTYPE);  // Création de l'objet dht
#include <DYIRDaikin.h>
int Txd = 6, Rxd = 7; // Pins du SIM800L
SoftwareSerial gsm(Txd, Rxd); // Attention : RX, TX : il faut relier Rx de l'Arduino au Tx du SIM800L

#define DYIRDAIKIN_SOFT_IR //Pin de la LED infrarouge
DYIRDaikin irdaikin;
int led=3;

String reponse, numeroSMS, SMS;
unsigned long t0;

void setup() {
  pinMode(led,OUTPUT);
  Serial.begin(9600);
  gsm.begin(9600);
  delay(2000);
  dht.begin();
  Serial.println("Initialisation...");
  gsm.println("AT");
  while (!message("OK",1000,0)) gsm.println("AT");
  gsm.println("AT+CNUM");            // Affiche n° de la carte SIM utilisée !! Non fonctinnel !!
  message("OK",20000,1);
  Serial.print ("Qualite reseau : ");
  gsm.println("AT+CSQ");            // Qualité du réseau, pb si CSQ = 0, le plus est le mieux
  message("OK",10000,1);
  gsm.println("AT+CMGF=1");         // Mode Texte
  message("OK",1000,0);
  gsm.println("AT+CMGD=1,4"); // effacer les SMS en mémoire dans la carte SIM
  message("OK",2000,0);      // car on lit toujours le message N°1 de la carte SIM...

#ifdef DYIRDAIKIN_SOFT_IR
  irdaikin.begin(3);
  #else
  irdaikin.begin();
  #endif
  }

//Définition des "programmes" du Daikin (Chauffage, Confort, Airco, AllOff)

void Chauffage(){
  irdaikin.on();
  irdaikin.setSwing_off();
  irdaikin.setMode(3);
  irdaikin.setFan(5);//FAN speed to MAX
  irdaikin.setTemp(18);
  //----everything is ok to execute send command-----
  irdaikin.sendCommand(); 
}

void Airco(){
  irdaikin.on();
  irdaikin.setSwing_on();
  irdaikin.setMode(0);
  irdaikin.setFan(5);//FAN speed to MAX
  irdaikin.setTemp(20);
  //----everything is ok to execute send command-----
    irdaikin.sendCommand();
  
}

void AllOff(){
  irdaikin.off();
  //----everything is ok to execute send command-----
  irdaikin.sendCommand();
  
}

void Confort(){
  irdaikin.on();
  irdaikin.setSwing_off();
  irdaikin.setMode(3);
  irdaikin.setFan(1);//FAN speed
  irdaikin.setTemp(24);
  //----everything is ok to execute send command-----
  irdaikin.sendCommand();
}


void loop() {
  if (message("+CMTI:",20000,0)) LireSMS();   // Si nouveau SMS disponible SIM800 envoie +CMTI:
}

//Autre fonction :lecture SMS

void LireSMS(){
  gsm.println("AT+CMGF=1");  // Mode Texte
  message("OK",1000,0);
  gsm.println("AT+CMGR=1");  // Lit le premier message disponible sur la carte SIM
  message("OK",2000,1);
  // Récupérer N° de telephone emetteur pour lui répondre
  int test=reponse.indexOf("+32");
  numeroSMS = reponse.substring(test,test+12);
  Serial.println("SMS recu depuis : " + numeroSMS);
  
 // Analyse du message reçu :
  
  if (reponse.indexOf("Chauffage")>0) {
     SMS = "Ordre recu : Allumer le chauffage a 18 degres !";
     Chauffage();
     }
     else {
       if (reponse.indexOf("Off")>0){
       SMS = "Ordre recu : Eteindre tout !";
       AllOff();
       }
       else {
         if (reponse.indexOf("Airco")>0){
         SMS = "Ordre recu : Allumer airco a 20 degres !";
         Airco();
         }
         else {
          if (reponse.indexOf("Confort")>0){
          SMS = "Ordre recu : Allumer chauffage a 24 degres !";
          Confort();
          }
         else {
          if (reponse.indexOf("Temp")>0){
          float h = dht.readHumidity();
          float t = dht.readTemperature();
          if (isnan(h) || isnan(t) ){
             Serial.println(F("Impossible de se connecter au capteur DHT !"));
             }
          Serial.println("Température "+String(t,1)+" C");
          Serial.println("Humidité "+String(h,1)+" %");
          SMS = "La temperature actuelle est de "+String(t,1)+" C" + " avec une humidite de "+String(h,1)+ " pourcents";   //A string to regroup the whole message as it's composed of Strings and Float --> to a single string,
          }
          else {
            SMS = "Je n'ai pas compris !";
          }
        }
         }
      }
    }
  gsm.println("AT+CMGD=1,4"); // effacer les SMS de la Carte SIM
  message("OK",1000,0);
  Serial.print("Message à envoyer : ");
  Serial.println(SMS);
  
  // Envoyer la confirmation de l'ordre par SMS 
  Serial.println("Envoi du message de confirmation"); 
  gsm.println("AT+CMGS=\""+ numeroSMS +"\"");
  message(">",1000,0);
  gsm.print(SMS);
  gsm.write(26); // Caractère de fin 26 <Ctrl-Z>
  gsm.println(""); 
  message("+CMGS:",10000,1);  
}
      
/* Sous Programme message(attente, timeout, affiche)
Après avoir envoyé une instruction AT au module SIM800,celui-ci répond. 
message("OK", 1000,1) :
  Vérifie que la réponse contient le terme "OK" (attente)
  Il attend au maximum 1000ms (timeout) la réponse
  Affiche (affiche=1) la réponse et le temps mis pour l'obtenir
  ou n'affiche pas (affiche=0) sauf si la réponse attendue n'est pas correcte
  renvoie vrai si le message contient OK en moins de 1000 ms
*/

boolean message(String attente, unsigned int timeout, boolean affiche) {
  t0 = millis();
  reponse="";
  while (millis() - t0 < timeout) {
    while(gsm.available()) reponse.concat(char(gsm.read()));
    if (reponse.indexOf(attente)>0){        // Lit encore 100 ms le port série  
      delay (100);                          // pour être sur de ne rien louper...
       while(gsm.available()) reponse.concat(char(gsm.read()));
        break;}}
  if (affiche || reponse.indexOf(attente)==-1 ) {      
    Serial.print("Attente = "+ attente +" "+ reponse.indexOf(attente)+" duree ");
    Serial.print(millis()-t0); Serial.println(" ms");
    Serial.println(reponse);}
  
  if (reponse.indexOf(attente)>0) return true;
  else return false;
}
