// Função principal responsável por identificar e tomar decisões em cruzamentos
void cruzamento() {
  leitura();  // Atualiza os valores dos sensores de linha e cor

  // Verifica o tipo de cruzamento com base nos sensores
  bool t_cruz = t_cruzamento();     // Cruzamento em T — verificar por faixas verdes
  bool cruz_dr = cruzamento_DR();  // Interseção no meio para a direita
  bool cruz_er = cruzamento_ER();  // Interseção no meio para a esquerda
  bool cruz_d = cruzamento_D();    // Curva em L para a direita
  bool cruz_e = cruzamento_E();    // Curva em L para a esquerda
  bool cruz_r = cruzamento_R();    // Cruzamento completo (+) — verificar por verdes

  // Caso o cruzamento seja em T, +, ou interseções laterais
  if (t_cruz || cruz_dr || cruz_er || cruz_r) {
    Serial.println("Cruzamento em T");
    parar();                        // Interrompe o movimento
    delay(1500);                    // Espera para estabilizar
    situacao('T');                  // Sinaliza que está em cruzamento do tipo T
    verde();                        // Verifica se há verde (decisão de caminho)
    avancar(velAvancar);            // Continua andando para frente
    delay(200);                     // Avança levemente após decisão
  }

  // Caso detecte uma curva para a direita (formato de L)
  else if (cruz_d) {
    Serial.println("Cruzamento Direita");
    situacao('k');                  // Sinaliza cruzamento à direita
    avancar(velAvancar);            // Avança para se posicionar para a curva
    delay(250);                     // Avanço extra para alinhar melhor antes da curva
    parar();
    curvaDir(velCurva);             // Inicia a curva para a direita
    delay(500);                     // Duração da curva

    // Se o sensor da frente não detecta a linha preta, continua a curva até alinhar
    if (leituraSfe < Preto + 50) {
      while (leituraSfe < 800) {
        curvaDir(velCurva);
        leitura();                  // Atualiza leitura durante a curva
      }
    }

    parar();
    avancar(velAvancar);            // Avança para centralizar na linha
    delay(170);
    parar();
  }

  // Caso detecte uma curva para a esquerda (formato de L invertido)
  else if (cruz_e) {
    Serial.println("Cruzamento Esquerda");
    situacao('i');                  // Sinaliza cruzamento à esquerda
    avancar(velAvancar);            // Avança antes da curva
    delay(250);
    parar();
    curvaEsq(velCurva);             // Inicia curva à esquerda
    delay(500);                     // Duração da curva

    // Se o sensor da frente não detecta a linha, continua curvando até alinhar
    if (leituraSfd < Preto + 50) {
      while (leituraSfd < Preto) {
        curvaEsq(velCurva);
        leitura();                  // Atualiza leitura durante a curva
      }
    }

    parar();
    avancar(velAvancar);            // Avança para realinhar o robô na linha
    delay(180);
    parar();
  }
}

// Função que detecta interseção à direita no meio (tipo "DR")
bool cruzamento_DR() {
  // Sensor S1 vê preto, os outros dois sensores laterais veem branco,
  // e pelo menos um sensor da frente detecta linha
  if (leituraS1 < Preto && leituraS3 > Preto && leituraS4 > Preto &&
      (leituraSfd > Preto - 50 || leituraSfe > Preto - 50)) return true;
  return false;
}

// Função que detecta interseção à esquerda no meio (tipo "ER")
bool cruzamento_ER() {
  if (leituraS1 > Preto && leituraS2 > Preto && leituraS4 < Preto &&
      (leituraSfd > Preto - 50 || leituraSfe > Preto - 50)) return true;
  return false;
}

// Função que detecta uma curva simples para a direita
bool cruzamento_D() {
  if (leituraS1 < Preto && leituraS3 > Preto && leituraS4 > Preto &&
      (leituraSfd < Preto - 40 && leituraSfe < Preto - 40)) return true;
  return false;
}

// Função que detecta uma curva simples para a esquerda
bool cruzamento_E() {
  if (leituraS1 > Preto + 100 && leituraS2 > Preto + 100 && leituraS4 < Preto - 100 &&
      (leituraSfd < Preto - 100 && leituraSfe < Preto - 100)) return true;
  return false;
}

// Função que detecta cruzamento completo (tipo "+")
bool cruzamento_R() {
  if (leituraS1 > Preto && leituraS2 > Preto && leituraS3 > Preto && leituraS4 > Preto &&
      (leituraSfd > Preto - 50 || leituraSfe > Preto - 50)) return true;
  return false;
}

// Função que detecta cruzamento em formato de T
bool t_cruzamento() {
  if (leituraS2 > Preto - 100 && leituraS3 > Preto - 100 &&
      leituraSfd < Preto && leituraSfe < Preto &&
      leituraS1 > Preto - 100 && leituraS4 > Preto - 100) return true;
  return false;
}
