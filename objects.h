struct Comet{
  int x=SPAWNXY,y=SPAWNXY,hp,vel;
	byte size;

  void defineHp(int life){
    hp = life;
  }
 };

class Btn { // a melhorar
  public:
  int pino, btnclick, btnsubida;
  Btn(int p) {
  pino = p;
  btnsubida =0;
  btnclick =0;
  pinMode(pino, INPUT_PULLUP);
  }
   bool press() {
     if (digitalRead(pino)){
       btnclick = 1;
       btnsubida = 0;
     } else {
     btnsubida = 1;
     }
     if (btnsubida == 1 && btnclick ==1) {
      btnclick = 0;
       btnsubida = 0;
       return true;
     }
     else {
      return false; 
     }
  }
  };

class Timer {
	public:
	unsigned long tempoAnterior;
	Timer(){
		tempoAnterior =0;
	}
	bool tempo(unsigned long temporizador){
		if (millis() - tempoAnterior >= temporizador){
			tempoAnterior = millis();
			return true;
		}
		return false;
	}
	void reset(){
		tempoAnterior = millis();
	}

	};

struct Timers{
	Timer global;
	Timer energy;
	Timer shootSound;
	Timer startSound;
	Timer explosionSound;
	Timer animation;
	Timer cooldawn;
	Timer ammo;
	Timer shieldState;
	Timer shieldBlink;
	Timer powerUp;
	Timer dbPoints;
	};
	Timers timer;

struct Shoot{
  int x,y;
	byte vel;
  bool hasShoot = false;
	};

struct Sound{
	int state=1;
	bool canPlay = false;

	void reset(){
		state=1;
	}
	};

struct Itens{ // sem funçao real ainda
  int x,y,vel=2,qnt=0;
	unsigned long timeActive;
  bool hasSpawned = false, isOn = false, blinkState = false, blink=true;
	};

struct OrgItens {
	Itens energy;
	Itens hp;
	Itens ammo;
	Itens shield;
	Itens dbPoints;
	};

struct Sounds{
	Sound shoot;
	Sound explosion;
	};

struct Comets{
	Comet small[MAX_COM];
	Comet big[MAX_COM];
	};

struct CStats {
  byte qntCom, velMin, velMax;
  CStats(byte qntcom, byte vmin, byte vmax){
    qntCom = qntcom;
    velMin = vmin;
    velMax = vmax;
  }
	};
struct CometStats{
  CStats smallC{0,2,2};
  CStats bigC{3,2,2};
	};

struct NavePos{
  int x,y;
	};

struct Menu{
int x=0,y=0;

void attPos(){
	if(x>2)x=2;
	if(x<0)x=0;
	if(y>1)y=1;
	if(y<0)y=0;
	}
	};

struct Fases{
	int pontos, bvmin, bvmax, bqnt, svmin, svmax, sqnt;

	};

struct gFases{
	Fases faseEz[11] = {
// pts, bvmin, bvmax, bqnt, svmin, svmax, sqnt
  {0,     0,     0,     0,    2,     2,    4},
	{300,   0,     0,     0,    0,     0,    2}, 
	{700,   0,     0,     0,    0,     0,    1},
	{1100,  0,     0,     0,    0,     0,    2},
	{1700,  0,     0,     0,    0,     0,    1},
	{2500,  0,     0,     0,    0,     1,    1},
	{3000,  0,     0,     0,    0,     0,    1},
	{3500,  0,     0,     0,    0,     0,    1},
	{4000,  0,     0,     0,    1,     0,    1},
	{5000,  0,     0,     0,    0,     1,    1},
	{7000,  0,     0,     0,    0,     0,    0}
	//      0      0      0     3      4     sla 
	};
Fases faseMid[11] = {
// pts, bvmin, bvmax, bqnt, svmin, svmax, sqnt
  {0,     2,     2,     1,    2,     2,    5},
	{500,   0,     0,     0,    0,     1,    1},
	{1000,  0,     0,     1,    0,     0,    1},
	{1800,  0,     0,     0,    0,     1,    1},
	{2600,  0,     0,     0,    0,     0,    0},
	{3600,  0,     0,     0,    1,     0,   -1},
	{4600,  0,     0,     1,    0,     1,    0},
	{5800,  0,     0,     1,    0,     0,   -2},
	{7000,  0,     1,     0,    0,     0,   -3},
	{8500,  1,     0,     2,    1,     0,   -1},
	{11000, 0,     0,     0,    0,     0,    0}
	//      2      3      6      2     5     2
	};

Fases faseHard[11] = {
// pts, bvmin, bvmax, bqnt, svmin, svmax, sqnt
  {0,     2,     2,     3,    0,     0,    0},
	{800,   0,     0,     0,    0,     0,    0},
	{1600,  0,     0,     1,    0,     0,    0},
	{2600,  0,     1,     0,    0,     0,    0},
	{3800,  0,     0,     1,    0,     0,    0},
	{5200,  0,     0,     1,    0,     0,    0},
	{6500,  0,     0,     2,    0,     0,    0},
	{8000,  0,     0,     2,    0,     0,    0},
	{10000, 0,     0,     0,    0,     0,    0},
	{12000, 1,     0,     0,    0,     0,    0},
	{18000, 0,     0,     0,    5,     6,    1}
	//      3      4      9     5      6     0
	};
	};
gFases fases;
Menu menuPos;
NavePos nave1;
OrgItens itens;
CometStats cond;
Comets comet;
Sounds sound;
Shoot shoot[MAX_TIROS];
Btn butao(BTN);
Btn usePup(JBTN);