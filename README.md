# 🏴‍☠️ Caça ao Tesouro

Projeto desenvolvido em linguagem C como atividade acadêmica da disciplina de Programação Estruturada.

## 📖 Sobre o Projeto

Caça ao Tesouro é um jogo em que o jogador percorre um mapa bidimensional em busca de moedas espalhadas pelo cenário enquanto evita pedras que causam dano.

O objetivo é coletar todas as moedas disponíveis antes que a vida do jogador chegue a zero.

O projeto possui duas versões:

* **main.c** → Versão em terminal (console).
* **grafico.c** → Versão gráfica utilizando a biblioteca Allegro 5.

---

# 🎯 Objetivos

O projeto foi desenvolvido com o objetivo de aplicar conceitos fundamentais da linguagem C, incluindo:

* Estruturas de dados.
* Manipulação de arquivos.
* Alocação dinâmica de memória.
* Algoritmos de busca.
* Algoritmos de ordenação.
* Persistência de dados.
* Desenvolvimento de jogos simples.

---

# 🚀 Funcionalidades

## Sistema de Movimentação

O jogador pode se mover pelo mapa utilizando:

| Tecla | Movimento |
| ----- | --------- |
| W     | Cima      |
| A     | Esquerda  |
| S     | Baixo     |
| D     | Direita   |

---

## Sistema de Vida

O jogador inicia com:

```text
100 pontos de vida
```

Ao colidir com uma pedra:

```text
-20 pontos de vida
```

Quando a vida chega a zero:

```text
GAME OVER
```

---

## Sistema de Pontuação

Cada moeda coletada vale:

```text
+10 pontos
```

Total de moedas:

```text
15 moedas
```

Pontuação máxima:

```text
150 pontos
```

---

## Histórico de Movimentos

O sistema registra todos os movimentos realizados durante a partida através de uma estrutura do tipo Pilha.

Ao final do jogo é exibido o histórico completo das ações executadas.

Exemplo:

```text
w d d s s a w
```

---

# 🗺️ Mapa

O mapa possui dimensões:

```text
20 x 20
```

Elementos presentes:

| Símbolo | Descrição    |
| ------- | ------------ |
| P       | Jogador      |
| $       | Moeda        |
| #       | Pedra        |
| .       | Espaço vazio |

---

# 💾 Persistência de Dados

## mapa.dat

Arquivo binário utilizado para armazenar:

* Posições das moedas.
* Posições das pedras.

O arquivo é criado automaticamente na primeira execução.

---

## scores.txt

Arquivo texto utilizado para armazenar:

```text
Nome Pontuação
```

Exemplo:

```text
Renan 150
Maria 130
Pedro 100
```

---

# 🏆 Sistema de Ranking

O ranking suporta até:

```text
50 jogadores
```

Funcionalidades:

* Atualização automática da melhor pontuação.
* Exibição do Top 5.
* Busca Linear.
* Busca Binária.
* Ordenação por Insertion Sort.

---

# 🔎 Algoritmos Utilizados

## Busca Linear

Utilizada para localizar jogadores pelo nome.

Complexidade:

```text
O(n)
```

---

## Busca Binária

Utilizada para localizar a posição do jogador no ranking.

Complexidade:

```text
O(log n)
```

---

## Insertion Sort

Utilizado para ordenar o ranking em ordem decrescente.

Complexidade:

```text
O(n²)
```

---

# 🏗️ Estruturas Utilizadas

## Pilha

Responsável por armazenar o histórico de movimentos.

## Posicao

Armazena coordenadas do mapa.

## Jogador

Armazena:

* Nome
* Vida
* Pontuação
* Posição
* Estado

## Moeda

Representa os itens coletáveis.

## Pedra

Representa obstáculos do cenário.

## ScoreEntry

Representa entradas do ranking.

---

# ⚙️ Compilação

## Versão Console

### Linux

```bash
gcc main.c -o jogo
./jogo
```

### Windows (MinGW)

```bash
gcc main.c -o jogo.exe
jogo.exe
```

---

# 🎮 Versão Gráfica

Necessário possuir a biblioteca Allegro 5 instalada.

Compilação:

```bash
gcc grafico.c -o grafico ^
-lallegro ^
-lallegro_main ^
-lallegro_primitives
```

Execução:

```bash
grafico.exe
```

---

# 📂 Estrutura do Projeto

```text
├── main.c
├── grafico.c
├── mapa.dat
├── scores.txt
├── README.md
```

---

# 👨‍💻 Tecnologias Utilizadas

* Linguagem C
* GCC
* Allegro 5
* Arquivos Binários
* Arquivos Texto

---

# 👥 Autores

Maria Graziele de Lima Magalhães

Renan Nascimento Oliveira

---

# 📜 Licença

Projeto desenvolvido para fins acadêmicos.
