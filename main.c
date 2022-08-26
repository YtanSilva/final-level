
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#define ARESTA 40
#define LARGURA 1080
#define ALTURA 440
#define COLUNA 27
#define LINHA 11

typedef struct{
    int xPos;
    int yPos;
}JOGADOR;

void movDir( int *xPos, int *yPos, int tecla); // Altera o valor da posição baseado no valor apertado no teclado

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna);// Retorna 1 se o movimento eh valido

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
    int posX, posY, i, j, x, tecla;
    Color cor[50] = {LIGHTGRAY, RED, GREEN, MAGENTA, SKYBLUE, VIOLET,  BLACK};
    char letras[] = {'W', 'J', 'P', '  ', 'M', 'K','D'};
    JOGADOR jogador;

    for(i = 0; i < LINHA; i++){
        for(j = 0; j < COLUNA; j++){
            if(matriz[i][j] == 'J'){
                jogador.xPos = i;
                jogador.yPos = j;
                matriz[i][j] = ' ';
            }
        }
    }


    //Inicializacoes de variaveis e aspectos da raylib
    srand(time(NULL));
    posX = 0;//coordenada x central
    posY = 0;//coordenada y central


    InitWindow(LARGURA, ALTURA, "Jogo"); // Ajusta o titulo da janela
    SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo

    //Loop principal
    while (!WindowShouldClose())// Detecta se deve continuar
    {
        tecla = GetKeyPressed();

        posY = 0;

        if( tecla == KEY_DOWN || tecla == KEY_UP || tecla == KEY_LEFT || tecla == KEY_RIGHT) // Movimentacao do jogador
            if(podeMover(jogador.xPos, jogador.yPos, matriz, tecla, LINHA, COLUNA))
                movDir(&jogador.xPos, &jogador.yPos, tecla);

        BeginDrawing();
        ClearBackground(RAYWHITE);//Limpa e define uma cor de fundo


        for(i = 0; i < LINHA; i++){
            for(j = 0; j < COLUNA; j++){

                if(matriz[i][j] == 'W'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[0]);//Desenha o quadrado na posicao adequada
                    posX += 40;
                }else if(matriz[i][j] == 'M'){
                    DrawRectangle(posX, posY, ARESTA, ARESTA, cor[1]);//Desenha o quadrado na posicao adequada
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

        EndDrawing();
        }

    CloseWindow();// Fecha janela
    return 0;
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

int podeMover( int xPos, int yPos, char matriz[][COLUNA], int tecla, int linha, int coluna)
{
    int flag_mov = 0;

    movDir(&xPos, &yPos, tecla);

    if(xPos >= 0 && xPos < coluna && yPos >= 0 && yPos < linha && matriz[yPos][xPos] != 'W' && matriz[yPos][xPos] != 'D')
        flag_mov = 1;

    return flag_mov;
}
