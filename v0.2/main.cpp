/*
 * C++ Wakanda Reservation Cache
 */
# include <iostream>
# include <cmath>
# include <map>
# include <cstdlib>
# include <ctime>
# include <limits>
# include <cstring>
# include <fstream>
# include <vector>
# include <climits>
# include <unordered_map>
#include <thread>
# define LONG_PRIME 32993
# define MIN(a,b) (a < b ? a : b)
# define MAX(a,b) (a < b ? b : a )
# define ui unsigned int
using namespace std;
using namespace std::chrono;

/*
 * Class Declaration
 */
class CountMinSketch
{
    private:
        ui w,d;
        float eps;
        float gamma;
        ui aj, bj;
        ui total;
        int **C;
        int **hashes;
        void genajbj(int **hashes, int i);
    public:
        /*
         * Constructor
         */
        CountMinSketch(float ep, float gamm, ui w_ = 0, ui d_ = 0)
        {
            if (!(0.009 <= ep && ep < 1))
            {
                cout << "eps must be in this range: [0.01, 1)" << endl;
                exit(1);
            }
            else if (!(0 < gamm && gamm < 1))
            {
                cout << "gamma must be in this range: (0,1)" << endl;
                exit(1);
            }
            eps = ep;
            gamma = gamm;
            if(w_ == 0) w = ceil(exp(1) / eps);
            else w = w_;
            if(d_ == 0) d = ceil(log(1 / gamma));
            else d = d_;
            total = 0;
            C = new int *[d];
            ui i, j;
            for (i = 0; i < d; i++)
            {
                C[i] = new int[w];
                for (j = 0; j < w; j++)
                {
                    C[i][j] = 0;
                }
            }
            srand(time(NULL));
            hashes = new int* [d];
            for (i = 0; i < d; i++)
            {
                hashes[i] = new int[2];
                genajbj(hashes, i);
            }
        }
        /*
         * Destructor
         */
        ~CountMinSketch()
        {
            ui i;
            for (i = 0; i < d; i++)
            {
                delete[] C[i];
            }
            delete[] C;
            for (i = 0; i < d; i++)
            {
                delete[] hashes[i];
            }
            delete[] hashes;
        }
        /*
         * Update Int Value
         */
        vector<int> update(int item, int c)
        {
            total = total + c;
            ui hashval = NULL;

            vector<int> v;
            for (ui j = 0; j < d; j++)
            {
                hashval = (hashes[j][0] * item + hashes[j][1]) % w;
                v.push_back(j);
                v.push_back(hashval);
                if(C[j][hashval] + c < USHRT_MAX) C[j][hashval] = C[j][hashval] + c;
            }
            return v;
        }
 
        /*
         * Update Str Data
         */
        vector<int> update(string str_, int c)
        {
            int hashval = hashstr(str_);
            return update(hashval, c);
        }
        /*
         * Estimate Min Value
         */
        ui estimate(int item)
        {
            int minval = numeric_limits<int>::max();
            unsigned int hashval = NULL;
            for (unsigned int j = 0; j < d; j++)
            {
                hashval = (hashes[j][0] * item + hashes[j][1]) % w;
                minval = MIN(minval, C[j][hashval]);
            }
            return minval;
        }
        /*
         * Estimate Min Str Value
         */
        ui estimate(string str_)
        {
            int hashval = hashstr(str_);
            return estimate(hashval);
        }
        /*
         * Total Count 
         */
        ui totalcount()
        {
            return total;
        }
        /*
         * Hashing String to Int
         */        
        ui hashstr(string str_)
        {
        	const char *str = str_.c_str();
            unsigned long hash = 5381;
            int c;
            while (c = *str++)
            {
                hash = ((hash << 5) + hash) + c;
            }
            return hash;
        }
};
 
/*
* Generate Random Hashes
*/
void CountMinSketch::genajbj(int** hashes, int i)
{
    hashes[i][0] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
    hashes[i][1] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
}

string to_lowest_denom(float count) {
	string denom = "ns";
	if(count > 1000) {
		count = count / 1000;
		denom = "us";
	}
	if(count > 1000) {
		count = count / 1000;
		denom = "ms";
	}
	if(count > 1000) {
		count = count / 1000;
		denom = "s";
	}

	return to_string(count) + denom;
}

std::chrono::time_point<std::chrono::high_resolution_clock> tracer(std::chrono::time_point<std::chrono::high_resolution_clock> start, string msg, int divide) {
	auto stop = high_resolution_clock::now(); //stop first to not include math or cout in timediff
	auto diff = duration_cast<nanoseconds>(stop - start); 
	float count = diff.count();
	float split = count / divide;

	cout << msg << ":" << to_lowest_denom(count) << ":" << to_lowest_denom(split) << endl;
	return high_resolution_clock::now(); //start next clock
}

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

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

int main() {

	chrono::high_resolution_clock::time_point t = high_resolution_clock::now(); 

	/*std::ofstream outfile;
	outfile.open("sellingIds.txt", std::ios_base::app); // append instead of overwrite

	for(unsigned int i = 0; i<100000000; ++i)
		outfile << random_string(35) << endl;*/

	long buckets = 30000000;
	int hashes = 3;
	int sellingIdCount = 10000000;

	std::ifstream ifs("sellingIds.txt");

	std::string line;

	CountMinSketch z(0.01, 0.1, buckets, hashes);

	/*
	#include "sys/types.h"
	#include "sys/sysinfo.h"

	struct sysinfo memInfo;

	sysinfo (&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	//Add other values in next statement to avoid int overflow on right hand side...
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	*/

	CountMinSketch sketchMin(0.01, 0.1, buckets, hashes);
	unordered_map<string, int> actualMap;

	unsigned int lines = 0;

	vector<int> v_;

	int duplicateLine = 0;

	while(std::getline(ifs, line)) {
		const char *b = line.c_str();
		int randomNumber = rand() % 100;
		if(actualMap.find(line) != actualMap.end()){
			duplicateLine++;
		} else {
			vector<int> v__ = sketchMin.update(b, randomNumber);
			//cout << sketchMin.estimate(b) << endl;
			actualMap.emplace(line, randomNumber);
			for(unsigned int i = 0; i<v__.size(); ++i)
				v_.push_back(v__[i]);
			if(lines>sellingIdCount) break;
		    lines++;
		}
	}

	cout << "Duplicate lines=" << duplicateLine << endl;

	t = tracer(t, "initialize minSketch", lines);

	int max_ = 0;
	const char *b = ":";
	for(unsigned int i=0; i<v_.size(); ++i) {
		string a_ = to_string(v_[i++]);
		string c_ = to_string(v_[i]);
		const char *a = a_.c_str();
		const char *c = c_.c_str();
		char bucket[30];
		strcpy(bucket, a);
		strcat(bucket, b);
		strcat(bucket, c);
		z.update(bucket, 1);
		max_ = MAX(z.estimate(bucket), max_);
	}

	t = tracer(t, "count collisions", lines);

	cout << "Lines inserted: " << lines << endl;
	cout << "Max collisions: " << max_ << endl; 

	int correct = 0;
	int wrong = 0;
	double wrongAmount = 0;
	int maxWrong = 0;
	int oversellCount = 0;
	std::unordered_map<string, int>::iterator it = actualMap.begin();
	while(it != actualMap.end())
	{
		int actual_ = it->second;
		const char *b = (it->first).c_str();
		int estimate_ = sketchMin.estimate(b);
		if(actual_ != estimate_)  {
			wrong++;
			int diff = estimate_ - actual_;
			if(diff < 0) oversellCount++;
			maxWrong = MAX(maxWrong, diff);
			wrongAmount += diff;
		}
		else correct++;
		it++;
	}

	int checked = correct + wrong;

	cout << "buckets: " << buckets << endl;
	cout << "hashes: " << hashes << endl;
	cout << "sellingIds: " << sellingIdCount << endl;
	cout << "correct: " << correct << endl;
	cout << "wrong: " << wrong << endl;
	cout << "pct: " << (100-(((double)wrong / (double)(correct+wrong))*100)) << "%" << endl;
	cout << "max wrong: " << maxWrong << endl;
	cout << "avg wrong: " << (wrongAmount/wrong) << endl;
	cout << "oversellCount: " << oversellCount << endl;

	t = tracer(t, "compare to real", checked);

	//const char *sId = "1D14B4A4C1D94F9CB1A155ED1B7EF2C6";

	return 0;
}