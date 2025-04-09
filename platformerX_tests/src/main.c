/**
 * Para compilar & rodar:
 * ---------------------
 *   CTRL + SHIFT + B   (gera out/rom.bin )			<< compilar
 *   [F1], Run Task, Run Gens						<< executar
 *
 * Para compilar & rodar (com extensão Genesis Code):
 * -------------------------------------------------
 *  OBS: instalar extensao Genesis Code e configurar "Gens Path"
 *
 *  Executar: $ cmd									<< pelo terminal do VSCode (extensão não funciona em PowerShell)
 *  F1 -> Genesis Code Compile Project				<< compilar
 *  F1 -> Genesis Code Compiler & Run Project		<< compilar & executar
 * 
 * Para fazer:
 * ----------------------
 * @todo Fundo com parallax
 * @todo Controle de DOWN, PRESSED e RELEASED para botoes
 * @todo Colisão com tiles sólidos
 * @todo Adicionar obstaculos
 * @todo Adicionar inimigos
 *
 * Feito:
 * ----------------------
 * Carrega mapa TMX
 * Controle e pulo do personagem
 * Câmera no level
 * Câmera segue jogador
 * Impedir que o personagem saia do mapa (laterais)
 * Impedir que saia pelo topo da tela
 * Geração de mapa de colisão a partir do tmx * 
  */
#include <genesis.h>

// #include "sprite_eng.h"
// #include "resources.h"
// #include "structs.h"

// %GDK%\bin\make -f %GDK%\makefile.gen asm

#define MAX 6

u8 a = 5;
u8 v[MAX] = {0};

u8 times2(u8 x) {
	x *= 2;
	return x;
}

int main()
{
	a = times2(5);
	for (u8 i = 0; i < MAX; i++) {
		a += i;
	}
	a /= 5;
	domagic(a, v);

	return 0;
}