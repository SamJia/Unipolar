#include "joseki.h"
#include "def.h"
#include <fstream>


int main() {
	// string s2 = "14 B D 10 W C 4 B L 10 W K 4 B L 7";
	// string s1 = "12 B D 10 W C 4 B L 10 W K 3";
	// string s3 = "14 B D 10 W C 4 B L 10 W K 4";
	string s4 = "N 13 G 7 N 13";
	TireTree tt;
	// tt.insert(s1);
	// tt.insert(s2);
	// tt.insert(s3);
	tt.load();
	// cout << tt.size()<<endl;
	// cout << tt.find(s4)<<endl;
	cout << tt.findBest(s4);
	// cout << tt.size()<<endl;
	cout<<"Done\n";

}
