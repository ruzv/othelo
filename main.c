#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>


#define WIN_SIZE 800

#define BOARD_MARGIN 100
#define BOT_DELAY 300
#define BOT_SEARCH_DEPTH 4
#define WHITE 1
#define BLACK -1

const int SQUARE_SIZE = (WIN_SIZE-(2*BOARD_MARGIN))/8;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Event event;
int is_mouse_click = 0;
int is_quit_main_loop = 0;

// 8 x 8 square board and last value is the current stone colors move
int board[65];

void init_sdl(){
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(
		WIN_SIZE,
		WIN_SIZE,
		SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS,
		&window,
		&renderer
	);
}

int event_handler(){
	while(SDL_PollEvent(&event)){
		if(event.type == SDL_QUIT){
			return 0;
		}else if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == SDLK_ESCAPE){
				return 0;
			}
		}else if(event.type == SDL_MOUSEBUTTONDOWN){
			if(event.button.button == SDL_BUTTON_LEFT){
				is_mouse_click = 1;
				return 1;
			}
		}
	}
	if(is_quit_main_loop){
		return 0;
	}else{
		return 1;
	}
}

void init_board(int* board){
	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			board[(y*8)+x] = 0;
		}
	}
	// starting stones
	board[27] = WHITE;
	board[28] = BLACK;
	board[35] = BLACK;
	board[36] = WHITE;
	board[64] = WHITE; // whites turn to place stone
}

void draw_board(int* board){
	// fill bg color
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderFillRect(renderer, NULL);

	// draw board lines
	for(int i=0; i<8; i++){

		hlineRGBA(renderer, BOARD_MARGIN, BOARD_MARGIN+(SQUARE_SIZE*8), BOARD_MARGIN+(i*SQUARE_SIZE), 80, 80, 80, 255);
		vlineRGBA(renderer, BOARD_MARGIN+(i*SQUARE_SIZE), BOARD_MARGIN, BOARD_MARGIN+(SQUARE_SIZE*8), 80, 80, 80, 255);
	}
	// x1, x2, y
	hlineRGBA(renderer, BOARD_MARGIN, BOARD_MARGIN+(8*SQUARE_SIZE), BOARD_MARGIN+(8*SQUARE_SIZE), 80, 80, 80, 255);
	vlineRGBA(renderer, BOARD_MARGIN+(8*SQUARE_SIZE), BOARD_MARGIN, BOARD_MARGIN+(SQUARE_SIZE*8), 80, 80, 80, 255);
	// draw stones
	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			if(board[(y*8)+x] == WHITE){
				filledCircleRGBA(renderer, BOARD_MARGIN+(x*SQUARE_SIZE)+(SQUARE_SIZE/2), BOARD_MARGIN+(y*SQUARE_SIZE)+(SQUARE_SIZE/2), (Sint16)(((float)SQUARE_SIZE)*0.4), 255, 255, 255, 255);
			}else if(board[(y*8)+x] == BLACK){
				filledCircleRGBA ( renderer, BOARD_MARGIN+(x*SQUARE_SIZE)+(SQUARE_SIZE/2), BOARD_MARGIN+(y*SQUARE_SIZE)+(SQUARE_SIZE/2), (Sint16)(((float)SQUARE_SIZE)*0.4), 0, 0, 0, 255 ) ;
			}
		}
	}
	// show new drawn board
	SDL_RenderPresent(renderer);
}

void print_board(int* board){
	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			putc('|', stdout);
			if(board[(y*8)+x] == WHITE){
				putc('O', stdout);
			}else if(board[(y*8)+x] == BLACK){
				putc('X', stdout);
			}else{
				putc(' ', stdout);
			}
		}
		puts("|");
	}
}

void end_of_game(){
	int w = 0;
	int b = 0;

	for(int i=0; i<64; i++){
		if(board[i] == WHITE){
			w += 1;
		}else if(board[i] == BLACK){
			b += 1;
		}
	}

	printf("end of game\n");
	print_board(board);
	printf("score:\n");
	printf("white: %d\n", w);
	printf("black: %d\n", b);
	if(w > b){
		printf("WHITE WINS\n");
	}else if(b > w){
		printf("BLACK WINS\n");
	}else{
		printf("DRAW\n");
	}

}


int stone_flips_line(int* board, int x, int y, int dx, int dy){
	int flips = 0;

	while(1){
		x += dx;
		y += dy;
		if(x >= 0 && x < 8 && y >=0 && y < 8){ // while is in board bounds
			if(board[(y*8)+x] == (board[64]*(-1))){ // found opposite color stone
				flips += 1;
			}else if(board[(y*8)+x] == 0){ // found empty space
				return 0;
			}else if(board[(y*8)+x] == board[64]){// found encompasing stone
				return flips;
			}
		}else{
			return 0;
		}
	}
}

// returns 1 if move is valid
// returns 0 if move is not valid
int stone_flips(int* board, int x, int y){

	if(board[(y*8)+x] != 0){ // board position already taken
		return 0;
	}

	int flips = 0;

	for(int dy=-1; dy<2; dy++){

		for(int dx=-1; dx<2; dx++){

			if(!((dx == 0) && (dy == 0))){ // ! direction 0, 0

				// printf("%d %d\n", dx, dy);
				// printf("%d\n", stone_flips_line(board, x, y, dx, dy));
				flips += stone_flips_line(board, x, y, dx, dy);

			}

		}

	}

	// printf("%d\n", flips);
	// printf("\n");

	return flips;
}

void place_stone(int* b, int x, int y){

	b[(y*8)+x] = b[64];

	for(int dy=-1; dy<2; dy++){
		for(int dx=-1; dx<2; dx++){
			if(!((dx == 0) && (dy == 0))){ // ! direction 0, 0
				int f = stone_flips_line(b, x, y, dx, dy);
				int j = x;
				int k = y;
				//printf("%d\n", f);
				for(int i=0; i<f; i++){
					j += dx;
					k += dy;
					b[(k*8)+j] = b[64];
				}
			}
		}
	}

}

// returns 0 if no more avaible moves
// returns if there is an avaible move
int is_avaible_moves(int* board){
	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			if(stone_flips(board, x, y) > 0){
				return 1;
			}
		}
	}
	return 0;
}


// returns 1 on succesfull move
// returns 0 on failure
int player_human(){

	if(is_mouse_click == 1){// player has clicked
		is_mouse_click = 0;
		int x, y;
		SDL_GetMouseState(&x, &y);
		x -= BOARD_MARGIN;
		y -= BOARD_MARGIN;
		if(x < 0){
			return 0;
		}
		if(y < 0){
			return 0;
		}
		x /= SQUARE_SIZE;
		y /= SQUARE_SIZE;

		if(x >= 0 && x < 8 && y >=0 && y < 8){ // check if click is with in board bounds

			if(stone_flips(board, x, y) > 0){ // is valid move
				place_stone(board, x, y);
				return 1;
			}else{
				printf("invalid move\n");
				return 0;
			}
		}
		return 0;
	}else{
		// return failure, wait for mouse click
		return 0;
	}
	return 0;
}

// returns 1 on succesfull move
// returns 0 on failure
int player_bot0(){
	SDL_Delay(BOT_DELAY);
	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			if(stone_flips(board, x, y) > 0){
				place_stone(board, x, y);
				return 1;
			}

		}
	}
	return 0;
}

int player_bot1(){
	SDL_Delay(BOT_DELAY);

	int flips = 0;
	int max_flips = 0;
	int bx, by; // best move

	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			flips = stone_flips(board, x, y);
			if(flips > max_flips){
				max_flips = flips;
				bx = x;
				by = y;
			}
		}
	}
	place_stone(board, bx, by);
	return 1;
}

int bot2_minimax(int* b, int depth){

	if(depth == 0){
		int sum = 0;
		for(int i=0; i<64; i++){
			sum += b[i];
		}
		return sum;
		// evaluate

	}else{

		int b_copy[65];

		if(b[64] == WHITE){ // maximazeing
			int max = -300; // random large negative 
			int evl;
			for(int y=0; y<8; y++){
				for(int x=0; x<8; x++){
					if(stone_flips(b, x, y) > 0){ // found a posible move

						// for(int i=0; i<65; i++){
						// 	b_copy[i] = b[i];
						// }
						memcpy(b_copy, b, 65*sizeof(int));


						place_stone(b_copy, x, y);
						b_copy[64] = BLACK;
						evl = bot2_minimax(b_copy, depth-1);
						if(evl > max){
							max = evl;
						}

					}
				}
			}

			return max;

		}else if(b[64] == BLACK){ // minimazeing
			int min = 300; // random large num
			int evl;;
			for(int y=0; y<8; y++){
				for(int x=0; x<8; x++){
					if(stone_flips(b, x, y) > 0){ // found a posible move

						// for(int i=0; i<65; i++){
						// 	b_copy[i] = b[i];
						// }
						memcpy(b_copy, b, 65*sizeof(int));

						place_stone(b_copy, x, y);
						b_copy[64] = WHITE;
						evl = bot2_minimax(b_copy, depth-1);
						if(evl < min){
							min = evl;
						}

					}
				}
			}
			return min;
		}

	}

}

int player_bot2(){
	SDL_Delay(BOT_DELAY);

	int b_copy[65];

	int first_eval = 1;
	int eval;
	int best_eval = 0;
	int bx=0, by=0; // best move

	for(int y=0; y<8; y++){
		for(int x=0; x<8; x++){
			if(stone_flips(board, x, y) > 0){ 

				// for(int i=0; i<65; i++){
				// 	b_copy[i] = board[i];
				// }
				memcpy(b_copy, board, 65*sizeof(int));

				place_stone(b_copy, x, y);

				if(board[64] == WHITE){
					b_copy[64] = BLACK;
				}else if(board[64] == BLACK){
					b_copy[64] = WHITE;
				}
				

				eval = bot2_minimax(b_copy, 5);


				if(first_eval){
					first_eval = 0;
					best_eval = eval;
					bx = x;
					by = y;
				}else{
					if(board[64] == WHITE){
						if(eval > best_eval){
							best_eval = eval;
							bx = x;
							by = y;
						}
					}else if(board[64] == BLACK){
						if(eval < best_eval){
							best_eval = eval;
							bx = x;
							by = y;
						}
					}
				}
			}
		}
	}

	//printf("%d\n", best_eval);
	place_stone(board, bx, by);
	return 1;
}


int (*player1)() = NULL;
int (*player2)() = NULL;
void player_init(int argc, char* argv[]){
	printf("playing:  ");
	if(argc == 3){
		if(strcmp(argv[1], "hu") == 0){
			player1 = &player_human;
			printf("human");
		}else if(strcmp(argv[1], "b0") == 0){
			player1 = &player_bot0;
			printf("bot0");
		}else if(strcmp(argv[1], "b1") == 0){
			player1 = &player_bot1;
			printf("bot1");
		}else if(strcmp(argv[1], "b2") == 0){
			player1 = &player_bot2;
			printf("bot2");
		}else{
			printf("invalid player");
			is_quit_main_loop = 1;
		}
		printf("[WHITE]  vs  ");
		if(strcmp(argv[2], "hu") == 0){
			player2 = &player_human;
			printf("human");
		}else if(strcmp(argv[2], "b0") == 0){
			player2 = &player_bot0;
			printf("bot0");
		}else if(strcmp(argv[2], "b1") == 0){
			player2 = &player_bot1;
			printf("bot1");
		}else if(strcmp(argv[2], "b2") == 0){
			player2 = &player_bot2;
			printf("bot2");
		}else{
			printf("invalid player");
			is_quit_main_loop = 1;
		}
		printf("[BLACK]\n");

	}else{
		printf("invalid programm usage\n");
		printf("usage:\n");
		printf("    othelo [player 1] [player 2]\n");
		printf("players can be:\n");
		printf("    hu  -  human\n");
		printf("    b0  -  bot easy\n");
		printf("    b1  -  bot dificult\n");
		printf("    b2  -  bot hard\n");
		is_quit_main_loop = 1;
	}


}

int had_opposite_move = 1;
int is_move = -1;
void player_manager(){

	if(board[64] == WHITE){ // whites move

		if(is_move == -1){ // must check if there is a move
			is_move = is_avaible_moves(board);
		}
		if(is_move == 1){
			if((*player1)() == 1){
				// set board[64] to other players move
				board[64] = BLACK;
				draw_board(board);
				is_move = -1;
				had_opposite_move = 1;

			}
		}else{
			is_move = -1;
			if(had_opposite_move == 0){
				end_of_game();
				is_quit_main_loop = 1;
				return;
			}
			had_opposite_move = 0;
			board[64] = BLACK;
			printf("white has no avaible moves. skiping turn\n");
		}

	}else if(board[64] == BLACK){ // blacks move

		if(is_move == -1){ // must check if there is a move
			is_move = is_avaible_moves(board);
		}
		if(is_move == 1){
			if(player_bot1() == 1){
				// set board[64] to other players move
				board[64] = WHITE;
				draw_board(board);
				is_move = -1;
				had_opposite_move = 1;
			}
		}else{
			if(had_opposite_move == 0){ // oponent in previous move skipped to, end of game.
				end_of_game();
				is_quit_main_loop = 1;
				return;
			}
			had_opposite_move = 0;
			is_move = -1;
			board[64] = WHITE;
			printf("black has no avaible moves. skiping turn\n");
		}


	}

}

int main(int argc, char* argv[]){
	init_sdl();

	player_init(argc, argv);

	init_board(board);
	print_board(board);

	draw_board(board);

	while(event_handler()){


		player_manager();

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}