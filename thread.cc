#include "thread.h"

__BEGIN_API
// Declaração inicial do uid das threads
int unsigned Thread::thread_count = 0;
// Declaração inicial do ponteiro que aponta para thread
// que está rodando
Thread* Thread::_running = 0;

void Thread::thread_exit(int exit_code) {
    //Implementação da destruição da thread
    //UPDATE: debug = db<>....
    delete this->_context;
    Thread::thread_count --;
}

/*
* Retorna o id de uma thread em específico
*/
int Thread::id() {
    return this->_id;
}

__END_API