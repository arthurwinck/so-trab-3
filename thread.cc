#include "thread.h"
#include "main_class.h"

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
};

void Thread::init(void (*main)(void *)) {
    //TODO - Adicionar código de debug

    //Criação de adição de casting para as funções, além de adicionar uma string ao final para obedecer a chamada da função
    Thread* main_thread = new Thread((void(*)(char*)) main, (char*) "Thread Main");
    Thread* dispatcher_thread = new Thread((void(*)(char*))Thread::dispatcher, (char*) "Thread Dispatcher");

    //TODO - O que colocar em prev* já que é a primeira thread a rodar?
    // Troca de CONTEXTO, criação de um contexto vazio para realizar a troca
    Context* mock_context = new CPU::Context();
    CPU::switch_context(mock_context, main_thread->_context);
};

void Thread::dispatcher() {
    while (thread_count>0){
        
    }
}

__END_API