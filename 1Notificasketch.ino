//Librerie di connessione
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
//librerie per il servo
#include <Wire.h>
#include <RTClib.h>
#include <ESP32Servo.h>
#include "DHT.h"

#define DHTPIN 4     // Pin al quale Ã¨ collegato il sensore DHT
#define DHTTYPE DHT22   // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);
// Sostituisci con i tuoi dati di rete
const char* ssid = "TIM-72204652";
const char* password = "9DDcNQZhxUDSSzezGcUdfuuu";
// Chiavi API di Pushover
const char* pushoverUserKey = "ubgwth6sp27sxcgcb9ha7xdoo6vdtn";
const char* pushoverApiToken = "a43r47bd8ohkhmzjiq3gppf4wumxph";

// Prototipo della funzione
void inviaNotificaPushover(const String& messaggio);
// Indirizzo del server dove inviare la richiesta
const char* serverName = "http://192.168.1.42/Medicinali/dbconnect.php";

// Timer impostato a 1 minuto (60000 millisecondi)
unsigned long timerDelay = 60000;
unsigned long lastTime = 0;

//DEFINIZIONE VARIABILI DEL MECCANISMO
#define trigPin 13 // Definisci TrigPin
#define echoPin 14 // Definisci EchoPin
#define MAX_DISTANCE 700 // Distanza massima del sensore
#define servoPin1 15 // Pin del servo motore 1
#define servoPin2 18 // Pin del servo motore 2 (aggiornato)

Servo myservo1, myservo2;
float timeOut = MAX_DISTANCE * 58.8; 
int soundVelocity = 340; // Velocità del suono = 340m/s
bool servo1Activated = false; // Variabile per tenere traccia se il servo 1 si è già mosso
bool servo2Activated = false; // Variabile per tenere traccia se il servo 2 si è già mosso

RTC_DS3231 rtc; // Crea un oggetto RTC
int ora1=0;
int ora2=0;
int minuti1=0;
int minuti2=0;
String orario_assunzione="";
int pillole_rimanenti_scompartimento1 = 1;
int pillole_rimanenti_scompartimento2 = 1;
bool notificainviata1=false;
bool notificainviata2=false;
bool nonAssunto1=false;
bool nonAssunto2=false;
bool Assunto1=true;
bool Assunto2=true;
const unsigned long periodoGiorno = 86400;
DateTime ultimoInvio;
void setup() {
//SETUP SERVO
 pinMode(trigPin, OUTPUT); // Imposta trigPin come output
  pinMode(echoPin, INPUT); // Imposta echoPin come input
  Serial.begin(921600); // Avvia la comunicazione seriale a 115200 baud
  myservo1.setPeriodHertz(50); // Servo standard a 50 hz
  myservo1.attach(servoPin1, 500, 2500); // Collega il servo 1
  myservo2.setPeriodHertz(50); // Servo standard a 50 hz
  myservo2.attach(servoPin2, 500, 2500); // Collega il servo 2
  
  // Inizializza il modulo RTC
  if (!rtc.begin()) {
    Serial.println("Impossibile trovare il modulo RTC");
    while (1);
  }

  // Se il modulo RTC ha perso l'ora, imposta nuovamente l'ora
  if (rtc.lostPower()) {
    Serial.println("RTC ha perso l'ora, impostare nuovamente l'ora!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }


//SETUP CONNESSIONE

  Serial.begin(921600);
  
  // Connessione al WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connessione al WiFi in corso");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnesso al WiFi");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Timer impostato a 1 minuto (variabile timerDelay)");
  dht.begin();
}

void loop() {
  // Invia una richiesta HTTP POST ogni minuto
  if ((millis() - lastTime) > timerDelay) {
    // Controlla lo stato della connessione WiFi
    if(WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      
      // Inizia la connessione e invia la richiesta HTTP
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      // Invia la richiesta POST (senza dati perché il server restituirà l'orario per entrambi gli scompartimenti)
      int httpResponseCode = http.POST("");
      
      if (httpResponseCode == 200) {
        String response = http.getString();
        Serial.println(response);
        
        // Alloca il buffer JSON
        DynamicJsonDocument doc(1024);
        
        // Deserializza la risposta JSON
        DeserializationError error = deserializeJson(doc, response);
        
        if (!error) {
  // Estrai gli orari per entrambi gli scompartimenti
  const char* orario_scompartimento1 = doc[0]["orario_impostato"];
  const char* orario_scompartimento2 = doc[1]["orario_impostato"];
  
  // Aggiungi qui il codice per estrarre il numero di pillole rimanenti
   pillole_rimanenti_scompartimento1 = doc[0]["numero_pillole_rimanenti"].as<int>();
   pillole_rimanenti_scompartimento2 = doc[1]["numero_pillole_rimanenti"].as<int>();

  // Dividi l'orario in ore e minuti per lo scompartimento 1
  String orario1 = String(orario_scompartimento1);
  ora1 = orario1.substring(0, orario1.indexOf(':')).toInt();
  minuti1 = orario1.substring(orario1.indexOf(':') + 1).toInt();
  
  // Dividi l'orario in ore e minuti per lo scompartimento 2
  String orario2 = String(orario_scompartimento2);
  ora2 = orario2.substring(0, orario2.indexOf(':')).toInt();
  minuti2 = orario2.substring(orario2.indexOf(':') + 1).toInt();
  
  // Stampa ore e minuti per entrambi gli scompartimenti
  Serial.print("Scompartimento 1 - Ora: ");
  Serial.print(ora1);
  Serial.print(", Minuti: ");
  Serial.println(minuti1);
  Serial.print("Scompartimento 1 - Pillole rimanenti: ");
  Serial.println(pillole_rimanenti_scompartimento1);
  
  Serial.print("Scompartimento 2 - Ora: ");
  Serial.print(ora2);
  Serial.print(", Minuti: ");
  Serial.println(minuti2);
  Serial.print("Scompartimento 2 - Pillole rimanenti: ");
  Serial.println(pillole_rimanenti_scompartimento2);
  
} else {
  Serial.print("Errore nella deserializzazione del JSON: ");
  Serial.println(error.c_str());
}
      } else {
        Serial.print("Codice di errore HTTP: ");
        Serial.println(httpResponseCode);
      }
      
      // Libera le risorse
      http.end();
    } else {
      Serial.println("WiFi disconnesso");
    }
    lastTime = millis();
  }


  DateTime now = rtc.now(); // Ottiene l'ora corrente dal modulo RTC
  
  // Orari di attivazione per il servo 1 (scompartimento 1)
  DateTime startTime1(now.year(), now.month(), now.day(), ora1, minuti1);
  DateTime endTime1(now.year(), now.month(), now.day(), ora1, minuti1+1);

  // Orari di attivazione per il servo 2 (scompartimento 2)
  DateTime startTime2(now.year(), now.month(), now.day(), ora2, minuti2);
  DateTime endTime2(now.year(), now.month(), now.day(), ora2, minuti2+1);



  // Attiva il servo 1 se l'ora corrente è nell'intervallo desiderato e il servo non si è ancora mosso
  if(pillole_rimanenti_scompartimento1>0){

    if(pillole_rimanenti_scompartimento1==3 && notificainviata1==false){
      inviaNotificaPushover("Ti rimangono 3 pillole. Ricorda di riempire lo scompartimeto 1");
      notificainviata1=true;
    }
    
    
    if (now >= startTime1 && now < endTime1 && !servo1Activated) {
      activateServo(myservo1, &servo1Activated, 1);
       if(Assunto1 && servo1Activated){
        inviaNotificaPushover("hai assunto la pillola 1");
        Assunto1=false;
  }
     }

  }else{
    Serial.println("Scompartimento 1 vuoto!");
    inviaNotificaPushover("Sono finite le pillole del primo scompartimento");
    servo1Activated = false;
  }
  // Attiva il servo 2 se l'ora corrente è nell'intervallo desiderato e il servo non si è ancora mosso
  if(pillole_rimanenti_scompartimento2>0){
    if(pillole_rimanenti_scompartimento2==3 && notificainviata2==false){
      inviaNotificaPushover("Ti rimangono 3 pillole. Ricorda di riempire lo scompartimento 2");
      notificainviata2=true;
    }
  if (now >= startTime2 && now < endTime2 && !servo2Activated) {
    activateServo(myservo2, &servo2Activated,2);
    if(Assunto2 && servo2Activated){
        inviaNotificaPushover("hai assunto la pillola 2");
        Assunto2=false;
      }
  }

  }else{
    Serial.println("Scompartimento 2 vuoto!");
    inviaNotificaPushover("Sono finite le pillole del secondo scompartimento");
    servo2Activated = false;
  }

  delay(1000); // Ritardo tra i cicli di loop

}

// Funzione per attivare il servo
void activateServo(Servo &servo, bool *servoActivated, int scompartimento) {
   float distance = getSonar();
  Serial.printf("Distance: %f cm\n", distance);
  if (distance <= 10.00 && !*servoActivated) {
    DateTime now = rtc.now();
    orario_assunzione = String(now.hour()) + ":" + String(now.minute()); // Formatta l'orario come "HH:MM"
    Serial.println("Orario di attivazione: " + orario_assunzione);
    
    sendActivationTimeToServer(orario_assunzione, scompartimento); // Invia l'orario al server
    
    float temperature = dht.readTemperature(); // Legge la temperatura dal sensore DHT22
    sendTemperatureToServer(temperature, scompartimento); // Invia la temperatura al server

    *servoActivated = true;
    if(&servo==&myservo1){//verso antioario (servo di destra9
    for (int pos = 0; pos <= 90; pos += 1) {
      servo.write(pos);
      delay(15);
    }
    for (int pos = 90; pos >= 0; pos -= 1) {
      servo.write(pos);
      delay(15);
    }
    }else if(&servo==&myservo2){
      for (int pos = 180; pos >= 90; pos -= 1) {
      servo.write(pos);
      delay(15);
    }
    for (int pos = 90; pos <= 180; pos += 1) {
      servo.write(pos);
      delay(15);
    }
    }
  }
}

// Funzione per ottenere la distanza dal sensore sonar
float getSonar() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long pingTime = pulseIn(echoPin, HIGH, timeOut);
  float distance = (float)pingTime * soundVelocity / 2 / 10000;
  return distance;
}

void sendActivationTimeToServer(String orario_assunzione, int scompartimento) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.1.42/Medicinali/save_activation_time.php"); // Sostituisci con l'URL effettivo del tuo script PHP
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    String httpRequestData = "orario_assunzione=" + orario_assunzione + "&scompartimento=" + String(scompartimento);
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode == 200) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Errore durante l'invio dei dati: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("WiFi non connesso");
  }
}
void inviaNotificaPushover(const String& messaggio) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Inizia la connessione e invia la richiesta HTTP a Pushover
    http.begin("https://api.pushover.net/1/messages.json");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepara i dati della richiesta POST
    String httpRequestData = "token=" + String(pushoverApiToken) + "&user=" + String(pushoverUserKey) + "&message=" + messaggio;
    
    // Invia la richiesta POST
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode == 200) {
      Serial.println("Notifica inviata con successo");
    } else {
      if (httpResponseCode == -1) {
        Serial.println("Errore: connessione HTTP fallita o timeout");
      } else {
        Serial.print("Errore durante l'invio della notifica: ");
        Serial.println(httpResponseCode);
       Serial.println(http.errorToString(httpResponseCode));
      }
    }
    
    http.end(); // Chiudi la connessione
  } else {
    Serial.println("WiFi non connesso. Impossibile inviare la notifica.");
  }
}

void sendTemperatureToServer(float temperature, int scompartimento) {
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String httpRequestData = "temperature=" + String(temperature) + "&scompartimento=" + String(scompartimento);
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Errore nell'invio della temperatura, codice: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  }
}
