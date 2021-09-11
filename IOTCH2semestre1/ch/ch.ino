#include <ArduinoJson.h>
const int TAMANHO = 200;
String recebi;

int sensorValue; //vermelho
int sensorValue2; //infra
int conts=0;
float spo2;//variavel para calcular o nivel em um dado tempo
float spo2Perc;//percentagem de oxigenio
float val = 0;
float oxi;
int OxiPin1= A1;
int OxiPin2= A2;

volatile unsigned long cont = 0;
unsigned long tempo = 0;
long VeloAngu = 0;
long bpm;
long BatiPin = 2;
long bati;

int TermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tf, Tc, temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //valores constante para calculo

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10);
  pinMode(TermistorPin, INPUT);

  pinMode(OxiPin1, INPUT);
  pinMode(OxiPin2, INPUT);

  pinMode(BatiPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(BatiPin), interrupcao, RISING);
}

void loop() {
  if (Serial.available() > 0) {
      recebi = Serial.readString();

      if (recebi == "ligaTemp"){
		temp = readTemp(TermistorPin);
	  }

	  if (recebi == "ligaFreq"){
		bati = readBati(BatiPin);
	  }

	  if (recebi == "ligaOxi"){
		  oxi = readOxi( OxiPin1, OxiPin2);
	  }

	 
  }

  // formato de leitura do node-red
  StaticJsonDocument <TAMANHO> json;
  json["topic"] = "Sensor";
  json["temp1"] = Tc;
  json["freq"] = bpm;
  json["oxi"] = val;
  serializeJson( json, Serial);
  Serial.println();

}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float readTemp(int ThermistorPin){
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculo R2, demonstração no arquivo pdf da aula
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// Equação de Steinhart–Hart 
  Tc = T - 273.15;
  return Tc;
}
//função que faz leitura dos pinos de luminosidade e retorna o valor do Oximetro em %
float readOxi( int OxiPin1, int OxiPin2){

  sensorValue = analogRead(OxiPin1); //Vermelho
  sensorValue = - sensorValue+900; //sensorValue = sensorValue; //programa inicial
  
  sensorValue2 = analogRead(OxiPin2); //Infra - Vermelho
  sensorValue2 = - sensorValue2+700; //sensorValue2 = sensorValue2; //programa inicial

  float oxi1 = 37+(((float)sensorValue2)/((float)sensorValue + (float)sensorValue2))*100; //calculo de oximetria (37 valor para ajuste)
  
  conts = 1;
  spo2 = oxi1+spo2;
  spo2Perc=spo2/conts;
  spo2=0;
   
  val = (int)spo2Perc;
  return val;

}

//função que é responsavel por calcular os Batimentos do Usuario
long readBati ( long BatiPin){
    if( (millis() - tempo) > 999){
    tempo = millis();

    VeloAngu = 2 * 3.14 * cont;
	bpm = VeloAngu * 9.55;
    cont = 0;
  }
}

void interrupcao(){
  cont++;
}