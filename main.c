#include "raylib.h"
#include "stdio.h"

#define LARGURA 1080
#define ALTURA 440
#define ARESTA 40
#define COLUNA 27
#define LINHA 11
#define N 100
#define RAIO 20
#define EXPLOSAO 3




typedef struct{
    int xPos;
    int yPos;
    int ultMov;
}JOGADOR;

typedef struct{
    int xPos;
    int yPos;
    char tipo;
    int direcao;
    int movCont;
}CRIATURA;

typedef struct{
    double tempo;
    int plantada;
    int xPos, yPos;
    int xExp[EXPLOSAO*EXPLOSAO], yExp[EXPLOSAO*EXPLOSAO], estadoExp;
    double tempoExp;
}BOMBA;

int verifColisMap(int xPos, int yPos, char mapa[][COLUNA]);

int verifColisCord(int xPos1, int yPos1, int xPos2, int yPos2);

void movDir( int *xPos, int *yPos, int tecla); // Altera o valor da posição baseado no valor apertado no teclado

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna, BOMBA bomba);// Retorna 1 se o movimento eh valido

int tempoCmp(double tempo, float tempoPerc); // Marca o passar de um tempo "tempoPerc"

void plantarBomba(BOMBA *bomba, JOGADOR jogador); // Função que executa as funcionalidades iniciais de plantar a bomba

void expBomba(BOMBA *bomba, int raio); // Função que passa as posicoes x,y da explosao

void controleBomba(BOMBA *bomba, char matriz[][COLUNA], int tecla, JOGADOR jogador); // Funcao que executa todos os controles de estado da bomba

void desenharMapa(char letra, int *posX, int *posY);

void leMapa(char matriz[LINHA][COLUNA], int*posX, int *posY);


int main(void)
{

    char matriz[LINHA][COLUNA] = {
        'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W',
        'W','J',' ',' ','P','P','P','P',' ',' ',' ',' ',' ','P',' ',' ',' ',' ','M','W','W','W','W',' ',' ','K','W',
        'W',' ','W',' ','M',' ',' ',' ','W','W','W','D','W','W','W','D','W','W',' ',' ',' ',' ',' ',' ','W',' ','W',
        'W',' ','W','W','W','D','D','W','W','W','W','P','W','P','W','P','W','W','W','W','W',' ','P','P','P',' ','W',
        'W',' ',' ',' ','K',' ',' ',' ',' ',' ','W',' ','D','P','W',' ',' ',' ','M',' ',' ',' ','P',' ',' ',' ','W',
        'W','W','W','W','W','W','W',' ','W','W','W',' ','W','P','W',' ','W',' ','P','P','P','P','P','P','P','P','W',
        'W','K',' ',' ',' ',' ','D',' ',' ',' ',' ',' ','W','P','W',' ',' ',' ','D',' ',' ',' ',' ',' ',' ','K','W',
        'W',' ','W','W','W','W','W',' ','W','W','W','W','W','W','W','W','W',' ','W',' ','W','W','W','W','W',' ','W',
        'W',' ','M',' ',' ',' ','D',' ','P','P','P','P','P','P','P','P','P',' ','D',' ',' ',' ',' ',' ','W','M','W',
        'W','P','K',' ','W',' ',' ','P','W','W','W','W','W','W','W','W','W','P',' ',' ',' ','W',' ',' ',' ',' ','W',
        'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W',
    };


    //Declaracao de variaveis
    int posX, posY, i, j, x, tecla, cont = 0;

    JOGADOR jogador;
    CRIATURA criatura[N];
    BOMBA bomba;
    double tempCriatura = GetTime();


    for(i = 0; i < LINHA; i++){
        for(j = 0; j < COLUNA; j++){

            if(matriz[i][j] == 'J'){
                jogador.xPos = j;
                jogador.yPos = i;
                matriz[i][j] = ' ';
            }
            if(matriz[i][j] == 'M'){
                criatura[cont].xPos = j;
                criatura[cont].yPos = i;
                criatura[cont].tipo = matriz[i][j];
                cont++;
            }
        }
    }


    //Inicializacoes de variaveis e aspectos da raylib
    srand(time(NULL));
    posX = 0;//coordenada x central
    posY = 0;//coordenada y central
    bomba.plantada = 0;
    jogador.ultMov = KEY_RIGHT;
    bomba.estadoExp = 0;
    InitWindow(LARGURA, ALTURA, "Jogo"); // Ajusta o titulo da janela
    SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo

    for(i = 0; i < cont; i++){ // Zerando o contador de movimento
        criatura[i].movCont = 0;
        criatura[i].direcao = rand() % 4 + KEY_RIGHT;
    }


    //Loop principal
    while (!WindowShouldClose())// Detecta se deve continuar
    {
        tecla = GetKeyPressed();

        posY = 0;


        // Movimentacao do jogador
        if( tecla == KEY_DOWN || tecla == KEY_UP || tecla == KEY_LEFT || tecla == KEY_RIGHT){
            jogador.ultMov = tecla; // Salvando ultimo movimento apertado
            if(podeMover(jogador.xPos, jogador.yPos, matriz, tecla, LINHA, COLUNA, bomba))
                movDir(&jogador.xPos, &jogador.yPos, tecla);
        }



        // Controle da bomba
        controleBomba(&bomba,matriz,tecla,jogador);


        //Controle Criaturas
        if(tempoCmp(tempCriatura,1)){
                tempCriatura = GetTime();
            for(i = 0; i < cont; i++){
                if(criatura[i].movCont == 5 || !podeMover(criatura[i].xPos, criatura[i].yPos, matriz, criatura[i].direcao, LINHA, COLUNA, bomba)){
                    criatura[i].direcao = rand()%4 + KEY_RIGHT;
                    criatura[i].movCont = 0;
                }

                if(podeMover(criatura[i].xPos, criatura[i].yPos, matriz, criatura[i].direcao, LINHA, COLUNA, bomba)){
                    movDir(&criatura[i].xPos, &criatura[i].yPos, criatura[i].direcao);
                    criatura[i].movCont++;
                }

                printf("\n%d %d", criatura[i].xPos, criatura[i].yPos);
            }
            printf("\n\n");
        }


        BeginDrawing();
        ClearBackground(RAYWHITE);//Limpa e define uma cor de fundo


        leMapa(*matriz, &posX, &posY);


        DrawRectangle(jogador.xPos * ARESTA, jogador.yPos * ARESTA, ARESTA, ARESTA, GREEN);

        for(i = 0; i < cont; i++)
            DrawCircle(criatura[i].xPos * ARESTA + RAIO, criatura[i].yPos * ARESTA + RAIO, RAIO, GREEN);

        if(bomba.plantada) // Desenhar a bomba se ela estiver plantada
            DrawCircle(bomba.xPos * ARESTA + RAIO, bomba.yPos * ARESTA + RAIO, RAIO, WHITE);
        if(bomba.estadoExp){
            for(i = 0; i < EXPLOSAO * EXPLOSAO; i++){
                if(matriz[bomba.yExp[i]][bomba.xExp[i]] != 'W')
                    DrawCircle(bomba.xExp[i] * ARESTA + RAIO,bomba.yExp[i] * ARESTA + RAIO, RAIO, RED);
            }
        }
        EndDrawing();
        }

    CloseWindow();// Fecha janela
    return 0;
}

void leMapa(char matriz[LINHA][COLUNA], int *posX, int *posY){
    int i, j;

    for(i = 0; i < LINHA; i++){
            for(j = 0; j < COLUNA; j++){

                if(matriz[i][j] == 'W'){
                    desenharMapa('W', &posX, &posY);//Desenha o quadrado na posicao adequada
                }else if(matriz[i][j] == 'M'){
                   desenharMapa('M', &posX, &posY);
                }else if(matriz[i][j] == 'K'){
                    desenharMapa('K', &posX, &posY);
                }else if(matriz[i][j] == 'P'){
                    desenharMapa('P', &posX, &posY);
                }else if(matriz[i][j] == 'D'){
                    desenharMapa('D', &posX, &posY);
                }else if(matriz[i][j] == ' '){
                    desenharMapa(' ', &posX, &posY);
                }
            }

            posX = 0;
            posY += 40;
        }
}

void desenharMapa(char letra, int *posX, int *posY){
    Color cor[50] = {LIGHTGRAY, RED, GREEN, MAGENTA, SKYBLUE, VIOLET,  BLACK};
    switch(letra){
        case 'W':
            DrawRectangle(posX, posY, ARESTA, ARESTA, LIGHTGRAY);//Desenha o quadrado na posicao adequada
            posX += 40;
            break;
        case 'M':
             DrawRectangle(posX, posY, ARESTA, ARESTA, BLACK);//Desenha o quadrado na posicao adequada
             posX += 40;
             break;
        case 'K':
            DrawRectangle(posX, posY, ARESTA, ARESTA, MAGENTA);//Desenha o quadrado na posicao adequada
            posX += 40;
            break;
        case 'P':
             DrawRectangle(posX, posY, ARESTA, ARESTA, SKYBLUE);//Desenha o quadrado na posicao adequada
             posX += 40;
             break;
        case 'D':
            DrawRectangle(posX, posY, ARESTA, ARESTA, VIOLET);//Desenha o quadrado na posicao adequada
            posX += 40;
            break;
        case ' ':
             DrawRectangle(posX, posY, ARESTA, ARESTA, BLACK);//Desenha o quadrado na posicao adequada
             posX += 40;
             break;

    }
}


void plantarBomba(BOMBA *bomba, JOGADOR jogador)
{
    bomba -> tempo = GetTime(); // Instante que a bomba foi plantada
    bomba -> plantada = 1;

    movDir(&jogador.xPos, &jogador.yPos, jogador.ultMov); // Proxima posicao da bomba
    bomba -> xPos = jogador.xPos;
    bomba -> yPos = jogador.yPos;
}
int tempoCmp(double tempo, float tempoPerc)
{
    int passouTempo = 0;

    if(GetTime() - tempo > tempoPerc)// Se o tempo for maior que o esperado
        passouTempo = 1;


    return passouTempo;
}

void movDir( int *xPos, int *yPos, int tecla)
{
       switch(tecla){
    case KEY_DOWN:
        *yPos += 1;
        break;
    case KEY_UP:
        *yPos -= 1;
        break;
    case KEY_RIGHT:
        *xPos += 1;
        break;
    case KEY_LEFT:
        *xPos -= 1;
        break;
    }
}

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna, BOMBA bomba)
{
    int flag_mov = 0;

    movDir(&xPos, &yPos, tecla);

    if(verifColisMap(xPos,yPos,matriz) // Não bater em parede
       && (verifColisCord(xPos,yPos, bomba.xPos,bomba.yPos) || !bomba.plantada )) // Não for na mesma posicao da bomba
        flag_mov = 1;

    return flag_mov;
}

void expBomba(BOMBA *bomba, int raio)
{
    int i, j, contPos = 0;

    for(i = bomba->xPos - raio/2; i <= bomba->xPos + raio/2; i++){
        for(j = bomba->yPos - raio/2; j <= bomba->yPos + raio/2; j++){
            //Lendo todos os pontos da matriz detro da explosao
            bomba->xExp[contPos] = i;
            bomba->yExp[contPos] = j;
            contPos++;
        }
    }
}

void controleBomba(BOMBA *bomba, char matriz[][COLUNA], int tecla, JOGADOR jogador)
{
    int i;

    if(tecla == KEY_SPACE && !bomba->plantada &&
       podeMover(jogador.xPos, jogador.yPos, matriz, jogador.ultMov, LINHA, COLUNA, *bomba)){ // Bomba so pode ser mandada se o jogador estiver olhando para um ponto livre
        plantarBomba(bomba, jogador);
        bomba->tempoExp = 0;

    }
    else if(tempoCmp(bomba->tempo, 4) && bomba->plantada) // Se acabar o tempo
    {
        bomba->plantada = 0;
        expBomba(bomba, EXPLOSAO);
        bomba->tempoExp = GetTime();
        bomba->estadoExp = 1;

    }
    else if(tempoCmp(bomba->tempoExp, .15) && bomba->estadoExp){
        bomba->estadoExp = 0;
      for(i = 0; i < 9; i++)
        if(matriz[bomba->yExp[i]][bomba->xExp[i]] == 'D')
              matriz[bomba->yExp[i]][bomba->xExp[i]] = ' ';
    }
}

int verifColisCord(int xPos1, int yPos1, int xPos2, int yPos2)
{
    int retorno = 1;

    if(xPos1 == xPos2 && yPos1 == yPos2)
        retorno = 0;

    return retorno;
}

int verifColisMap(int xPos, int yPos, char mapa[][COLUNA])
{
    int retorno = 1;

    if((mapa[yPos][xPos] == 'D' || mapa[yPos][xPos] == 'W'))
        retorno = 0;

   return retorno;

}
