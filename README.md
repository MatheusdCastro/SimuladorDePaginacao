# Trabalho final Sistemas Operacionais
Simulador de Substituição de Páginas e Page Fault com GTK

Descrição:
-----------
Este é um simulador gráfico para demonstrar os algoritmos de substituição de páginas FIFO e LRU, utilizando a biblioteca GTK em C.

Funcionalidades:
- Interface gráfica com GTK.
- Escolha entre os algoritmos FIFO e LRU para substituição de páginas.
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


Execução:
---------
Após a compilação, execute o simulador com:

./PageFaultGUI

Como usar:
----------
1. Escolha o algoritmo de substituição de páginas (FIFO ou LRU) usando o menu suspenso.
2. Clique no botão "Próximo Acesso" para simular um acesso à memória.
3. Observe as atualizações nos quadros de memória e na lista de acessos realizados.
4. Para reiniciar a simulação, clique em "Reiniciar".

Observações:
------------
- A quantidade de acessos, páginas e quadros é definida por macros no código.
- Para alterar esses valores, edite as definições no início do arquivo PageFaultGUI.c

