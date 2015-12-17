#include "joseki.h"
#include <fstream>


int main() {
	string s1 = "12 B D 10 W C 4 B L 10 W K 3";
	string s2 = "14 B D 10 W C 4 B L 10 W K 4";
	// string s3 = "10 B D 10 W K 10 B D 4 W K 4 B H 11";
	// // cin >> s2;
	// // cin >> s3;
	string s4 = "B D 10";
	TireTree tt;
	tt.insert(s1);
	tt.insert(s2);
	// cout<<"--------------------"<<endl;
	// tt.insert(s3);
	// cout << tt.find(s4);
	ifstream in("static_40.dic");
	string a;
	while (getline(in,a)){
		tt.insert(a);
		// cout<<a<<endl;
	}
	cout << tt.find(s4)<<endl;
	cout << tt.findBest(s4);
	cout<<"Done\n";

}
