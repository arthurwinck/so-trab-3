#include "system.h"

__BEGIN_API
//Definição do método init, Header já especifica que é Static
void System::init() {
    setvbuf (stdout, 0,_IONBF, 0);
}

__END_API