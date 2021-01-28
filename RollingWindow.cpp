#include "RollingWindow.h"
#include<iostream>
#include <vector>

using std::cout;
using std::endl;

int main(){	
	std::vector<int> v {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	RollingWindow<int, 4> r {v};
	auto rit = r.data().begin();
	cout << "INCREMENTING" << endl;
	while(r.can_increment()){
		cout << "items: " << r << endl;
		++r;
		rit = r.data().begin();
	}
	cout << "items: " << r << endl;
	cout << "DECREMENTING" << endl;
	while(r.can_decrement()){
		cout << "items: " << r << endl;
		--r;
		rit = r.data().begin();
	}
	cout << "items: " << r << endl;
}