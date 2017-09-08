
#include <stdio.h>
#include <mutex>


#define LOG_LINE_LENGHT = 256;
#define LOG_HISTORY_LENGHT = 256;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace Logger {

	char* logBuffer;

	std::mutex mtx;

	bool initialized = false;

	void init();

	void log(const char* string){
		mtx.lock();
		printf("%s\n", string);
		mtx.unlock();
	}

	void error(const char* string){
		mtx.lock();
		printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "\n", string);
		mtx.unlock();
	}

	void warn(const char* string){
		mtx.lock();
		printf(ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET "\n", string);
		mtx.unlock();
	}

	void init(){

		logBuffer = (char*)malloc(256*256*sizeof(char));

		log("Logger initialized, testing...");
		error("LOGGER :: ERROR :: TESTING");
		warn("LOGGER :: WARN :: TESTING");

		initialized = true;

	}

}