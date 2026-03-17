/*
=========================================================================
Code in WIP aprox. 85%
preciso melhorar/add:
nome de funções
otimização
menu
animação vitoria
boss
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <consts_bitmaps.h>
#include <objects.h>

int fase = 0,energy = 100,
totalHP =3, qntLoop=0, frame=0;
bool tester=false;

int points = 0, shoots=30;
bool animationOnOff = true;
 
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool winCondition(int pts){
  return (points > pts);
  }

bool loseCondition(){
  return (energy <= 0 || totalHP <= 0);
  }

int pontosRandom(){
  if(tester) return 500;
  else if(itens.dbPoints.isOn)return random(80, 130);
  else return random(40, 65);
  }
int spawnY(byte sizeC) {
  return random(0, LIMIT_Y - sizeC);
  }

int spawnX() {
  return random(MIN_COM_RESP, MAX_COM_RESP); 
  }

int filtrarADC(int pin) {
  int soma = 0;
  for (int i = 0; i < 10; i++) {
    soma += analogRead(pin);
  }
  int media = soma / 10; 
  return media;
  }


void comeco(){ 
  timer.global.reset();
  while(!butao.press() ){
    if (timer.global.tempo(500)){
      oled.clearDisplay();
      oled.drawBitmap(0,0, start, 128,64, SSD1306_WHITE);
      qntLoop++;
      if (qntLoop >= 4){
        oled.setCursor(20, 57);
        oled.print("pressione tiro");
        qntLoop=0;
      }
      oled.display();
     }
   }
   qntLoop=0;
  }


void generateNave(){  
  int leituraX = filtrarADC(JX) - centroX;
  int leituraY = filtrarADC(JY) - centroY;

  if(abs(leituraX) < DEADZONE) leituraX = 0;
  if(abs(leituraY) < DEADZONE) leituraY = 0;

  nave1.x -= leituraX / 700;
  nave1.y += leituraY / 700;

  if(nave1.x > 120)nave1.x = 120;
  if(nave1.x < -4) nave1.x = -4;
  if(nave1.y < -4) nave1.y = -4;
  if(nave1.y > 48) nave1.y = 48;
  }

void mostraNave(){
  oled.drawBitmap(nave1.x, nave1.y, nave, 8, 8, SSD1306_WHITE);
  }

void disparo(){
  if (butao.press() && shoots > 0){
    sound.shoot.canPlay = true;
    timer.shootSound.reset();
    for(int a =0;a<MAX_TIROS;a++){
      if (!shoot[a].hasShoot){
        shoot[a].x = nave1.x + 6;
        shoot[a].y = nave1.y;
        shoot[a].hasShoot = true;
        shoots-=1;
        break;
       }
      }
    }
  }

void comets(byte qtdCom, byte velMin, byte velMax, Comet comets[]){
    for (int i = 0; i < qtdCom; i++) {
    if (comets[i].x == SPAWNXY && comets[i].y == SPAWNXY){
    comets[i].x = spawnX();
    comets[i].y = spawnY(comets[i].size);
    comets[i].vel = random(velMin, velMax+1); 
    }    
    comets[i].x -= comets[i].vel; 

    if (comets[i].x < -comets[i].size) { 
      comets[i].x = spawnX();
      comets[i].y = spawnY(comets[i].size);
      comets[i].vel  = random(velMin, velMax+1);
      int a = random(0, 1001);
      if (a >= 1000){
        comets[i].vel = velMax + 5;
      }
    } 
  }
  }

void drawButton(int x, int y, int cursorX, int cursorY, const char* msg, int menuX, int menuY){
  oled.drawRoundRect(x, y, 51, 15, 3, SSD1306_WHITE);
  oled.setCursor(cursorX, cursorY);
  oled.print(msg);
  if(menuPos.x == menuX && menuPos.y == menuY){
  oled.drawRoundRect(x-2, y-2, 55, 19, 3, SSD1306_WHITE);
  }
  }

void getPosMenu(){
  int X = filtrarADC(JX) - centroX;
  int Y = filtrarADC(JY) - centroY;

  if(abs(X) < DEADZONE+400) X = 0;
  if(abs(Y) < DEADZONE+400) Y = 0;

  // é contraditorio eu sei
  if(X > 0){menuPos.y--; delay(200);} 
  if(X < 0){menuPos.y++; delay(200);} 
  if(Y > 0){menuPos.x++; delay(200);} 
  if(Y < 0){menuPos.x--; delay(200);} 

  menuPos.attPos();
  }

void menu(){
  comeco();
  delay(300);
  int x = 5;
  int y = 5;
  while(!butao.press()){
  oled.clearDisplay();
  getPosMenu();
  drawButton(x,y,17,9,"EASY",0,0);
  drawButton(x,y+20,13,29,"MEDIUM",1,0);
  drawButton(x,y+40,17,49,"HARD",2,0);

  drawButton(x+60,y,67,9,"TUTORIAL",0,1);
  drawButton(x+60,y+20,77,29,"INFO",1,1);
  drawButton(x+60,y+40,72,49,"DEVMOD",2,1);

    Serial.print("X");
    Serial.println(filtrarADC(JX));
    Serial.print("Y");
    Serial.println(filtrarADC(JY));

  oled.display();
  } 
  if(menuPos.x == 0 && menuPos.y == 0) estado = EASY;
  if(menuPos.x == 1 && menuPos.y == 0) estado = MEDIUM;
  if(menuPos.x == 2 && menuPos.y == 0) estado = HARD;
  if(menuPos.x == 0 && menuPos.y == 1) estado = TUTORIAL; // pra nao dar erro por enquanto
  if(menuPos.x == 1 && menuPos.y == 1) estado = INFO;
  if(menuPos.x == 2 && menuPos.y == 1) {
    tester=true;
    comeco();
    delay(100);
  }
  }
void spawn(Itens &powerUp, byte spawnrate){
      if (!powerUp.hasSpawned && random(0, 100) > 100 - spawnrate) {
      powerUp.y = spawnY(8);
      powerUp.x = 128; 
      powerUp.hasSpawned = true;
    }
  }
void move(Itens &powerUp){
    if (powerUp.hasSpawned) {  
    powerUp.x -= powerUp.vel;
    if (powerUp.x <= -8) {
      powerUp.hasSpawned = false;
      powerUp.x = 200;
      }
    }
  }
  void render(Itens &powerUp, const uint8_t* bitmap){
    if (powerUp.hasSpawned) {
    oled.drawBitmap(powerUp.x, powerUp.y, bitmap, 8, 8, SSD1306_WHITE);
    }
  }

void colisaoComet(CStats &ct, Comet c[], byte tam, const uint8_t* bitmap, byte hp){
    for(int i=0;i<MAX_TIROS;i++){
    if(shoot[i].hasShoot){

      for(int a=0;a<ct.qntCom;a++){
        if((shoot[i].x +8 >= c[a].x && shoot[i].x < c[a].x +tam)
         && ((shoot[i].y < c[a].y +tam) && (shoot[i].y +8 > c[a].y)) ){ 
          shoot[i].hasShoot = false;
          c[a].hp--;
          if(c[a].hp <= 0){
            oled.drawBitmap(c[a].x, c[a].y, bitmap, tam, tam, SSD1306_WHITE);
            c[a].x = spawnX();
            c[a].y = spawnY(c[a].size);
            points += pontosRandom(); 
            sound.explosion.canPlay = true;
            timer.explosionSound.reset();
            c[a].hp = hp;
          }
          break;
        }
      }
      }
    }
    }

void colisaoNave(CStats &cs, Comet c[], byte tam){
  for (int i=0;i<cs.qntCom;i++){
    if((nave1.x+7 >= c[i].x && nave1.x <= c[i].x +tam) &&
     (c[i].y +tam-1 >= nave1.y && c[i].y <= nave1.y+7)){
      if(itens.shield.isOn == false) totalHP--;
      points += random(40, 60);
      c[i].hp--;
      c[i].x = spawnX();
      c[i].y = spawnY(c[i].size);
      break;
    }
  }
  }

void colisaoPowerup(Itens &c, int &boost, int value, byte opc){ //opc 1= energy, 2 ammo, 3 shield
  if(c.hasSpawned && (c.x <= nave1.x+8 && c.x >= nave1.x) &&
   (c.y+8 >= nave1.y  &&  c.y <= nave1.y +8) ){
    c.hasSpawned = false;
    c.x = 200;
    if(opc == 1) boost = value;
    else if(opc == 2) boost += value;
    else if(opc == 3){
      itens.shield.qnt++;
    } 
    else {
      itens.dbPoints.isOn = true;
      timer.dbPoints.reset();
    }
   }
  }
void colisao(){

  colisaoComet(cond.smallC, comet.small, comet.small[0].size, explosao, 1);
  colisaoComet(cond.bigC, comet.big, comet.big[0].size, explosaoG, 3);

  colisaoNave(cond.smallC, comet.small, comet.small[0].size);
  colisaoNave(cond.bigC, comet.big, comet.big[0].size);
  
  colisaoPowerup(itens.energy, energy, 100, 1);
  colisaoPowerup(itens.ammo, shoots, 10, 2);
  colisaoPowerup(itens.hp, totalHP, 1, 2);
  colisaoPowerup(itens.shield, shoots, 0, 3); // gambiarra
  colisaoPowerup(itens.dbPoints, shoots, 0, 4);

  }

void renderCometa(){

  for(int i=0; i<cond.smallC.qntCom;i++){
  oled.drawBitmap(comet.small[i].x, comet.small[i].y, cometa, 8, 8, SSD1306_WHITE);
  }

  for(int i=0; i<cond.bigC.qntCom;i++){
    if(comet.big[i].hp >= 3) oled.drawBitmap(comet.big[i].x, comet.big[i].y, cometaG0, 16, 16, SSD1306_WHITE);
    else if(comet.big[i].hp == 2) oled.drawBitmap(comet.big[i].x, comet.big[i].y, cometaG1, 16, 16, SSD1306_WHITE);
    else if(comet.big[i].hp <= 1) oled.drawBitmap(comet.big[i].x, comet.big[i].y, cometaG2, 16, 16, SSD1306_WHITE);
  }

  }

void renderTiro(){
  for(int i=0;i<MAX_TIROS;i++){
    if(shoot[i].hasShoot){
      oled.drawBitmap(shoot[i].x, shoot[i].y, tiro, 8, 8, SSD1306_WHITE);
      shoot[i].x += 5;
      if(shoot[i].x > 128){
        shoot[i].hasShoot = false;
         }
        }
      }
    }


void gprint(byte x, byte y, int rng){
  oled.setCursor(x, y);
  oled.print(rng);
  }
 

void painel(){
  oled.drawLine(0,55,128,55,SSD1306_WHITE);
  oled.drawBitmap(0, 57, vida, 8, 8, SSD1306_WHITE);
  gprint(9,57,totalHP);
  oled.drawBitmap(20, 57, bateria_painel, 8,8, SSD1306_WHITE);
  gprint(26,57,int(energy));
  oled.drawBitmap(41, 57, pontos, 8, 8, SSD1306_WHITE); 
  gprint(48,57,points);
  oled.drawBitmap(80, 57, ammoBitmap, 8,8, SSD1306_WHITE);
  gprint(88,57,shoots); // testes
  oled.drawBitmap(102, 57, shield,8,8, SSD1306_WHITE);
  gprint(114,57,itens.shield.qnt);
  } 

void badEnding(){
    oled.clearDisplay();
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();
    animationOnOff = true;
    delay(500); // evita skip sem querer
    timer.global.reset();

    while(!butao.press() ) {
    animationEnding(70, fim_ruim, 9); // menor = mais rapido

    if (frame >= 9){
    oled.clearDisplay();
    animationOnOff = false;
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();

    if (timer.cooldawn.tempo(2500)){
      animationOnOff = true;
    }
    }
  }
  }

void perdasTotais(){
  if(timer.energy.tempo(300) ) energy--;
  if(energy >= 100) energy = 100;
  if(timer.ammo.tempo(1500)) shoots++;
  if(shoots >= 80) shoots = 80;
  if(timer.dbPoints.tempo(10000)) itens.dbPoints.isOn = false;
  if(timer.shieldState.tempo(5000) && itens.shield.isOn){
    itens.shield.isOn = false;
    itens.shield.blinkState = false;
    itens.shield.blink = true;
  } 
    itens.shield.timeActive = millis() - timer.shieldState.tempoAnterior;
    if(itens.shield.isOn && itens.shield.timeActive > 3000) itens.shield.blinkState = true;
    else itens.shield.blinkState = false;
    if(itens.shield.blinkState && timer.shieldBlink.tempo(200)) itens.shield.blink = !itens.shield.blink;
 }

void powerUps() {
  if(usePup.press() && itens.shield.qnt > 0){
  itens.shield.isOn = true;
  timer.shieldState.reset();
  itens.shield.blinkState = false;
  itens.shield.qnt--;
  }
  if(timer.powerUp.tempo(1000)){
    spawn(itens.energy, 15);
    spawn(itens.ammo, 15);
    spawn(itens.hp, 5);
    spawn(itens.shield, 6);
    spawn(itens.dbPoints, 3);
  }

  if(energy < 20) spawn(itens.energy, 50);
  if(shoots == 0) spawn(itens.ammo, 30);
  move(itens.energy);
  move(itens.ammo);
  move(itens.hp);
  move(itens.shield);
  move(itens.dbPoints);
  }

void renderPower(){
  render(itens.ammo, ammoBitmap);
  render(itens.energy, bateria);
  render(itens.hp, vida);
  render(itens.shield, shield);
  render(itens.dbPoints, doublepts);
  if(itens.shield.blink && itens.shield.isOn){
    oled.drawBitmap(nave1.x-4, nave1.y-4, shieldEffect, 16, 16, SSD1306_WHITE);
  } 
  }

void subirFase(Fases f[], int faseN, const char* msg){
  cond.smallC.qntCom += f[faseN].sqnt;
  cond.smallC.velMax += f[faseN].svmax;
  cond.smallC.velMin += f[faseN].svmin;
  cond.bigC.qntCom += f[faseN].bqnt;
  cond.bigC.velMax += f[faseN].bvmax;
  cond.bigC.velMin += f[faseN].bvmin;

  faseM();
  mensagem_subirFase(msg);
  }

void fases_cond(Fases f[]){
  if(fase == 0){
    cond.smallC.qntCom = f[0].sqnt;
    cond.smallC.velMax = f[0].svmax;
    cond.smallC.velMin = f[0].svmin;
    cond.bigC.qntCom = f[0].bqnt;
    cond.bigC.velMax = f[0].bvmax;
    cond.bigC.velMin = f[0].bvmin;
    fase++;
  }
  else {
    if(fase < 10 && points >= f[fase].pontos){
      subirFase(f, fase, "FASE ");
      fase++;
    }
    }
    }


void faseM() {
  timer.global.reset();
  energy += 30;
  shoots += 10;

  if(cond.smallC.qntCom > MAX_COM) cond.smallC.qntCom = MAX_COM;
  if(cond.bigC.qntCom > MAX_COM) cond.bigC.qntCom = MAX_COM;

  resetComet(comet.small, cond.smallC, 1);
  resetComet(comet.big, cond.bigC, 1);

 }

void resetComet(Comet c[], CStats &cs, bool opc){
  if(opc){
    for (int i = 0; i < cs.qntCom; i++) {
      c[i].x = spawnX();
      c[i].y = spawnY(c[i].size);
      c[i].vel  = random(cs.velMin, cs.velMax +1);
  }
  }
  else {
      for(int i=0;i<MAX_COM;i++){
      c[i].x = c[i].y = SPAWNXY;
      }
    }
  }
void goodEnding() {
  oled.clearDisplay();
  oled.drawBitmap(0,0, tela_win[0], 128,64, SSD1306_WHITE);
  oled.display();
  delay(500); // evita skip sem querer
  while(!butao.press() ) {
    animationEnding(50, tela_win, 6); // menor = mais rapido

    if (frame >= 6){
    oled.clearDisplay();
    animationOnOff = false;
    oled.drawBitmap(0,0, tela_win[0], 128,64, SSD1306_WHITE);
    oled.display();

    if (timer.cooldawn.tempo(2500)){
      animationOnOff = true;
    }
    }
  }

  }

void animationEnding(int vel, const uint8_t bitmap[][1024], int f){
  if(animationOnOff){
  if(timer.animation.tempo(vel) ) {
  oled.clearDisplay();
  oled.drawBitmap(0,0, bitmap[frame], 128,64, SSD1306_WHITE);
  oled.display();
  frame++;
  if(frame > f){
    frame=0;
  }
  }    
  }
  }

void mostrar_pontucao(){
  oled.setTextSize(2);
  for(int a=0;a<points;a+=47){
    oled.clearDisplay();
    oled.drawBitmap(0,0, pontucao, 128,64, SSD1306_WHITE);
    gprint(45, 30, a);
    oled.display();
  }
  timer.global.reset();
   while(!butao.press() ) {
   if(timer.global.tempo(90000)){
    oled.setCursor(0,0);
    oled.clearDisplay();
    oled.print("pabens axoeaster egg");
    oled.display();
   }
   }
  qntLoop=0;
  totalHP=3;
  fase = 0;
  points = 0;
  frame=0;
  shoots = 30;
  energy = 100;
  itens.shield.qnt = 0;
  tester=false;

  nave1.x = 10;
  nave1.y = 34;
  resetComet(comet.small, cond.smallC, 0); // gambiarra leve nos 2
  resetComet(comet.big, cond.bigC, 0);

  oled.setTextSize(1);
  estado = MENU;
  }

void sound_shoot(){
  if(sound.shoot.canPlay){
    switch (sound.shoot.state) {
    case 1:
      tone(SPK, 1800, 30);
      sound.shoot.state++;
      break;
    case 2:
      if(timer.shootSound.tempo(35)){
        tone(SPK, 1500, 25);
        sound.shoot.state++;
      } 
      break;
    case 3:
      if(timer.shootSound.tempo(30)){
        tone(SPK, 1200, 20);
        sound.shoot.state++;
      } 
      break;  
    case 4: 
      if(timer.shootSound.tempo(25) ){
        tone(SPK, 900, 15);
        sound.shoot.reset();
        sound.shoot.canPlay = false;
      }
      break;
    }
  }
  }

void sound_explosion() {
  if(sound.explosion.canPlay) {
    switch(sound.explosion.state){
      case 1:
      tone(SPK, 850, 30);
      sound.explosion.state++;
      break;

      case 2:
      if(timer.explosionSound.tempo(35)) {
      tone(SPK, 600, 40);
      sound.explosion.state++;
      } 
      break;

      case 3:
      if(timer.explosionSound.tempo(45)) {
      tone(SPK, 750, 30);
      sound.explosion.state++;
      } 
      break;

      case 4:
      if(timer.explosionSound.tempo(35)) {
      tone(SPK, 450, 30);
      sound.explosion.state++;
      } 
      break;      

      case 5:
      if(timer.explosionSound.tempo(75)) {
      tone(SPK, 250, 120);
      sound.explosion.reset();
      sound.explosion.canPlay = false;
      } 
      break;        
    }
  }
  }

void mensagem_subirFase(const char* mensagem) {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(30,15);
  oled.print(mensagem);
  if(fase == 9) oled.print("67");
  else oled.print(fase+1);
  oled.setTextSize(1);
  oled.display();
  delay(300); // mata ninguem
  while(!(butao.press()) );
  }

void gameGenerate(){
  if(estado == EASY) fases_cond(fases.faseEz);
  if(estado == MEDIUM) fases_cond(fases.faseMid);
  if(estado == HARD) fases_cond(fases.faseHard);
  generateNave();
  disparo();
  sound_shoot();
  comets(cond.smallC.qntCom, cond.smallC.velMin, cond.smallC.velMax, comet.small);
  comets(cond.bigC.qntCom, cond.bigC.velMin, cond.bigC.velMax, comet.big);
  powerUps();
  colisao();
  sound_explosion();
  perdasTotais();
  }

void gameRender(){ 
  oled.clearDisplay();
  mostraNave();
  renderPower();
  renderCometa();
  renderTiro();
  painel();
  }

void winLoseCondition(int pts){
  if(winCondition(pts)) estado = WIN;
  if(loseCondition())  estado = LOSS;
  }

void setup() {
  Serial.begin(115200);
  Wire.begin(6,7);
  pinMode(JX, INPUT);
  pinMode(JY, INPUT);
  pinMode(SPK, OUTPUT);
  estado = MENU;

  for(int x=0;x<MAX_COM;x++){
    comet.big[x].defineHp(3);
    comet.big[x].size = 16;
    comet.small[x].defineHp(1);
    comet.small[x].size = 8;
  }

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true);
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  randomSeed(analogRead(0)); 
  }

void loop() {
  if(estado == MENU){
  menu();
  //comeco();
  //estado = MEDIUM;

  }
  else if(estado == TUTORIAL){
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0,0);
    oled.println("Cometas = ruim");
    oled.println("powerUp = bom");
    oled.println("bugs = nao");
    oled.println("usar joystick para ativar shield");
    oled.println("passa de fase matando cometa");
    oled.display();
    while(!butao.press());
    menu();
    
  }
  else if(estado == INFO){
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0,0);
    oled.print("Codigo feito inteiramente por: Ryan Acosta 1221, codigo disponivel no github");
    oled.display();
    while(!butao.press());
    menu();
  }
  else if(estado == EASY){
  gameGenerate();
  gameRender();
  winLoseCondition(6000);

  }
  else if(estado == MEDIUM){
  gameGenerate();
  gameRender();
  winLoseCondition(10000);

  }
  else if(estado == HARD){
  gameGenerate();
  gameRender();
  winLoseCondition(15000);

  }

  if(estado == LOSS){
    badEnding();
    mostrar_pontucao();
   }
  if(estado == WIN){
    goodEnding();
    mostrar_pontucao();
  }
  oled.display();
  }

