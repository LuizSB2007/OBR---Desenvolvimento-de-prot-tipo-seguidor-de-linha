//Função responsável por realizar as leituras dos sensores de cor e luz
void leitura() {
  leituraSfd = analogRead(sfd);
  leituraSfe = analogRead(sfe);
  leituraS1 = analogRead(s1);
  leituraS2 = analogRead(s2);
  leituraS3 = analogRead(s3);
  leituraS4 = analogRead(s4);
}

//Função usada para mostrar as leituras de todos os sensores
void mostraLeituras() {
  Serial.print("Leitura refletancia: ");
  Serial.println(String(leituraS1) + " " + String(leituraS2) + " " + String(leituraSf) + " " + String(leituraS3) + " " + String(leituraS4));
  Serial.print("Leitura cor: ");
  //Serial.println(String(leituraCD) + " " + String(leituraCE));
  Serial.print("Leitura ultra: ");
  Serial.println(ultrassonico_cm('F'));
  Serial.println("-----------------------------\n");
  delay(500);
}