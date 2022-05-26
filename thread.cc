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
    db<Thread>(TRC) << "Thread dispatcher iniciou execução";
    //TODO:Ajustes de sintaxe/
    while (thread_count>0){ //Enquanto ouverem Threads de usuário//
        Thread* next_thread = Thread::Ready_Queue::begin(); //Next_thread=Cabeça da Fila de Threads
        (Thread::_dispatcher)->_state= State::READY; //Estado da next_thread alterado para ready
        Thread::Ready_Queue::insert(Thread::_dispatcher); //Insere dispatcher no Ready_Queue
        
        //Definir a next_thread como a thread rodando
        Thread::_running= next_thread; 
        next_thread->_state = State::RUNNING;
        CPU::switch_context(/*TODO:inserir contexto atual*/,next_thread->_context);
        //remover next_thread da fila se tiver acabdo
        if (next_thread->_state == State::FINISHING){
            Thread::Ready_Queue::remove_head();
        }

    };
    (Thread_::_dispatcher)->_state = State::FINISHING; //Dispatcher em finishing
    Thread::Ready_Queue::remove(Thread->_link); //Remove dispatcher da fila
    //TODO:ver como referenciar para main thread
    CPU::switch_context((Thread::_dispatcher), main_thread); //Troca de contexto para main




}

__END_API