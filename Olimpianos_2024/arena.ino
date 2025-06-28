int velArenaAvancar = 180;   // Velocidade para andar em linha reta na arena
int velArenaCurva = 200;     // Velocidade para realizar curvas na arena

// Função principal para lógica de entrada e navegação na arena
void arena(char saida) {
  leitura(); // Atualiza as leituras dos sensores de linha

  // Verifica se todos os sensores estão dentro de faixas específicas que indicam que o robô está dentro da arena
  if ((leituraS1 > 450 && leituraS1 < 630) && (leituraS2 > 400 && leituraS2 < 600) && 
      (leituraS3 > 250 && leituraS3 < 450) && (leituraS4 > 350 && leituraS4 < 550) &&
      (leituraSfe < 500 && leituraSfd < 500)) {
    
    Serial.println('A');        // Indica que está na Arena
    avancar(velAvancar);        // Move o robô para frente
    delay(100);                 // Aguarda um tempo curto
    parar();                    // Para o robô
    situacao('A');              // Atualiza a situação como "Arena"

    // Decide a direção que o robô deve seguir com base no parâmetro 'saida'
    if (saida == 'D') arenaDireita('C');
    else if (saida == 'E') arenaEsquerda('P');
    else if (saida == 'C') arenaCentro('D');
    else if (saida == 'R') arenaReta('E');
  }
}

// Função para movimentação pela arena na direção da direita
void arenaDireita(char direcao) {
  avancar(velArenaAvancar);
  delay(2000);
  parar();

  curvaDir(velArenaAvancar); // Realiza curva à direita
  delay(1000);
  parar();

  voltar(velArenaAvancar); // Dá ré
  delay(3000);
  parar();

  if (direcao == 'P') {
    while (ultrassonico_cm('F') > 6) { // Enquanto não está perto do obstáculo da frente
      if (leituraS2 > Preto && leituraS3 > Preto) {
        delay(600);
        return;
      }
      avancar(velArenaAvancar);
    }
    parar();

    if (ultrassonico_cm('E') > 15) { // Se há espaço à esquerda, curva para direita
      curvaDir(velArenaCurva);
      delay(1000);
    }

    while (true) {
      if (leituraS2 > Preto && leituraS3 > Preto) break;
      leitura();
      situacao('R');
      avancar(velArenaAvancar);
    }
    delay(500);

  } else if (direcao == 'C') {
    avancar(velArenaAvancar);
    delay(1000);
    parar();

    curvaEsq(velArenaCurva); // Curva à esquerda
    delay(800);
    parar();

    avancar(velArenaAvancar);
    delay(3000);

    curvaDir(velArenaCurva); // Curva à direita
    delay(700);
    parar();

    if (ultrassonico_cm('F') > 60) {
      arenaReta(' ');
      return;
    }

    while (ultrassonico_cm('E') < 15) { // Segue enquanto estiver próximo da parede esquerda
      avancar(velArenaAvancar);
    }
    parar();

    while (ultrassonico_cm('E') >= 15) { // Quando sair da parede, continua
      avancar(velArenaAvancar);
    }
    parar();

    voltar(velArenaAvancar); // Dá ré
    delay(900);
    parar();

    curvaEsq(velArenaCurva); // Curva à esquerda para alinhar
    delay(1200);
    parar();

    while (true) { // Até encontrar linha novamente
      if (leituraS2 > Preto && leituraS3 > Preto) break;
      leitura();
      situacao('R');
      avancar(velArenaAvancar);
    }
    delay(500);
  }
}

// Função para movimentação pela arena na direção da esquerda
void arenaEsquerda(char direcao) {
  avancar(velArenaAvancar);
  delay(2000);
  parar();

  curvaEsq(velArenaAvancar); // Curva à esquerda
  delay(1600);
  parar();

  voltar(velArenaAvancar + 50); // Dá ré com velocidade maior
  delay(3000);
  parar();

  while (ultrassonico_cm('F') > 7) {
    if (leituraS2 > Preto && leituraS3 > Preto) {
      delay(300);
      return;
    }
    avancar(velArenaAvancar);
  }
  parar();

  if (direcao == 'P') {
    if (ultrassonico_cm('E') > 15) {
      curvaEsq(velArenaCurva);
      delay(1350);
    }
    avancar(velArenaAvancar);
    delay(1500);

  } else if (direcao == 'C') {
    curvaDir(velArenaCurva);
    delay(2800);
    
    voltar(velArenaAvancar);
    delay(3000);

    avancar(velArenaAvancar);
    delay(1000);
    parar();

    curvaEsq(velArenaCurva);
    delay(800);
    parar();

    avancar(velArenaAvancar);
    delay(4500);

    curvaDir(velArenaCurva);
    delay(700);
    parar();

    if (ultrassonico_cm('F') > 60) {
      arenaReta(' ');
      return;
    }

    while (ultrassonico_cm('E') < 15) {
      avancar(velArenaAvancar);
    }
    parar();

    while (ultrassonico_cm('E') >= 15) {
      avancar(velArenaAvancar);
    }
    parar();

    voltar(velArenaAvancar);
    delay(900);
    parar();

    curvaEsq(velArenaCurva);
    delay(1200);
    parar();

    while (true) {
      if (leituraS2 > Preto && leituraS3 > Preto) break;
      leitura();
      situacao('R');
      avancar(velArenaAvancar);
    }
    delay(1000);
  }
}

// Função para movimentação pela arena na direção central
void arenaCentro(char direcao) {
  avancar(velArenaAvancar);
  while (ultrassonico_cm('F') > 7); // Vai até se aproximar de um obstáculo
  parar();

  curvaDir(velArenaCurva);
  delay(2000);
  parar();

  voltar(velArenaAvancar);
  delay(3000);
  parar();

  avancar(velArenaAvancar);
  delay(1200);
  parar();

  // Faz curva baseada na direção desejada (esquerda ou direita)
  direcao == 'E' ? curvaEsq(velArenaCurva) : curvaDir(velArenaCurva);
  direcao == 'E' ? delay(1150) : delay(1000);
  parar();

  while (ultrassonico_cm('F') > 7) {
    if (leituraS2 > Preto && leituraS3 > Preto) {
      delay(500);
      return;
    }
    avancar(velArenaAvancar);
  }
  parar();

  direcao == 'E' ? curvaEsq(velArenaCurva) : curvaDir(velArenaCurva);
  direcao == 'E' ? delay(1050) : delay(1000);

  while (true) {
    if (leituraS2 > Preto && leituraS3 > Preto) break;
    leitura();
    situacao('R');
    avancar(velArenaAvancar);
  }
  delay(500);
  return;  
}

// Função para seguir em linha reta na arena
void arenaReta(char direcao) {
  while (ultrassonico_cm('F') > 7) {
    if (leituraS2 > Preto && leituraS3 > Preto) {
      delay(500);
      return;
    }
    avancar(velArenaAvancar);
  }
  // Após seguir em frente, curva para esquerda ou direita dependendo da direção
  direcao == 'E' ?  curvaEsq(velArenaCurva) : curvaDir(velArenaCurva);
  direcao == 'E' ?  delay(1100) : delay(1000);
}
