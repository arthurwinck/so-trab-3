#include "thread.h"
#include "main_class.h"

__BEGIN_API

// Declaração inicial dos atributos dda classe
int unsigned Thread::thread_count = 0;
Thread* Thread::_running = 0;
Thread Thread::_dispatcher;
Thread Thread::_main;
Thread::Ready_Queue Thread::_ready;
CPU::Context Thread::_main_context;

//Thread::Ready_Queue Thread::_ready = new Thread::Ready_Queue();

void Thread::thread_exit(int exit_code) {
    //Implementação da destruição da thread
    db<Thread>(TRC) << "Método thread_exit iniciou execução\n";
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
    db<Thread>(TRC) << "Método Thread::init iniciou execução\n";

    //Criação de adição de casting para as funções, além de adicionar uma string ao final para obedecer a chamada da função

    //Preciso incrementar o thread_count aqui?
    Thread* main_thread = new Thread((void(*)(char*)) main, (char*) "Thread Main");
    Thread* dispatcher_thread = new Thread((void(*)(char*)) Thread::dispatcher, (char*) "Thread Dispatcher");
    
    // Inicialização dos atributos _dispatcher e _main
    Thread::_dispatcher = *dispatcher_thread;
    Thread::_main = *main_thread;

    //Inicialização do atributo lista _ready
    Thread::_ready = Thread::Ready_Queue();

    //Inicialização do atributo contexto
    Thread::_main_context = *Thread::_main._context;

    //Atualização do _running para main
    Thread::_running = main_thread;


    // Troca de CONTEXTO, criação de um contexto vazio para realizar a troca
    Context* mock_context = new CPU::Context();
    CPU::switch_context(mock_context, main_thread->_context);
};

void Thread::dispatcher() {
    db<Thread>(TRC) << "Thread dispatcher iniciou execução\n";
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

//Destrutor da classe
Thread::~Thread() {
    if (this->_context) {
        delete this->_context;
    }
}

__END_API