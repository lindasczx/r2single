#include "build.h"

// version
#define _S(x) _S1(x)
#define _S1(x) #x
#define _W(x) _W1(x)
#define _W1(x) L##x
#define FILEVER                         1,50,0,APIVER
#define FILEVERSTR                      "1.50.0." _S(APIVER) /*" BETA"*/ "\0"
#define PRODUCTVER                      FILEVER
#define PRODUCTVERSTR                   FILEVERSTR

// res
#define IDT_TYPELIB1                    1

