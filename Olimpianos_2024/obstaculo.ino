int tempoEsperado = 0;              // Variável para controle futuro de tempo (não usada diretamente neste trecho)
bool temObstaculoEm90 = false;      // Flag que indica se o obstáculo está em uma curva de 90°

int velObstaculo = 175;             // Velocidade base usada durante desvios
int velObstaculoCurva = 185;        // Velocidade base usada durante curvas em desvios

// Função principal que executa o desvio de obstáculo
bool obstaculo(int lado, bool curvaEm90) {
  if (inclinacao() < -70) return;         // Cancela a ação se a inclinação estiver muito acentuada (ex: robô tombado)

  temObstaculoEm90 = curvaEm90;           // Salva se o obstáculo está em uma curva de 90°
  situacao('O');                          // Indica estado de "Obstáculo"
  parar();                                // Para o robô
  delay(100);

  voltar(velObstaculo);                   // Recuar um pouco
  delay(300);
  parar();
  delay(100);

  // Chama a rotina de desvio de acordo com o lado informado
  if (lado == 'D') desvioD();
  else if (lado == 'E') desvioE();
}

// Rotina de desvio para o lado direito
void desvioD() {
  curvaDir(velObstaculoCurva + 5);  // Faz curva para a direita
  delay(1100);
  parar();
  delay(100);

  avancar(velObstaculo);            // Avança em linha reta após a curva
  delay(1100);

  curvaEsq(velObstaculoCurva + 10); // Faz curva para a esquerda para contornar o obstáculo
  delay(1100);
  parar();

  avancar(velObstaculo);            // Avança novamente

  // Se o obstáculo estava em uma curva de 90°, aciona lógica especial
  if (temObstaculoEm90) {
    while (leituraSfd < 700 || leituraSfe < 700) {  // Procura a linha preta
      avancar(velObstaculo);
      leitura();
    }
    delay(500);
    parar();
    obstaculo90('D');               // Executa a correção final para retornar à linha
    return;
  }

  delay(2450);                      // Tempo para avançar até sair do desvio
  parar();

  curvaEsq(velObstaculoCurva);      // Realinha para a esquerda
  delay(1100);
  parar();

  avancar(velObstaculo);            // Avança e começa a procurar a linha
  delay(200);
  leitura();

  while (leituraSfd < 700 || leituraSfe < 700) {  // Busca pela linha preta
    leitura();
    avancar(velObstaculo - 10);
  }

  delay(500);
  parar();
  leitura();

  while (leituraSfe < 850) {        // Ajuste fino de alinhamento na linha
    leitura();
    curvaDir(velObstaculoCurva - 10);
  }

  delay(100);
  parar();
}

// Rotina de desvio para o lado esquerdo
void desvioE() {
  curvaEsq(velObstaculoCurva);      // Curva para a esquerda
  delay(1080);
  parar();
  delay(100);

  avancar(velObstaculo);            // Avança para contornar o obstáculo
  delay(1300);
  parar();
  delay(100);

  curvaDir(velObstaculoCurva);      // Curva para a direita para alinhar com o trajeto
  delay(980);
  parar();

  avancar(velObstaculo);            // Continua avançando

  if (temObstaculoEm90) {           // Se era obstáculo em curva de 90°
    while (leituraSfd < 700 || leituraSfe < 700) {
      avancar(velObstaculo);
      leitura();
    }
    delay(500);
    parar();
    obstaculo90('E');               // Corrige com curva específica
    return;
  }

  delay(2300);                      // Tempo para sair do desvio
  parar();

  curvaDir(velObstaculoCurva);      // Realinha com o trajeto original
  delay(970);
  parar();

  leitura();

  while (leituraSfd < 800 || leituraSfe < 800) {  // Procura linha preta
    leitura();
    avancar(velObstaculo);
  }

  delay(400);
  parar();
  leitura();

  while (leituraSfd < 800 || leituraSfe < 800) {  // Ajuste fino de alinhamento
    leitura();
    curvaEsq(velObstaculoCurva);
  }

  delay(100);
  parar();
  situacao('L');                    // Volta ao estado de "Linha"
  delay(100);
  desligaLeds();                    // Desliga os LEDs (possivelmente indicadores)
  delay(100);
}

// Corrige o robô após desvio em curva de 90°
void obstaculo90(char lado) {
  parar();
  delay(100);

  switch (lado) {
    case 'D':                       // Caso o desvio tenha sido para a direita
      curvaDir(velObstaculoCurva);
      delay(1500);
      parar();
      break;

    case 'E':                       // Caso o desvio tenha sido para a esquerda
      curvaEsq(velObstaculoCurva);
      delay(1500);
      parar();
      break;
  }
}
