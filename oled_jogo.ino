#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <coisasQueQueroEsconder.h>

byte fase = 1,energy_loss = 1,energy = 100;
byte qntComet8=0, velMin=2, velMax=2; // C = cometa
byte qntComet16=3, velMin16=2, velMax16=2; // G = cometa grande

int bateryX = 200, bateryY;
int comX8[MAX_COM],comY8[MAX_COM],comVel8[MAX_COM];  // depois transformar em struct
int comX16[MAX_COM],comY16[MAX_COM],comVel16[MAX_COM], comHp16[MAX_COM]; 
int tiroX[MAX_TIROS], tiroY[MAX_TIROS];
int points = 0,totalHP =3, shoots=30;
int naveX = 50, naveY=20, qntLoop=0;
int frame=0, soundShootState=1, soundExplosion=1; 
int qtdcom_ant = -1, velMax_ant = -1, velMin_ant = -1;
int qtdcom_antG = -1, velMax_antG = -1, velMin_antG = -1;


bool tiroAtivo[MAX_TIROS], energyOnOff=false;
bool animationOnOff = true, soundShoot = false, soundExplosionState = false;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int spawnY() {
  return random(0, LIMIT_Y - TAM_OBJ);
  }

int spawnX() {
  return random(MIN_COM_RESP, MAX_COM_RESP); 
  }

int spawnGY() {
  return random(-8, LIMIT_Y - TAM_OBJG);
  }

int filtrarADC(int pin) {
  int soma = 0;
  for (int i = 0; i < 20; i++) {
    soma += analogRead(pin);
  }
  int media = soma / 20; 
  return media;
  }

void setup() {
  Serial.begin(115200);
  Wire.begin(22, 21);
  pinMode(JX, INPUT);
  pinMode(JY, INPUT);
  pinMode(SPK, OUTPUT);

  for(int x=0;x<MAX_COM;x++){
    comX8[x] = comY8[x] = comVel8[x] = 400;
    comX16[x] = comY16[x] = comVel16[x] = 400;
    comHp16[x] = 3;
  }

  if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    while (true);
  }

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(SSD1306_WHITE);
  oled.println("Codigo feito por: ");
  oled.println("Ryan Acosta Heredia");
  oled.println("2 ano ETEHL");
  oled.println("ultima att feita em:");
  oled.println("24/02/26 00:15 ");
  oled.display();
  delay(4000);
  randomSeed(analogRead(0)); 
  comeco();
  }


void comeco(){ 
  timer.reset();
  while(!butao.press() ){
    if (timer.tempo(500)){
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

void loop() {
  oled.clearDisplay();
  mostraNave();
  fases_cond();
  disparo();
  musga_disparo();
  cometas(qntComet8, velMin, velMax);
  cometasGrandes(qntComet16, velMin16, velMax16);
  spawn_energia();
  colisao();
  musga_explosao_com();
  renderCometa();
  renderTiro();
  perdasTotais();
  painel();
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


void mostraNave(){
  int leituraX = filtrarADC(JX) - centroX;
  int leituraY = filtrarADC(JY) - centroY;

  if(abs(leituraX) < DEADZONE) leituraX = 0;
  if(abs(leituraY) < DEADZONE) leituraY = 0;

  float velX = leituraX * -0.0012;
  float velY = leituraY * 0.0012;

  naveX += velX;
  naveY += velY;

  if(naveX > 100)naveX = 100;
  if(naveX < -4) naveX = -4;
  if(naveY < -4) naveY = -4;
  if(naveY > 48) naveY = 48;
  
  oled.drawBitmap(naveX, naveY, nave, 8, 8, SSD1306_WHITE);
  }

void cometas(byte qtd_com, byte velMin, byte velMax){
  if(qtdcom_ant != qtd_com || velMin_ant != velMin || velMax_ant != velMax){
    for (int i = 0; i < qtd_com; i++) {
    if (comX8[i] == 400){
    comX8[i] = spawnX();
    }
    if (comY8[i] == 400){
    comY8[i] = spawnY();
    }
    comVel8[i]  = random(velMin, velMax+1);     
  }
  qtdcom_ant = qtd_com;
  velMin_ant = velMin;
  velMax_ant = velMax;
  }
  
  for (int i = 0; i < qtd_com; i++) {
    comX8[i] -= comVel8[i]; 

    if (comX8[i] < -8) { 
      comX8[i] = spawnX();
      comY8[i] = spawnY();
      comVel8[i]  = random(velMin, velMax+1);
      int a = random(0, 1001);
      if (a >= 1000){
        comVel8[i] = velMax + 5;
      }
    } 
  }
  }

void cometasGrandes(byte qtd_comG, byte velMinG, byte velMaxG){
  if(qtdcom_antG != qtd_comG || velMin_antG != velMinG || velMax_antG != velMaxG){
    for (int i = 0; i < qtd_comG; i++) {
    if (comX16[i] == 400){
    comX16[i] = spawnX();
    }
    if (comY16[i] == 400){
    comY16[i] = spawnGY();
    }
    comVel16[i]  = random(velMinG, velMaxG+1);     
  }
  qtdcom_antG = qtd_comG;
  velMin_antG = velMinG;
  velMax_antG = velMaxG;
  }
  
  for (int i = 0; i < qtd_comG; i++) {
    comX16[i] -= comVel16[i]; 

    if (comX16[i] < -15) { 
      comX16[i] = spawnX();
      comY16[i] = spawnGY();
      comVel16[i]  = random(velMinG, velMaxG+1);
      comHp16[i] = 3;
    } 
  }
  }


void disparo(){
  if (butao.press() && shoots > 0){
    soundShoot = true;
    musga_tiro.reset();
    for(int a =0;a<MAX_TIROS;a++){
      if (!tiroAtivo[a]){
        tiroX[a] = naveX + 6;
        tiroY[a] = naveY;
        tiroAtivo[a] = true;
        shoots-=1;
        break;
       }
      }
    }
  }

void colisao(){
  // colisao cometas
  for(int i=0;i<MAX_TIROS;i++){
    if(tiroAtivo[i]){

      for(int a=0;a<qntComet8;a++){
        if((tiroX[i]+8 >= comX8[a] && tiroX[i] < comX8[a]+8)
         && ((tiroY[i] < comY8[a]+8) && (tiroY[i]+8 > comY8[a])) ){ // funciona e o + é pra deixar mais ez
          tiroAtivo[i] = false;
          oled.drawBitmap(comX8[a], comY8[a], explosao, 8, 8, SSD1306_WHITE);
          comX8[a] = spawnX();
          comY8[a] = spawnY();
          points += random(40, 81);
          soundExplosionState = true;
          musga_boom.reset();
          break;
        }
      }

      for(int a=0;a<qntComet16;a++){
        if((tiroX[i]+8 >= comX16[a] && tiroX[i] < comX16[a]+16)
         && ((tiroY[i] < comY16[a]+16) && (tiroY[i]+8 > comY16[a])) ){ // funciona e o + é pra deixar mais ez
          tiroAtivo[i] = false;
          comHp16[a]--;
          if(comHp16[a] <= 0){
            oled.drawBitmap(comX16[a], comY16[a], explosaoG, 16,16, SSD1306_WHITE);
            comX16[a] = spawnX();
            comY16[a] = spawnGY();
            points += random(40, 181);
            soundExplosionState = true;
            musga_boom.reset();
            comHp16[a] = 3;
          }
          break;
      }
    }
  }
  } // colisao nave
  for (int c=0;c<qntComet8;c++){
    if((naveX+7 >= comX8[c] && naveX <= comX8[c]+8) && (comY8[c]+7 >= naveY && comY8[c] <= naveY+7)) {
      totalHP--;
      comX8[c] = spawnX();
      comY8[c] = spawnY();
      break;
    }
  }
  for (int c=0;c<qntComet16;c++){
    if((naveX+7 >= comX16[c] && naveX <= comX16[c]+15) && (comY16[c]+15 >= naveY && comY16[c] <= naveY+7)) {
      totalHP--;
      comHp16[c]--;
      comX16[c] = spawnX();
      comY16[c] = spawnGY();
      break;
    }
  }
  
   // colisao energia 
  if(energyOnOff && (bateryX <= naveX+8 && bateryX >= naveX) && (bateryY+8 >= naveY  &&  bateryY <= naveY +8) ){ // feio mas funcional, adaptarei dps para mov livre de x e y
    energyOnOff = false;
    bateryX = 200;
    energy = 100;
  }
  }

void renderCometa(){

  for(int i=0; i<qntComet8;i++){
  oled.drawBitmap(comX8[i], comY8[i], cometa, 8, 8, SSD1306_WHITE);
  }

  for(int i=0; i<qntComet16;i++){
    if(comHp16[i] >= 3) oled.drawBitmap(comX16[i], comY16[i], cometaG0, 16, 16, SSD1306_WHITE);
    else if(comHp16[i] == 2) oled.drawBitmap(comX16[i], comY16[i], cometaG1, 16, 16, SSD1306_WHITE);
    else if(comHp16[i] == 1) oled.drawBitmap(comX16[i], comY16[i], cometaG2, 16, 16, SSD1306_WHITE);
  }

  }

void renderTiro(){
  for(int i=0;i<MAX_TIROS;i++){
    if(tiroAtivo[i]){
      oled.drawBitmap(tiroX[i], tiroY[i], tiro, 8, 8, SSD1306_WHITE);
      tiroX[i] += 5;
      if(tiroX[i] > 128){
        tiroAtivo[i] = false;
         }
        }
      }
    }


void gprint(byte x, byte y, int random){
  oled.setCursor(x, y);
  oled.print(random);
  }
 

void painel(){
  oled.drawLine(0,55,128,55,SSD1306_WHITE);
  oled.drawBitmap(0, 57, vida, 8, 8, SSD1306_WHITE);
  gprint(9,57,totalHP);
  oled.drawBitmap(20, 57, bateria_painel, 8,8, SSD1306_WHITE);
  gprint(26,57,int(energy));
  oled.drawBitmap(41, 57, pontos, 8, 8, SSD1306_WHITE); 
  gprint(48,57,points);
  oled.drawBitmap(80, 57, cometa_display, 8,8, SSD1306_WHITE);
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
    timer.reset();

    while(!butao.press() ) {
    animacao_final_ruim(70); // menor = mais rapido

    if (frame >= 9){
    oled.clearDisplay();
    animationOnOff = false;
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();

    if (cd.tempo(2500)){
      animationOnOff = true;
    }
    }
  }
  }

void perdasTotais(){
    if(t_energia.tempo(250) ) energy -= energy_loss;
    if(municao.tempo(1000)) shoots++;
 }

void spawn_energia() {
  if(timer.tempo(1000) ) {
    int spawnrateEne = random(0, 100);
    if (spawnrateEne >= 87){ // 13% spawnrate / s
      if(!energyOnOff){
        bateryY = spawnY();
        }
      energyOnOff = true;
      }
    }

  if(energyOnOff){
    oled.drawBitmap(bateryX, bateryY, bateria,8,8, SSD1306_WHITE);
    bateryX-= velMin;
    if(bateryX <= -8){
      energyOnOff = false;
      bateryX = 200;
    }
  }
 }

void fases_cond(){
  if(fase == 1){
    qntComet8=0; velMin=2; velMax=2;
    qntComet16=3; velMin16=2; velMax16=2; // G = cometa grande
  }
  if(fase == 1 && points >= 500){
    fase++;

    velMax16++; // 3>4
    subirFase();
    mensagem_subirFase("FASE 2:", "+ vel max  com grande", "", "", "");
   }
  else if(fase == 2 && points >= 1000){
    fase++;

    qntComet8+=3; // 0>3
    subirFase();
    mensagem_subirFase("FASE 3:", "+3 com pequenos", "", "", "");
   }
  else if(fase == 3 && points >= 1500){
    fase++;
    qntComet8++; //3>4
    velMax++; //2>3
    qntComet16--; //3>2
    subirFase();
    mensagem_subirFase("FASE 4:", "+ com pequeno", "+ vel com pequeno", "- com grande", "");
  }
  else if(fase == 4 && points >= 2000){
    fase++;
    qntComet8+=2; // 4>6
    velMin++; //2>3
    qntComet16-=2; //2>0
    subirFase();
    mensagem_subirFase("FASE 5:", "+2 com pequeno", "+ vel min com pequeno", "- com grande", "");
  }
  else if(fase == 5 && points >= 2800){
    fase++;
    qntComet8+=2; //6>8
    subirFase();
    mensagem_subirFase("FASE 6:", "+2 com pequeno", "", "", "");

  }
    else if(fase == 6 && points >= 3700){
    fase++;
    qntComet8+=3; //8>10
    subirFase();
    mensagem_subirFase("FASE 7:", "+3 com pequeno", "", "", "");
  }
    else if(fase == 7 && points >= 4700){
    fase++;
    qntComet16++; //0>1
    velMax16++; //3>4
    subirFase();
    mensagem_subirFase("FASE 8:", "+ com grande", "+ vel max com grande", "", "");
  }
    else if(fase == 8 && points >= 6000){
    fase++;
    qntComet16++; //0>2
    qntComet8+=2; //11>13
    subirFase();
    mensagem_subirFase("FASE 9:", "+2 com pequeno", "+ com grande", "", "");
  }
    else if(fase == 9 && points >= 7777){
    fase=67;
    qntComet16++; //2>3
    velMin16++; //2>3
    qntComet8+=5; //13>18
    velMax++; // 3>4
    subirFase();
    mensagem_subirFase("FASE 67:", "+5 com pequeno", "+ vel max com pequeno", "+ com grande", "+ vel min com grande");

  }
  }


void subirFase() {
  timer.reset();
  bateryX = 128; // bom ne
  bateryY = spawnY();
  energyOnOff = false;
  energy += 30;
  shoots += random(0, 20);

  if(qntComet8 > MAX_COM) qntComet8 = MAX_COM;
  if(qntComet16 > MAX_COM) qntComet16 = MAX_COM;


  for (int i = 0; i < qntComet8; i++) {
    comX8[i] = spawnX();
    comY8[i] = spawnY();
    comVel8[i]  = random(velMin, velMax);
  }
  for (int i = 0; i < qntComet16; i++) {
    comX16[i] = spawnX();
    comY16[i] = spawnGY();
    comVel16[i]  = random(velMin, velMax);
  }

 }

void final_bom() {
  oled.clearDisplay();
  oled.drawBitmap(0,0, tela_win, 128,64, SSD1306_WHITE);
  oled.display();
  delay(500); // evita skip sem querer
  while(!(butao.press() ) ) {
  if (timer.tempo(500) ){
  oled.clearDisplay();
  oled.drawBitmap(0,0, tela_win, 128,64, SSD1306_WHITE);
  oled.display();
  }
  }

  }

void animacao_final_ruim(int vel){
  if(animationOnOff){
  if(animation.tempo(vel) ) {
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
  timer.reset();
   while(!butao.press() ) {
   if(timer.tempo(90000)){
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
  qntComet8 = fase1_qntC;
  velMin = fase1_vmin;
  velMax = fase1_vmax;
  frame=0;
  shoots = 30;
  energy = 100;

  for(int i=0;i<MAX_COM;i++){
  comX8[i] = spawnX();
  comY8[i] = spawnY();
  }
  oled.setTextSize(1);
  }

void musga_disparo(){
  if(soundShoot){
    switch (soundShootState) {
    case 1:
      tone(SPK, 1800, 30);
      soundShootState++;
      break;
    case 2:
      if(musga_tiro.tempo(35)){
        tone(SPK, 1500, 25);
        soundShootState++;
      } 
      break;
    case 3:
      if(musga_tiro.tempo(30)){
        tone(SPK, 1200, 20);
        soundShootState++;
      } 
      break;  
    case 4: 
      if(musga_tiro.tempo(25) ){
        tone(SPK, 900, 15);
        soundShootState = 1;
        soundShoot = false;
      }
      break;
    }
  }
  }

void musga_explosao_com() {
  if(soundExplosionState) {
    switch(soundExplosion){
      case 1:
      tone(SPK, 850, 30);
      soundExplosion++;
      break;

      case 2:
      if(musga_boom.tempo(35)) {
      tone(SPK, 600, 40);
      soundExplosion++;
      } 
      break;

      case 3:
      if(musga_boom.tempo(45)) {
      tone(SPK, 750, 30);
      soundExplosion++;
      } 
      break;

      case 4:
      if(musga_boom.tempo(35)) {
      tone(SPK, 450, 30);
      soundExplosion++;
      } 
      break;      

      case 5:
      if(musga_boom.tempo(75)) {
      tone(SPK, 250, 120);
      soundExplosion=1;
      soundExplosionState = false;
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
