#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>    
#include <math.h>       


typedef struct {
    int r, g, b;
} Pixel;


Pixel* lerImagem(const char* nome, int* largura, int* altura) {
    FILE* arq = fopen(nome, "r");  
    if (!arq) {
        printf("Erro ao abrir imagem.\n");
        return NULL;
    }

    char formato[3];
    fscanf(arq, "%s", formato); 
    if (strcmp(formato, "P3") != 0) {
        printf("Formato não suportado.\n");
        fclose(arq);
        return NULL;
    }

    fscanf(arq, "%d %d", largura, altura);  
    int max;
    fscanf(arq, "%d", &max);  

    // Aloca memória para os pixels
    Pixel* imagem = malloc((*largura) * (*altura) * sizeof(Pixel));

    // Lê os pixels RGB (um por um)
    for (int i = 0; i < (*largura) * (*altura); i++) {
        fscanf(arq, "%d", &imagem[i].r);
        fscanf(arq, "%d", &imagem[i].g);
        fscanf(arq, "%d", &imagem[i].b);
    }

    fclose(arq);  // Fecha o arquivo
    return imagem;
}

// Função para salvar uma imagem no formato PPM 
void salvarImagem(const char* nome, Pixel* img, int largura, int altura) {
    FILE* arq = fopen(nome, "w");  // Abre para escrita
    fprintf(arq, "P3\n%d %d\n255\n", largura, altura); 

    // Escreve os pixels RGB
    for (int i = 0; i < largura * altura; i++) {
        fprintf(arq, "%d %d %d\n", img[i].r, img[i].g, img[i].b);
    }
    fclose(arq);
}

// Converte imagem colorida para tons de cinza
void tonsDeCinza(Pixel* img, int total) {
    for (int i = 0; i < total; i++) {
        int cinza = img[i].r * 0.299 + img[i].g * 0.587 + img[i].b * 0.114;
        img[i].r = img[i].g = img[i].b = cinza;
    }
}

// Gera imagem negativa (255 - valor de cada canal)
void negativo(Pixel* img, int total) {
    for (int i = 0; i < total; i++) {
        img[i].r = 255 - img[i].r;
        img[i].g = 255 - img[i].g;
        img[i].b = 255 - img[i].b;
    }
}

// Aplica efeito raio-x: cinza elevado a 1.5
void raioX(Pixel* img, int total) {
    for (int i = 0; i < total; i++) {
        int cinza = img[i].r * 0.299 + img[i].g * 0.587 + img[i].b * 0.114;
        int novo = pow(cinza, 1.5);  // Aumenta contraste
        if (novo > 255) novo = 255; // Garante limite de 255
        img[i].r = img[i].g = img[i].b = novo;
    }
}

// Aplica efeito sépia: aumenta R e G, reduz B
void sepia(Pixel* img, int total) {
    for (int i = 0; i < total; i++) {
        int r = img[i].r;
        int g = img[i].g;
        int b = img[i].b;

        int novaR = fmin(255, r * 1.1 + 10);
        int novaG = fmin(255, g * 1.1 + 10);
        int novaB = fmax(0, b * 0.9 - 10);

        img[i].r = novaR;
        img[i].g = novaG;
        img[i].b = novaB;
    }
}

// rotaciona imagem 90°
Pixel* rotacionar90(Pixel* img, int* largura, int* altura) {
    int novaLargura = *altura;
    int novaAltura = *largura;

    Pixel* novaImg = malloc(novaLargura * novaAltura * sizeof(Pixel));
    if (!novaImg) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    for (int y = 0; y < *altura; y++) {         // altura original
        for (int x = 0; x < *largura; x++) {     // largura original
            int novoX = y;                       // vira nova largura
            int novoY = novaAltura - 1 - x;       // espelha verticalmente
            novaImg[novoY * novaLargura + novoX] = img[y * (*largura) + x];
        }
    }

    *largura = novaLargura;
    *altura = novaAltura;
    return novaImg;
}

// Rotaciona imagem 180° (inverte todos os pixels)
void rotacionar180(Pixel* img, int largura, int altura) {
    int total = largura * altura;
    for (int i = 0; i < total / 2; i++) {
        Pixel temp = img[i];
        img[i] = img[total - 1 - i];
        img[total - 1 - i] = temp;
    }
}

int main() {
    char nome[100];
    printf("Digite o nome da imagem : ");
    scanf("%s", nome);

    int largura, altura;
    Pixel* img = lerImagem(nome, &largura, &altura);
    if (!img) return 1;

    printf("\nEscolha uma transformacao:\n");
    printf("1 - Tons de cinza\n");
    printf("2 - Negativo\n");
    printf("3 - Raio-X\n");
    printf("4 - Sepia\n");
    printf("5 - Rotacionar 90 graus\n");
    printf("6 - Rotacionar 180 graus\n");

    int op;
    printf("Opcao: ");
    scanf("%d", &op);

    if (op == 1) tonsDeCinza(img, largura * altura);
    else if (op == 2) negativo(img, largura * altura);
    else if (op == 3) raioX(img, largura * altura);
    else if (op == 4) sepia(img, largura * altura);
    else if (op == 5) {
        Pixel* nova = rotacionar90(img, &largura, &altura); // passa largura/altura por referência
        free(img);  // libera a imagem original
        img = nova; // aponta img para a imagem rotacionada
    }
    
    else if (op == 6) rotacionar180(img, largura, altura);
    else {
        printf("Opcao invalida.\n");
        free(img);
        return 1;
    }

    // Salva o resultado
    salvarImagem("imagemAlterada.ppm", img, largura, altura);
    printf("\nImagem transformada salva como 'imagemAlterada.ppm'\n");

    free(img);  // Libera memória
    return 0;
}
