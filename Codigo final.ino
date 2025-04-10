/*
Trabalho de GB de circuitos microprocessados
criar um codigo em arduino para um carrinho ir de um ponto A para B utilizando os caminhos existentes
Codigo que busca um caminho para o carrinho seguir, caso haja um obstaculo, ele bloqueara o caminho, voltar um nó e criara outro caminho para prosseguir
*/


#define Liga 7
#define oVelocidade1 6
#define oVelocidade2 5
#define oM11 9
#define oM12 10
#define oM21 11
#define oM22 12
#define iSensor1 8
#define iSensor2 13
#define iSensorFrente 2
#define TAM 7
#define xIni 0
#define yIni 2
#define xFim 4
#define yFim 6
#define maxPassos 12

typedef struct caminho
{
  int direcao;      //1=Norte 2=Oeste 3=Sul 4=Leste
} caminho;

caminho camin[maxPassos];
int apontador[maxPassos];

int geraCaminho (char mapa[TAM][TAM], int x, int y, int passo = 0){

    int p1,p2,p3,p4,pf;
    char m[TAM][TAM];
    
    if (x < 0 || x >= TAM || y < 0 || y >= TAM || passo>maxPassos)return 0;
    
    char aqui = mapa[x][y];
    
    if (aqui == 'X' || aqui == 'I')return 0;

    if (aqui == 'S') return passo;
    
    for(int i=0;i<TAM;i++){
        for(int j=0;j<TAM;j++){
            m[i][j]=mapa[i][j];
        }
    }
    m[x][y] = 'I';
    
    p1 = geraCaminho (m, x + 1, y, passo+1);
    p2 = geraCaminho (m, x, y - 1, passo+1);
    p3 = geraCaminho (m, x, y + 1, passo+1);
    p4 = geraCaminho (m, x - 1, y, passo+1);
    
    if ((p1<=p2 || p2==0) && (p1<=p3 || p3==0) && (p1<=p4 || p4==0) && p1!=0){
        if(p1<apontador[passo] || apontador[passo]==0){
            camin[passo].direcao = 3;
            apontador[passo]=p1;
        }
        pf = p1;
    }else if ((p2<=p3 || p3==0) && (p2<=p4 || p4==0) && p2!=0){
        if(p2<apontador[passo] || apontador[passo]==0){
            camin[passo].direcao = 2;
            apontador[passo]=p2;
        }
        pf = p2;
    }else if ((p3<=p4 || p4==0) && (p3!=0)){
        if(p3<apontador[passo] || apontador[passo]==0){
            camin[passo].direcao = 4;
            apontador[passo]=p3;
        }
        pf = p3;
    }else if (p4!=0){
        if(p4<apontador[passo] || apontador[passo]==0){
            camin[passo].direcao = 1;
            apontador[passo]=p4;
        }
        pf = p4;
    }else {
        pf = 0;
    }
    return pf;
}


void frenteTras(bool re=false,int v=255){

  digitalWrite(oM11,re);
  digitalWrite(oM12,!re);
  digitalWrite(oM21,re);
  digitalWrite(oM22,!re);
  
  analogWrite(oVelocidade1,v);
  analogWrite(oVelocidade2,v);
  while(!digitalRead(iSensor1) || !digitalRead(iSensor2)){
    if(digitalRead(iSensor1)){
      analogWrite(oVelocidade1,0);
    }else if(digitalRead(iSensor2)){
      analogWrite(oVelocidade2,0);
    }else{ 
      analogWrite(oVelocidade1,v);
      analogWrite(oVelocidade2,v);
    }
  }
  parar();
  if(re){
  	digitalWrite(oM11,!re);
  	digitalWrite(oM12,re);
  	digitalWrite(oM21,!re);
  	digitalWrite(oM22,re);
  }
  while(digitalRead(iSensor1) || digitalRead(iSensor2)){
    if(!digitalRead(iSensor1)){
      analogWrite(oVelocidade1,0);
    }else if(!digitalRead(iSensor2)){
      analogWrite(oVelocidade2,0);
    }else{ 
      analogWrite(oVelocidade1,v);
      analogWrite(oVelocidade2,v);
    }
  }
  parar();
}

void virar(bool direita=true,int v=255){
  int chave1=false;
  int chave2=false;

  digitalWrite(oM11,direita);
  digitalWrite(oM12,!direita);
  digitalWrite(oM21,!direita);
  digitalWrite(oM22,direita);
  
  analogWrite(oVelocidade1,v);
  analogWrite(oVelocidade2,v);
  //delay(700);
  if(!direita){
    while(!chave2){
      if(!chave1 && digitalRead(iSensor1)){
        chave1=true;
      }
      if(chave1 && !digitalRead(iSensor1)){
        chave2=true;
      }
    }
  }else{
    while(!chave2){
      if(!chave1 && digitalRead(iSensor2)){
        chave1=true;
      }
      if(chave1 && !digitalRead(iSensor2)){
        chave2=true;
      }
    }
  }
  parar();
}

void parar(){  
    analogWrite(oVelocidade1,0);
    analogWrite(oVelocidade2,0);
}

bool ligado;
int direcao;
int posX;
int posY;
int p;

void setup() {
    Serial.begin(9600);
    pinMode(oVelocidade1, OUTPUT);
    pinMode(oVelocidade2, OUTPUT);
    pinMode(oM11, OUTPUT);
    pinMode(oM12, OUTPUT);
    pinMode(oM21, OUTPUT);
    pinMode(oM22, OUTPUT);
    pinMode(iSensor1, INPUT);
    pinMode(iSensor2, INPUT);
    pinMode(iSensorFrente, INPUT);
    attachInterrupt(digitalPinToInterrupt(iSensorFrente), voltar, RISING);    
    ligado = false;
  	char mapa[TAM][TAM];
    for (int i = 0; i < TAM; i++){
        for (int j = 0; j < TAM; j++){
    	    if (i%2==1 && j%2==1) mapa[i][j] = 'X';
    	    else mapa[i][j] = '0';
    	}
    }
    mapa[xFim][yFim] = 'S';

    geraCaminho (mapa, xIni, yIni);
  	
  	direcao=3;
  	posX=xIni;
  	posY=yIni;
}

void loop() {
  int x;
  while(!ligado){
    ligado = digitalRead(Liga);
  }
  frenteTras(false);
  for(p=0;p<apontador[0]+1;p+=2){
     if(p==apontador[0]){
      	x=direcao-4;
     }else{
        x=direcao-camin[p].direcao;
     }
     Serial.println(x);
     direcao=camin[p].direcao;
     switch (direcao){
     case 1:
       posX-=2;
       break;
     case 2:
       posY-=2;
       break;
     case 3:
       posX+=2;
       break;
     case 4:
       posY+=2;
       break;
     }
  	 switch(x){
     case 0:
       frenteTras(false);
       break;
     case -1:
     case 3:
       virar(false);
       frenteTras(false);
       break;
     case 1:
     case -3:
       virar(true);
       frenteTras(false);
       break;
     case 2:
     case -2:
       virar(false);
       virar(false);
       frenteTras(false);
     break;
  }
  }
  parar();
  direcao=3;
  ligado=false;

}

void voltar(){
  int x;
  frenteTras(true,220);
  char mapa[TAM][TAM];
  for (int i = 0; i < TAM; i++){
    for (int j = 0; j < TAM; j++){
      if (i%2==1 && j%2==1) mapa[i][j] = 'X';
      else mapa[i][j] = '0';
    }
  }
  mapa[xFim][yFim] = 'S';
  for(int i=0;i<maxPassos;i++){
    apontador[i]=0;
  }
  switch (direcao){
  case 1:
    posX+=2;
    mapa[posX-1][posY]='X';
    break;
  case 2:
    posY+=2;
    mapa[posX][posY-1]='X';
    break;
  case 3:
    posX-=2;
    mapa[posX+1][posY]='X';
    break;
  case 4:
    posY-=2;
    mapa[posX][posY+1]='X';
    break;
  }
  geraCaminho(mapa,posX,posY);
  x=direcao-camin[0].direcao;
  direcao=camin[0].direcao;

  p=0;

  switch(x){
  case 0:
    break;
  case -1:
  case 3:
    virar(false);
    break;
  case 1:
  case -3:
    virar(true);
    break;
  case 2:
  case -2:
    virar(false);
    virar(false);
  	break;
  }
  digitalWrite(oM11,LOW);
  digitalWrite(oM12,HIGH);
  digitalWrite(oM21,LOW);
  digitalWrite(oM22,HIGH);
}
