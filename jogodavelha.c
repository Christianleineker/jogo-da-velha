#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#define MAX 100

typedef struct {
    char nome[50];
    char senha[50];
    int vitorias;
    int empates;
    int derrotas;
    int partidas;
} Usuario;

typedef struct {
    char tabuleiro[3][3];
    char jogadorAtual;
    int concluida;
    char nome1[50];
    char nome2[50];
} Partida;

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

void ocultarSenha(char* senha) {
    int i = 0;
    char ch;
    while ((ch = getch()) != 13 && i < 49) {
        if (ch == 8 && i > 0) {
            i--;
            printf("\b \b");
        } else if (ch != 8) {
            senha[i++] = ch;
            printf("*");
        }
    }
    senha[i] = '\0';
    printf("\n");
}

int autenticar(Usuario* user) {
    FILE* f = fopen("usuarios.txt", "r");
    if (!f) return 0;

    Usuario temp;
    while (fscanf(f, "%49[^,],%49[^,],%d,%d,%d,%d\n", temp.nome, temp.senha, &temp.partidas, &temp.vitorias, &temp.empates, &temp.derrotas) != EOF) {
        if (strcmp(user->nome, temp.nome) == 0 && strcmp(user->senha, temp.senha) == 0) {
            *user = temp;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

void salvarUsuario(Usuario* user) {
    FILE* f = fopen("usuarios.txt", "r");
    FILE* temp = fopen("temp.txt", "w");
    Usuario u;
    int encontrado = 0;

    if (f) {
        while (fscanf(f, "%49[^,],%49[^,],%d,%d,%d,%d\n", u.nome, u.senha, &u.partidas, &u.vitorias, &u.empates, &u.derrotas) != EOF) {
            if (strcmp(user->nome, u.nome) == 0) {
                fprintf(temp, "%s,%s,%d,%d,%d,%d\n", user->nome, user->senha, user->partidas, user->vitorias, user->empates, user->derrotas);
                encontrado = 1;
            } else {
                fprintf(temp, "%s,%s,%d,%d,%d,%d\n", u.nome, u.senha, u.partidas, u.vitorias, u.empates, u.derrotas);
            }
        }
        fclose(f);
    }

    if (!encontrado) {
        fprintf(temp, "%s,%s,%d,%d,%d,%d\n", user->nome, user->senha, user->partidas, user->vitorias, user->empates, user->derrotas);
    }

    fclose(temp);
    remove("usuarios.txt");
    rename("temp.txt", "usuarios.txt");
}

void mostrarTabuleiro(Partida* p) {
    printf("\n");
    for (int i = 0; i < 3; i++) {
        printf(" %c | %c | %c \n", p->tabuleiro[i][0], p->tabuleiro[i][1], p->tabuleiro[i][2]);
        if (i < 2) printf("---|---|---\n");
    }
}

int checarVencedor(Partida* p) {
    for (int i = 0; i < 3; i++) {
        if (p->tabuleiro[i][0] == p->tabuleiro[i][1] && p->tabuleiro[i][1] == p->tabuleiro[i][2] && p->tabuleiro[i][0] != ' ') return 1;
        if (p->tabuleiro[0][i] == p->tabuleiro[1][i] && p->tabuleiro[1][i] == p->tabuleiro[2][i] && p->tabuleiro[0][i] != ' ') return 1;
    }
    if (p->tabuleiro[0][0] == p->tabuleiro[1][1] && p->tabuleiro[1][1] == p->tabuleiro[2][2] && p->tabuleiro[0][0] != ' ') return 1;
    if (p->tabuleiro[0][2] == p->tabuleiro[1][1] && p->tabuleiro[1][1] == p->tabuleiro[2][0] && p->tabuleiro[0][2] != ' ') return 1;
    return 0;
}

int tabuleiroCheio(Partida* p) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (p->tabuleiro[i][j] == ' ')
                return 0;
    return 1;
}

void salvarPartidaNaoFinalizada(Partida* p) {
    FILE* f = fopen("partida_salva.txt", "w");
    fwrite(p, sizeof(Partida), 1, f);
    fclose(f);
}

int carregarPartidaNaoFinalizada(Partida* p) {
    FILE* f = fopen("partida_salva.txt", "r");
    if (!f) return 0;
    fread(p, sizeof(Partida), 1, f);
    fclose(f);
    remove("partida_salva.txt");
    return 1;
}

void jogar(Usuario* user, int comLogin) {
    Partida p;
    memset(&p, 0, sizeof(p));
    strcpy(p.nome1, comLogin ? user->nome : "X");
    strcpy(p.nome2, "O");
    p.jogadorAtual = 'X';

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            p.tabuleiro[i][j] = ' ';

    int x, y;
    while (1) {
        mostrarTabuleiro(&p);
        printf("\nJogador %c - Linha e Coluna (ou -1 -1 para salvar e sair): ", p.jogadorAtual);
        scanf("%d %d", &x, &y);
        if (x == -1 && y == -1) {
            salvarPartidaNaoFinalizada(&p);
            return;
        }
        if (x >= 0 && x < 3 && y >= 0 && y < 3 && p.tabuleiro[x][y] == ' ') {
            p.tabuleiro[x][y] = p.jogadorAtual;
            if (checarVencedor(&p)) {
                mostrarTabuleiro(&p);
                printf("\nJogador %c venceu!\n", p.jogadorAtual);
                if (comLogin) {
                    user->partidas++;
                    if (p.jogadorAtual == 'X') user->vitorias++;
                    else user->derrotas++;
                    salvarUsuario(user);
                }
                return;
            }
            if (tabuleiroCheio(&p)) {
                mostrarTabuleiro(&p);
                printf("\nEmpate!\n");
                if (comLogin) {
                    user->partidas++;
                    user->empates++;
                    salvarUsuario(user);
                }
                return;
            }
            p.jogadorAtual = (p.jogadorAtual == 'X') ? 'O' : 'X';
        }
    }
}

void estatisticas() {
    FILE* f = fopen("usuarios.txt", "r");
    if (!f) return;

    Usuario u;
    printf("\nEstatísticas:\n");
    while (fscanf(f, "%49[^,],%49[^,],%d,%d,%d,%d\n", u.nome, u.senha, &u.partidas, &u.vitorias, &u.empates, &u.derrotas) != EOF) {
        printf("%s: %d partidas, %d vitórias, %d empates, %d derrotas\n", u.nome, u.partidas, u.vitorias, u.empates, u.derrotas);
    }
    fclose(f);
}

int main() {
    int escolha;
    Usuario user;

    while (1) {
        printf("\n1. Login\n2. Jogar sem login\n3. Continuar partida\n4. Estatísticas\n5. Sair\nEscolha: ");
        scanf("%d", &escolha);
        limparBuffer();

        if (escolha == 1) {
            printf("Nome: ");
            fgets(user.nome, 50, stdin);
            user.nome[strcspn(user.nome, "\n")] = 0;
            printf("Senha: ");
            ocultarSenha(user.senha);
            if (!autenticar(&user)) {
                printf("Usuário não encontrado. Criando novo.\n");
                user.partidas = user.vitorias = user.empates = user.derrotas = 0;
                salvarUsuario(&user);
            }
            jogar(&user, 1);
        } else if (escolha == 2) {
            jogar(NULL, 0);
        } else if (escolha == 3) {
            Partida p;
            if (carregarPartidaNaoFinalizada(&p)) {
                printf("\nPartida carregada com sucesso!\n");
                jogar(NULL, 0);
            } else {
                printf("\nNenhuma partida salva encontrada.\n");
            }
        } else if (escolha == 4) {
            estatisticas();
        } else if (escolha == 5) {
            break;
        }
    }
    return 0;
}