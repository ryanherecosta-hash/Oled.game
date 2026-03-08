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
  int x,y,vel=3;
  bool hasSpawned = false, isOn = false;

	bool spawn(byte spawnrate){
	byte rng = random(0, 100);
    if (rng >= 100 - spawnrate){return true;}
		else {return false;}
	}

	};

struct OrgItens {
	Itens energy;
	Itens hp;
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
  CStats(byte qntCom, byte vmin, byte vmax){
    qntCom = qntCom;
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

NavePos nave1;
OrgItens itens;
CometStats cond;
Comets comet;
Sounds sound;
Shoot shoot[MAX_TIROS];
Btn butao(BTN);