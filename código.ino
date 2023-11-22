// O protótipo foi montando com base no ESP8266 e sensor ECG AD8232
// O Código utilizado para o trabalho foi divulgado pelo How to Electronics

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
#define WIFISSID ""                                          //Nome do Wifi
#define PASSWORD ""                                          //Senha do Wifi
#define TOKEN ""                                             //Teken do Ubidots   
#define MQTT_CLIENT_NAME "myecgsensor"                      //Nome do cliente MQTT 
                                                            
 
//constantes
#define VARIABLE_LABEL "myecg" // Atribuir o rótulo da variável
#define DEVICE_LABEL "esp8266" // Atribuir o rótulo do dispositivo
 
#define SENSOR A0 // Defina o A0 como SENSOR
 
char mqttBroker[]  = "industrial.api.ubidots.com";
char payload[100];
char topic[150];
char str_sensor[10];
 
//funções auxiliares

WiFiClient ubidots;
PubSubClient client(ubidots);
 
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}
 
void reconnect() {
  // Loop até estarmos reconectados
  while (!client.connected()) {
    Serial.println("Tentando conexão MQTT ...");
    
    // Tentativa de conexão
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Conectado");
    } else {
      Serial.print("Falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 2 segundos");
      // Aguarde 2 segundos antes de tentar novamente
      delay(2000);
    }
  }
}
 
// Função Principal

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Atribuir o pino como INPUT
  pinMode(SENSOR, INPUT);
 
  Serial.println();
  Serial.print("Esperando por WiFi ...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);  
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
 
  sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(payload, "%s", ""); // Limpa a carga útil
  sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adiciona o rótulo da variável
  
  float myecg = analogRead(SENSOR); 
  
  /* 4 é a largura mínima, 2 é a precisão; valor flutuante é copiado para str_sensor*/
  dtostrf(myecg, 4, 2, str_sensor);
  
  sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
  Serial.println("Publicação de dados na nuvem Ubidots");
  client.publish(topic, payload);
  client.loop();
  delay(10);
}
