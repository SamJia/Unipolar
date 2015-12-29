#include "joseki.h"
#include "def.h"
#include <fstream>

float bonus[unipolar::BoardSizeSquare(unipolar::BOARD_SIZE)];


int main() {
	for(int i = 0; i < unipolar::BoardSizeSquare(unipolar::BOARD_SIZE); ++i)
		bonus[i] = 0;
	// string s2 = "14 B D 10 W C 4 B L 10 W K 4 B L 7";
	// string s1 = "12 B D 10 W C 4 B L 10 W K 3";
	// string s3 = "14 B D 10 W C 4 B L 10 W K 4";
	string s4 = "B K 10";
	TireTree tt;
	// tt.insert(s1);
	// tt.insert(s2);
	// tt.insert(s3);
	ifstream in("static_20.dic");
	string a;
	while (getline(in,a)){
		tt.insert(a);
	}
	// cout << tt.size()<<endl;
	// cout << tt.find(s4)<<endl;
	tt.findBest(s4, bonus);
	// cout << tt.size()<<endl;
	cout<<"Done\n";

}
