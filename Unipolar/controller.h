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
	int GTPPlay(Board &board, string &seq);
	int GTPGenmove(Board &board, TireTree &joseki, string &seq);
	int GTPShowboard(Board &board);
	int GTPQuit();
	int GTPFinalScore();
	int GTPFinalStatusList();
	double komi_;
	int step_count;
};

int Controller::Run(Board &board) {
	// printf("get in Run\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	// freopen("commands.txt", "r", stdin);
	//Load the joseki
	TireTree joseki;
	step_count = 0;
	// has to be absolute path!!
	// ifstream in("D:\\Document\\AI\\Proj\\Unipolar\\Unipolar\\static_20.dic");
	joseki.load();
	string command;
	// command.resize(100);
	// fout << "start reading commands" << endl;
	while (true) {
		// ofstream fout("C:\\Users\\Sam\\Documents\\Visual Studio 2015\\Projects\\Unipolar\\x64\\Release\\log.txt", ios::app);
		cin >> command;
		// fout << '"' << command << ':' << command.length() << ':' << (command == "name") << '"';
		// command.erase(0,command.find_first_not_of(" \n"));
		// command.erase(command.find_last_not_of(" \n") + 1);
		// scanf("%s", &command[0]);
		// fout << "read command : " << command << endl;
		if (command == "name") {
			// fout << "GTPName";
			// fout.close();
			GTPName();
		}
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
		else if (command == "play") {
			GTPPlay(board, seq);
			++step_count;
			if (bonus_ratio <= 0.005)
				bonus_ratio += 0.0001;
			if (komi <= 6.5)
				komi += 0.25;
		}
		else if (command == "genmove") {
			GTPGenmove(board, joseki, seq);
			++step_count;
			if (bonus_ratio <= 0.005)
				bonus_ratio += 0.0001;
			if (komi <= 6.5)
				komi += 0.25;
		}
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
	cout << "= Unipolar_with_jsk\n" << endl;
	// printf("= Unipolar_with_jsk\n\n");
	return 0;
}

int Controller::GTPProtocolVersion() {
	cout << "= 2\n" << endl;
	return 0;
}

int Controller::GTPVersion() {
	cout << "= 2.0\n" << endl;
	return 0;
}

int Controller::GTPListCommands() {
	cout << "= name\nprotocol_version\nversion\nlist_commands\nboardsize\nclear_board\nkomi\nplay\ngenmove\nshowboard\nquit\nfinal_score\nfinal_status_list\n" << endl;
	return 0;
}

int Controller::GTPBoardsize() {
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPClearBoard(Board &board) {
	board.ClearBoard();
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPKomi() {
	cin >> komi_;
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPPlay(Board &board, string &seq) {
	char color, x_char;
	string y_str;
	int x, y;
	PointState state = EMPTY_POINT;
	char tmp_str[50000];
	cin >> color >> x_char >> y_str;
	snprintf(tmp_str, sizeof(tmp_str), "%s %c %s", seq.c_str(), x_char, y_str.c_str());
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
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPGenmove(Board &board, TireTree &joseki, string &seq) {
	// printf("get in GTPGenmove\n");
	// board.PlayMove(Move(0, 0));
	// printf("play move at 0,0 done\n");
	char color;
	cin >> color;
	PointState state = (color == 'b') ? BLACK_POINT : WHITE_POINT;
	int posi = -1;

	double joseki_bonus[unipolar::BoardSizeSquare(unipolar::BOARD_SIZE)];
	for(int i = 0; i < unipolar::BoardSizeSquare(unipolar::BOARD_SIZE); ++i)
		joseki_bonus[i] = 0;

	// a theshold for joseki analysis.
	//cout << "step is " << step_count << endl;
	if (step_count < JOSEKI_STEP) {
		//cout << "using joseki!\n";
		// even if seq is null.
		posi = joseki.findBest(seq, joseki_bonus);

		if (posi == -1) {
			posi = joseki.findBest(seqold, joseki_bonus);
			if (posi >= 0) {
				int x = posi / BOARD_SIZE;
				int y = posi % BOARD_SIZE + 1;
				char x_char = (x > 7 ? x + 1 : x) + 'A';
				char tmp_str[50000];
				color = (color == 'b') ? 'w' : 'b';
				snprintf(tmp_str, sizeof(tmp_str), "%s %c %d", seqold.c_str(), x_char, y);
				seq = (string)tmp_str;
				// cout<<"sb:"<<seq<<endl;
				posi = joseki.findBest(seq, joseki_bonus);
				// cout<<posi<<endl;
				color = (color == 'b') ? 'w' : 'b';
			}
		}
	}
	//for(int i = 0 ; i < 13; ++i){
	//	for (int j = 0; j < 13; ++j)
	//		printf("%.3f ", joseki_bonus[i*13+j]);
	//	printf("\n");
	//}
	Move move;
	move = UCT(joseki_bonus).GenMove(board, state);
	board.PlayMove(move);
	if (move.position == POSITION_PASS) {
		cout << "= PASS\n" << endl;
		return 0;
	}
	int x = move.position / BOARD_SIZE, y = move.position % BOARD_SIZE + 1;
	char x_char = (x > 7 ? x + 1 : x) + 'A';
	cout << "= " << x_char << y << '\n' << endl;
	// printf("= %c%d\n\n", x_char, y);

	char tmp_str[50000];
	snprintf(tmp_str, sizeof(tmp_str), "%s %c %d", seq.c_str(), x_char, y);
	seqold = seq = string(tmp_str);
	// cout << "next seq: " << seq << endl;
	return 0;
}

int Controller::GTPShowboard(Board &board) {
	board.Print();
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPQuit() {
	cout << "= \n" << endl;
	exit(0);
}

int Controller::GTPFinalScore() {
	cout << "= \n" << endl;
	return 0;
}

int Controller::GTPFinalStatusList() {
	cout << "= \n" << endl;
	return 0;
}

#endif
