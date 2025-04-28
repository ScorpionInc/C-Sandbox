#include <stdio.h>

#define PASTE_TWO_TOKENS(token_1, token_2) token_1 ## token_2
#define PASTE_TWO_TOKENS_INDIRECT(token_1, token_2) PASTE_TWO_TOKENS(token_1 , token_2)

#define DA A
//#define DB DA ## B// Fails: DAB
//#define DB PASTE_TWO_TOKENS(DA, B)// Fails: DAB
#define DB PASTE_TWO_TOKENS_INDIRECT(DA, B)//Success

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)

#define DC 'C'
#define DD 420

#define DE int

int main(){
	printf("> '"STRINGIFY(DB)"%c%i'\n", DC, DD);
	return 0;
}
