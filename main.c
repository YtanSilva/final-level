#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#define ARESTA 40
#define LARGURA 1080
#define ALTURA 440
#define COLUNA 27
#define LINHA 11
#define N 100
#define RAIO 20
#define EXPLOSAO 3
#define QTD_BOMBA 3


enum estadoBomba{
    LIVRE,
    SOLTA,
    EXPLODIU
};

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
    int xPos, yPos;
    int xExp[EXPLOSAO*EXPLOSAO], yExp[EXPLOSAO*EXPLOSAO];
    int estado;
}BOMBA;



int verifColisMap(int xPos, int yPos, char mapa[][COLUNA], char carac[]);// Retorna 0 caso haja alguma colisao com um elemento do mapa e 1 caso contrario

int verifColisCord(int xPos, int yPos, int xPosPassiv, int yPosPassiv);// Retorna 0 caso as duas coordenadas sejam iguais e 1 caso sejam diferentes

void movDir( int *xPos, int *yPos, int tecla); // Altera o valor da posição baseado no valor apertado no teclado

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna, BOMBA bomba[]);// Retorna 1 se o movimento eh valido

int tempoCmp(double tempo, float tempoPerc); // Retorna 1 se o "tempoPerc" tiver sido passado

void plantarBomba(BOMBA *bomba, JOGADOR jogador); // Função que executa as funcionalidades iniciais de plantar a bomba

void expBomba(BOMBA *bomba, int raio); // Função que passa as posicoes x,y da explosao

void controleBomba(BOMBA *bomba, char matriz[][COLUNA], int tecla, JOGADOR jogador, CRIATURA criatura[], int qtdCriatura); // Funcao que executa todos os controles de estado da bomba

void controleCriatura(CRIATURA *criatura, char mapa[][COLUNA], BOMBA *bomba, int qtdCriatura); // Funcao que generaliza o controle de movimento de monstro

void interacoes(JOGADOR *jogador, BOMBA *bomba, CRIATURA *criatura, char mapa[][COLUNA], int qtdCriatura); // Funcao que cuida de todas as interacoes entre os diferentes "objetos" do jogo

void desenhaMapa(char matriz[][COLUNA], Color cor[50], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], int cont) ; // funcao que aplica as estilizações as matrizes

void salvarJogo(char mapa[][COLUNA], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], int qtdCriatura);

void lerEstadoJogo(JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], int qtdCriatura);

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
    Color cor[50] = {LIGHTGRAY, RED, GREEN, MAGENTA, SKYBLUE, VIOLET,  BLACK};
    JOGADOR jogador;
    CRIATURA criatura[N];
    BOMBA bomba[QTD_BOMBA];
    double tempCriatura = GetTime();


    for(i = 0; i < LINHA; i++){
        for(j = 0; j < COLUNA; j++){
            if(matriz[i][j] == 'J'){
                jogador.xPos = j;
                jogador.yPos = i;
                matriz[i][j] = ' ';
            }
            if(matriz[i][j] == 'M' || matriz[i][j] == 'K'){
                criatura[cont].xPos = j;
                criatura[cont].yPos = i;
                criatura[cont].tipo = matriz[i][j];
                cont++;
                matriz[i][j] = ' ';
            }
        }
    }


    //Inicializacoes de variaveis e aspectos da raylib
    srand(time(NULL));
    posX = 0;//coordenada x central
    posY = 0;//coordenada y central
    for(i = 0; i < QTD_BOMBA; i++)
        bomba[i].estado = LIVRE;
    jogador.ultMov = KEY_RIGHT;
    InitWindow(LARGURA, ALTURA, "Jogo"); // Ajusta o titulo da janela
    SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo

    for(i = 0; i < cont; i++){ // Zerando o contador de movimento
        criatura[i].movCont = 0;
        criatura[i].direcao = rand() % 4 + KEY_RIGHT;
    }
    //Parte de teste de leitura do jogo salvo, depois tem que apagar
    if(0){
        lerEstadoJogo(jogador, criatura, bomba, cont);
        bomba[0].tempo = GetTime() - bomba[0].tempo;
        bomba[1].tempo = GetTime() - bomba[1].tempo;
        bomba[2].tempo = GetTime() - bomba[2].tempo;
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
        // Salvando o estado
        if(tecla == KEY_ENTER){ // Se a tecla for enter salvar o jogo
            salvarJogo(matriz,jogador,criatura,bomba,cont);
            CloseWindow(); // Fechando a janela, pode ser retirado dps
        }


        // Controle da bomba
        controleBomba(bomba,matriz,tecla,jogador, criatura, cont);


        //Controle Criaturas
        if(tempoCmp(tempCriatura,1)){
                tempCriatura = GetTime();
                controleCriatura(criatura, matriz, bomba, cont);
        }

        //Interacoes entre personagens
        interacoes(&jogador, bomba, criatura, matriz, cont);

        BeginDrawing();
        ClearBackground(RAYWHITE);//Limpa e define uma cor de fundo
        desenhaMapa(matriz, cor, jogador, criatura, bomba, cont);
        EndDrawing();
    }
    CloseWindow();// Fecha janela
    return 0;
}



void desenhaMapa(char matriz[][COLUNA], Color cor[50],  JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[],int cont){
    int i,j;
    int posX=0, posY=0;

        for(i = 0; i < LINHA; i++){
            for(j = 0; j < COLUNA; j++){
                if(matriz[i][j] == 'W'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[0]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == 'M'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[6]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == 'K'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[2]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == 'P'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[3]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == 'D'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[5]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == ' '){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[6]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }
            }
            posX = 0;
            posY += 40;
        }

        DrawRectangle(jogador.xPos * ARESTA, jogador.yPos * ARESTA, ARESTA, ARESTA, cor[4]);

       for(i = 0; i < cont; i++)
            DrawRectangle(criatura[i].xPos * ARESTA, criatura[i].yPos * ARESTA, ARESTA, ARESTA, cor[1]);

        for(i = 0; i < QTD_BOMBA; i++){
            if(bomba[i].estado == SOLTA) // Desenhar a bomba se ela estiver plantada
                DrawCircle(bomba[i].xPos * ARESTA + RAIO, bomba[i].yPos * ARESTA + RAIO, RAIO, WHITE);
            if(bomba[i].estado == EXPLODIU){
                for(j = 0; j < EXPLOSAO * EXPLOSAO; j++){
                    if(matriz[bomba[i].yExp[j]][bomba[i].xExp[j]] != 'W')
                        DrawCircle(bomba[i].xExp[j] * ARESTA + RAIO,bomba[i].yExp[j] * ARESTA + RAIO, RAIO, RED);
                }
            }

        }
}

void plantarBomba(BOMBA *bomba, JOGADOR jogador)
{
    bomba -> tempo = GetTime(); // Instante que a bomba foi plantada
    bomba -> estado = SOLTA;

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

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna, BOMBA bomba[])
{
    int flag_mov = 1, i;

    movDir(&xPos, &yPos, tecla);

    if(!verifColisMap(xPos,yPos,matriz, "WD")) // Não bater em parede
        flag_mov = 0;
    else
       for(i = 0; i < QTD_BOMBA; i++)
            if (!(verifColisCord(xPos,yPos, bomba[i].xPos,bomba[i].yPos)) && bomba[i].estado == SOLTA ) // Não for na mesma posicao da bomba
                flag_mov = 0;

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

void controleBomba(BOMBA *bomba, char matriz[][COLUNA], int tecla, JOGADOR jogador, CRIATURA criatura[], int qtdCriatura)
{
    int i, j,  espaco_vazio = 1, bombaTplant = 1;

    movDir(&jogador.xPos, &jogador.yPos, jogador.ultMov); // Simulando movimento para ter a posicao em que a bomba sera plantada

    for(i = 0; i < qtdCriatura; i++)
        if(!verifColisCord(jogador.xPos, jogador.yPos, criatura[i].xPos, criatura[i].yPos)){ // Se a bomba tiver tocado em algum monstro
            espaco_vazio = 0;
        }

    if(espaco_vazio && verifColisMap(jogador.xPos, jogador.yPos, matriz, " ")){ // Se a posicao na matriz nao for vazia
        espaco_vazio = 0;
    }

    for(i = 0; i < QTD_BOMBA; i++)
        if(!verifColisCord(jogador.xPos, jogador.yPos, bomba[i].xPos, bomba[i].yPos) && bomba[i].estado == SOLTA)
            espaco_vazio = 0;

    i = 0;
    while(i < QTD_BOMBA && bombaTplant){

        if(tecla == KEY_SPACE && bomba[i].estado == LIVRE &&  espaco_vazio){ // Bomba so pode ser plantada se o jogador estiver olhando para um ponto livre

            plantarBomba(&bomba[i], jogador);
            bombaTplant = 0;
        }

            i++;
    }

    for(i = 0; i < QTD_BOMBA; i++){
        if(tempoCmp(bomba[i].tempo, 4) && bomba[i].estado == SOLTA){ // Se acabar o tempo a bomba explode
            printf("\nPlantou");
            bomba[i].estado = EXPLODIU;
            expBomba(&bomba[i], EXPLOSAO);
            bomba[i].tempo = GetTime();


        }
        else if(tempoCmp(bomba[i].tempo, .15) && bomba[i].estado == EXPLODIU){ // Finalizando a explosao da bomba
            bomba[i].estado = LIVRE;
            printf("\nExplodiu");
        }
    }
}

int verifColisCord(int xPos1, int yPos1, int xPos2, int yPos2)
{
    int retorno = 1;

    if(xPos1 == xPos2 && yPos1 == yPos2) // Se a posicao for igual
        retorno = 0;

    return retorno;
}

int verifColisMap(int xPos, int yPos, char mapa[][COLUNA], char carac[])
{
    int retorno = 1, i, tam;

    tam = strlen(carac);

    for(i = 0; i < tam; i++)
    if((mapa[yPos][xPos] == carac[i]))
        retorno = 0;

   return retorno;

}

void controleCriatura(CRIATURA *criatura, char mapa[][COLUNA], BOMBA *bomba, int qtdCriatura)
{
    int i;
    for(i = 0; i < qtdCriatura; i++){
        if(criatura[i].movCont == 5 || !podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, LINHA, COLUNA, bomba)){ // Se tiver se movido 5 vezes ou bater numa parede
            criatura[i].movCont = 0;
            do{
                criatura[i].direcao = rand()%4 + KEY_RIGHT;
            }while(!podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, LINHA, COLUNA, bomba)); // Garantindo que a nova direcao seja uma direcao valida
        }

        if(podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, LINHA, COLUNA, bomba)){ // Executando a movimentacao da criatura
            movDir(&criatura[i].xPos, &criatura[i].yPos, criatura[i].direcao);
            criatura[i].movCont++;
        }
    }
}

void interacoes(JOGADOR *jogador, BOMBA *bomba, CRIATURA *criatura, char mapa[][COLUNA], int qtdCriatura)
{
    int i, j, contBomba;

    if(!verifColisMap(jogador->xPos,jogador->yPos,mapa, "P"))
        printf("\nPeguei uma pocao magica");
    for(contBomba = 0; contBomba < QTD_BOMBA; contBomba++){
        if(bomba[contBomba].estado == EXPLODIU){// verificacao de colisao da explosao
            for(i = 0; i < EXPLOSAO * EXPLOSAO; i++){

                if(!verifColisMap(bomba[contBomba].xExp[i], bomba[contBomba].yExp[i], mapa, "D"))
                    mapa[bomba[contBomba].yExp[i]][bomba[contBomba].xExp[i]] = ' ';

                if(!verifColisCord(bomba[contBomba].xExp[i],bomba[contBomba].yExp[i], jogador->xPos, jogador->yPos)){ // Se a bomba explodir no jogador
                    printf("\nSofri dano da bomba");
                }

                for(j = 0; j < qtdCriatura; j++){
                    if(criatura[i].tipo == 'M' && !verifColisCord(bomba[contBomba].xExp[i], bomba[contBomba].yExp[i], criatura[j].xPos, criatura[j].yPos)) // Se a posicao do monstro for a mesma da bomba
                        printf("\nMonstro morreu");
                }
            }
        }
    }
    for(i = 0; i < qtdCriatura; i++){
        if(!verifColisCord(jogador->xPos, jogador->yPos, criatura[i].xPos, criatura[i].yPos)){ // Se a posicao for igual do monstro
            if(criatura[i].tipo == 'M' ){ // Se passar por um monstro
                printf("\nsofri dano do monstro");
            }
            else                            // Se passar por uma criatura magica
                printf("\nPassei por uma criatura magica");
        }
    }
}

void salvarJogo(char mapa[][COLUNA], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], int qtdCriatura)
{
    FILE *arq;
    int i, j;

    //Salvando informações padrão do mapa
    if(!(arq = fopen("save.txt", "w")))
        printf("Erro de salvamento");
    else{
        for(i = 0; i < LINHA; i++){
                for(j = 0; j < COLUNA; j++)
                    fputc(mapa[i][j], arq);
                fputc('\n', arq);
        }
        fclose(arq);
    }

    // Salvando informacoes especificas do estado (monstro e jogo)
    if(!(arq = fopen("save.bin", "wb")))
        printf("Erro de salvamento");
    else{
        fwrite(&jogador, sizeof(JOGADOR), 1, arq);
        fwrite(criatura, sizeof(CRIATURA), qtdCriatura, arq);

        bomba[0].tempo = GetTime() - bomba[0].tempo;
        bomba[1].tempo = GetTime() - bomba[1].tempo;
        bomba[2].tempo = GetTime() - bomba[0].tempo;

        fwrite(bomba, sizeof(BOMBA), QTD_BOMBA, arq);

        fclose(arq);
    }
}

void lerEstadoJogo(JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], int qtdCriatura)
{
    FILE *arq;
    int i, j;

    if(!(arq = fopen("save.bin", "rb")))
        printf("Erro de carregamento");
    else{
        fread(&jogador, sizeof(JOGADOR), 1, arq);
        fread(criatura, sizeof(CRIATURA), qtdCriatura, arq);
        fread(bomba, sizeof(BOMBA), QTD_BOMBA, arq);

        fclose(arq);
    }
}
