
#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#define ARESTA 40
#define LARGURA 1080
#define ALTURA 440
#define COLUNA 27
#define LINHA 11
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
    int posX, posY, continua, i, j, x;
    Color cor[4][50] = {LIGHTGRAY, RED, GREEN, MAGENTA, SKYBLUE, VIOLET,  BLACK};
    char letras[] = {'W', 'J', 'P', '  ', 'M', 'K','D'};

    //Inicializacoes de variaveis e aspectos da raylib
    srand(time(NULL));
    posX = 0;//coordenada x central
    posY = 0;//coordenada y central
    continua = 1;

    InitWindow(LARGURA, ALTURA, "Movimento"); // Ajusta o titulo da janela
    SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo

    //Loop principal
    while (!WindowShouldClose() && continua)// Detecta se deve continuar
    {
        // A partir do input do usuario atualiza o estado/contexto
        if (IsKeyPressed(KEY_RIGHT)) posX += ARESTA;
        if (IsKeyPressed(KEY_LEFT)) posX -= ARESTA;
        if (IsKeyPressed(KEY_UP)) posY -= ARESTA;
        if (IsKeyPressed(KEY_DOWN)) posY += ARESTA;

        posY = 0;

        //Processa o estado
        if(posX < 0 || posY < 0 || posX > LARGURA || posY > ALTURA){//Se esta fora, termina o laco e nao desenha
            continua = 0;
        }else{//Se ainda esta dentro, atualiza a representacao grafica a partir do estado/contexto

            BeginDrawing();
            ClearBackground(RAYWHITE);//Limpa e define uma cor de fundo


            for(i = 0; i < LINHA; i++){
                   for(j = 0; j < COLUNA; j++){

                        if(matriz[i][j] == 'W'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][0]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == 'M'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][1]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == 'K'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][2]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == 'P'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][3]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == 'J'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][4]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == 'D'){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][5]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }else if(matriz[i][j] == ' '){
                            DrawRectangle(posX, posY, ARESTA, ARESTA, (Color) cor[0][6]);//Desenha o quadrado na posicao adequada
                            posX += 40;
                        }
                    }
                posX = 0;
                posY += 40;
                }
            }
            EndDrawing();
        }

    CloseWindow();// Fecha janela
    return 0;
}
