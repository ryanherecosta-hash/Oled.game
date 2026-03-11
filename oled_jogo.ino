#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <consts_bitmaps.h>
#include <objects.h>

int fase = 1,energy = 100,
totalHP =3, qntLoop=0, frame=0;

int points = 0, shoots=30;
bool animationOnOff = true;
itens.shield.timeActive = 3000;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int spawnY(byte sizeC) {
  return random(-sizeC + sizeC/2, LIMIT_Y - sizeC);
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


void mostraNave(){
  int leituraX = filtrarADC(JX) - centroX;
  int leituraY = filtrarADC(JY) - centroY;

  if(abs(leituraX) < DEADZONE) leituraX = 0;
  if(abs(leituraY) < DEADZONE) leituraY = 0;

  nave1.x -= leituraX / 800;
  nave1.y += leituraY / 800;

  if(nave1.x > 100)nave1.x = 100;
  if(nave1.x < -4) nave1.x = -4;
  if(nave1.y < -4) nave1.y = -4;
  if(nave1.y > 48) nave1.y = 48;
  
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
  
void spawn(Itens &powerUp, byte spawnrate){
      if (!powerUp.hasSpawned && random(0, 100) > 100 - spawnrate) {
      powerUp.y = spawnY(8);
      powerUp.x = 128; 
      powerUp.hasSpawned = true;
    }
  }
void generate(Itens &powerUp){
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
         && ((shoot[i].y < c[a].y +tam) && (shoot[i].y +8 > c[a].y)) ){ // funciona e o + é pra deixar mais ez
          shoot[i].hasShoot = false;
          c[a].hp--;
          if(c[a].hp <= 0){
            oled.drawBitmap(c[a].x, c[a].y, bitmap, tam, tam, SSD1306_WHITE);
            c[a].x = spawnX();
            c[a].y = spawnY(c[a].size);
            points += random(35, 55);
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
      c[i].hp--;
      c[i].x = spawnX();
      c[i].y = spawnY(c[i].size);
      break;
    }
  }
  }

void colisaoPowerup(Itens &c, int &boost, int value, bool opc){ //opc 1= energy, 2 ammo, 3 shield
  if(c.hasSpawned && (c.x <= nave1.x+8 && c.x >= nave1.x) &&
   (c.y+8 >= nave1.y  &&  c.y <= nave1.y +8) ){
    c.hasSpawned = false;
    c.x = 200;
    if(opc == 1) boost = value;
    else if(opc == 2) boost += value;
    else {
      itens.shield.isOn = true;
      timer.shieldState.reset();
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
  oled.drawBitmap(102, 57, sp_i, 16,16, SSD1306_WHITE);
  gprint(114,57,fase);
  } 

void final_ruim(){
    oled.clearDisplay();
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();
    animationOnOff = true;
    delay(500); // evita skip sem querer
    timer.global.reset();

    while(!butao.press() ) {
    animacao_final_ruim(70); // menor = mais rapido

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
    if(timer.energy.tempo(250) ) energy--;
    if(timer.ammo.tempo(1500) && fase < 6) shoots++;
    if(timer.shieldState.tempo(5000) && itens.shield.isOn = true) itens.shield.isOn = false;
 }

void powerUps() {
  if(timer.spawnEnergy.tempo(1000)) spawn(itens.energy, 10);
  generate(itens.energy);
  if(timer.spawnAmmo.tempo(1000)) spawn(itens.ammo, 13);
  generate(itens.ammo);
  if(timer.spawnHp.tempo(1000)) spawn(itens.hp, 5);
  generate(itens.hp);
  if(timer.spawnShield.tempo(1000)) spawn(itens.shield, 5);
  generate(itens.shield);

  }

void renderPower(){
  render(itens.ammo, ammoBitmap);
  render(itens.energy, bateria);
  render(itens.hp, vida); 
  }

void fases_cond(){
  if(fase == 1){
    cond.smallC.qntCom=0; cond.smallC.velMin=2; cond.smallC.velMax=2;
    cond.bigC.qntCom=3; cond.bigC.velMin=2; cond.bigC.velMax=2; // G = cometa grande
  }
  if(fase == 1 && points >= 700){
    fase++;

    cond.bigC.qntCom++;
    subirFase();
    mensagem_subirFase("FASE 2:", "+ vel max  com grande", "", "", "");
   }
  else if(fase == 2 && points >= 1500){
    fase++;

    cond.smallC.qntCom+=3; 
    subirFase();
    mensagem_subirFase("FASE 3:", "+3 com pequenos", "", "", "");
   }
  else if(fase == 3 && points >= 2500){
    fase++;
    cond.smallC.qntCom++; 
    cond.smallC.velMax++; 
    cond.bigC.qntCom--; 
    subirFase();
    mensagem_subirFase("FASE 4:", "+ com pequeno", "+ vel com pequeno", "- com grande", "");
  }
  else if(fase == 4 && points >= 3500){
    fase++;
    cond.smallC.qntCom+=2; 
    cond.bigC.qntCom-=2; 
    subirFase();
    mensagem_subirFase("FASE 5:", "+2 com pequeno", "+ vel min com pequeno", "- com grande", "");
  }
  else if(fase == 5 && points >= 4500){
    fase++;
    cond.smallC.qntCom+=2; 
    cond.bigC.qntCom--;
    subirFase();
    mensagem_subirFase("FASE 6:", "+2 com pequeno", "", "", "");

  }
    else if(fase == 6 && points >= 6000){
    fase++;
    cond.smallC.qntCom+=3; 
    subirFase();
    mensagem_subirFase("FASE 7:", "+3 com pequeno", "sem recarga", "", "");
  }
    else if(fase == 7 && points >= 7000){
    fase++;
    cond.bigC.qntCom++; 
    cond.bigC.velMax++; 
    subirFase();
    mensagem_subirFase("FASE 8:", "+ com grande", "+ vel max com grande", "", "");
  }
    else if(fase == 8 && points >= 8000){
    fase++;
    cond.bigC.qntCom++; 
    cond.smallC.qntCom+=2; 
    subirFase();
    mensagem_subirFase("FASE 9:", "+2 com pequeno", "+ com grande", "", "");
  }
    else if(fase == 9 && points >= 9000){
    fase=67;
    cond.bigC.qntCom+=2;
    cond.bigC.velMin++; 
    cond.smallC.qntCom+=5; 
    cond.smallC.velMax++;
    subirFase();
    mensagem_subirFase("FASE 67:", "+5 com pequeno", "+ vel max com pequeno", "+ com grande", "+ vel min com grande");

  }
  }


void subirFase() {
  timer.global.reset();
  itens.energy.x = 128; 
  itens.energy.y = spawnY(8);
  itens.energy.hasSpawned = false;
  energy += 30;
  shoots += random(0, 20);

  if(cond.smallC.qntCom > MAX_COM) cond.smallC.qntCom = MAX_COM;
  if(cond.bigC.qntCom > MAX_COM) cond.bigC.qntCom = MAX_COM;


  for (int i = 0; i < cond.smallC.qntCom; i++) {
    comet.small[i].x = spawnX();
    comet.small[i].y = spawnY(comet.small[i].size);
    comet.small[i].vel  = random(cond.smallC.velMin, cond.smallC.velMax +1);
  }
  for (int i = 0; i < cond.bigC.qntCom; i++) {
    comet.big[i].x = spawnX();
    comet.big[i].y = spawnY(comet.big[i].size);
    comet.big[i].vel  = random(cond.bigC.velMin, cond.bigC.velMax +1);
  }

 }

void final_bom() {
  oled.clearDisplay();
  oled.drawBitmap(0,0, tela_win, 128,64, SSD1306_WHITE);
  oled.display();
  delay(500); // evita skip sem querer
  while(!(butao.press() ) ) {
  if (timer.global.tempo(500) ){
  oled.clearDisplay();
  oled.drawBitmap(0,0, tela_win, 128,64, SSD1306_WHITE);
  oled.display();
  }
  }

  }

void animacao_final_ruim(int vel){
  if(animationOnOff){
  if(timer.animation.tempo(vel) ) {
  oled.clearDisplay();
  oled.drawBitmap(0,0, fim_ruim[frame], 128,64, SSD1306_WHITE);
  oled.display();
  frame++;
  if(frame > 9){
    frame=0;
  }
  }    
  }
  }

void mostrar_pontucao(){
  oled.setTextSize(2);
  for(int a=0;a<points;a+=17){
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
  fase = 1;
  points = 0;
  frame=0;
  shoots = 30;
  energy = 100;

  for(int i=0;i<MAX_COM;i++){
  comet.small[i].x = comet.small[i].y = 500;
  comet.big[i].x = comet.big[i].y = 500;
  }
  oled.setTextSize(1);
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

void mensagem_subirFase(String mensagem, String m1, String m2, String m3, String m4) {
  oled.clearDisplay();
  oled.setTextSize(2);
  oled.setCursor(0,0);
  oled.println(mensagem);
  oled.setTextSize(1);
  oled.println(m1);
  oled.println(m2);
  oled.println(m3);
  oled.println(m4);
  oled.display();
  delay(300); // mata ninguem
  while(!(butao.press()) );
  }

void gameGenerate(){
  fases_cond();
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
  renderPower();
  mostraNave();
  renderCometa();
  renderTiro();
  painel();
  }
void setup() {
  Serial.begin(115200);
  Wire.begin(22, 21);
  pinMode(JX, INPUT);
  pinMode(JY, INPUT);
  pinMode(SPK, OUTPUT);

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
  comeco();
  }

void loop() {
  gameGenerate();
  gameRender();

  if(totalHP <= 0 || energy <= 0){
    final_ruim();
    mostrar_pontucao();
   }
  if(points >= 10000){
    final_bom();
    mostrar_pontucao();
  }
  oled.display();
  }

