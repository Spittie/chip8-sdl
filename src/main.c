#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "SDL/SDL.h"

int main(int argc, char **argv) {
	// Random!
	srand(time(NULL));
	
	// Chars
	unsigned char chip8_fontset[80] =
	{ 
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	
	// SDL Stuff
	
	int scale=10;
	
	SDL_Surface* screen = NULL;
	SDL_Init( SDL_INIT_EVERYTHING );
	screen = SDL_SetVideoMode(64*scale, 32*scale, 32, SDL_HWSURFACE);
	SDL_Event event;
	int quit = 0;
	

	
	// CPU Stuff
	
	uint8_t V[16];
	uint16_t I;
	uint16_t PC;
	uint16_t SP;
	uint16_t stack[16];
	uint8_t delay_t;
	uint8_t sound_t;

	// Ram
	uint8_t ram[4096];
	
	// Key
	uint8_t key[16];
	
	// GFX
	uint8_t gfx[64][32];
	uint8_t oldgfx[64][32];
	uint8_t draw;

	uint16_t opcode;
	
	// Initialize
	PC = 0x200;
	opcode = 0;
	I = 0;
	SP = 0;
	draw = 0;
	delay_t = 0;
	sound_t = 0;

	for(int i=0; i < 16; ++i){
			V[i]=0;
	}
	
	for(int i=0; i < 16; ++i){
			stack[i]=0;
	}
	
	for(int i=0; i < 16; ++i){
			key[i]=0;
	}
	
	for(int i=0; i < 4096; ++i){
			ram[i]=0;
	}
	
	for(int i=0; i < 64; ++i){
		for(int j=0; j < 32; ++j) {
			gfx[i][j]=0x0;
		}
	}
	
	for(int i=0; i < 80; ++i) {
			ram[i] = chip8_fontset[i];
	}
	
	if (argc < 2) {
			printf("Lol, imma need a file\n");
			return 1;
	}
	
	SDL_WM_SetCaption(argv[1], 0);
	
	printf("Loading file: %s\n", argv[1]);
	FILE * pFile = fopen(argv[1], "rb");
	if (pFile == NULL) {
			printf("Error loading file\n");
			return 1;
	}
	
	fseek(pFile, 0L, SEEK_END);
	int fSize = ftell(pFile);
	rewind(pFile);
	printf("Size: %d\n", fSize);
	
	unsigned char *buffer=malloc(fSize);
	
	fread(buffer, fSize, 1, pFile);
	fclose(pFile);
	
	for(int i=0; i <= fSize; i++) {
			ram[i+0x200] = buffer[i];
			printf("%X ", ram[i+0x200]);
	}

	printf("\n");
	
	// Start emulation
	
	while(quit == 0) {
		
		opcode = ram[PC + 0];
		opcode <<= 8;
		opcode |= ram[PC + 1];
		
  		printf("opcode: %X\n", opcode);
		printf("%d\n", delay_t);
//  		printf("& opcode: %X\n", opcode & 0xF000);
//  		printf("&& opcode: %X\n", opcode & 0x00FF);

		while (SDL_PollEvent (&event)) {
			if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
						case SDLK_1: key[0x1] = 1; break;
						case SDLK_2: key[0x2] = 1; break;
						case SDLK_3: key[0x3] = 1; break;
						case SDLK_4: key[0xC] = 1; break;
						case SDLK_q: key[0x4] = 1; break;
						case SDLK_w: key[0x5] = 1; break;
						case SDLK_e: key[0x6] = 1; break;
						case SDLK_r: key[0xD] = 1; break;
						case SDLK_a: key[0x7] = 1; break;
						case SDLK_s: key[0x8] = 1; break;
						case SDLK_d: key[0x9] = 1; break;
						case SDLK_f: key[0xE] = 1; break;
						case SDLK_z: key[0xA] = 1; break;
						case SDLK_x: key[0x0] = 1; break;
						case SDLK_c: key[0xB] = 1; break;
						case SDLK_v: key[0xF] = 1; break;
						case SDLK_ESCAPE: exit(1); break;
				}
			} else if (event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym) {
						case SDLK_1: key[0x1] = 0; break;
						case SDLK_2: key[0x2] = 0; break;
						case SDLK_3: key[0x3] = 0; break;
						case SDLK_4: key[0xC] = 0; break;
						case SDLK_q: key[0x4] = 0; break;
						case SDLK_w: key[0x5] = 0; break;
						case SDLK_e: key[0x6] = 0; break;
						case SDLK_r: key[0xD] = 0; break;
						case SDLK_a: key[0x7] = 0; break;
						case SDLK_s: key[0x8] = 0; break;
						case SDLK_d: key[0x9] = 0; break;
						case SDLK_f: key[0xE] = 0; break;
						case SDLK_z: key[0xA] = 0; break;
						case SDLK_x: key[0x0] = 0; break;
						case SDLK_c: key[0xB] = 0; break;
						case SDLK_v: key[0xF] = 0; break;
				}
			} else if (event.type == SDL_QUIT) {
				quit = 1;
			}
		}
		
		if (draw == 1) {
			SDL_Rect rect;

			rect.x = 0;
			rect.y = 0;
			rect.h = 64*scale;
			rect.w = 64*scale;
			//SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
			
			for (int i=0; i<64; i++) {
				for (int j=0; j<32; j++) {
					if (gfx[i][j] ^ oldgfx[i][j] == 1) {
						rect.x = i*scale;
						rect.y = j*scale;
						rect.w = scale;
						rect.h = scale;
						if (gfx[i][j] == 1) {
							SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 255, 255, 255));
						}
						if (gfx[i][j] == 0) {
							SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, 0, 0, 0));
						}
					}
				}
			}
			SDL_Flip(screen);
			
			draw == 0;
		
		}
		
		switch(opcode & 0xF000) {
			
			case 0x0000: {
				switch(opcode & 0x00FF) {
					case 0x00E0: {
						for(int i=0; i < 64; ++i) {
							for(int j=0; j < 32; ++j) {	
								gfx[i][j] = 0x0;
							}
						}
						draw = 1;
						PC += 2;
					}
					break;
					
					case 0X00EE: {
						SP--;
						PC = stack[SP];
						PC += 2;
					}
					break;
					
					default: {
							printf("Unknown opcode\n");
							
					}
					break;
				}
			}
			break;
			
			case 0x1000: {
				PC = opcode & 0X0FFF;
			}
			break;
			
			case 0x2000: {
				stack[SP] = PC;
				SP++;
				PC = opcode & 0x0FFF;
			}
			break;

			case 0x3000: {
				if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
					PC += 4;
				} else {
					PC += 2;
				}
			}
			break;
			
			case 0x4000: {
				if (V[(opcode & 0x0F00) >> 8] != ((opcode & 0x00FF))) {
					PC += 4;
				} else {
					PC += 2;
				}
			}
			break;
			
			case 0x5000: {
				if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
					PC += 4;
				} else {
					PC += 2;
				}
			}
			break;
			
			case 0x6000: {
				V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
				PC += 2;
			}
			break;
			
			case 0x7000: {
				V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
				PC += 2;
			}
			break;
			
			case 0x8000: {
				switch(opcode & 0x000F) {
					case 0x0000: {
						V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0001: {
						V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0002: {
						V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0003: {
						V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0004: {
						if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
							V[0xF] = 1;
						} else {
							V[0xF] = 0;
						}
						V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0005: {
						if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
							V[0xF] = 0;
						} else {
							V[0xF] = 1;
						}
						V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
						PC += 2;
					}
					break;
					
					case 0x0006: {
						V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
						V[(opcode & 0x0F00) >> 8] >>= 1;
						PC += 2;
					}
					break;
					
					case 0x0007: {
						if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
							V[0xF] = 0;	
						} else {
							V[0xF] = 1;
						}
						V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
						PC += 2;
					}
					break;
					
					case 0x000E: {
						V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
						V[(opcode & 0x0F00) >> 8] <<= 1;
						PC += 2;
					}
					break;
					
					default: {
						printf("Unknown opcode\n");
						
					}
					break;
				}
			}
			break;
			
			case 0x9000: {
				if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
					PC += 4;
				} else {
					PC += 2;
				}
			}
			break;
			
			case 0xA000: {
				I = opcode & 0x0FFF;
				PC += 2;
			}
			break;
			
			case 0xB000: {
				PC = (opcode & 0x0FFF) + V[0x0];
			}
			break;
			
			case 0xC000: {
				V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
				PC += 2;
			}
			break;
			
			case 0xD000: {
				uint8_t x = V[(opcode & 0x0F00) >> 8];
				uint8_t y = V[(opcode & 0x00F0) >> 4];
				uint8_t n = opcode & 0x000F;
				uint8_t pixel;
				
				V[0xF] = 0;
				
				for (int i=0; i<64; i++) {
					for (int j=0; j<32; j++) {
						oldgfx[i][j] = gfx[i][j];
					}
				}
				
				for(int i=0; i < n; i++) {
					pixel = ram[I + i];
					for(int j=0; j < 8; j++) {
						if((pixel & (0x80 >> j)) != 0) {
							if(gfx[x+j][y+i] == 1) {
								V[0xF] = 1;	
							}
						gfx[x+j][y+i] ^= 1;
						}
					}
				}
				
				draw = 1;
				PC += 2;
			}
			break;
			
			case 0xE000: {
				switch(opcode & 0x00FF) {
					case 0x009E: {
						if (key[V[(opcode & 0x0F00) >> 8]] == 1) {
							key[V[(opcode & 0x0F00) >> 8]] = 0;
							PC += 4;
						} else {
							PC += 2;
						}
					}
					break;
						
					case 0x00A1 : {
						if (key[V[(opcode & 0x0F00) >> 8]] != 1) {
							PC += 4;
						} else {
							PC += 2;
						}
					}
					break;
					
					default: {
						printf("Unknown opcode\n");
						
					}
					break;
				}
			}
			break;
			
			
			case 0xF000: {
				switch(opcode & 0x00FF) {
					case 0x0007: {
						V[(opcode & 0x0F00) >> 8] = delay_t;
						PC += 2;
					}
					break;
					
					case 0x000A: {
						int keypress = 0;
						
						for (int i=0; i < 16; ++i) {
							if (key[i] != 0) {
								key[i] = 0;
								V[(opcode & 0x0F00) >> 8] = i;
								keypress = 1;
							}
						}
						
						if (keypress == 0) {
							break;
						}
						
						PC += 2;
					}
					break;
					
					case 0x0015: {
						delay_t = V[(opcode & 0x0F00) >> 8];
						PC += 2;
					}
					break;
					
					case 0x0018: {
						sound_t = V[(opcode & 0x0F00) >> 8];
						PC += 2;
					}
					break;
					
					case 0x001E: {
						if(I + V[(opcode & 0x0F00) >> 8] > 0xFFF) {
							V[0xF] = 1;
						} else {
							V[0xF] = 0;
						}
						I += V[(opcode & 0x0F00) >> 8];
						PC += 2;
					}
					break;
					
					case 0x0029: {
						I = V[(opcode & 0x0F00) >> 8] * 0x5;
						PC += 2;
					}
					break;
					
					case 0x0033: {
						ram[I] = V[(opcode & 0x0F00) >> 8] / 100;
						ram[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
						ram[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
						PC += 2;
					}
					break;
					
					case 0x0055: {
						for (int i=0; i<=((opcode & 0x0F00) >> 8); ++i) {
							ram[I + i] = V[i];
						}
						// Original interpreter?
						I += ((opcode & 0x0F00) >> 8) + 1;
						PC += 2;
					}
					break;
					
					case 0x0065: {
						for (int i=0; i<=((opcode & 0x0F00) >> 8); ++i) {
							V[i] = ram[I + i];
						}
						// Original interpreter?
						I += ((opcode & 0x0F00) >> 8) + 1;
						PC += 2;
					}
					break;
					
					default: {
						printf("Unknown opcode\n");
						
					}
					break;
				}
			}	
			break;
			
			default: {
				printf("Unknown opcode\n");
 				
			}
			break;
		}
		
	if(delay_t > 0) {
		delay_t--;
	}
	if(sound_t > 0) {
		sound_t--;
	}
	
 	SDL_Delay(1);
	
	}
	
}
