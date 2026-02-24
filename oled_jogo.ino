#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <coisasQueQueroEsconder.h>

byte fase = 1,eneloss = 1,energia = 100;
byte qntC=0, vmin=2, vmax=2, fmusga=1; // C = cometa
byte qntCG=3, vminG=2, vmaxG=2; // G = cometa grande

int batX = 200, batY;
int comx[MAX_COM],comy[MAX_COM],vel[MAX_COM];  // depois melhorar esses nomes (geral)
int comGX[MAX_COM],comGY[MAX_COM],velG[MAX_COM], hpG[MAX_COM];  // depois melhorar esses nomes (geral)
int tiroX[MAX_TIROS], tiroY[MAX_TIROS];
int ptwin = 0,qntVidas =3, qntTiros=30;
int naveX = 50, naveY=20, volta=0;
int frame=0, musga_tiro_estado=1, musga_explosao=1; // depois deixar menos analfabeto
int qtdcom_ant = -1, velMax_ant = -1, velMin_ant = -1;
int qtdcom_antG = -1, velMax_antG = -1, velMin_antG = -1;


bool tiroAtivo[MAX_TIROS], energiAtiva=false;
bool anim_ativo = true, musga_Atiro = false, musga_explodido = false;

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
    comx[x] = comy[x] = vel[x] = 400;
    comGX[x] = comGY[x] = velG[x] = 400;
    hpG[x] = 3;
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
      volta++;
      if (volta >= 4){
        oled.setCursor(20, 57);
        oled.print("pressione tiro");
        volta=0;
      }
      oled.display();
     }
   }
   volta=0;
  }

void loop() {
  oled.clearDisplay();
  mostraNave();
  fases_cond();
  disparo();
  musga_disparo();
  cometas(qntC, vmin, vmax);
  cometasGrandes(qntCG, vminG, vmaxG);
  spawn_energia();
  colisao();
  musga_explosao_com();
  renderCometa();
  renderTiro();
  perdasTotais();
  painel();
  if(qntVidas <= 0 || energia <= 0){
    final_ruim();
    mostrar_pontucao();
   }
  if(ptwin >= 10000){
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
    if (comx[i] == 400){
    comx[i] = spawnX();
    }
    if (comy[i] == 400){
    comy[i] = spawnY();
    }
    vel[i]  = random(velMin, velMax+1);     
  }
  qtdcom_ant = qtd_com;
  velMin_ant = velMin;
  velMax_ant = velMax;
  }
  
  for (int i = 0; i < qtd_com; i++) {
    comx[i] -= vel[i]; 

    if (comx[i] < -8) { 
      comx[i] = spawnX();
      comy[i] = spawnY();
      vel[i]  = random(velMin, velMax+1);
      int a = random(0, 1001);
      if (a >= 1000){
        vel[i] = velMax + 5;
      }
    } 
  }
  }

void cometasGrandes(byte qtd_comG, byte velMinG, byte velMaxG){
  if(qtdcom_antG != qtd_comG || velMin_antG != velMinG || velMax_antG != velMaxG){
    for (int i = 0; i < qtd_comG; i++) {
    if (comGX[i] == 400){
    comGX[i] = spawnX();
    }
    if (comGY[i] == 400){
    comGY[i] = spawnGY();
    }
    velG[i]  = random(velMinG, velMaxG+1);     
  }
  qtdcom_antG = qtd_comG;
  velMin_antG = velMinG;
  velMax_antG = velMaxG;
  }
  
  for (int i = 0; i < qtd_comG; i++) {
    comGX[i] -= velG[i]; 

    if (comGX[i] < -15) { 
      comGX[i] = spawnX();
      comGY[i] = spawnGY();
      velG[i]  = random(velMinG, velMaxG+1);
      hpG[i] = 3;
    } 
  }
  }


void disparo(){
  if (butao.press() && qntTiros > 0){
    musga_Atiro = true;
    musga_tiro.reset();
    for(int a =0;a<MAX_TIROS;a++){
      if (!tiroAtivo[a]){
        tiroX[a] = naveX + 6;
        tiroY[a] = naveY;
        tiroAtivo[a] = true;
        qntTiros-=1;
        break;
       }
      }
    }
  }

void colisao(){
  // colisao cometas
  for(int i=0;i<MAX_TIROS;i++){
    if(tiroAtivo[i]){

      for(int a=0;a<qntC;a++){
        if((tiroX[i]+8 >= comx[a] && tiroX[i] < comx[a]+8)
         && ((tiroY[i] < comy[a]+8) && (tiroY[i]+8 > comy[a])) ){ // funciona e o + é pra deixar mais ez
          tiroAtivo[i] = false;
          oled.drawBitmap(comx[a], comy[a], explosao, 8, 8, SSD1306_WHITE);
          comx[a] = spawnX();
          comy[a] = spawnY();
          ptwin += random(40, 81);
          musga_explodido = true;
          musga_boom.reset();
          break;
        }
      }

      for(int a=0;a<qntCG;a++){
        if((tiroX[i]+8 >= comGX[a] && tiroX[i] < comGX[a]+16)
         && ((tiroY[i] < comGY[a]+16) && (tiroY[i]+8 > comGY[a])) ){ // funciona e o + é pra deixar mais ez
          tiroAtivo[i] = false;
          hpG[a]--;
          if(hpG[a] <= 0){
            oled.drawBitmap(comGX[a], comGY[a], explosaoG, 16,16, SSD1306_WHITE);
            comGX[a] = spawnX();
            comGY[a] = spawnGY();
            ptwin += random(40, 181);
            musga_explodido = true;
            musga_boom.reset();
            hpG[a] = 3;
          }
          break;
      }
    }
  }
  } // colisao nave
  for (int c=0;c<qntC;c++){
    if((naveX+7 >= comx[c] && naveX <= comx[c]+8) && (comy[c]+7 >= naveY && comy[c] <= naveY+7)) {
      qntVidas--;
      comx[c] = spawnX();
      comy[c] = spawnY();
      break;
    }
  }
  for (int c=0;c<qntCG;c++){
    if((naveX+7 >= comGX[c] && naveX <= comGX[c]+15) && (comGY[c]+15 >= naveY && comGY[c] <= naveY+7)) {
      qntVidas--;
      hpG[c]--;
      comGX[c] = spawnX();
      comGY[c] = spawnGY();
      break;
    }
  }
  
   // colisao energia 
  if(energiAtiva && (batX <= naveX+8 && batX >= naveX) && (batY+8 >= naveY  &&  batY <= naveY +8) ){ // feio mas funcional, adaptarei dps para mov livre de x e y
    energiAtiva = false;
    batX = 200;
    energia = 100;
  }
  }

void renderCometa(){

  for(int i=0; i<qntC;i++){
  oled.drawBitmap(comx[i], comy[i], cometa, 8, 8, SSD1306_WHITE);
  }

  for(int i=0; i<qntCG;i++){
    if(hpG[i] >= 3) oled.drawBitmap(comGX[i], comGY[i], cometaG0, 16, 16, SSD1306_WHITE);
    else if(hpG[i] == 2) oled.drawBitmap(comGX[i], comGY[i], cometaG1, 16, 16, SSD1306_WHITE);
    else if(hpG[i] == 1) oled.drawBitmap(comGX[i], comGY[i], cometaG2, 16, 16, SSD1306_WHITE);
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
  gprint(9,57,qntVidas);
  oled.drawBitmap(20, 57, bateria_painel, 8,8, SSD1306_WHITE);
  gprint(26,57,int(energia));
  oled.drawBitmap(41, 57, pontos, 8, 8, SSD1306_WHITE); 
  gprint(48,57,ptwin);
  oled.drawBitmap(80, 57, cometa_display, 8,8, SSD1306_WHITE);
  gprint(88,57,qntTiros); // testes
  oled.drawBitmap(102, 57, sp_i, 16,16, SSD1306_WHITE);
  gprint(114,57,fase);
  } 

void final_ruim(){
    oled.clearDisplay();
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();
    anim_ativo = true;
    delay(500); // evita skip sem querer
    timer.reset();

    while(!butao.press() ) {
    animacao_final_ruim(70); // menor = mais rapido

    if (frame >= 9){
    oled.clearDisplay();
    anim_ativo = false;
    oled.drawBitmap(0,0, fim_ruim[0], 128,64, SSD1306_WHITE);
    oled.display();

    if (cd.tempo(2500)){
      anim_ativo = true;
    }
    }
  }
  }

void perdasTotais(){
    if(t_energia.tempo(250) ) energia -= eneloss;
    if(municao.tempo(1000)) qntTiros++;
 }

void spawn_energia() {
  if(timer.tempo(1000) ) {
    int spawnrateEne = random(0, 100);
    if (spawnrateEne >= 87){ // 13% spawnrate / s
      if(!energiAtiva){
        batY = spawnY();
        }
      energiAtiva = true;
      }
    }

  if(energiAtiva){
    oled.drawBitmap(batX, batY, bateria,8,8, SSD1306_WHITE);
    batX-= vmin;
    if(batX <= -8){
      energiAtiva = false;
      batX = 200;
    }
  }
 }

void fases_cond(){
  if(fase == 1){
    qntC=0; vmin=2; vmax=2; fmusga=1;
    qntCG=3; vminG=2; vmaxG=2; // G = cometa grande
  }
  if(fase == 1 && ptwin >= 500){
    fase++;

    vmaxG++; // 3>4
    subirFase();
    mensagem_subirFase("FASE 2:", "+ vel max  com grande", "", "", "");
   }
  else if(fase == 2 && ptwin >= 1000){
    fase++;

    qntC+=3; // 0>3
    subirFase();
    mensagem_subirFase("FASE 3:", "+3 com pequenos", "", "", "");
   }
  else if(fase == 3 && ptwin >= 1500){
    fase++;
    qntC++; //3>4
    vmax++; //2>3
    qntCG--; //3>2
    subirFase();
    mensagem_subirFase("FASE 4:", "+ com pequeno", "+ vel com pequeno", "- com grande", "");
  }
  else if(fase == 4 && ptwin >= 2000){
    fase++;
    qntC+=2; // 4>6
    vmin++; //2>3
    qntCG-=2; //2>0
    subirFase();
    mensagem_subirFase("FASE 5:", "+2 com pequeno", "+ vel min com pequeno", "- com grande", "");
  }
  else if(fase == 5 && ptwin >= 2800){
    fase++;
    qntC+=2; //6>8
    subirFase();
    mensagem_subirFase("FASE 6:", "+2 com pequeno", "", "", "");

  }
    else if(fase == 6 && ptwin >= 3700){
    fase++;
    qntC+=3; //8>10
    subirFase();
    mensagem_subirFase("FASE 7:", "+3 com pequeno", "", "", "");
  }
    else if(fase == 7 && ptwin >= 4700){
    fase++;
    qntCG++; //0>1
    vmaxG++; //3>4
    subirFase();
    mensagem_subirFase("FASE 8:", "+ com grande", "+ vel max com grande", "", "");
  }
    else if(fase == 8 && ptwin >= 6000){
    fase++;
    qntCG++; //0>2
    qntC+=2; //11>13
    subirFase();
    mensagem_subirFase("FASE 9:", "+2 com pequeno", "+ com grande", "", "");
  }
    else if(fase == 9 && ptwin >= 7777){
    fase=67;
    qntCG++; //2>3
    vminG++; //2>3
    qntC+=5; //13>18
    vmax++; // 3>4
    subirFase();
    mensagem_subirFase("FASE 67:", "+5 com pequeno", "+ vel max com pequeno", "+ com grande", "+ vel min com grande");

  }
  }


void subirFase() {
  timer.reset();
  batX = 128; // bom ne
  batY = spawnY();
  energiAtiva = false;
  energia += 30;
  qntTiros += random(0, 20);

  if(qntC > MAX_COM) qntC = MAX_COM;
  if(qntCG > MAX_COM) qntCG = MAX_COM;


  for (int i = 0; i < qntC; i++) {
    comx[i] = spawnX();
    comy[i] = spawnY();
    vel[i]  = random(vmin, vmax);
  }
  for (int i = 0; i < qntCG; i++) {
    comGX[i] = spawnX();
    comGY[i] = spawnGY();
    velG[i]  = random(vmin, vmax);
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
  if(anim_ativo){
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
  for(int a=0;a<ptwin;a+=17){
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
  volta=0;
  qntVidas=3;
  fase = 1;
  ptwin = 0;
  qntC = fase1_qntC;
  vmin = fase1_vmin;
  vmax = fase1_vmax;
  frame=0;
  qntTiros = 30;
  energia = 100;

  for(int i=0;i<MAX_COM;i++){
  comx[i] = spawnX();
  comy[i] = spawnY();
  }
  oled.setTextSize(1);
  }

void musga_disparo(){
  if(musga_Atiro){
    switch (musga_tiro_estado) {
    case 1:
      tone(SPK, 1800, 30);
      musga_tiro_estado++;
      break;
    case 2:
      if(musga_tiro.tempo(35)){
        tone(SPK, 1500, 25);
        musga_tiro_estado++;
      } 
      break;
    case 3:
      if(musga_tiro.tempo(30)){
        tone(SPK, 1200, 20);
        musga_tiro_estado++;
      } 
      break;  
    case 4: 
      if(musga_tiro.tempo(25) ){
        tone(SPK, 900, 15);
        musga_tiro_estado = 1;
        musga_Atiro = false;
      }
      break;
    }
  }
  }

void musga_explosao_com() {
  if(musga_explodido) {
    switch(musga_explosao){
      case 1:
      tone(SPK, 850, 30);
      musga_explosao++;
      break;

      case 2:
      if(musga_boom.tempo(35)) {
      tone(SPK, 600, 40);
      musga_explosao++;
      } 
      break;

      case 3:
      if(musga_boom.tempo(45)) {
      tone(SPK, 750, 30);
      musga_explosao++;
      } 
      break;

      case 4:
      if(musga_boom.tempo(35)) {
      tone(SPK, 450, 30);
      musga_explosao++;
      } 
      break;      

      case 5:
      if(musga_boom.tempo(75)) {
      tone(SPK, 250, 120);
      musga_explosao=1;
      musga_explodido = false;
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
