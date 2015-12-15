#ifndef UNIPOLAR_CONTROLLER_H_
#define UNIPOLAR_CONTROLLER_H_
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "def.h"
#include "board.h"
#include "uct.h"

#include <iostream>
#include <fstream>
using namespace std;

// ofstream fout("D:\\Sam\\Lesson\\AI\\Project\\Unipolar\\unipolar.log", ios::app);
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
	int GTPQuit();
	int GTPFinalScore(){printf("= \n\n");}
	int GTPFinalStatusList(){printf("= \n\n");}
	float komi_;
};

int Controller::Run(Board &board) {
	// printf("get in Run\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	// freopen("commands.txt", "r", stdin);
	string command;
	// command.resize(100);
	// fout << "start reading commands" << endl;
	while (true) {
		cin >> command;
		// scanf("%s", &command[0]);
		// fout << "read command : " << command << endl;
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
		else if (command == "quit")
			GTPQuit();
		else if (command == "final_score")
			GTPFinalScore();
		else if (command == "final_status_list")
			GTPFinalStatusList();
		// else
		// 	fout << "not match : " << command << endl;

		getline(cin, command);
	}
	return 0;
}

int Controller::GTPName(){
	printf("= Unipolar\n\n");
	return 0;
}

int Controller::GTPProtocolVersion(){
	printf("= 2\n\n");
	return 0;
}

int Controller::GTPVersion(){
	printf("= 1.0\n\n");
	return 0;
}

int Controller::GTPListCommands(){
	printf("= name\nprotocol_version\nversion\nlist_commands\nboardsize\nclear_board\nkomi\nplay\ngenmove\nshowboard\nquit\nfinal_score\nfinal_status_list\n\n");
	return 0;
}

int Controller::GTPBoardsize(){
	printf("= \n\n");
	return 0;
}

int Controller::GTPClearBoard(Board &board){
	board.ClearBoard();
	printf("= \n\n");
	return 0;
}

int Controller::GTPKomi(){
	cin >> komi_;
	printf("= \n\n");
	return 0;
}

int Controller::GTPPlay(Board &board){
	char color, x_char;
	string y_str;
	int x, y;
	PointState state;
	cin >> color >> x_char >> y_str;
	if(y_str != "ASS" and y_str != "ass"){
		x = (x_char > 'I' ? x_char - 1 : x_char) - 'A';
		y = atoi(y_str.c_str()) - 1;
		state = color == 'B' ? BLACK_POINT : WHITE_POINT;
		board.PlayMove(Move(state, x*BOARD_SIZE+y));
	}
	else{

		board.PlayMove(Move(state, POSITION_PASS));
	}
	printf("= \n\n");
	return 0;
}

int Controller::GTPGenmove(Board &board){
	// printf("get in GTPGenmove\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	char color;
	scanf(" %c", &color);
	PointState state = color == 'b' ? BLACK_POINT : WHITE_POINT;
	Move move = UCT().GenMove(board, state);
	board.PlayMove(move);
	if(move.position == POSITION_PASS){
		printf("= PASS\n\n");
		return 0;
	}
	int x = move.position / BOARD_SIZE, y = move.position % BOARD_SIZE + 1;
	char x_char = (x > 7 ? x + 1 : x) + 'A';
	printf("= %c%d\n\n", x_char, y);
	return 0;
}

int Controller::GTPShowboard(Board &board){
	board.Print();
	printf("= \n\n");
	return 0;
}

int Controller::GTPQuit(){
	printf("= \n\n");
	exit(0);
}

#endif

