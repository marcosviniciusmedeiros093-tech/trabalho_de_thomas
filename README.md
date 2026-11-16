# A Lenda dos Três Reinos
PS: eu só vi que era pra ter github no quando eu terminei o projeto, logo vai ficar sem historico, NO GRUPO QUE VC MANDOU A IMAGEM MADE TODAS AS ESPECIFICAÇÕES QUE VOCE VAI COBRAR, tmb é importante dizer que por causa desse prazo RIDICULO o README foi feito com auxilio de IA
**Aluno:** Marcos Vinícius de Medeiros Silva
**Matrícula:** 20260015488
**Disciplina:** DCT1106 — Programação
**Curso:** Bacharelado em Sistemas da Informação (BSI) — UFRN/DCT
**Professor:** Thommas K. S. Flores
**Período letivo:** 2026.2

Jogo de tabuleiro RPG em modo texto, desenvolvido integralmente em linguagem C, executado no terminal. O jogador escolhe um herói, avança por um tabuleiro de 12 casas lançando um dado de 6 faces, enfrenta monstros em combates por turnos, coleta recompensas, evolui seus atributos e cumpre a missão de cada uma das três fases para vencer.

---

## Sumário

1. [História do jogo](#1-história-do-jogo)
2. [Requisitos da disciplina atendidos](#2-requisitos-da-disciplina-atendidos)
3. [Personagens jogáveis](#3-personagens-jogáveis)
4. [Fases, monstros e missões](#4-fases-monstros-e-missões)
5. [Dinâmica do jogo](#5-dinâmica-do-jogo)
6. [Tutorial de compilação e execução](#6-tutorial-de-compilação-e-execução)
7. [Estrutura do código](#7-estrutura-do-código)
8. [Tratamento de entradas inválidas](#8-tratamento-de-entradas-inválidas)
9. [Imagens dos personagens e monstros](#9-imagens-dos-personagens-e-monstros)
10. [Vídeo de demonstração](#10-vídeo-de-demonstração)

---

## 1. História do jogo

Uma névoa estranha tomou conta do reino. Uma antiga Chave foi selada na **Floresta Sombria**, vigiada por um guardião feito de raízes. Atrás da porta que a Chave abre, dorme uma **Masmorra Antiga** onde um cavaleiro amaldiçoado guarda o Artefato perdido. No fim de tudo, na **Fortaleza do Dragão**, a origem da escuridão espera: o **Dragão Ancestral**, senhor do Tesouro Lendário.

Um único herói aceita a travessia. Ele deverá sobreviver às três fases, derrotar os nove monstros que as habitam e chegar ao fim da aventura sem que sua vida chegue a zero.

**Objetivo final:** concluir as missões das três fases (vencendo o chefe de cada uma) para recuperar o Tesouro Lendário e vencer o jogo.

---

## 2. Requisitos da disciplina atendidos

| Nº | Requisito mínimo | Como o projeto atende |
|----|------------------|-----------------------|
| 1 | Personagens jogáveis | 3 classes distintas (Guerreiro, Mago, Arqueiro), cada uma com atributos próprios, arma, armadura e uma **habilidade especial com efeito de jogo diferente**. |
| 2 | Monstros por fase | **3 monstros diferentes em cada uma das 3 fases** (9 no total), com vida/ataque/defesa/nível/XP próprios e dificuldade crescente. |
| 3 | Fases | **3 fases** com história, missão principal e chefe próprios; a fase seguinte só é liberada após a conclusão da missão atual. |
| 4 | Uso de `struct` | Structs para `Personagem`, `Monstro`, `Casa` e `Fase` (o tabuleiro completo de cada fase está dentro da struct `Fase`). |
| 5 | Uso de `enum` | Quatro enumerações: `ClassePersonagem`, `TipoCasa`, `AcaoCombate` e `ResultadoCombate`. Nenhum "número mágico" nas regras. |
| 6 | Ponteiros | Toda alteração de estado (vida, vida máxima, XP, nível, atributos, casas visitadas, posição no tabuleiro) é feita por funções que recebem **ponteiros** para as structs. Nenhuma função devolve cópia da struct para a `main` sobrescrever. |
| 7 | Modularização | `main` contém apenas o laço do menu; as regras estão distribuídas em ~30 funções coesas com responsabilidade única. |

**Requisitos técnicos do enunciado:**

- Linguagem: **C99** (compila também em ANSI C).
- Bibliotecas: apenas a biblioteca padrão — `stdio.h`, `stdlib.h`, `string.h`, `time.h`. Nenhuma biblioteca externa.
- Persistência: **nenhuma** — todo o estado vive em memória, como pede o enunciado.
- Interface: 100 % terminal (texto).
- Compilação: sem erros e sem warnings com `gcc -Wall -Wextra -std=c99`.
- Aleatoriedade: `srand(time(NULL))` uma única vez em `main`, e `rand()` encapsulado em `rolarDado()`.

---

## 3. Personagens jogáveis

Ao iniciar um novo jogo o programa exibe as três classes, o jogador escolhe uma e confirma (é possível voltar e escolher novamente antes de começar).

### 3.1 Tabela comparativa

| Classe | Vida máx. | Força | Defesa | Agilidade | Inteligência | Arma (bônus) | Armadura (bônus) | Defesa efetiva |
|--------|-----------|-------|--------|-----------|--------------|--------------|------------------|----------------|
| **Guerreiro** | 120 | 12 | 8 | 5 | 5 | Espada Longa (+3 ataque) | Armadura de Placas (+2 defesa) | **10** |
| **Mago** | 80 | 5 | 4 | 6 | 14 | Cajado Mágico (+5 ataque) | Túnica Arcana (+1 defesa) | **5** |
| **Arqueiro** | 100 | 8 | 6 | 13 | 6 | Arco e Flechas (+4 ataque) | Armadura de Couro (+1 defesa) | **7** |

> **Defesa efetiva** é o valor realmente usado nos cálculos: `defesa + bonusArmadura`. O Guerreiro tem a maior defesa efetiva (10) e também a maior vida (120), mas o menor valor de agilidade e inteligência; o Mago compensa a defesa baixa com inteligência 14 e bônus de arma +5; o Arqueiro fica no meio, com agilidade 13 (fuga e crítico).

Todos começam no **nível 1**, com **0 XP** e vida igual à vida máxima.

### 3.2 Atributo usado no ataque

O atributo que entra na fórmula de ataque depende da classe (`atributoDeAtaque`):

| Classe | Atributo de ataque |
|--------|--------------------|
| Guerreiro | Força |
| Mago | Inteligência |
| Arqueiro | Força + Agilidade / 3 |

### 3.3 Habilidades especiais

Cada habilidade pode ser usada **uma única vez por combate** (controlado por `personagem->habilidadeUsada`).

| Classe | Habilidade | Efeito de jogo |
|--------|------------|----------------|
| **Guerreiro** | **Golpe Pesado** | Soma **+10** ao ataque do turno. |
| **Mago** | **Esfera Arcana** | Soma **+8** ao ataque **e reduz a defesa do inimigo pela metade** naquele ataque (ignora parte da armadura). |
| **Arqueiro** | **Disparo Preciso** | Chance de crítico de `25 + agilidade × 2` % (limitada a 70 %). Se acertar, **dobra o dano**. |

---

## 4. Fases, monstros e missões

Cada fase possui **12 casas**, **3 monstros** e **1 missão**. O chefe fica sempre na **casa 12**; derrotá-lo conclui a missão e libera a fase seguinte. Ao concluir uma fase (exceto a última) o herói **recupera 15 pontos de vida** como descanso.

### 4.1 Visão geral

| Fase | Nome | Missão | Chefe |
|------|------|--------|-------|
| 1 | Floresta Sombria | Encontrar a Chave Antiga e derrotar o Guardião da Floresta | Guardião da Floresta |
| 2 | Masmorra Antiga | Recuperar o Artefato Antigo derrotando o Cavaleiro Amaldiçoado | Cavaleiro Amaldiçoado |
| 3 | Fortaleza do Dragão | Derrotar o Dragão Ancestral e recuperar o Tesouro Lendário | Dragão Ancestral |

### 4.2 Monstros por fase

**Fase 1 — Floresta Sombria**
*Uma floresta antiga foi tomada por criaturas e uma névoa estranha.*

| Monstro | Descrição | Vida | Ataque | Defesa | Nível | XP |
|---------|-----------|------|--------|--------|-------|----|
| Goblin Saqueador | Um goblin rápido que protege os caminhos da floresta. | 34 | 7 | 3 | 1 | 35 |
| Lobo Cinzento | Um lobo selvagem que ataca em alta velocidade. | 42 | 8 | 4 | 1 | 40 |
| Guardião da Floresta *(chefe)* | Uma criatura feita de raízes que protege a chave antiga. | 66 | 10 | 6 | 2 | 70 |

**Fase 2 — Masmorra Antiga**
*A chave abriu a entrada de uma masmorra esquecida, cheia de armadilhas.*

| Monstro | Descrição | Vida | Ataque | Defesa | Nível | XP |
|---------|-----------|------|--------|--------|-------|----|
| Aranha Gigante | Uma aranha enorme que domina os corredores escuros. | 62 | 10 | 6 | 2 | 60 |
| Orc da Cripta | Um guerreiro orc que guarda os salões subterrâneos. | 76 | 12 | 7 | 3 | 70 |
| Cavaleiro Amaldiçoado *(chefe)* | Um cavaleiro antigo preso a uma armadura encantada. | 108 | 14 | 9 | 3 | 100 |

**Fase 3 — Fortaleza do Dragão**
*A última fortaleza guarda o tesouro lendário e a origem da escuridão.*

| Monstro | Descrição | Vida | Ataque | Defesa | Nível | XP |
|---------|-----------|------|--------|--------|-------|----|
| Demônio das Chamas | Um demônio que espalha fogo pelos salões da fortaleza. | 96 | 15 | 9 | 4 | 90 |
| Golem de Pedra | Um guardião pesado, difícil de ferir. | 122 | 14 | 12 | 4 | 105 |
| Dragão Ancestral *(chefe)* | A criatura lendária que comanda as forças da fortaleza. | 165 | 18 | 13 | 5 | 160 |

> A progressão de dificuldade é explícita nos números: vida dos monstros de 34 → 165, ataque de 7 → 18, defesa de 3 → 13 e nível 1 → 5.

### 4.3 Tabuleiro de cada fase

O caminho tem 12 casas em "serpente": a linha de cima vai da casa 1 à 4, a do meio volta da 8 à 5, e a de baixo vai da 9 à 12.

```
[01.] ---> [02M] ---> [03$] ---> [04+]
                                  |
[08$] <--- [07!] <--- [06M] <--- [05!]      <- exemplo ilustrativo de leitura
  |
[09.] ---> [10.] ---> [11.] ---> [12B]
```

Configuração real de cada fase (por número da casa):

| Casa | Fase 1 — Floresta | Fase 2 — Masmorra | Fase 3 — Fortaleza |
|------|-------------------|-------------------|--------------------|
| 1 | vazia | vazia | vazia |
| 2 | Combate (Goblin) | Combate (Aranha) | Armadilha |
| 3 | Tesouro | Armadilha | Combate (Demônio) |
| 4 | Cura | Tesouro | Tesouro |
| 5 | Armadilha | Combate (Orc) | Cura |
| 6 | Combate (Lobo) | Cura | Combate (Golem) |
| 7 | vazia | vazia | Armadilha |
| 8 | Tesouro | Armadilha | Tesouro |
| 9 | vazia | Tesouro | vazia |
| 10 | Cura | vazia | Cura |
| 11 | vazia | Cura | vazia |
| 12 | **Missão / Chefe** (Guardião) | **Missão / Chefe** (Cavaleiro) | **Missão / Chefe** (Dragão) |

### 4.4 Legenda do mapa exibido no terminal

| Símbolo | Significado |
|---------|-------------|
| `[*07]` | Posição atual do herói |
| `M` | Casa de combate |
| `B` | Chefe / missão |
| `$` | Tesouro |
| `+` | Cura |
| `!` | Armadilha |
| `.` | Casa vazia |
| `x` | Casa já concluída (visitada) |

---

## 5. Dinâmica do jogo

### 5.1 Fluxo geral

```
Início
 └─ Menu principal: [1] Novo jogo   [2] Como jogar   [3] Sair
     └─ 1. Escolher o personagem (classe + confirmação)
         └─ 2. Iniciar a Fase N: exibir história e missão
             └─ 3. Lançar o d6 e mover o herói no tabuleiro
                 └─ 4. Executar o evento da casa
                     ├─ vida do herói = 0?  → GAME OVER
                     └─ missão da fase cumprida?
                         ├─ não → volta ao passo 3
                         └─ sim → era a última fase?
                                   ├─ não → Fase N+1 (passo 2)
                                   └─ sim → VITÓRIA
```

### 5.2 Movimentação

- O jogador lança um **d6** (`rand() % 6 + 1`) e avança esse número de casas.
- O limite do tabuleiro é tratado: se a posição ultrapassar a casa 12, o herói **para na casa 12** (`moverJogador` faz o *clamp*). Nunca é possível sair do tabuleiro.
- Se o herói **fugir** de um combate, ele **recua uma casa** (`recuarJogador`, que também respeita o limite inferior 1) e poderá tentar novamente depois — a casa permanece não visitada.
- Casas **já visitadas** não disparam eventos novamente.

### 5.3 Tipos de casa e efeitos

| Casa | Efeito ao cair nela |
|------|---------------------|
| **Vazia** (`.`) | Mensagem narrativa; nada acontece. Casa marcada como visitada. |
| **Combate** (`M`) | Inicia combate por turnos contra o monstro configurado na casa. |
| **Tesouro** (`$`) | Ganha **+20 XP** e recupera **+8 de vida**. |
| **Cura** (`+`) | Recupera **+25 de vida** (limitado à vida máxima). |
| **Armadilha** (`!`) | Perde **12 de vida** (a vida nunca fica negativa). |
| **Missão / Chefe** (`B`) | Combate contra o chefe da fase. Se vencer, a **missão é concluída** e a fase termina. |

### 5.4 Combate por turnos

```
C1. Monstro aparece (nome, descrição, nível, vida, ataque, defesa)
C2. Jogador escolhe: [1] Atacar  [2] Defender  [3] Habilidade  [4] Fugir
C3. Fugiu com sucesso? → fim do combate, sem recompensa, recua 1 casa
C4. Jogador rola d20 e calcula ataque e dano
C5. Dano é aplicado na vida do monstro (via ponteiro)
C6. Monstro morreu? → XP + bônus aleatório (via ponteiro) → volta ao tabuleiro
C7. Senão, o monstro rola d20, ataca e aplica dano no herói (via ponteiro)
C8. Herói morreu? → GAME OVER. Senão → novo turno (volta a C2)
```

**Ações disponíveis**

| Ação | Comportamento |
|------|---------------|
| **Atacar** | Ataque normal com d20. |
| **Defender** | O herói não ataca; o dano do próximo ataque do monstro é **reduzido à metade** (`dano = (dano + 1) / 2`, arredondando para cima). |
| **Habilidade especial** | Usa a habilidade da classe — **apenas uma vez por combate**. Se já usada, o turno é reaproveitado sem consumir a ação. |
| **Fugir** | Teste `d6 + agilidade ≥ 11`. Sucesso → combate encerrado sem recompensa e o herói recua uma casa. Falha → o monstro ataca normalmente. |

### 5.5 Fórmulas de ataque, defesa e dano

| Cálculo | Fórmula implementada |
|---------|----------------------|
| Dado | `rolarDado(lados) = rand() % lados + 1` |
| Ataque do herói | `d20 + atributoDeAtaque(classe) + bonusArma` |
| Dano ao monstro | `max(1, Ataque − Defesa do monstro)` |
| Dano ao monstro (Mago, Esfera Arcana) | `max(1, (Ataque+8) − DefesaMonstro/2)` |
| Dano ao monstro (Arqueiro, crítico) | `2 × max(1, Ataque − Defesa do monstro)` |
| Ataque do monstro | `d20 + ataque base do monstro` |
| Defesa efetiva do herói | `defesa + bonusArmadura` |
| Dano ao herói | `max(1, Ataque do monstro − Defesa efetiva)`; se o herói estiver defendendo, `(dano + 1) / 2` |
| Chance de crítico (Arqueiro) | `min(70, 25 + agilidade × 2)` % |
| Teste de fuga | `d6 + agilidade ≥ 11` |

Como o dano mínimo é sempre 1, **nenhum ataque é inútil**: todo combate é finito e termina.

**Exemplo numérico** — Guerreiro (vida 120, força 12, defesa 8 + 2 de armadura = defesa efetiva 10, bônus de arma +3) contra o Goblin Saqueador (vida 34, ataque 7, defesa 3):

| Turno | Ação do herói | Reação do monstro |
|-------|---------------|-------------------|
| 1 | d20 = 14 → Ataque = 14 + 12 + 3 = 29 → Dano = max(1, 29 − 3) = **26**. Goblin: 34 → 8. | d20 = 9 → Ataque = 9 + 7 = 16 → Dano = max(1, 16 − 10) = **6**. Herói: 120 → 114. |
| 2 | d20 = 8 → Ataque = 8 + 12 + 3 = 23 → Dano = max(1, 23 − 3) = **20**. Goblin: 8 → 0. | Goblin derrotado: não reage. Herói recebe **35 XP** + bônus aleatório de atributo. |

O mesmo Guerreiro, defendendo-se no turno 1, sofreria `(6 + 1) / 2 = 3` de dano em vez de 6 — útil contra o Dragão Ancestral (ataque 18).

### 5.6 Recompensas e evolução

**Ao derrotar um monstro:**
1. Recebe o **XP** de recompensa do monstro (`ganharXP`, via ponteiro).
2. Recebe um **bônus aleatório de atributo** (`aplicarBonusAleatorio`, via ponteiro) sorteado entre 5 possibilidades: **+1 Força**, **+1 Defesa**, **+1 Agilidade**, **+1 Inteligência** ou **+5 Vida máxima** (curando 5 na hora).

**Subida de nível** (`ganharXP`): enquanto `xp ≥ nível × 100`, o herói sobe um nível e recebe:
- **+10 de vida máxima**
- **+1 de Força**
- **+1 de Defesa**
- **+10 de vida** (cura imediata)

Ou seja, o XP funciona como uma barreira cumulativa: nível 2 custa 100 XP, nível 3 custa 300 XP no total (200 adicionais), e assim por diante.

### 5.7 Condições de vitória e derrota

| Resultado | Condição |
|-----------|----------|
| **Vitória** | Vencer o chefe (casa 12) da **terceira fase** — ou seja, concluir as três missões. O programa exibe "PARABÉNS! Você concluiu as três fases e venceu a aventura!" e o resumo final do herói. |
| **Derrota (GAME OVER)** | A vida do herói chega a **0** em qualquer combate ou armadilha. O programa exibe "GAME OVER" e o resumo do herói. |

---

## 6. Tutorial de compilação e execução

### 6.1 Pré-requisitos

Um compilador C (`gcc`) e um terminal. O projeto usa **apenas a biblioteca padrão**, portanto **não é necessário** instalar bibliotecas adicionais nem usar a flag `-lm`.

**Linux (Debian/Ubuntu):**
```bash
sudo apt update && sudo apt install build-essential
```

**macOS:** instale as *Command Line Tools* da Apple:
```bash
xcode-select --install
```

**Windows:** instale o [MinGW-w64](https://www.mingw-w64.org/) (ou use o GCC dentro do WSL). Depois de instalado, adicione a pasta `bin` do MinGW ao `PATH` para que o comando `gcc` funcione no Prompt de Comando / PowerShell.

### 6.2 Compilar

Estando na raiz do projeto (onde está o arquivo `.c`):

**Linux / macOS / WSL:**
```bash
gcc -Wall -Wextra -std=c99 -O2 -o jogo main.c
```

**Windows (PowerShell ou CMD):**
```bat
gcc -Wall -Wextra -std=c99 -O2 -o jogo.exe main.c
```

> Compila sem erros e sem warnings. Se o seu código estiver dividido em vários arquivos `.c`, compile todos de uma vez: `gcc ... -o jogo src/*.c`.

### 6.3 Executar

**Linux / macOS:**
```bash
./jogo
```

**Windows:**
```bat
jogo.exe
```

### 6.4 Primeiros passos no jogo

1. Escolha `[1] Novo jogo` no menu principal.
2. Escolha uma classe (`1` Guerreiro, `2` Mago, `3` Arqueiro), veja a ficha e confirme com `1`.
3. Leia a história e a missão da fase, observe o mapa e pressione `Enter`/digite a opção pedida.
4. A cada rodada o **d6 é rolado automaticamente** — o herói avança e o evento da casa que ele ocupar é resolvido.
5. Em combate, use `1` Atacar, `2` Defender, `3` Habilidade (uma vez por combate) ou `4` Fugir.
6. Derrote o chefe da casa 12 para concluir a fase e seguir para a próxima.
7. Ao final das três fases, o jogo exibe a vitória e o resumo do personagem.

## 7. Estrutura do código

### 7.1 Constantes

```c
#define QTD_FASES     3   /* número de fases do jogo        */
#define QTD_MONSTROS  3   /* monstros por fase              */
#define QTD_CASAS     12  /* casas do tabuleiro por fase    */
```

### 7.2 Enumerações (`enum`)

| Enum | Valores | Papel no jogo |
|------|---------|---------------|
| `ClassePersonagem` | `GUERREIRO = 1`, `MAGO`, `ARQUEIRO` | Classe do herói; define atributo de ataque e habilidade especial. |
| `TipoCasa` | `CASA_VAZIA`, `CASA_COMBATE`, `CASA_TESOURO`, `CASA_CURA`, `CASA_ARMADILHA`, `CASA_MISSAO` | Tipo de evento de cada casa do tabuleiro. |
| `AcaoCombate` | `ACAO_ATACAR = 1`, `ACAO_DEFENDER`, `ACAO_HABILIDADE`, `ACAO_FUGIR` | Ações do jogador no turno de combate. |
| `ResultadoCombate` | `MONSTRO_DERROTADO`, `HEROI_FUGIU`, `HEROI_DERROTADO` | Retorno de `combate()`, usado para decidir o que fazer no tabuleiro. |

### 7.3 Estruturas (`struct`)

| Struct | Campos principais |
|--------|-------------------|
| `Personagem` | `nome`, `classe` (enum), `vida`, `vidaMax`, `forca`, `defesa`, `agilidade`, `inteligencia`, `xp`, `nivel`, `arma`, `armadura`, `bonusArma`, `bonusArmadura`, `habilidadeUsada` |
| `Monstro` | `nome`, `descricao`, `vida`, `ataque`, `defesa`, `nivel`, `xpRecompensa` |
| `Casa` | `tipo` (enum), `indiceMonstro` (índice em `Fase.monstros`, `-1` se não houver), `visitada` |
| `Fase` | `nome`, `historia`, `missao`, `monstros[3]`, `casas[12]` |

O tabuleiro é, portanto, um **vetor de structs** `Casa` dentro da struct `Fase` — cada fase tem seu próprio tabuleiro, monstros e missão.

### 7.4 Principais funções

**Interface / exibição**

| Função | Responsabilidade |
|--------|------------------|
| `main` | Laço do menu principal (novo jogo, instruções, sair) e `srand(time(NULL))`. |
| `mostrarInstrucoes` | Tela "Como jogar". |
| `mostrarMapa` | Desenha o tabuleiro em três linhas, com posição atual e símbolos. |
| `simboloDaCasa` / `imprimirCasa` | Símbolo de cada tipo de casa e formatação de cada casa no mapa. |
| `mostrarPersonagem` | Ficha completa do herói (atributos, equipamentos, habilidade). |
| `mostrarMonstro` | Anúncio do inimigo com nível, vida, ataque e defesa. |

**Criação e configuração**

| Função | Responsabilidade |
|--------|------------------|
| `criarPersonagem` | Preenche a struct do herói conforme a classe escolhida. |
| `escolherPersonagem` | Menu de escolha da classe com confirmação. |
| `configurarMonstro` | Preenche um monstro com nome, descrição e atributos. |
| `definirCasa` | Configura uma casa (tipo, monstro, não visitada). |
| `prepararFase` | Zera o tabuleiro e grava nome/história/missão da fase. |
| `inicializarFases` | Define as 3 fases, seus 9 monstros e os 36 eventos de casa. |

**Mecânica e aleatoriedade**

| Função | Responsabilidade |
|--------|------------------|
| `rolarDado(int lados)` | `rand() % lados + 1` — usado para d6 e d20. |
| `calcularDano` | `max(1, ataque − defesa)`. |
| `atributoDeAtaque` | Escolhe força/inteligência/força+agilidade conforme a classe. |
| `atacarMonstro` | Rola d20, aplica bônus de arma e habilidade, calcula e aplica o dano. |
| `combate` | Laço completo do combate por turnos; devolve `ResultadoCombate`. |
| `moverJogador(int *posicao, int passos)` | Avança e trata o limite do tabuleiro. |
| `recuarJogador(int *posicao)` | Recuo de uma casa após fuga. |
| `executarEvento` | Resolve o evento da casa atual; devolve 1 se a missão foi concluída. |

**Atualização de estado por ponteiro** (requisito 6)

| Função | Assinatura | Efeito |
|--------|------------|--------|
| `receberDano` | `(Personagem *p, int dano)` | Reduz vida, com piso em 0. |
| `danoNoMonstro` | `(Monstro *m, int dano)` | Reduz vida do monstro, com piso em 0. |
| `curarPersonagem` | `(Personagem *p, int qtd)` | Cura respeitando a vida máxima. |
| `ganharXP` | `(Personagem *p, int qtd)` | Soma XP e aplica as subidas de nível. |
| `aplicarBonusAleatorio` | `(Personagem *p)` | Sorteia e aplica +1 atributo ou +5 vida máxima. |

**Fluxo e progressão**

| Função | Responsabilidade |
|--------|------------------|
| `verificarMissao` | Verifica se a casa 12 (chefe) da fase foi visitada/concluída. |
| `jogar` | Fluxo completo: escolher herói → 3 fases → vitória ou game over → resumo. |
| `lerInteiro` | Leitura robusta de inteiros no intervalo `[min, max]`. |

### 7.5 Organização do repositório

```
.
├── README.md          # este arquivo
├── main.c             # código-fonte em C
├── Makefile           # (opcional) compilação automatizada
├── images/
│   ├── personagens/   # imagens dos personagens (geradas por IA)
│   └── monstros/      # imagens dos monstros (geradas por IA)
└── video/
    └── link_video.md  # link do vídeo de demonstração
```

---

## 8. Tratamento de entradas inválidas

A função `lerInteiro` centraliza **toda** a leitura de números do programa e garante que ele nunca trave:

- Lê a linha com `fgets` (nunca usa `scanf("%d")` direto, evitando lixo no buffer).
- Converte com `strtol` e valida o ponteiro final, aceitando apenas espaços, `\t` e `\r` após o número.
- **Letras ou texto** → "Digite somente um número." e a pergunta é repetida.
- **Número fora do intervalo** → "Escolha um valor entre X e Y." e a pergunta é repetida.
- **Fim de entrada (EOF / Ctrl+D)** → o programa encerra de forma limpa com mensagem, em vez de entrar em laço infinito.

Outras robustezas:

- `receberDano`, `danoNoMonstro` e `curarPersonagem` mantêm a vida sempre dentro de `[0, vidaMax]`.
- `moverJogador` e `recuarJogador` nunca deixam a posição sair do intervalo `[0, 11]`.
- `executarEvento` valida o índice do monstro da casa antes de usá-lo (`indiceMonstro` inválido gera mensagem de erro em vez de acesso fora do vetor).
- Nenhuma alocação dinâmica: não há vazamento de memória.

---

## 9. Imagens dos personagens e monstros

*(Opcional, recomendado pelo enunciado.)* As imagens devem ser salvas em `images/personagens/` e `images/monstros/` e referenciadas aqui.

```markdown
![Guerreiro](images/personagens/guerreiro.png)
![Mago](images/personagens/mago.png)
![Arqueiro](images/personagens/arqueiro.png)
![Dragão Ancestral](images/monstros/dragao_ancestral.png)
```

**Transparência (boa prática exigida pelo enunciado):**

| Item | Valor |
|------|-------|
| Ferramenta utilizada | `<ex.: gerador de imagens por IA — nome da ferramenta>` |
| Prompt empregado | `<prompt utilizado para gerar as imagens>` |

---

## 10. Vídeo de demonstração

**Link:** O video esta contido no link do drive: https://drive.google.com/drive/folders/11gzsocyNiLZasbDPQ05XrKL4_yOMqfTM?usp=sharing

## 11. Observações e limitações conhecidas

1. **Balanceamento das classes:** as três classes são viáveis, mas com estilos diferentes. O Guerreiro (120 de vida, defesa efetiva 10) é o mais tolerante a erros; o Mago (80 de vida, defesa efetiva 5) depende de usar a Esfera Arcana no momento certo; o Arqueiro (100 de vida, agilidade 13) tem a maior chance de crítico e a fuga mais confiável. O Dragão Ancestral (ataque 18) chega a causar 13 de dano por golpe no Mago, por isso as casas de cura e tesouro são essenciais na fase 3.
2. **Combate único por casa:** cada casa de combate guarda um monstro fixo (não sorteado). Ao perder por fuga, o herói recua e pode voltar depois; se vencer, a casa fica marcada como concluída.
3. **Sem persistência:** o progresso não é salvo entre execuções — o enunciado não exige e a implementação mantém tudo em memória, como especificado.
4. **Aleatoriedade:** a semente vem de `time(NULL)`, portanto partidas diferentes produzem resultados diferentes.

---

## Licença e autoria

Projeto desenvolvido individualmente para a avaliação prática da disciplina **DCT1106 — Programação** (UFRN/DCT, 2026.2). Uso educacional.
