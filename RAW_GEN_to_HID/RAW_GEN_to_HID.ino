const int LED[2] = {7, 8};
const int botao[2] = {2, 3};
#define QUANTIDADE_BOTOES 2

bool nivel_passado_botao[QUANTIDADE_BOTOES] = {0};
bool nivel_antes_debounce_botao[QUANTIDADE_BOTOES] = {0};
bool leitura_atual[QUANTIDADE_BOTOES] = {0};
unsigned long ultima_mudanca_botao_entrada[QUANTIDADE_BOTOES] = {0};
int debounce_Delay[QUANTIDADE_BOTOES] = {0};
int contagem[2] = {0, 0};

void setup() {
  for (int i = 0; i < QUANTIDADE_BOTOES; i++){
    nivel_passado_botao[i] = HIGH;
    nivel_antes_debounce_botao[i] = HIGH;
    leitura_atual[i] = HIGH;
    ultima_mudanca_botao_entrada[i] = 0;
    debounce_Delay[i] = 100;

    pinMode(botao[i], INPUT_PULLUP);
    pinMode(LED[i], OUTPUT);
  }
  Serial.begin(9600);
}

void loop() {
  for(int i = 0; i < QUANTIDADE_BOTOES; i++) {
    leitura_atual[i] = digitalRead(botao[i]);
    if (leitura_atual[i] != nivel_antes_debounce_botao[i]) {
      ultima_mudanca_botao_entrada[i] = millis();
    }
    nivel_antes_debounce_botao[i] = leitura_atual[i];
    if ((millis() - ultima_mudanca_botao_entrada[i]) > debounce_Delay[i]) {
      if (leitura_atual[i] != nivel_passado_botao[i]) {
        nivel_passado_botao[i] = leitura_atual[i];
        if (nivel_passado_botao[i] == LOW){
          contagem[i]++;
          Serial.println(String("Botão: ") + botao[i] + " Contagem: " + contagem[i]);
            if (contagem[i] >= 4) {
              digitalWrite(LED[i], !digitalRead(LED[i]));
              contagem[i] = 0;
            }

        }
      }
    }
  }
}
