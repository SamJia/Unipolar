#ifndef UNIPOLAR_CONTROLLER_H_
#define UNIPOLAR_CONTROLLER_H_
#include <string>
#include "def.h"
#include "board.h"

typedef int (*gtp_fn_ptr)();

template <int BOARD_SIZE>
class Controller
{
public:
	Controller() = default;
	~Controller() = default;
	static int Run();
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Controller)
private:
	static int GTPProtocolVersion();
	static int GTPName();
	static int GTPVersion();
	static int GTPKnownCommand();
	static int GTPListCommands();
	static int GTPQuit();
	static int GTPBoardsize();
	static int GTPClearBoard();
	static int GTPKomi();
	static int GTPFixedHandicap();
	static int GTPPlaceFreeHandicap();
	static int GTPSetFreeHandicap();
	static int GTPPlay();
	static int GTPGenmove();
	static int GTPFinalScore();
	static int GTPFinalStatusList();
	static int GTPShowboard();

	int komi_;

};

#include <iostream>
#include <string>
static int Controller::Run() {
	std::string command;
	while (true) {
		cin >> command;
		if (command == "protocol_version")
			GTPProtocolVersion();
		else if (command == "name")
			GTPName();
		else if (command == "version")
			GTPVersion();
		else if (command == "known_command")
			GTPKnownCommand();
		else if (command == "list_commands")
			GTPListCommands();
		else if (command == "quit")
			GTPQuit();
		else if (command == "boardsize")
			GTPBoardsize();
		else if (command == "clear_board")
			GTPClearBoard();
		else if (command == "komi")
			GTPKomi();
		else if (command == "fixed_handicap")
			GTPFixedHandicap();
		else if (command == "place_free_handicap")
			GTPPlaceFreeHandicap();
		else if (command == "set_free_handicap")
			GTPSetFreeHandicap();
		else if (command == "play")
			GTPPlay();
		else if (command == "genmove")
			GTPGenmove();
		else if (command == "final_score")
			GTPFinalScore();
		else if (command == "final_status_list")
			GTPFinalStatusList();
		else if (command == "showboard")
			GTPShowboard();
	}
}

static int Controller::GTPProtocolVersion() {

}

static int Controller::GTPName() {

}

static int Controller::GTPVersion() {

}

static int Controller::GTPKnownCommand() {

}

static int Controller::GTPListCommands() {

}

static int Controller::GTPQuit() {

}

static int Controller::GTPBoardsize() {

}

static int Controller::GTPClearBoard() {

}

static int Controller::GTPKomi() {

}

static int Controller::GTPFixedHandicap() {

}

static int Controller::GTPPlaceFreeHandicap() {

}

static int Controller::GTPSetFreeHandicap() {

}

static int Controller::GTPPlay() {

}

static int Controller::GTPGenmove() {
	
}

static int Controller::GTPFinalScore() {

}

static int Controller::GTPFinalStatusList() {

}

static int Controller::GTPShowboard() {

}


#endif

