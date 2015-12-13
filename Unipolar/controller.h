#ifndef UNIPOLAR_CONTROLLER_H_
#define UNIPOLAR_CONTROLLER_H_
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include "def.h"
#include "board.h"
#include "uct.h"

#include <iostream>
using namespace std;

class Controller {
public:
	Controller() = default;
	~Controller() = default;
	int Run(Board &board);
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Controller)
private:
	int GTPName();
	int GTPProtocolVersion();
	int GTPVersion();
	int GTPListCommands();
	int GTPBoardsize();
	int GTPClearBoard(Board &board);
	int GTPKomi();
	int GTPPlay(Board &board);
	int GTPGenmove(Board &board);
	int GTPShowboard(Board &board);
	float komi_;
};

int Controller::Run(Board &board) {
	std::string command;
	while (true) {
		cin >> command;
		if (command == "name")
			GTPName();
		else if (command == "protocol_version")
			GTPProtocolVersion();
		else if (command == "version")
			GTPVersion();
		else if (command == "list_commands")
			GTPListCommands();
		else if (command == "boardsize")
			GTPBoardsize();
		else if (command == "clear_board")
			GTPClearBoard(board);
		else if (command == "komi")
			GTPKomi();
		else if (command == "play")
			GTPPlay(board);
		else if (command == "genmove")
			GTPGenmove(board);
		else if (command == "showboard")
			GTPShowboard(board);
		getline(cin, command);
	}
}

int Controller::GTPName(){
	printf("= Unipolar\n");
}

int Controller::GTPProtocolVersion(){
	printf("= 2\n");
}

int Controller::GTPVersion(){
	printf("= 1.0\n");
}

int Controller::GTPListCommands(){
	printf("= name\nprotocol_version\nversion\nlist_commands\nboardsize\nclear_board\nkomi\nplay\ngenmove\nshowboard\n\n");
}

int Controller::GTPBoardsize(){
	printf("= \n");
}

int Controller::GTPClearBoard(Board &board){
	board.ClearBoard();
	printf("= \n");
}

int Controller::GTPKomi(){
	cin >> komi_;
	printf("= \n");
}

int Controller::GTPPlay(Board &board){
	char color, x_char;
	string y_str;
	int x, y;
	PointState state;
	cin >> color >> x_char >> y_str;
	if(y_str != "ASS"){
		x = (x_char > 'I' ? x_char - 1 : x_char) - 'A';
		y = stoi(y_str.c_str());
		state = color == 'B' ? BLACK_POINT : WHITE_POINT;
		board.PlayMove(Move(state, x*BOARD_SIZE+y));
	}
	printf("= \n");
}

int Controller::GTPGenmove(Board &board){
	char color;
	scanf(" %c", &color);
	PointState state = color == 'b' ? BLACK_POINT : WHITE_POINT;
	Move move = UCT().GenMove(board, state);
	board.PlayMove(move);
	int x = move.position / BOARD_SIZE, y = move.position % BOARD_SIZE;
	char x_char = (x > 7 ? x + 1 : x) + 'A';
	printf("= %c%d\n", x_char, y);
}

int Controller::GTPShowboard(Board &board){
	board.Print();
	printf("= \n");
}


#endif

