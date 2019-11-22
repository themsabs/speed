#include <iostream>
#include <fstream>
#include <unordered_set>

using namespace std;

string random_string( size_t length ) {
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}


int main() {

	std::ofstream outfile;
	outfile.open("sellingIds.txt", std::ios_base::app); // append instead of overwrite

	unordered_set<string> seen;
	unsigned int seencount = 0;

	for(unsigned int i = 0; i<1000000000; ++i) {
		if(i % 100000 == 0) cout << i << endl;
		string newstr = random_string(35);
		if(seen.find(newstr) != seen.end()) {
			seencount++;
			i--;
		} else outfile << newstr << endl;
	}

}