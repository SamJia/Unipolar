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
#include "joseki.h"

#include <iostream>
#include <fstream>
using namespace std;

// ofstream fout("D:\\Sam\\Lesson\\AI\\Project\\Unipolar\\unipolar.log", ios::app);
string seq, seqold;

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
	int GTPGenmove(Board &board, TireTree &joseki, string &seq);
	int GTPShowboard(Board &board);
	int GTPQuit();
	int GTPFinalScore();
	int GTPFinalStatusList();
	float komi_;
};

int Controller::Run(Board &board) {
	// printf("get in Run\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	// freopen("commands.txt", "r", stdin);
	//Load the joseki
	TireTree joseki;
	ifstream in("C:\\Users\\user\\Documents\\GitHub\\Unipolar\\Unipolar\\static_40.dic");
	string a;
	while (getline(in, a)) {
		joseki.insert(a);
	}

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
			GTPGenmove(board, joseki, seq);
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

int Controller::GTPName() {
	printf("= Unipolar_with_jsk\n\n");
	return 0;
}

int Controller::GTPProtocolVersion() {
	printf("= 2\n\n");
	return 0;
}

int Controller::GTPVersion() {
	printf("= 1.0\n\n");
	return 0;
}

int Controller::GTPListCommands() {
	printf("= name\nprotocol_version\nversion\nlist_commands\nboardsize\nclear_board\nkomi\nplay\ngenmove\nshowboard\nquit\nfinal_score\nfinal_status_list\n\n");
	return 0;
}

int Controller::GTPBoardsize() {
	printf("= \n\n");
	return 0;
}

int Controller::GTPClearBoard(Board &board) {
	board.ClearBoard();
	printf("= \n\n");
	return 0;
}

int Controller::GTPKomi() {
	cin >> komi_;
	printf("= \n\n");
	return 0;
}

int Controller::GTPPlay(Board &board) {
	char color, x_char;
	string y_str;
	int x, y;
	PointState state = EMPTY_POINT;
	char *tmp_str = new char[50000];
	cin >> color >> x_char >> y_str;
	snprintf(tmp_str, sizeof(tmp_str), "%s %c %c %s", seq.c_str(), color, x_char, y_str.c_str());
	seq = string(tmp_str);
	if (y_str != "ASS" && y_str != "ass") {
		x = (x_char > 'I' ? x_char - 1 : x_char) - 'A';
		y = atoi(y_str.c_str()) - 1;
		state = color == 'B' ? BLACK_POINT : WHITE_POINT;
		board.PlayMove(Move(state, x * BOARD_SIZE + y));
	}
	else {
		board.PlayMove(Move(state, POSITION_PASS));
	}
	printf("= \n\n");
	return 0;
}

int Controller::GTPGenmove(Board &board, TireTree &joseki, string &seq) {
	// printf("get in GTPGenmove\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	char color;
	cin >> color;
	PointState state = color == 'b' ? BLACK_POINT : WHITE_POINT;
	int posi = -1;
	// if(seq != "") {
	//     posi = joseki.findBest(seq);
	//     if(posi == -1) {
	//         int posi = joseki.findBest(seqold);
	//     }
	// }else{
	//     int a[] = {29, 35, 41, 42, 48, 49, 120, 121, 127, 128, 134, 140};
	//     posi = a[rand()%12];
	// }
	Move move;

	if (posi >= 0) {
		move = Move(state, posi);
	} else {
		move = UCT().GenMove(board, state);
	}
	board.PlayMove(move);
	if (move.position == POSITION_PASS) {
		printf("= PASS\n\n");
		return 0;
	}
	int x = move.position / BOARD_SIZE, y = move.position % BOARD_SIZE + 1;
	char x_char = (x > 7 ? x + 1 : x) + 'A';
	printf("= %c%d\n\n", x_char, y);

	char tmp_str[50000];
	snprintf(tmp_str, sizeof(tmp_str), "%s %c %c %d", seq.c_str(), color - 'a' + 'A', x_char, y);
	seqold = seq = string(tmp_str);

	return 0;
}

int Controller::GTPShowboard(Board &board) {
	board.Print();
	printf("= \n\n");
	return 0;
}

int Controller::GTPQuit() {
	printf("= \n\n");
	exit(0);
}

int Controller::GTPFinalScore() {
	printf("= \n\n");
	return 0;
}

int Controller::GTPFinalStatusList() {
	printf("= \n\n");
	return 0;
}

#endif
