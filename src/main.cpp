#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC Address (NODEMCU)-NodoCentral
uint8_t broadcastAddress[] = {0xEC, 0xFA, 0xBC, 0x21, 0x2A, 0x8E}; //

const int analogPin = A0;   // pin analógico
const int ledPin = 5;       // pin de parpadeo en la recepción del callback
bool receivedOk = false;    // indica si se recibió un mensaje

typedef struct struct_message { // Estructura del menaje a enviar
  int valueA0;
} struct_message;

// Iniciar una variable con la estructura del mensaje
struct_message myData;

//unsigned long lastTime = 0;  
//unsigned long T = 1000;  // periodo de muestreo
uint32_t T = 8e6;  // Tiempo en modo deep-sleep en microsegundos

// Función Callback cuando la data es enviada
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Estado del último envío: ");
  if (sendStatus == 0){
    Serial.println("OK");
    receivedOk = true;
  }
  else{
    Serial.println("Envío fallido");
  }
}
 
void setup() {
  pinMode(ledPin,OUTPUT);
  
  Serial.begin(115200); // Iniciar comunicación serial
  WiFi.mode(WIFI_STA);  // Iniciar Wi-Fi en modo estación
  WiFi.disconnect();
  
  // Iniciar ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error inicializando ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); // Asignar rol de controlador  
  esp_now_register_send_cb(OnDataSent); // Asignar función callback
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0); // Registrar receptor (par)

  myData.valueA0 = analogRead(analogPin); // Setear valores del mensaje a enviar
  //myData.id = BOARD_ID;
  Serial.println();
  Serial.print("A0: ");
  Serial.println(myData.valueA0);
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // Enviar mensaje via ESP-NOW
}
 
void loop() {
  //if ((millis() - lastTime) > T) { 

    //simulación opcional de la lectura analógica
    /*if(analog>=850){
      incr = -5;  
    }
    if(analog<=250){
      incr = 5;  
    }
    analog = analog + incr;
    */
    

    //lastTime = millis();
  //}
  
  if(receivedOk){
    digitalWrite(ledPin,HIGH);
    delay(100);
    //digitalWrite(ledPin,LOW);
    //receivedOk = false;

    ESP.deepSleep(T, WAKE_RF_DEFAULT); // Calibración de señal de radio si es necesario 
    }
}