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
    db<Thread>(TRC) << "Método thread_exit iniciou execução";
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
    db<Thread>(TRC) << "Método Thread::init iniciou execução";

    //Criação de adição de casting para as funções, além de adicionar uma string ao final para obedecer a chamada da função
    Thread* main_thread = new Thread((void(*)(char*)) main, (char*) "Thread Main");
    Thread* dispatcher_thread = new Thread((void(*)(char*))Thread::dispatcher, (char*) "Thread Dispatcher");

    // Troca de CONTEXTO, criação de um contexto vazio para realizar a troca
    Context* mock_context = new CPU::Context();
    CPU::switch_context(mock_context, main_thread->_context);
};

void Thread::dispatcher() {
    db<Thread>(TRC) << "Thread dispatcher iniciou execução";
    //TODO:Ajustes de sintaxe/
    while (thread_count>0){ //Enquanto ouverem Threads de usuário//
        
        
        Ready_Queue::Element* next_element = Thread::_ready.head(); 
        Thread* next_thread = next_element->object(); // Pegar o objeto Thread de dentro do elemento
        Thread::_dispatcher._state = State::READY; //Estado da next_thread alterado para ready
        Thread::_ready.insert_head(&Thread::_dispatcher._link); //Insere dispatcher no Ready_Queue
        //Definir a next_thread como a thread rodando
        Thread::_running= next_thread; 
        next_thread->_state = State::RUNNING;
        //remover next_thread da fila se tiver acabdo
        if (next_thread->_state == State::FINISHING){
            Thread::_ready.remove_head();
        }

        CPU::switch_context(Thread::_dispatcher._context,next_thread->_context);
 
    };

    Thread::_dispatcher._state = State::FINISHING; //Dispatcher em finishing
    Thread::_ready.remove(&Thread::_dispatcher._link); //Remover dispatcher da fila
    CPU::switch_context(Thread::_dispatcher._context, &Thread::_main_context); //Troca de contexto para main

}

__END_API