#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define QTD_FASES 3
#define QTD_MONSTROS 3
#define QTD_CASAS 12

typedef enum {
    GUERREIRO = 1,
    MAGO,
    ARQUEIRO
} ClassePersonagem;

typedef enum {
    CASA_VAZIA,
    CASA_COMBATE,
    CASA_TESOURO,
    CASA_CURA,
    CASA_ARMADILHA,
    CASA_MISSAO
} TipoCasa;

typedef enum {
    ACAO_ATACAR = 1,
    ACAO_DEFENDER,
    ACAO_HABILIDADE,
    ACAO_FUGIR
} AcaoCombate;

typedef enum {
    MONSTRO_DERROTADO,
    HEROI_FUGIU,
    HEROI_DERROTADO
} ResultadoCombate;

typedef struct {
    char nome[40];
    ClassePersonagem classe;
    int vida;
    int vidaMax;
    int forca;
    int defesa;
    int agilidade;
    int inteligencia;
    int xp;
    int nivel;
    char arma[40];
    char armadura[40];
    int bonusArma;
    int bonusArmadura;
    int habilidadeUsada;
} Personagem;

typedef struct {
    char nome[40];
    char descricao[120];
    int vida;
    int ataque;
    int defesa;
    int nivel;
    int xpRecompensa;
} Monstro;

typedef struct {
    TipoCasa tipo;
    int indiceMonstro;
    int visitada;
} Casa;

typedef struct {
    char nome[60];
    char historia[180];
    char missao[180];
    Monstro monstros[QTD_MONSTROS];
    Casa casas[QTD_CASAS];
} Fase;




static char simboloDaCasa(const Casa *casa)
{
    if (casa->visitada) {
        return 'x';
    }

    switch (casa->tipo) {
        case CASA_COMBATE:
            return 'M';
        case CASA_TESOURO:
            return '$';
        case CASA_CURA:
            return '+';
        case CASA_ARMADILHA:
            return '!';
        case CASA_MISSAO:
            return 'B';
        case CASA_VAZIA:
        default:
            return '.';
    }
}

static void imprimirCasa(const Fase *fase, int indice, int posicaoJogador)
{
    if (indice == posicaoJogador) {
        printf("[*%02d]", indice + 1);
    } else {
        printf("[%02d%c]", indice + 1, simboloDaCasa(&fase->casas[indice]));
    }
}

void mostrarMapa(const Fase *fase, int posicaoJogador)
{
    printf("\nMAPA: %s\n", fase->nome);

    imprimirCasa(fase, 0, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 1, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 2, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 3, posicaoJogador);
    printf("\n                                  |\n");

    imprimirCasa(fase, 7, posicaoJogador);
    printf(" <--- ");
    imprimirCasa(fase, 6, posicaoJogador);
    printf(" <--- ");
    imprimirCasa(fase, 5, posicaoJogador);
    printf(" <--- ");
    imprimirCasa(fase, 4, posicaoJogador);
    printf("\n  |\n");

    imprimirCasa(fase, 8, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 9, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 10, posicaoJogador);
    printf(" ---> ");
    imprimirCasa(fase, 11, posicaoJogador);

    printf("\n\nLegenda: * voce | M combate | B chefe/missao | $ tesouro\n");
    printf("         + cura | ! armadilha | . casa vazia | x casa concluida\n");
}


static int lerInteiro(const char *mensagem, int minimo, int maximo)
{
    char linha[128];
    char *fim;
    long valor;

    while (1) {
        printf("%s", mensagem);

        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\nEntrada encerrada. O jogo sera fechado.\n");
            exit(0);
        }

        valor = strtol(linha, &fim, 10);

        while (*fim == ' ' || *fim == '\t' || *fim == '\r') {
            fim++;
        }

        if (fim == linha || (*fim != '\n' && *fim != '\0')) {
            printf("Digite somente um numero.\n\n");
        } else if (valor < minimo || valor > maximo) {
            printf("Escolha um valor entre %d e %d.\n\n", minimo, maximo);
        } else {
            return (int)valor;
        }
    }
}

static int rolarDado(int lados)
{
    return rand() % lados + 1;
}

static int calcularDano(int ataque, int defesa)
{
    int dano = ataque - defesa;

    if (dano < 1) {
        dano = 1;
    }

    return dano;
}

static void receberDano(Personagem *personagem, int dano)
{
    personagem->vida -= dano;

    if (personagem->vida < 0) {
        personagem->vida = 0;
    }
}

static void danoNoMonstro(Monstro *monstro, int dano)
{
    monstro->vida -= dano;

    if (monstro->vida < 0) {
        monstro->vida = 0;
    }
}

static void curarPersonagem(Personagem *personagem, int quantidade)
{
    personagem->vida += quantidade;

    if (personagem->vida > personagem->vidaMax) {
        personagem->vida = personagem->vidaMax;
    }
}

static void ganharXP(Personagem *personagem, int quantidade)
{
    personagem->xp += quantidade;
    printf("Voce recebeu %d pontos de experiencia.\n", quantidade);

    while (personagem->xp >= personagem->nivel * 100) {
        personagem->nivel++;
        personagem->vidaMax += 10;
        personagem->forca++;
        personagem->defesa++;
        curarPersonagem(personagem, 10);

        printf("\n*** Nivel %d alcancado! ***\n", personagem->nivel);
        printf("Vida maxima, forca e defesa aumentaram.\n");
    }
}

static void aplicarBonusAleatorio(Personagem *personagem)
{
    int bonus = rand() % 5;

    printf("Recompensa extra: ");

    switch (bonus) {
        case 0:
            personagem->forca++;
            printf("+1 de forca.\n");
            break;
        case 1:
            personagem->defesa++;
            printf("+1 de defesa.\n");
            break;
        case 2:
            personagem->agilidade++;
            printf("+1 de agilidade.\n");
            break;
        case 3:
            personagem->inteligencia++;
            printf("+1 de inteligencia.\n");
            break;
        default:
            personagem->vidaMax += 5;
            curarPersonagem(personagem, 5);
            printf("+5 de vida maxima.\n");
            break;
    }
}

static void mostrarPersonagem(const Personagem *personagem)
{
    printf("\n SEU PERSONAGEM \n");
    printf("Nome: %s\n", personagem->nome);
    printf("Nivel: %d | XP: %d\n", personagem->nivel, personagem->xp);
    printf("Vida: %d/%d\n", personagem->vida, personagem->vidaMax);
    printf("Forca: %d | Defesa: %d | Agilidade: %d | Inteligencia: %d\n",
           personagem->forca, personagem->defesa,
           personagem->agilidade, personagem->inteligencia);
    printf("Arma: %s (+%d ataque)\n", personagem->arma, personagem->bonusArma);
    printf("Armadura: %s (+%d defesa)\n",
           personagem->armadura, personagem->bonusArmadura);

    switch (personagem->classe) {
        case GUERREIRO:
            printf("Habilidade: Golpe Pesado - adiciona 10 ao ataque.\n");
            break;
        case MAGO:
            printf("Habilidade: Esfera Arcana - ataque magico e defesa inimiga reduzida.\n");
            break;
        case ARQUEIRO:
            printf("Habilidade: Disparo Preciso - pode causar dano critico.\n");
            break;
        default:
            break;
    }
}

static void criarPersonagem(Personagem *personagem, int escolha)
{
    memset(personagem, 0, sizeof(*personagem));
    personagem->nivel = 1;

    switch (escolha) {
        case GUERREIRO:
            strcpy(personagem->nome, "Guerreiro");
            personagem->classe = GUERREIRO;
            personagem->vidaMax = 120;
            personagem->forca = 12;
            personagem->defesa = 8;
            personagem->agilidade = 5;
            personagem->inteligencia = 5;
            strcpy(personagem->arma, "Espada Longa");
            strcpy(personagem->armadura, "Armadura de Placas");
            personagem->bonusArma = 3;
            personagem->bonusArmadura = 2;
            break;

        case MAGO:
            strcpy(personagem->nome, "Mago");
            personagem->classe = MAGO;
            personagem->vidaMax = 80;
            personagem->forca = 5;
            personagem->defesa = 4;
            personagem->agilidade = 6;
            personagem->inteligencia = 14;
            strcpy(personagem->arma, "Cajado Magico");
            strcpy(personagem->armadura, "Tunica Arcana");
            personagem->bonusArma = 5;
            personagem->bonusArmadura = 1;
            break;

        case ARQUEIRO:
            strcpy(personagem->nome, "Arqueiro");
            personagem->classe = ARQUEIRO;
            personagem->vidaMax = 100;
            personagem->forca = 8;
            personagem->defesa = 6;
            personagem->agilidade = 13;
            personagem->inteligencia = 6;
            strcpy(personagem->arma, "Arco e Flechas");
            strcpy(personagem->armadura, "Armadura de Couro");
            personagem->bonusArma = 4;
            personagem->bonusArmadura = 1;
            break;

        default:
            break;
    }

    personagem->vida = personagem->vidaMax;
}

static void escolherPersonagem(Personagem *personagem)
{
    int escolha;
    int confirmar;

    do {
        printf("\nESCOLHA SEU HEROI\n");
        printf("1 - Guerreiro: muita vida e golpe forte\n");
        printf("2 - Mago: inteligencia alta e magia\n");
        printf("3 - Arqueiro: agilidade e chance de dano critico\n");

        escolha = lerInteiro("Escolha uma classe: ", 1, 3);
        criarPersonagem(personagem, escolha);
        mostrarPersonagem(personagem);
        confirmar = lerInteiro("Confirmar? [1] Sim  [2] Escolher novamente: ", 1, 2);
    } while (confirmar == 2);
}

static void configurarMonstro(Monstro *monstro, const char *nome,
                              const char *descricao, int vida, int ataque,
                              int defesa, int nivel, int xp)
{
    strcpy(monstro->nome, nome);
    strcpy(monstro->descricao, descricao);
    monstro->vida = vida;
    monstro->ataque = ataque;
    monstro->defesa = defesa;
    monstro->nivel = nivel;
    monstro->xpRecompensa = xp;
}

static void definirCasa(Fase *fase, int indice, TipoCasa tipo, int indiceMonstro)
{
    fase->casas[indice].tipo = tipo;
    fase->casas[indice].indiceMonstro = indiceMonstro;
    fase->casas[indice].visitada = 0;
}

static void prepararFase(Fase *fase, const char *nome,
                         const char *historia, const char *missao)
{
    int i;

    strcpy(fase->nome, nome);
    strcpy(fase->historia, historia);
    strcpy(fase->missao, missao);

    for (i = 0; i < QTD_CASAS; i++) {
        definirCasa(fase, i, CASA_VAZIA, -1);
    }
}

static void inicializarFases(Fase fases[])
{
    prepararFase(&fases[0], "Floresta Sombria",
                 "Uma floresta antiga foi tomada por criaturas e uma nevoa estranha.",
                 "Encontrar a Chave Antiga e derrotar o Guardiao da Floresta.");

    configurarMonstro(&fases[0].monstros[0], "Goblin Saqueador",
                      "Um goblin rapido que protege os caminhos da floresta.",
                      34, 7, 3, 1, 35);
    configurarMonstro(&fases[0].monstros[1], "Lobo Cinzento",
                      "Um lobo selvagem que ataca em alta velocidade.",
                      42, 8, 4, 1, 40);
    configurarMonstro(&fases[0].monstros[2], "Guardiao da Floresta",
                      "Uma criatura feita de raizes que protege a chave antiga.",
                      66, 10, 6, 2, 70);

    definirCasa(&fases[0], 1, CASA_COMBATE, 0);
    definirCasa(&fases[0], 2, CASA_TESOURO, -1);
    definirCasa(&fases[0], 3, CASA_CURA, -1);
    definirCasa(&fases[0], 4, CASA_ARMADILHA, -1);
    definirCasa(&fases[0], 5, CASA_COMBATE, 1);
    definirCasa(&fases[0], 7, CASA_TESOURO, -1);
    definirCasa(&fases[0], 9, CASA_CURA, -1);
    definirCasa(&fases[0], 11, CASA_MISSAO, 2);

    prepararFase(&fases[1], "Masmorra Antiga",
                 "A chave abriu a entrada de uma masmorra esquecida, cheia de armadilhas.",
                 "Recuperar o Artefato Antigo derrotando o Cavaleiro Amaldicoado.");

    configurarMonstro(&fases[1].monstros[0], "Aranha Gigante",
                      "Uma aranha enorme que domina os corredores escuros.",
                      62, 10, 6, 2, 60);
    configurarMonstro(&fases[1].monstros[1], "Orc da Cripta",
                      "Um guerreiro orc que guarda os saloes subterraneos.",
                      76, 12, 7, 3, 70);
    configurarMonstro(&fases[1].monstros[2], "Cavaleiro Amaldicoado",
                      "Um cavaleiro antigo preso a uma armadura encantada.",
                      108, 14, 9, 3, 100);

    definirCasa(&fases[1], 1, CASA_COMBATE, 0);
    definirCasa(&fases[1], 2, CASA_ARMADILHA, -1);
    definirCasa(&fases[1], 3, CASA_TESOURO, -1);
    definirCasa(&fases[1], 4, CASA_COMBATE, 1);
    definirCasa(&fases[1], 5, CASA_CURA, -1);
    definirCasa(&fases[1], 7, CASA_ARMADILHA, -1);
    definirCasa(&fases[1], 8, CASA_TESOURO, -1);
    definirCasa(&fases[1], 10, CASA_CURA, -1);
    definirCasa(&fases[1], 11, CASA_MISSAO, 2);

    prepararFase(&fases[2], "Fortaleza do Dragao",
                 "A ultima fortaleza guarda o tesouro lendario e a origem da escuridao.",
                 "Derrotar o Dragao Ancestral e recuperar o Tesouro Lendario.");

    configurarMonstro(&fases[2].monstros[0], "Demonio das Chamas",
                      "Um demonio que espalha fogo pelos saloes da fortaleza.",
                      96, 15, 9, 4, 90);
    configurarMonstro(&fases[2].monstros[1], "Golem de Pedra",
                      "Um guardiao pesado, dificil de ferir.",
                      122, 14, 12, 4, 105);
    configurarMonstro(&fases[2].monstros[2], "Dragao Ancestral",
                      "A criatura lendaria que comanda as forcas da fortaleza.",
                      165, 18, 13, 5, 160);

    definirCasa(&fases[2], 1, CASA_ARMADILHA, -1);
    definirCasa(&fases[2], 2, CASA_COMBATE, 0);
    definirCasa(&fases[2], 3, CASA_TESOURO, -1);
    definirCasa(&fases[2], 4, CASA_CURA, -1);
    definirCasa(&fases[2], 5, CASA_COMBATE, 1);
    definirCasa(&fases[2], 6, CASA_ARMADILHA, -1);
    definirCasa(&fases[2], 7, CASA_TESOURO, -1);
    definirCasa(&fases[2], 9, CASA_CURA, -1);
    definirCasa(&fases[2], 11, CASA_MISSAO, 2);
}

static void mostrarMonstro(const Monstro *monstro)
{
    printf("\nUm inimigo apareceu: %s!\n", monstro->nome);
    printf("%s\n", monstro->descricao);
    printf("Nivel %d | Vida %d | Ataque %d | Defesa %d\n",
           monstro->nivel, monstro->vida, monstro->ataque, monstro->defesa);
}

static int atributoDeAtaque(const Personagem *personagem)
{
    switch (personagem->classe) {
        case MAGO:
            return personagem->inteligencia;
        case ARQUEIRO:
            return personagem->forca + personagem->agilidade / 3;
        case GUERREIRO:
        default:
            return personagem->forca;
    }
}

static void atacarMonstro(Personagem *personagem, Monstro *monstro, int usarHabilidade)
{
    int dado = rolarDado(20);
    int ataque = dado + atributoDeAtaque(personagem) + personagem->bonusArma;
    int defesaUsada = monstro->defesa;
    int dano;

    if (usarHabilidade) {
        if (personagem->classe == GUERREIRO) {
            ataque += 10;
            printf("Golpe Pesado! +10 no ataque.\n");
        } else if (personagem->classe == MAGO) {
            ataque += 8;
            defesaUsada /= 2;
            printf("Esfera Arcana! O ataque ignora parte da defesa inimiga.\n");
        } else if (personagem->classe == ARQUEIRO) {
            int chanceCritico = 25 + personagem->agilidade * 2;
            if (chanceCritico > 70) {
                chanceCritico = 70;
            }

            if (rand() % 100 < chanceCritico) {
                printf("Disparo Preciso: acerto critico!\n");
                dano = calcularDano(ataque, defesaUsada) * 2;
                printf("Voce tirou %d pontos de vida do inimigo.\n", dano);
                danoNoMonstro(monstro, dano);
                return;
            }

            printf("Disparo Preciso: o tiro nao foi critico desta vez.\n");
        }
    }

    dano = calcularDano(ataque, defesaUsada);
    printf("Seu dado foi %d. Ataque total: %d.\n", dado, ataque);
    printf("Voce causou %d de dano.\n", dano);
    danoNoMonstro(monstro, dano);
}

static ResultadoCombate combate(Personagem *personagem, Monstro *monstro)
{
    int acao;
    int defendendo;
    int ataqueMonstro;
    int defesaPersonagem;
    int dano;
    int tentativaFuga;

    personagem->habilidadeUsada = 0;
    mostrarMonstro(monstro);

    while (personagem->vida > 0 && monstro->vida > 0) {
        defendendo = 0;

        printf("%s: %d/%d de vida | %s: %d de vida\n",
               personagem->nome, personagem->vida, personagem->vidaMax,
               monstro->nome, monstro->vida);
        printf("[1] Atacar  [2] Defender  [3] Habilidade especial  [4] Fugir\n");
        acao = lerInteiro("Escolha sua acao: ", 1, 4);

        switch (acao) {
            case ACAO_ATACAR:
                atacarMonstro(personagem, monstro, 0);
                break;

            case ACAO_DEFENDER:
                defendendo = 1;
                printf("Voce preparou a defesa. O proximo dano sera reduzido.\n");
                break;

            case ACAO_HABILIDADE:
                if (personagem->habilidadeUsada) {
                    printf("Voce ja usou sua habilidade neste combate.\n");
                    continue;
                }
                personagem->habilidadeUsada = 1;
                atacarMonstro(personagem, monstro, 1);
                break;

            case ACAO_FUGIR:
                tentativaFuga = rolarDado(6) + personagem->agilidade;
                if (tentativaFuga >= 11) {
                    printf("Voce conseguiu fugir do combate.\n");
                    return HEROI_FUGIU;
                }
                printf("A fuga falhou. O inimigo se prepara para atacar.\n");
                break;

            default:
                break;
        }

        if (monstro->vida <= 0) {
            break;
        }

        ataqueMonstro = rolarDado(20) + monstro->ataque;
        defesaPersonagem = personagem->defesa + personagem->bonusArmadura;
        dano = calcularDano(ataqueMonstro, defesaPersonagem);

        if (defendendo) {
            dano = (dano + 1) / 2;
        }

        printf("%s atacou e causou %d de dano.\n", monstro->nome, dano);
        receberDano(personagem, dano);
    }

    if (personagem->vida <= 0) {
        printf("\nSeu heroi foi derrotado.\n");
        return HEROI_DERROTADO;
    }

    printf("\n%s foi derrotado!\n", monstro->nome);
    ganharXP(personagem, monstro->xpRecompensa);
    aplicarBonusAleatorio(personagem);
    return MONSTRO_DERROTADO;
}

static void moverJogador(int *posicao, int passos)
{
    *posicao += passos;

    if (*posicao >= QTD_CASAS - 1) {
        *posicao = QTD_CASAS - 1;
    }
}

static void recuarJogador(int *posicao)
{
    if (*posicao > 0) {
        (*posicao)--;
    }
    printf("Voce recuou uma casa e podera tentar novamente depois.\n");
}

static int executarEvento(Personagem *personagem, Fase *fase, int *posicao)
{
    Casa *casa = &fase->casas[*posicao];
    Monstro inimigo;
    ResultadoCombate resultado;
    int indiceMonstro;

    if (casa->visitada) {
        printf("Esta casa ja foi explorada. Nada novo por aqui.\n");
        return 0;
    }

    switch (casa->tipo) {
        case CASA_VAZIA:
            printf("A casa esta vazia. O caminho continua.\n");
            casa->visitada = 1;
            break;

        case CASA_TESOURO:
            printf("Voce encontrou um bau com moedas e suprimentos!\n");
            ganharXP(personagem, 20);
            curarPersonagem(personagem, 8);
            casa->visitada = 1;
            break;

        case CASA_CURA:
            printf("Uma fonte de energia recupera 25 pontos de vida.\n");
            curarPersonagem(personagem, 25);
            printf("Vida atual: %d/%d.\n", personagem->vida, personagem->vidaMax);
            casa->visitada = 1;
            break;

        case CASA_ARMADILHA:
            printf("Uma armadilha foi ativada! Voce perdeu 12 pontos de vida.\n");
            receberDano(personagem, 12);
            printf("Vida atual: %d/%d.\n", personagem->vida, personagem->vidaMax);
            casa->visitada = 1;
            break;

        case CASA_COMBATE:
        case CASA_MISSAO:
            indiceMonstro = casa->indiceMonstro;
            if (indiceMonstro < 0 || indiceMonstro >= QTD_MONSTROS) {
                printf("Erro: inimigo nao configurado nesta casa.\n");
                return 0;
            }

            inimigo = fase->monstros[indiceMonstro];
            if (casa->tipo == CASA_MISSAO) {
                printf("\n*** CHEFE DA FASE! ***\n");
                printf("Missao: %s\n", fase->missao);
            }

            resultado = combate(personagem, &inimigo);

            if (resultado == MONSTRO_DERROTADO) {
                casa->visitada = 1;
                if (casa->tipo == CASA_MISSAO) {
                    printf("\nMissao concluida: %s\n", fase->missao);
                    return 1;
                }
            } else if (resultado == HEROI_FUGIU) {
                recuarJogador(posicao);
            }
            break;

        default:
            break;
    }

    return 0;
}

static int verificarMissao(const Fase *fase)
{
    return fase->casas[QTD_CASAS - 1].visitada;
}

static void mostrarInstrucoes(void)
{
    printf("\n========== COMO JOGAR ==========\n");
    printf("1. Escolha Guerreiro, Mago ou Arqueiro.\n");
    printf("2. Em cada rodada, lance o dado de seis lados e avance no mapa.\n");
    printf("3. As casas podem ter combate, tesouro, cura, armadilha ou estar vazias.\n");
    printf("4. No combate, escolha atacar, defender, usar a habilidade ou tentar fugir.\n");
    printf("5. Vencer inimigos concede experiencia e um bonus aleatorio.\n");
    printf("6. Derrote o chefe na casa 12 para concluir a missao e liberar a proxima fase.\n");
    printf("7. O jogo termina com a derrota se sua vida chegar a zero.\n");
}

static void jogar(void)
{
    Personagem jogador;
    Fase fases[QTD_FASES];
    int faseAtual;
    int posicao;
    int dado;
    int missaoConcluida;

    inicializarFases(fases);
    escolherPersonagem(&jogador);

    for (faseAtual = 0; faseAtual < QTD_FASES && jogador.vida > 0; faseAtual++) {
        posicao = 0;
        missaoConcluida = 0;

        printf("FASE %d - %s\n", faseAtual + 1, fases[faseAtual].nome);
        printf("%s\n", fases[faseAtual].historia);
        printf("Missao: %s\n", fases[faseAtual].missao);

        while (jogador.vida > 0 && !missaoConcluida) {
            mostrarMapa(&fases[faseAtual], posicao);
            mostrarPersonagem(&jogador);

            dado = rolarDado(6);
            printf("\nVoce lancou o dado e tirou %d.\n", dado);
            moverJogador(&posicao, dado);
            printf("Voce avancou para a casa %d.\n", posicao + 1);

            missaoConcluida = executarEvento(&jogador, &fases[faseAtual], &posicao);
            if (!missaoConcluida) {
                missaoConcluida = verificarMissao(&fases[faseAtual]);
            }
        }

        if (jogador.vida <= 0) {
            break;
        }

        printf("\nFASE %d CONCLUIDA!\n", faseAtual + 1);
        if (faseAtual < QTD_FASES - 1) {
            printf("Voce encontrou um momento para descansar e recuperou 15 pontos de vida.\n");
            curarPersonagem(&jogador, 15);
        }
    }

    printf("\n========== FIM DE JOGO ==========\n");
    if (jogador.vida > 0) {
        printf("PARABENS! Voce concluiu as tres fases e venceu a aventura!\n");
    } else {
        printf("GAME OVER. Sua aventura terminou antes da ultima missao.\n");
    }

    mostrarPersonagem(&jogador);
}

int main(void)
{
    int opcao;

    srand((unsigned int)time(NULL));
    printf("===========================================\n");
    printf("       A LENDA DOS TRES REINOS\n");
    printf("===========================================\n");

    do {
        printf("\n[1] Novo jogo\n");
        printf("[2] Como jogar\n");
        printf("[3] Sair\n");
        opcao = lerInteiro("Escolha uma opcao: ", 1, 3);

        switch (opcao) {
            case 1:
                jogar();
                break;
            case 2:
                mostrarInstrucoes();
                break;
            case 3:
                printf("Obrigado por jogar!\n");
                break;
            default:
                break;
        }
    } while (opcao != 3);

    return 0;
}
