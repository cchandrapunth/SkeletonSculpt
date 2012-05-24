#include <fstream>
using namespace std;


class Log {
public: 
	ofstream m_stream;
    Log(char* filename);
    ~Log();
    void Write(char* logline);
    
};