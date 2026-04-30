#include <Arduino.h>
#include "GamecubeReader.h" // Usando a NOSSA biblioteca, e não a do NicoHood!

// Definindo o pino onde o fio DAT (Vermelho) está ligado
#define pinLed LED_BUILTIN
#define GC_DAT_PIN 4

// Cria o objeto do controle usando a nossa classe
GamecubeReader GamecubeController(GC_DAT_PIN);

// Declaração da função de print para o compilador achar
void print_gc_report(Gamecube_Report_t &gc_report);

void setup(){
  // Configura o LED da placa
  pinMode(pinLed, OUTPUT);

  // Inicia o Serial
  Serial.begin(115200);

  // Inicia o pino do controle
  GamecubeController.begin();

  Serial.println(F("Pressione qualquer tecla no Serial Monitor para comecar."));
  while (Serial.available() == 0); // Espera o usuário digitar algo no Serial

  Serial.println(F("Iniciando leitura do controle Gamecube no ESP32-S3..."));
  Serial.println();
}

void loop(){
  // Tenta ler os dados do controle (o nosso update() substitui o read() do NicoHood)
  if (GamecubeController.update())
  {
    // Se leu com sucesso, pega o relatório e imprime
    print_gc_report(GamecubeController.report);
    delay(100);

    // Nota: O Rumble foi omitido aqui porque não ligamos o circuito de 5V ainda!
  }
  else
  {
    // Se falhar (fio solto ou controle desconectado)
    Serial.println(F("Erro ao ler o controle de Gamecube."));
    digitalWrite(pinLed, HIGH);
    delay(1000);
  }
  digitalWrite(pinLed, LOW);
}

// Função para imprimir os dados, adaptada para os nomes da nossa struct
void print_gc_report(Gamecube_Report_t &gc_report){
  Serial.println();
  Serial.println(F("--- Relatorio do Controle Gamecube ---"));
  
  Serial.print(F("Start:\t")); Serial.println(gc_report.start);
  Serial.print(F("Y:\t")); Serial.println(gc_report.y);
  Serial.print(F("X:\t")); Serial.println(gc_report.x);
  Serial.print(F("B:\t")); Serial.println(gc_report.b);
  Serial.print(F("A:\t")); Serial.println(gc_report.a);
  
  Serial.print(F("L btn:\t")); Serial.println(gc_report.l);
  Serial.print(F("R btn:\t")); Serial.println(gc_report.r);
  Serial.print(F("Z:\t")); Serial.println(gc_report.z);

  Serial.print(F("Dup:\t")); Serial.println(gc_report.dUp);
  Serial.print(F("Ddown:\t")); Serial.println(gc_report.dDown);
  Serial.print(F("Dright:\t")); Serial.println(gc_report.dRight);
  Serial.print(F("Dleft:\t")); Serial.println(gc_report.dLeft);

  Serial.print(F("xAxis:\t")); Serial.println(gc_report.xAxis, DEC);
  Serial.print(F("yAxis:\t")); Serial.println(gc_report.yAxis, DEC);

  Serial.print(F("cxAxis:\t")); Serial.println(gc_report.cxAxis, DEC);
  Serial.print(F("cyAxis:\t")); Serial.println(gc_report.cyAxis, DEC);

  Serial.print(F("L gat:\t")); Serial.println(gc_report.leftAnalog, DEC);
  Serial.print(F("R gat:\t")); Serial.println(gc_report.rightAnalog, DEC);
  Serial.println(F("--------------------------------------"));
}