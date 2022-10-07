#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define ALTURA 480
#define ARESTA 40
#define N 100
#define RAIO 20
#define EXPLOSAO 3
#define QTD_BOMBA 3
#define BORDA 75
#define ESTOQUE 36

enum estadoJogo{
    JOGANDO,
    MENU,
    FIM
};
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
    int vida;
}CRIATURA;

typedef struct{
    double tempo;
    int xPos, yPos;
    int xExp[EXPLOSAO*EXPLOSAO], yExp[EXPLOSAO*EXPLOSAO];
    int estado, efeito;
}BOMBA;

typedef struct{
    int estado;
    int vida;
    int pontos;
    char arquivo[N];
    int xTam, yTam;
    int qtdCriatura;
}JOGO;

int nomeArquivo(char nomeArq[]); // Funcao que ve o nome do proximo arquivo (presume que os ultimos termos sejam numeros)

int verifColisMap(int xPos, int yPos, char mapa[][N], char carac[]);// Retorna 0 caso haja alguma colisao com um elemento do mapa e 1 caso contrario

int verifColisCord(int xPos, int yPos, int xPosPassiv, int yPosPassiv);// Retorna 0 caso as duas coordenadas sejam iguais e 1 caso sejam diferentes

void movDir( int *xPos, int *yPos, int tecla); // Altera o valor da posição baseado no valor apertado no teclado

int podeMover( int xPos, int yPos, char matriz[][N], int tecla, BOMBA bomba[]);// Retorna 1 se o movimento eh valido

int tempoCmp(double tempo, float tempoPerc); // Retorna 1 se o "tempoPerc" tiver sido passado

void plantarBomba(BOMBA *bomba, JOGADOR jogador); // Função que executa as funcionalidades iniciais de plantar a bomba

void expBomba(BOMBA *bomba, int raio); // Função que passa as posicoes x,y da explosao

void controleBomba(BOMBA *bomba, char matriz[][N], int tecla, JOGADOR jogador, CRIATURA criatura[], int qtdCriatura); // Funcao que executa todos os controles de estado da bomba

void controleCriatura(CRIATURA *criatura, char mapa[][N], BOMBA *bomba, int qtdCriatura); // Funcao que generaliza o controle de movimento de monstro

void interacoes(JOGADOR *jogador, BOMBA *bomba, CRIATURA *criatura, char mapa[][N], JOGO *jogo); // Funcao que cuida de todas as interacoes entre os diferentes "objetos" do jogo

void desenhaMapa(char matriz[][N], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], JOGO jogo) ; // funcao que aplica as estilizações as matrizes

void salvarJogo(char mapa[][N], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], JOGO jogo);

void lerEstadoJogo(JOGADOR *jogador, CRIATURA criatura[], BOMBA bomba[], JOGO *jogo);

int leMapa(char nomeArq[], char mapa[][N], JOGADOR *jogador, CRIATURA *criatura, int *xTam, int *yTam);

void initJogo(JOGADOR *jogador, CRIATURA *criatura, BOMBA *bomba, JOGO *jogo);

void controlePontos( int *pontos, int varPontos, int *vida);

int verifFimJogo(char mapa[][N], JOGO jogo);

void resetBomba (BOMBA *bomba);

void desEstoqueBomba(BOMBA bomba[], JOGADOR jogador);

void virarJog(int *direcao, int tecla);


int main(void)
{

    char matriz[N][N];

    //Declaracao de variaveis
    int  tecla, fim = 1;
    JOGADOR jogador;
    CRIATURA criatura[N];
    BOMBA bomba[QTD_BOMBA];
    JOGO jogo;
    double tempCriatura;
    jogo.estado = JOGANDO;
    sprintf(jogo.arquivo,"mapa1.txt");


    //Inicializacoes de variaveis
    srand(time(NULL));
    jogo.qtdCriatura = leMapa( jogo.arquivo, matriz, &jogador, criatura, &jogo.xTam, &jogo.yTam);
    initJogo(&jogador, criatura, bomba, &jogo);

    do{
        //Loop principal
        if(jogo.estado == JOGANDO){
                InitWindow(jogo.xTam * ARESTA, jogo.yTam * ARESTA + BORDA, "Jogo"); // Ajusta o titulo da janela
                SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo
                tempCriatura = GetTime();
                resetBomba(bomba);
            while ( jogo.estado == JOGANDO){// Detecta se deve continuar e faz o jogo acontecer

                tecla = GetKeyPressed();

                // Controle da bomba
                controleBomba(bomba,matriz,tecla,jogador, criatura, jogo.qtdCriatura);

                // Movimentacao do jogador
                switch(tecla){
                    case KEY_DOWN:
                    case KEY_UP:
                    case KEY_LEFT:
                    case KEY_RIGHT:
                        jogador.ultMov = tecla; // Salvando ultimo movimento apertado
                        if(podeMover(jogador.xPos, jogador.yPos, matriz, tecla, bomba))
                            movDir(&jogador.xPos, &jogador.yPos, tecla);
                        break;
                    case KEY_TAB:
                        jogo.estado = MENU;
                        resetBomba(bomba);
                        break;
                    case KEY_W:
                    case KEY_S:
                    case KEY_D:
                    case KEY_A:
                        virarJog(&jogador.ultMov, tecla);
                        break;
                }




                //Controle Criaturas
                if(tempoCmp(tempCriatura,1)){
                        tempCriatura = GetTime();
                        controleCriatura(criatura, matriz, bomba, jogo.qtdCriatura);
                }

                //Interacoes entre personagens
                interacoes(&jogador, bomba, criatura, matriz, &jogo);


                desenhaMapa(matriz, jogador, criatura, bomba, jogo);
                if(!verifFimJogo(matriz, jogo)){
                    nomeArquivo(jogo.arquivo);
                    fim = leMapa(jogo.arquivo,matriz,&jogador,criatura, &jogo.xTam, &jogo.yTam);
                    CloseWindow();
                    InitWindow(jogo.xTam * ARESTA, jogo.yTam * ARESTA + BORDA, "Jogo"); // Ajusta o titulo da janela
                    SetTargetFPS(60);
                }

                if(jogo.vida <= 0|| !fim)
                    jogo.estado = FIM;
            }
            CloseWindow();// Fecha janela

        }

        if(jogo.estado == MENU){
            InitWindow(ALTURA, ALTURA, "Menu"); // Ajusta o titulo da janela
            SetTargetFPS(60);//Ajusta o jogo para executar a 60 quadros por segundo
            while(jogo.estado == MENU){
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("Salvar(S)", ALTURA/3, ALTURA / 10, BORDA/2, BLACK);
                DrawText("Voltar(V)", ALTURA/3, 3 * ALTURA / 10, BORDA/2, BLACK);
                DrawText("Carregar Jogo(C)", ALTURA / 4, 5 * ALTURA / 10 , BORDA/2, BLACK);
                DrawText("Sair do jogo(Q)", ALTURA / 4, 7 * ALTURA / 10, BORDA/2, BLACK);
                DrawText("Novo Jogo(N)", ALTURA / 3 - 10, 9 * ALTURA / 10, BORDA / 2, BLACK);
                EndDrawing();

                tecla = GetKeyPressed();

                switch(tecla){
                case KEY_N:
                    jogo.estado = JOGANDO;
                    jogo.qtdCriatura = leMapa("mapa1.txt",matriz,&jogador,criatura, &jogo.xTam, &jogo.yTam);
                    initJogo(&jogador, criatura, bomba, &jogo);
                    break;
                case KEY_V:
                    jogo.estado = JOGANDO;
                    resetBomba(bomba);
                    break;
                case KEY_S:
                    salvarJogo(matriz,jogador,criatura,bomba, jogo);
                    break;
                case KEY_Q:
                    jogo.estado = FIM;
                    break;
                case KEY_C:
                    jogo.qtdCriatura = leMapa("save.txt", matriz, &jogador, criatura, &jogo.xTam, &jogo.yTam);
                    lerEstadoJogo(&jogador, criatura, bomba, &jogo);
                    jogo.estado = JOGANDO;
                    break;

                }
            }
            CloseWindow();
        }
    }while(jogo.estado != FIM);

    return 0;
}



void desenhaMapa(char matriz[][N],  JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], JOGO jogo){
    int i = 0,j = 0;
    int posX=0, posY=0;
    char texto[N];
    BeginDrawing();
    ClearBackground(BLACK);//Limpa e define uma cor de fundo
        for(i = 0; i < jogo.yTam; i++){
            for(j = 0; j < jogo.xTam; j++){
                switch (matriz[i][j]){
                case 'W':
                    DrawRectangle(j * ARESTA, i * ARESTA, ARESTA, ARESTA, LIGHTGRAY);
                    break;
                case 'P':
                    DrawRectangle(j * ARESTA, i * ARESTA, ARESTA, ARESTA, MAGENTA);
                    break;
                case 'D':
                    DrawRectangle(j * ARESTA, i * ARESTA, ARESTA, ARESTA, VIOLET);
                    break;
                }
            }
        }

        DrawRectangle(jogador.xPos * ARESTA, jogador.yPos * ARESTA, ARESTA, ARESTA, SKYBLUE);

        for(i = 0; i < jogo.qtdCriatura; i++)
           if(criatura[i].vida){
                if(criatura[i].tipo == 'M') // Se for monstro
                   DrawRectangle(criatura[i].xPos * ARESTA, criatura[i].yPos * ARESTA, ARESTA, ARESTA, RED);
                else // Se for criatura magica
                    DrawRectangle(criatura[i].xPos * ARESTA, criatura[i].yPos * ARESTA, ARESTA, ARESTA, YELLOW);

           }

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

        sprintf(texto,"%d \t%d Mapa: %d", jogo.pontos, jogo.vida, nomeArquivo(jogo.arquivo) - 1);
        DrawText( texto, BORDA/2, 40 * jogo.yTam + BORDA/2, BORDA/2, RAYWHITE);

        desEstoqueBomba(bomba, jogador);

        EndDrawing();

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

int podeMover( int xPos, int yPos, char matriz[][N], int tecla, BOMBA bomba[])
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

void controleBomba(BOMBA *bomba, char matriz[][N], int tecla, JOGADOR jogador, CRIATURA criatura[], int qtdCriatura)
{
    int i, j,  espaco_vazio = 1, bombaTplant = 1;

    movDir(&jogador.xPos, &jogador.yPos, jogador.ultMov); // Simulando movimento para ter a posicao em que a bomba sera plantada
    for(i = 0; i < qtdCriatura; i++)
        if(criatura[i].vida && !verifColisCord(jogador.xPos, jogador.yPos, criatura[i].xPos, criatura[i].yPos )){ // Se a bomba tiver tocado em algum monstro
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
        if(tempoCmp(bomba[i].tempo, 3) && bomba[i].estado == SOLTA){ // Se acabar o tempo a bomba explode
            bomba[i].estado = EXPLODIU;
            expBomba(&bomba[i], EXPLOSAO);
            bomba[i].tempo = GetTime();
            bomba[i].efeito = 1;


        }
        else if(tempoCmp(bomba[i].tempo, .15) && bomba[i].estado == EXPLODIU){ // Finalizando a explosao da bomba
            bomba[i].estado = LIVRE;
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

int verifColisMap(int xPos, int yPos, char mapa[][N], char carac[])
{
    int retorno = 1, i, tam;

    tam = strlen(carac);

    for(i = 0; i < tam; i++)
    if((mapa[yPos][xPos] == carac[i]))
        retorno = 0;

   return retorno;

}

void controleCriatura(CRIATURA *criatura, char mapa[][N], BOMBA *bomba, int qtdCriatura)
{
    int i, qtdTeste;
    for(i = 0; i < qtdCriatura; i++){
        if(criatura[i].vida){
            if(criatura[i].movCont == 5 || !podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, bomba)){ // Se tiver se movido 5 vezes ou bater numa parede
                criatura[i].movCont = 0;
                qtdTeste = 0;
                do{
                    criatura[i].direcao = rand()%4 + KEY_RIGHT;
                    qtdTeste++;
                }while(!podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, bomba) && qtdTeste < 10); // Garantindo que a nova direcao seja uma direcao valida
            }

            if(podeMover(criatura[i].xPos, criatura[i].yPos, mapa, criatura[i].direcao, bomba)){ // Executando a movimentacao da criatura
                movDir(&criatura[i].xPos, &criatura[i].yPos, criatura[i].direcao);
                criatura[i].movCont++;
            }
        }
    }
}

void interacoes(JOGADOR *jogador, BOMBA *bomba, CRIATURA *criatura, char mapa[][N], JOGO *jogo)
{
    int i = 0, j, contBomba;
    char mapaCpy [N][N];

    if(!verifColisMap(jogador->xPos,jogador->yPos,mapa, "P")){
        mapa[jogador -> yPos][jogador -> xPos] = ' ';
        controlePontos(&jogo->pontos,50, &jogo->vida);
    }

    for(contBomba = 0; contBomba < QTD_BOMBA; contBomba++){
        if(bomba[contBomba].efeito){// verificacao de colisao da explosao

            for(i = 0; i < EXPLOSAO * EXPLOSAO; i++){

                if(!verifColisMap(bomba[contBomba].xExp[i], bomba[contBomba].yExp[i], mapa, "D")){
                    mapa[bomba[contBomba].yExp[i]][bomba[contBomba].xExp[i]] = ' ';
                    controlePontos(&jogo->pontos,10,&jogo->vida);
                }

                if(!verifColisCord(bomba[contBomba].xExp[i],bomba[contBomba].yExp[i], jogador->xPos, jogador->yPos)){ // Se a bomba explodir no jogador
                    jogo -> vida--;
                    leMapa(jogo->arquivo, mapaCpy, jogador, criatura, &jogo->xTam, &jogo->yTam);
                }

                for(j = 0; j < jogo->qtdCriatura; j++){
                    if(criatura[j].vida && criatura[j].tipo == 'M' && !verifColisCord(bomba[contBomba].xExp[i], bomba[contBomba].yExp[i], criatura[j].xPos, criatura[j].yPos)){ // Se a posicao do monstro for a mesma da bomba
                        criatura[j].vida = 0;
                        controlePontos(&jogo->pontos, 100,&jogo->vida);
                    }
                }
            }
        bomba[contBomba].efeito = 0;
        }
    }

    for(i = 0; i < jogo->qtdCriatura; i++){

        if(criatura[i].vida && !verifColisCord(jogador->xPos, jogador->yPos, criatura[i].xPos, criatura[i].yPos)){ // Se a posicao for igual do monstro
            if(criatura[i].tipo == 'M' ){ // Se passar por um monstro
                controlePontos(&jogo->pontos, -100, &jogo->vida);
                jogo -> vida -= 1;
                leMapa(jogo->arquivo, mapaCpy, jogador, criatura,&jogo->xTam, &jogo->yTam);
                resetBomba(bomba);


            }
            else{                            // Se passar por uma criatura magica
                controlePontos(&jogo->pontos, 10, &jogo->vida);
                criatura[i].vida = 0;
            }
        }

    }
}

void salvarJogo(char mapa[][N], JOGADOR jogador, CRIATURA criatura[], BOMBA bomba[], JOGO jogo)
{
    FILE *arq;
    int i, j;

    //Salvando informações padrão do mapa
    if(!(arq = fopen("save.txt", "w")))
        printf("Erro de salvamento");
    else{
        for(i = 0; i < jogo.yTam; i++){
                for(j = 0; j < jogo.xTam; j++)
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
        fwrite(&jogo, sizeof(JOGO), 1, arq);
        fwrite(criatura, sizeof(CRIATURA), jogo.qtdCriatura, arq);
        fwrite(bomba, sizeof(BOMBA), QTD_BOMBA, arq);
        fclose(arq);
    }
}

void lerEstadoJogo(JOGADOR *jogador, CRIATURA criatura[], BOMBA bomba[], JOGO *jogo)
{
    FILE *arq;
    int i;

    if(!(arq = fopen("save.bin", "rb")))
        printf("Erro de carregamento");
    else{
        fread(jogador, sizeof(JOGADOR), 1, arq);
        fread(jogo, sizeof(JOGO),1,arq);
        fread(criatura, sizeof(CRIATURA), jogo->qtdCriatura, arq);
        fread(bomba, sizeof(BOMBA) ,QTD_BOMBA , arq);

        fclose(arq);
    }
}

int leMapa(char nomeArq[], char mapa[][N], JOGADOR *jogador, CRIATURA *criatura, int *xTam, int *yTam)
{
    FILE *arq;
    int tam = 0,numArq = 0, flagNum = 1, linha = 0, qtd = 0, coluna = 0;
    int temJogador = 0;


    if(!(arq = fopen(nomeArq,"rt"))){
        return 0;
    }

    else{
        while(!feof(arq)){
                mapa[linha][coluna] = getc(arq);
            switch(mapa[linha][coluna]){
                case '\n':
                    mapa[linha][coluna] = '\0';
                    linha++;
                    coluna = -1; // Vai ser adicionado 1 sempre
                    break;

                case 'J':
                    mapa[linha][coluna] = ' ';
                    jogador -> xPos = coluna;
                    jogador -> yPos = linha;
                    break;
                case 'K':
                case 'M':
                    criatura[qtd].xPos = coluna;
                    criatura[qtd].yPos = linha;
                    criatura[qtd].tipo = mapa[linha][coluna];
                    mapa[linha][coluna] = ' ';
                    qtd++;
                    break;
            }
                coluna++;
        }
        fclose(arq);
        *yTam = linha + 1;
        *xTam = strlen(mapa[linha - 1]);

        mapa[linha][strlen(mapa[linha]) - 1] = '\0'; // Ultimo item do mapa nao é \0
        return qtd; // Final para indicar se o jogo eh valido ( tem jogador ou nao)
    }
}

void initJogo(JOGADOR *jogador, CRIATURA *criatura, BOMBA *bomba, JOGO *jogo)
{
    int i;

    for(i = 0; i < QTD_BOMBA; i++){
        bomba[i].efeito = 0;
        bomba[i].estado = LIVRE;
    }

    jogador -> ultMov = KEY_RIGHT;

    jogo -> vida = 3;
    jogo -> pontos = 0;

    for(i = 0; i < jogo->qtdCriatura; i++){ // Zerando o contador de movimento
        criatura[i].movCont = 0;
        criatura[i].direcao = rand() % 4 + KEY_RIGHT;
        criatura[i].vida = 1;
    }
}

void controlePontos( int *pontos, int varPontos, int *vida)
{

    *pontos += varPontos;

    if(*pontos < 0)
        *pontos = 0;
    else if(*pontos - varPontos < 1000 && *pontos >= 1000){
        *vida += 1;
    }

}

int verifFimJogo(char mapa[][N], JOGO jogo)
{
    int i,j, continueJogo = 0;

    for(i = 0; i < jogo.yTam; i++){
        for(j = 0; j < jogo.xTam; j++){
            if(mapa[i][j] == 'P')
                continueJogo = 1;
        }
    }

    return continueJogo;
}

int nomeArquivo(char nomeArq[])
{

    int flagNum = 1, tam = 0, numArq;

    while(flagNum && nomeArq[tam] != '\0'){
        if(nomeArq[tam] <= '9' && nomeArq[tam] >= '0'){
            flagNum = 0;
            numArq = atoi(&nomeArq[tam]) + 1; // Pega o nome do proximo arquivo a ser aberto
            nomeArq[tam] = '\0';
        }
    tam++;
    }

    sprintf(nomeArq,"%s%d.txt",nomeArq,numArq);

    return numArq;
}

void resetBomba (BOMBA *bomba)
{
    int i;
    for(i = 0; i < QTD_BOMBA; i++){
            bomba[i].tempo = GetTime() - bomba[i].tempo;
            bomba[i].efeito = 0;

            if(bomba[i].estado == EXPLODIU)
                bomba[i].estado = LIVRE;
        }
}


void desEstoqueBomba(BOMBA bomba[], JOGADOR jogador)
{
    int varX, varY, basY, basX,i;
    basY = jogador.yPos * ARESTA;
    basX = jogador.xPos * ARESTA;
    switch(jogador.ultMov){
    case KEY_RIGHT:
        varX = 0;
        basX += ESTOQUE/9;
        varY = ESTOQUE/3;
        basY += ESTOQUE/9;
        break;
    case KEY_LEFT:
        varX = 0;
        basX += ESTOQUE;
        varY = -ESTOQUE/3;
        basY += ESTOQUE;
        break;
    case KEY_UP:
        varX = ESTOQUE/3;
        basX += ESTOQUE/9;
        varY = 0;
        basY += ESTOQUE;
        break;
    case KEY_DOWN:
        varX = -ESTOQUE/3;
        basX += ESTOQUE;
        varY = 0;
        basY += ESTOQUE/9;
    }

    for(i = 0; i < QTD_BOMBA; i++)
        if(bomba[i].estado == LIVRE)
            DrawCircle( basX + i * varX , basY + i*varY, ESTOQUE / 4 , WHITE);
}

void virarJog(int *direcao, int tecla)
{
    switch(tecla){
    case KEY_W:
        *direcao = KEY_UP;
        break;
    case KEY_D:
        *direcao = KEY_RIGHT;
        break;
    case KEY_A:
        *direcao = KEY_LEFT;
        break;
    case KEY_S:
        *direcao = KEY_DOWN;
        break;
    }
}
