#include "thread.h"
#include "main_class.h"

__BEGIN_API

// Declaração inicial do uid das threads
int unsigned Thread::thread_count = 0;
// Declaração inicial do ponteiro que aponta para thread
// que está rodando
Thread* Thread::_running = 0;
Thread::Ready_Queue Thread::_ready;
Thread Thread::_dispatcher;
Thread Thread::_main;
CPU::Context Thread::_main_context;


void Thread::thread_exit(int exit_code) {
    //Implementação da destruição da thread
    db<Thread>(TRC) << "Método thread_exit iniciou execução";
    //Correções - Solução do professor
    this->_state = FINISHING;
    //thread_exit chama o yield
    Thread::yield();
    
    //
    // delete this->_context;// Implementar no Destrutor da classe
    // Thread::thread_count --; // Implementar no dispatcher
    
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
    // Criação das threads para que vazamento de memória não ocorra
    new (&_main) Thread(main, (void *) "main");
    new (&_dispatcher) Thread((void (*) (void*)) &Thread::dispatcher, (void*) NULL);

    // Como inserir ele na fila usando o _link? 
    //_ready.insert()

    //Fazer assim dá ruim e pode dar vazamento de memória
    // Thread* main_thread = new Thread((void(*)(char*)) main, (char*) "Thread Main");
    // Thread* dispatcher_thread = new Thread((void(*)(char*))Thread::dispatcher, (char*) "Thread Dispatcher");

    _running = & _main;

    _main._state = State::RUNNING;
    new (&_main_context) CPU::Context();

    
    
    // Troca de CONTEXTO, criação de um contexto vazio para realizar a troca
    //Context* mock_context = new CPU::Context();
    CPU::switch_context(&_main_context, _main._context);
};

void Thread::dispatcher() {
    //Utilizar o contador de threads dentro do dispatcher
    db<Thread>(TRC) << "Thread dispatcher iniciou execução";
    //TODO:Ajustes de sintaxe/
    //Correções -> Checar enquanto thread_count for maior que 2
    while (thread_count>2){ //Enquanto ouverem Threads de usuário//
        
        //Errado - 
        //Ready_Queue::Element* next_element = Thread::_ready.head(); 
        
        Ready_Queue::Element* next_element = _ready.head();

        //Iterar sobre a lista até encontrar uma thread que n seja a dispatcher
        while (next_element->object()->_id != 1) {
            // Talvez n seja o melhor jeito, revisar outros métodos de list
            Ready_Queue::Element* next_element = next_element->next();
        }

        _dispatcher._state = State::READY; //Estado da next_thread alterado para ready
        _ready.insert(&_dispatcher._link); //Insere dispatcher no Ready_Queue
        
        Thread* next_thread = next_element->object(); // Pegar o objeto Thread de dentro do elemento
        _running= next_thread; //Definir a next_thread como a thread rodando 
        
        //remover next_thread da fila se tiver acabdo
        //Verificar lógica de testar o finishing
        if (next_thread->_state == State::FINISHING){
            //Não atualizar timestamp
            // Dispatcher não pode escolher thread com estado finishing, remove da lista
            Thread::_ready.remove_head();
        } else {
            next_thread->_state = State::RUNNING;
        }

        // Executar o switch_context da thread
        switch_context((&_dispatcher), next_thread);
 
    };

    Thread::_dispatcher._state = State::FINISHING; //Dispatcher em finishing
    Thread::_ready.remove(&Thread::_dispatcher._link); //Remover dispatcher da fila
    CPU::switch_context(Thread::_dispatcher._context, &Thread::_main_context); //Troca de contexto para main

}

Thread::~Thread() {
    delete this->_context;
}

__END_API