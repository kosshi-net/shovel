
#include <stdio.h>
#include <mutex>


#define LOG_LINE_LENGHT = 256;
#define LOG_HISTORY_LENGHT = 256;

namespace Logger {

	char* logBuffer;

	std::mutex mtx;

	bool initialized = false;

	void initialize(){

		logBuffer = (char*)malloc(256*256*sizeof(char));

		initialized = true;

	}


	void log(const char* string){
		mtx.lock();

		if(!initialized) initialize();

		printf("%s\n", string);

		mtx.unlock();
	}


}