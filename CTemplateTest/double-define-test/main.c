//#include "template.template"// Test for error message.

#ifndef SI_TEMPLATE_TYPE
#define SI_TEMPLATE_TYPE int
#include "template.template"
#ifdef SI_TEMPLATE_TYPE
#undef SI_TEMPLATE_TYPE
#endif//SI_TEMPLATE_TYPE
#endif//SI_TEMPLATE_TYPE

int main(int argc, char** argv){
	temp();
	return 0;
}
