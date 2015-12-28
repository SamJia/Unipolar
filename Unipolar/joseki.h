#ifndef UNIPOLAR_JOSEKI_H_
#define UNIPOLAR_JOSEKI_H_

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <time.h>
#include <queue>

using namespace std;

class TireTree {
private:
	struct node {
		int color; //white 1, black 0
		int posi;
		string x,y;
		int num;
		node *brother;
		node *child;
		node(int c, string x, string y):color(c),x(x),y(y),posi(-1), brother(NULL),child(NULL),num(0){}
		node(int c, int p):color(c),posi(p), brother(NULL),child(NULL),num(0){}
	};
	int convertPosi(string );
	void remove(node *);
	// bool isEqual();
	node *root;
public:
	int state;
	TireTree() {
		root = new node(-1, -1);
		state = -1;
	}
	int size() {
		return root->num;
	}
	~TireTree() {
		remove(root);
	}
	void insert(string &seq);
	int find(string &seq);
	int findBest(string &seq);
	// void normalize();
};

void TireTree::remove(node *n) {
	if (n == NULL)
		return;
	remove(n->child);
	remove(n->brother);
	delete n;
	return;
}

void TireTree::insert(string &seq) {
	state = 1;
	char color;
	string x, y;
	int len = seq.length(), idx = 0;
	int num = 0;
	node *tmpC = NULL, *tmp = root, *tmpB = NULL;
	stringstream mid_seq(seq);
	mid_seq >> num;
	root->num += num;
	while ((tmpC = tmp->child) && mid_seq >> color >> x >> y) {
		// cout << color << x << y << endl;
		// cout<<1<<endl;
		if (color == 'B' && tmpC->color == 0 || 
			color == 'W' && tmpC->color == 1) { //颜色正确
			// cout<<2<<endl;
			if (tmpC->x == x && tmpC->y == y) {
				tmp = tmpC;
				tmp->num += num;
				continue;
			} else {
				tmpB = tmpC->brother;
				// cout<<3<<endl;
				while (tmpB) {
					tmpC = tmpB;
					if (tmpB->x == x && tmp->y == y) {
						tmpB->num += num;
						break;
					}
					tmpB = tmpB->brother;
				}
				// cout<<4<<endl;
				if (tmpB) {tmp = tmpB; continue;}
				if (color == 'B')
					tmpC->brother = new node(0, x, y);
				else 
					tmpC->brother = new node(1, x, y);
				// cout<<5<<endl;
				tmpC->brother->num = num;
				tmpC = tmpC->brother;
				while (mid_seq >> color >> x >> y) {
					if (color == 'B')
						tmpC->child = new node(0, x, y);
					else 
						tmpC->child = new node(1, x, y);
					tmpC->child->num = num;
					tmpC = tmpC->child;
				}
				// cout<<6<<endl;

			}
		}
	}
	while (mid_seq >> color >> x >> y) {
		// cout << color << x << y << endl;
		if (color == 'B')
			tmp->child = new node(0, x, y);
		else 
			tmp->child = new node(1, x, y);
		tmp->num = num;
		tmp = tmp->child;
	}
}

int TireTree::find(string &pattern) {
	stringstream mid_seq(pattern);
	char color;
	string x, y;
	node *tmp = root->child, *tmpp = root;
	mid_seq >> color;
	while(tmp && mid_seq >> x >> y) {
		// cout<<color<<x<<y<<endl;
		// cout << tmp->color<<tmp->x<<tmp->y<<endl;
	    if ((color == 'B' && tmp->color == 0 || 
			color == 'W' && tmp->color == 1) &&
			tmp->x == x && tmp->y == y) {
	    	tmpp = tmp;
	    	tmp = tmp->child;
	    	continue;
	    } else {
	    	tmpp = tmp;
	    	tmp = tmp->brother;
	    	while (tmp) {
	    		if ((color == 'B' && tmp->color == 0 || 
					color == 'W' && tmp->color == 1) &&
					tmp->x == x && tmp->y == y) {
	    			tmpp = tmp;
	    			tmp = tmp->child;
	    			break;
	    		} else {
	    			tmpp = tmp;
	    			tmp = tmp->brother;
	    		}
	    	}
	    }
	    color = '\0';
	    mid_seq >> color;
	    if (color == '\0' && tmp) return tmp->num;
	}
    color = '\0';
    mid_seq >> color;
	if (color == '\0' && tmp) return tmp->num;
	return 0;
}

int TireTree::findBest(string &pattern) {
	// cout<<pattern<<'|';
	// if (pattern=="") {
	// 	int posi_set[] = {127,139,48};
	// 	return posi_set[rand()%3];
	// }
	// cout<<"from findBest:"<<pattern<<endl;
	stringstream mid_seq(pattern);
	char color;
	string x, y;
	node *tmp = root->child, *tmpp = root;
	mid_seq >> color;
	// cout<<"from findBest:1";
	while(tmp && mid_seq >> x >> y) {
		cout<<"in"<<color<<x<<y<<endl;
		// cout << tmp->color<<tmp->x<<tmp->y<<endl;
		// cout<<"from findBest:2";
	    if ((color == 'B' && tmp->color == 0 || 
			color == 'W' && tmp->color == 1) &&
			tmp->x == x && tmp->y == y) {
	    	tmpp = tmp;
		    mid_seq >> color;
	    	tmp = tmp->child;
	    	continue;
	    } else {
	    	tmpp = tmp;
	    	tmp = tmp->brother;
	    	while (tmp) {
	    		if ((color == 'B' && tmp->color == 0 || 
					color == 'W' && tmp->color == 1) &&
					tmp->x == x && tmp->y == y) {
	    			tmpp = tmp;
	    			tmp = tmp->child;
	    			break;
	    		} else {
	    			tmpp = tmp;
	    			tmp = tmp->brother;
	    		}
	    	}
	    }
	    color = '\0';
	    mid_seq >> color;
	    if (color == '\0') break;
	}
    color = '\0';
    mid_seq >> color;
	int MAX = -1;
	node *max_node = NULL;
	if (color == '\0' && tmp){
		while(tmp) {
		    if (tmp->num > MAX){
		    	// cout<<"from findBest:3";
		    	MAX = tmp->num;
		    	max_node = tmp;
		    }
	    	tmp = tmp->brother;
		}
	}
	// cout<<"from findBest:4";
	if (!max_node)
		return -1;
	else{
		// cout<<"from findBest:5";
		// cout<<max_node->x<<' '<<max_node->y<<endl;
		char x_char = max_node->x[0];
		int x = (x_char > 'I' ? x_char - 1 : x_char) - 'A';
		int y = y = atoi(max_node->y.c_str()) - 1;
		return x*13+y;
	}
}
// void TireTree::normalize(){
// 	node *tmp = root->child, *tmpHead = NULL, *tmpB = NULL;
// 	while(tmp) {
// 		tmpHead = tmp;
// 		double sum = tmp->num;
// 		tmpB = tmp->brother;
// 		while(tmpB) {
// 			sum += tmpB->num;
// 			tmpB = tmpB->brother;
// 		}
// 		tmpB = tmpHead;
// 		while(tmpB) {
// 			tmpB->num = (double)(tmpB->num+0.001)/(sum+0.001);
// 			tmpB = tmpB->brother;
// 		}
// 		tmp = tmp->child;
// 	}
// }

#endif
