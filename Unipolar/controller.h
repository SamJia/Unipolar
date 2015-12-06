#ifndef UNIPOLAR_CONTROLLER_H_
#define UNIPOLAR_CONTROLLER_H_
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include "def.h"
#include "board.h"

#include <iostream>


/* Maximum allowed line length in GTP. */
#define GTP_BUFSIZE 1000

/* Status returned from callback functions. */
#define GTP_QUIT    -1
#define GTP_OK       0
#define GTP_FATAL    1

/* Whether the GTP command was successful. */
#define GTP_SUCCESS  0
#define GTP_FAILURE  1

/* The following string should be defined in the core module of A.I. */
#define VERSION_STRING = "1024"
#define MIN_BOARD = "9"
#define MAX_BOARD = "19"


typedef int (*gtp_fn_ptr)();
typedef void (*gtp_transform_ptr)(int ai, int aj, int *bi, int *bj);

/* BE CAREFUL ABOUT THIS GLOBAL VARIABLE!!!!*/
int board_size;

struct gtp_command {
  const char *name;
  gtp_fn_ptr function;
};

void gtp_set_vertex_transform_hooks(gtp_transform_ptr in, gtp_transform_ptr out)
{
  vertex_transform_input_hook = in;
  vertex_transform_output_hook = out;
}


template <int BOARD_SIZE>
class Controller
{
public:
	Controller() = default;
	~Controller() = default;
	static int Run();
	// DISALLOW_COPY_AND_ASSIGN_AND_MOVE(Controller)
private:

	/* List of known commands. */
	static gtp_command commands[] = {
	  {"protocol_version",	GTPProtocolVersion},
	  {"name",            	GTPName},
	  {"version",         	GTPVersion},
	  {"known_command",   	GTPKnownCommand},
	  {"list_commands",   	GTPListCommands},
	  {"quit",            	GTPQuit},
	  {"boardsize",       	GTPBoardsize},
	  {"clear_board",    	GTPClearBoard},
	  {"komi",				GTPKomi},
	  {"fixed_handicap",	GTPFixedHandicap},
	  {"place_free_handicap",	GTPPlaceFreeHandicap},
	  {"set_free_handicap", GTPSetFreeHandicap},
	  {"play",            	GTPPlay},
	  {"genmove",           GTPGenmove},
	  {"final_score",    	GTPFinalScore},
	  {"final_status_list",	GTPFinalStatusList},
	  {"showboard",   		GTPShowboard},
	  {NULL,            	NULL}
	};

	bool ValidFixedHandicap(int handicap);
	void GTPPrintf(const char * format, ...);
	void GTPStartResponse(int haha);
	void GTPSuccess(const char * format, ...);
	void GTPFailure(const char * format, ...);
	void GTPFinishResponse();
	void GTPInternalSetBoardsize(int size);
	void GTPMPrintf(const char * format, ...);
	void GTPPrintVertices(int n, int movei[], int movej[]);
	void GTPPrintVertex(int n);
	int GTPDecodeCoord(char *s, int *i, int *j);
	int GTPDecodeMove(char *s, int *color, int *i, int *j);
	int GTPDecodeColor(char *s, int *color);

	static int PlaceHandicap(char *s, int fixed);
	static int GTPProtocolVersion();
	static int GTPName();
	static int GTPVersion();
	static int GTPKnownCommand(char *s);
	static int GTPListCommands(char *s);
	static int GTPQuit();
	static int GTPBoardsize(char *s);
	static int GTPClearBoard();
	static int GTPKomi(char *s);
	static int GTPFixedHandicap(char *s, int fixed);
	static int GTPPlaceFreeHandicap(char *s);
	static int GTPSetFreeHandicap(char *s);
	static int GTPPlay(char *s);
	static int GTPGenmove(char *s);
	static int GTPFinalScore(char *s);
	static int GTPFinalStatusList(char *s);
	static int GTPShowboard(char *s);

	int komi_;

	/* variables in gtp.c */
	static int gtp_boardsize = -1;
	static int current_id;
		/* Vertex transformation hooks. */
	static gtp_transform_ptr vertex_transform_input_hook = NULL;
	static gtp_transform_ptr vertex_transform_output_hook = NULL;

};

static int Controller::PlaceHandicap(char *s, int fixed) {
	int handicap;
	int first_stone = 1;
	if (!Board<BOARD_SIZE>::BoardEmpty()) {
		return GTPFailure("board not empty");
	}
	if (sscanf(s, "%d", &handicap) < 1)) {
		return GTPFailure("handicap not an integer");
	}
	if (hadicap < 2) {
		return GTPFailure("invalid handicap");
	}
	if (fixed && !ValidFixedHandicap(handicap)) {
		return GTPFailure("invalid handicap");
	}
	if (fixed) {
		/* 需要一个PlaceFixedHandicap的函数，不知道写在哪里好，他跟playmove是一起的*/
		;
	} else {
		/* 需要一个PlaceFreedHandicap的函数，不知道写在哪里好，他跟playmove是一起的*/
		;
	}
	GTPStartResponse(GTP_SUCCESS);
	for (int i  = 0; i < board_size; i++) 
		for (int j = 0; j < board_size; j++)
			if(Board<BOARD_SIZE>::GetBoard(m,n) != EMPTY) {
				if (first_stone) first_stone = 0;
				else GTPPrintf(" ");
				GTPMPrintf("%m",m,n);
			}

	return GTPFinishResponse();
}

static int Controller::Run() {
	char *s = new char[GTP_BUFSIZE];
	while (true) {
		scanf("%s",command);
		if (command == "protocol_version")
			GTPProtocolVersion();
		else if (command == "name")
			GTPName();
		else if (command == "version")
			GTPVersion();
		else if (command == "known_command")
			GTPKnownCommand(s);
		else if (command == "list_commands")
			GTPListCommands(s);
		else if (command == "quit")
			GTPQuit();
		else if (command == "boardsize")
			GTPBoardsize(s);
		else if (command == "clear_board")
			GTPClearBoard();
		else if (command == "komi")
			GTPKomi(s);
		else if (command == "fixed_handicap")
			GTPFixedHandicap(s);
		else if (command == "place_free_handicap")
			GTPPlaceFreeHandicap(s);
		else if (command == "set_free_handicap")
			GTPSetFreeHandicap(s);
		else if (command == "play")
			GTPPlay(s);
		else if (command == "genmove")
			GTPGenmove(s);
		else if (command == "final_score")
			GTPFinalScore(s);
		else if (command == "final_status_list")
			GTPFinalStatusList(s);
		else if (command == "showboard")
			GTPShowboard(s);
		delete [] s;
	}
}

bool ValidFixedHandicap(int handicap){
  if (handicap < 2 || handicap > 9)
    return 0;
  if (board_size % 2 == 0 && handicap > 4)
    return 0;
  if (board_size == 7 && handicap > 4)
    return 0;
  if (board_size < 7 && handicap > 0)
    return 0;
  
  return 1;	
}

void Controller::GTPPrintf(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
}

void Controller::GTPStartResponse(int status) {
	if (status == GTP_SUCCESS) {
		GTPPrintf("=");
	} else {
		GTPPrintf("?");
	}

	if (current_id < 0) {
		GTPPrintf(" ");
	} else {
		GTPPrintf("%d ", current_id);
	}
}

void Controller::GTPFinishResponse() {
	GTPPrintf("\n\n");
	return GTP_OK;
}

void Controller::GTPSuccess(const char *format, ...) {
	va_list ap;
	GTPStartResponse(GTP_SUCCESS);
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
	return GTPFinishResponse();
}

void Controller::GTPFailure(const char *format, ...) {
	va_list ap;
	GTPStartResponse(GTP_FAILURE);
	va_start(ap, format);
	vfprintf(stdout, format, ap);
	va_end(ap);
	return GTPFinishResponse();
}

void Controller::GTPInternalSetBoardsize(int size) {
	gtp_boardsize = size;
}

void Controller::GTPMPrintf(const char *fmt, ...) {	
	va_list ap;
	va_start(ap, fmt);
	
  	for (; *fmt; ++fmt) {
    	if (*fmt == '%') {
      		switch (*++fmt) {
      			case 'c': {
					/* rules of promotion => passed as int, not char */
					int c = va_arg(ap, int);
					putc(c, stdout);
					break;
      			}
      			case 'd': {
					int d = va_arg(ap, int);
					fprintf(stdout, "%d", d);
					break;
      			}
      			case 'f': {
					double f = va_arg(ap, double); /* passed as double, not float */
					fprintf(stdout, "%f", f);
					break;
      			}
      			case 's': {
					char *s = va_arg(ap, char *);
					fputs(s, stdout);
					break;
      			}
    			case 'm': {
					int m = va_arg(ap, int);
					int n = va_arg(ap, int);
        			GTPPrintVertex(m, n);
					break;
      			}
      			case 'C': {
					int color = va_arg(ap, int);
					if (color == WHITE)
	  					fputs("white", stdout);
					else if (color == BLACK)
				  		fputs("black", stdout);
					else
	  					fputs("empty", stdout);
					break;
      			}
     			default:
				fprintf(stdout, "\n\nUnknown format character '%c'\n", *fmt);
				break;
      		}
    	}
    	else
      		putc(*fmt, stdout);
	}
	va_end(ap);
}

void Controller::GTPPrintVertex(int i, int j) {
	GTPPrintVertices(1, &i, &j);
}

int Controller::GTPDecodeCoord(char *s, int *i, int *j) {
	char column;
	int row;
	int n;

	assert(gtp_boardsize > 0);

	if (sscanf(s, " %c%d%n", &column, &row, &n) != 2) {
		return 0;
	}
	if (tolower((int) column) == 'i'){
		return 0;
	}
	*j = tolower((int) column) - 'a';
	if (tolower((int) column) > 'i') {
		--*j;
	}
	*i = gtp_boardsize - row;

  	if (*i < 0 || *i >= gtp_boardsize || *j < 0 || *j >= gtp_boardsize){
    	return 0;
    }
	
  	if (vertex_transform_input_hook != NULL) {
    	(*vertex_transform_input_hook)(*i, *j, i, j);
	}

  return n;	
}

int GTPDecodeMove(char *s, int *color, int *i, int *j) {
	int n1, n2;
	int k;

	assert(gtp_boardsize > 0);

	n1 = GTPDecodeColor(s, color);
	if (n1 == 0) {
		return n1;
	}
	n2 = GTPDecodeCoord(s+n1, i, j);
	if (n2 == 0) {
	    char buf[6];
		if (sscanf(s + n1, "%5s%n", buf, &n2) != 1)
			return 0;
		for (k = 0; k < (int) strlen(buf); k++)
			buf[k] = tolower((int) buf[k]);
		if (strcmp(buf, "pass") != 0)
			return 0;
		*i = -1;
		*j = -1;
	}
}

int GTPDecodeColor(char *s, int *color) {
	char color_string[7];
	int i;
	int n;

	assert(gtp_boardsize > 0);

	if (sscanf(s, "%6s%n", color_string, &n) != 1) {
		return 0;
	}

	for (i = 0; i < (int) strlen(color_string); i++) {
		color_string[i] = tolower((int) color_string[i]);
	}

	if (strcmp(color_string, "b") == 0
      || strcmp(color_string, "black") == 0)
		*color = BLACK;
	else if (strcmp(color_string, "w") == 0
		|| strcmp(color_string, "white") == 0)
		*color = WHITE;
	else
		return 0;
  
	return n;

}


static int Controller::GTPProtocolVersion() {
	return GTPSuccess("2")
}

static int Controller::GTPName() {
	return GTPSuccess("UNIPOLAR");
}

static int Controller::GTPVersion() {
	return GTPSuccess(VERSION_STRING);
}

static int Controller::GTPKnownCommand(char *s) {
	char command_name[GTP_BUFSIZE];

	if (sscanf(s, "%s", command_name) < 1) {
		return GTPSuccess("false");
	}
	for (int i = 0; commands[i].name; i++) {
		if (!strcmp(command_name, commands[i].name)) {
			return GTPSuccess("true");
		}
	}
	return GTPSuccess("false");
}

static int Controller::GTPListCommands(char *s) {
	
	GTPStartResponse(GTP_SUCCESS);

	for (int i = 0; commands[i].name; ++i) {
		GTPPrintf("%s\n", commands[i].name);
	}
	GTPPrintf("\n");
	return GTP_OK;
}

static int Controller::GTPQuit() {
	GTPSuccess("");
	return GTP_QUIT;
}


static int Controller::GTPBoardsize(char *s) {
	int boardsize;

	if (sscanf(s, "%d", &boardsize) < 1) {
		return GTPFailure("boardsize not an integer");
	}
	if (boardsize < MIN_BOARD || boardsize > MAX_BOARD){
		return GTPFailure("unacceptable size");
	}
	board_size = boardsize;
	GTPInternalSetBoardsize(boardsize);
	/* run the ai module here !*/
	/* run the ai module here !*/
	/* run the ai module here !*/


}

static int Controller::GTPClearBoard() {
	Board<BOARD_SIZE>::ClearBoard();
	return GTPSuccess("");
}

static int Controller::GTPKomi() {
	if (sscanf(s, "%f", &komi) < 1) {
		return GTPFailure("komi not a float");
	}
	return GTPSuccess("");
}

static int Controller::GTPFixedHandicap(char *s) {
	return PlaceHandicap(s, 1);
}

static int Controller::GTPPlaceFreeHandicap(char *s) {
	return PlaceHandicap(s, 0);
}

static int Controller::GTPSetFreeHandicap(char *s) {
	int i, j;
	int n;
	int handicap = 0;

	if (!Board<BOARD_SIZE>::BoardEmpty()){
		return GTPFailure("board not empty");
	}
	while ((n = GTPDecodeCoord(s, &i, &j)) > 0) {
	    s += n;

	    if (Board<BOARD_SIZE>::GetBoard(i, j ) != EMPTY) {
	    	Board<BOARD_SIZE>::ClearBoard();
	    	return GTPFailure("repeated vertex");
	    }
	    /* AI */
	    PlayMove(i, j, BLACK);
	    handicap++;
	}

	if (sscanf(s, "%*s") != EOF) {
		Board<BOARD_SIZE>::ClearBoard();
		return GTPFailure("invalid coordinate");
	}

	if (handicap < 2 || handicap >= board_size * board_size) {
		Board<BOARD_SIZE>::ClearBoard();
		return GTPFailure("invalid handicap");
	}

	return GTPSuccess("");
}

static int Controller::GTPPlay(char *s) {
	int i, j;
	int color = EMPTY;

	if (!GTPDecodeMove(s, &color, &i, &j)) {
		return GTPFailure("invalid color or coordinate");
	}
	/* LegalMove function is from AI*/
	if (!LegalMove(i, j, color)){
		return GTPFailure("illeagal move");
	}
	PlayMove(i, j, color);
	return GTPSuccess("");
}

static int Controller::GTPGenmove(char *s) {
	int color = EMPTY;
	if (!GTPDecodeColor(s, &color)) {
		return GTPFailure("invalid color");
	}
	int i, j;
	/* from AI */
	GenerateMove(&i, &j, color);
	PlayMove(i, j, color);

	GTPStartResponse(GTP_SUCCESS);
	gtp_mprintf("%m", i, j);
	return GTPFinishResponse();
}

static int Controller::GTPFinalScore(char *s) {
	return gtp_success("0");
}

static int Controller::GTPFinalStatusList(char *s) {
	gtp_printf("\n\n");
	return GTP_OK;
}

static int Controller::GTPShowboard(char *s) {
	int i, j;
	int symbols[3] = {'.', 'O', 'X'};
	GTPStartResponse(GTP_SUCCESS);
	GTPPrintf("\n");
	printf("  ");
	for (int i = 0; i < board_size; ++i){
	    printf(" %c", 'A' + i + (i >= 8));
	}

	for (i = 0; i < board_size; i++) {
		printf("\n%2d", board_size - i);
    
		for (j = 0; j < board_size; j++)
			printf(" %c", symbols[Board<BOARD_SIZE>::GetBoard(i, j)]);

		printf("\n");
	}
	printf("  ");
	for (int i = 0; i < board_size; ++i){
	    printf(" %c", 'A' + i + (i >= 8));
	}

	gtp_printf("\n\n");
	return GTP_OK;
}


#endif

