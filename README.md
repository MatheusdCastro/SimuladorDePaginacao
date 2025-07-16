# Trabalho final Sistemas Operacionais
Simulador de Substituição de Páginas e Page Fault com GTK
Grupo:  
- Diego Duarte de Lima 553621
- Juan de Souza Holanda 557239
- Herik Mario Muniz Rocha 558167
- Matheus de Castro Vieira 553384
- Mateus Andrade Maia 552593
- Vinicius Lavor Lira 558164
 

Descrição:
-----------
Este é um simulador gráfico para demonstrar os algoritmos de substituição de páginas FIFO,LRU e WSClock, utilizando a biblioteca GTK em C.

Funcionalidades:
- Interface gráfica com GTK.
- Escolha entre os algoritmos FIFO,LRU e WSClock para substituição de páginas.
- Visualização dos quadros de memória física e dos acessos às páginas.
- Indicação visual de Page Faults (alteração de cor do quadro).
- Botão para avançar um acesso e botão para reiniciar a simulação.

Pré-requisitos:
---------------
- GTK 3 instalado no sistema.
- Compilador GCC ou compatível.

Exemplo de instalação no Ubuntu/Debian:
sudo apt update
sudo apt install libgtk-3-dev build-essential

Compilação:
-----------
Para compilar o programa, utilize o seguinte comando no terminal:

gcc -o PageFaultGUI PageFaultGUI.c `pkg-config --cflags --libs gtk+-3.0`

Ou então utilize o comando da makefile:

make


Execução:
---------
Após a compilação, execute o simulador com:

./PageFaultGUI

Ou execute com a makefile:

make run

Limpar execução:
---------
Para limpar a executável, utilize o comando clean da makefile:

make clean

Como usar:
----------
1. Escolha o algoritmo de substituição de páginas (FIFO, LRU ou WSClock) usando o menu suspenso.
2. Clique no botão "Próximo Acesso" para simular um acesso à memória.
3. Observe as atualizações nos quadros de memória e na lista de acessos realizados.
4. Para reiniciar a simulação, clique em "Reiniciar".

Observações:
------------
- A quantidade de acessos, páginas e quadros é definida por macros no código.
- Para alterar esses valores, edite as definições no início do arquivo PageFaultGUI.c

