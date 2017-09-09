#include <cstdio>


void (*func[128])(void);


void addCallback( void(*function)(void) ){
	for (int i = 0; i < 128; ++i) {
		if(func[i]==NULL) {
			
			func[i] = function;
			return;
		}
	}
	printf("ERROR\n");
}

void callAll(){
	for (int i = 0; i < 128; ++i) {
		if(func[i] != NULL)
			func[i]();
	}
}

void unbind( void(*function)(void) ){
	for (int i = 0; i < 128; ++i) {
		if(func[i]==function) {
			printf("DEL\n");
			func[i] = NULL;
		}
	}	
}


void functionToBeCalled(void){
	printf("PING!\n");
}


int main(){
	addCallback( *functionToBeCalled );
	addCallback( *functionToBeCalled );
	addCallback( *functionToBeCalled );
	addCallback( [](){ printf("LAMBDA!\n"); } );

	unbind( *functionToBeCalled );

	callAll();

	return 1;
}

namespace observer {

	typedef struct {
		void*(*fn)(void*);
	} Observer;



}