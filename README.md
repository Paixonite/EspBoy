# 🎮 EspBoy

![Status](https://img.shields.io/badge/status-em%20desenvolvimento-yellow)
![Plataforma](https://img.shields.io/badge/plataforma-ESP32--S3-blue)
![Framework](https://img.shields.io/badge/framework-Arduino-cyan)

Um console de jogos portátil, estilo "Game Boy", construído do zero utilizando um ESP32-S3 da LILYGO, componentes eletrônicos básicos e muita programação!

Este é um projeto pessoal com fins educacionais, desenvolvido como parte dos meus estudos no **Colégio Técnico da UFMG (Coltec)**, utilizando materiais e o espaço maker fornecidos pela instituição.

## 🕹️ Sobre o Projeto

O objetivo do EspBoy é ir além de simplesmente "fazer um jogo funcionar". A meta é construir uma plataforma de hardware e software robusta e modular, aprendendo na prática conceitos de:

- **Desenvolvimento de Hardware:** Montagem de circuitos, soldagem e integração de componentes.
- **Programação de Baixo Nível:** Interação direta com GPIOs, timers e periféricos do microcontrolador.
- **Arquitetura de Software:** Criação de um sistema organizado, com um "firmware principal" que gerencia "bibliotecas" de jogos independentes.
- **Gerenciamento de Energia:** Implementação de um sistema de bateria recarregável para portabilidade real.

Atualmente, o projeto conta com o clássico **Jogo da Cobrinha (Snake)** totalmente funcional.

## ⚙️ Hardware Utilizado

-   **Placa de Desenvolvimento:** LILYGO T-Display ESP32-S3 (com display TFT de 1.9" 170x320 integrado).
-   **Entradas:** 8 botões *push button* (Direcionais, Start, Select, A, B) em um protoboado.
-   **Áudio:** 1 Buzzer passivo para reprodução de melodias e efeitos sonoros.
-   **Alimentação:** Bateria de LiPo 3.7V recarregável, gerenciada pela própria placa.

## ✨ Features do Firmware

O software do EspBoy foi projetado para ser escalável e eficiente.

-   **🐍 Jogo da Cobrinha (Snake):** A primeira prova de conceito, completo com pontuação, tela de título e game over.
-   **🔋 Suporte a Bateria Recarregável:** O firmware inclui rotinas para habilitar e, futuramente, monitorar o nível da bateria, permitindo que o console seja jogado em qualquer lugar.
-   **🧩 Arquitetura de Software Modular (POO):** O código é organizado com um arquivo `.ino` principal que atua como um "mini-sistema operacional", e cada jogo é sua própria classe (biblioteca). Isso facilita a adição de novos jogos sem alterar o código principal.
-   **🎵 Gerenciador de Áudio Não-Bloqueante:** Um sistema de som que toca melodias e efeitos sonoros em segundo plano, sem usar `delay()` e sem travar a lógica do jogo.
-   **🕹️ Controles Responsivos:** A leitura dos botões é feita de forma não-bloqueante, com técnicas de *debounce* e detecção de borda (ação ao soltar o botão) para uma experiência de jogo precisa.
-   **🚀 Planos Futuros:**
    -   Implementação de um menu de seleção de jogos.
    -   Adição de mais clássicos (Tetris, Pac-Man).
    -   Exibição do nível da bateria na tela.
    -   Uso do modo *Deep Sleep* para economizar energia.

## 📂 Estrutura do Código

O projeto segue uma organização limpa que separa as responsabilidades do firmware:

```
EspBoy/
|-- EspBoy.ino          # Firmware principal: inicializa hardware, gerencia o jogo atual.
|-- pins.h              # Centraliza a definição de todos os pinos de hardware.
|-- audio_assets.h      # Define as notas musicais e as melodias do jogo.
|
|-- SoundManager/       # Classe que gerencia todo o áudio não-bloqueante.
|   |-- SoundManager.h
|   |-- SoundManager.cpp
|
|-- SnakeGame/          # Classe que contém toda a lógica do Jogo da Cobrinha.
|   |-- SnakeGame.h
|   |-- SnakeGame.cpp
```


## 🚀 Como Compilar e Usar

1.  **Ambiente:** O projeto foi desenvolvido usando a **Arduino IDE**.
2.  **Hardware:** Monte o circuito conforme as definições em `pins.h`.
3.  **Bibliotecas:**
    -   Instale o suporte para placas **ESP32** na Arduino IDE.
    -   Instale a biblioteca **TFT_eSPI** de Bodmer. (Pode ser necessário configurar o `User_Setup.h` da biblioteca para a placa LILYGO T-Display S3, embora muitas versões recentes a detectem automaticamente).
4.  **Compilação:** Clone este repositório, abra o arquivo `EspBoy.ino` na Arduino IDE, selecione a placa correta (LILYGO T-Display S3) e faça o upload.

## 🏫 Contexto do Projeto

Este projeto é uma iniciativa de aprendizado realizada no **Colégio Técnico (Coltec) da Universidade Federal de Minas Gerais (UFMG)**. Agradeço à instituição pelo fornecimento dos componentes eletrônicos e pelo acesso ao laboratório, que foram essenciais para a viabilização do EspBoy.

## 👤 Autor

-   **[Seu Nome Aqui]** - [Seu GitHub ou outra rede social, se quiser]
