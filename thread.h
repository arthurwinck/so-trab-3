#ifndef thread_h
#define thread_h

#include "cpu.h"
#include "traits.h"
#include "debug.h"
#include "list.h"
#include <ctime> 
#include <chrono>

__BEGIN_API

class Thread
{
protected:
    typedef CPU::Context Context;
public:

    typedef Ordered_List<Thread> Ready_Queue;

    // Thread State
    enum State {
        RUNNING,
        READY,
        FINISHING
    };

    /*
     * Construtor vazio. Necessário para inicialização, mas sem importância para a execução das Threads.
     */ 
    Thread() { }

    /*
     * Cria uma Thread passando um ponteiro para a função a ser executada
     * e os parâmetros passados para a função, que podem variar.
     * Cria o contexto da Thread.
     * PS: devido ao template, este método deve ser implementado neste mesmo arquivo .h
     */ 
    template<typename ... Tn>
    Thread(void (* entry)(Tn ...), Tn ... an);

    /*
     * Retorna a Thread que está em execução.
     */ 
    static Thread * running() { return _running; }

    /*
     * Método para trocar o contexto entre duas thread, a anterior (prev)
     * e a próxima (next).
     * Deve encapsular a chamada para a troca de contexto realizada pela class CPU.
     * Valor de retorno é negativo se houve erro, ou zero.
     */ 
    static int Thread::switch_context(Thread * prev, Thread * next) {
        db<Thread>(TRC) << "Trocando contexto Thread::switch_context()";
        if (prev && next) {
            //UPDATE: ORDEM ERRADA, primeiro se troca o _running depois executa switch_context (UPDATE: Fazemos isso em yield())
            // Se for feito do jeito inverso, quando chega em switch_context o código n executa mais 
            CPU::switch_context(prev->_context, next->_context);
            return 0;
        } else {
            return -1;
        }
    }
    /*
     * Termina a thread.
     * exit_code é o código de término devolvido pela tarefa (ignorar agora, vai ser usado mais tarde).
     * Quando a thread encerra, o controle deve retornar à main. 
     */  
    void thread_exit (int exit_code);

    /*
     * Retorna o ID da thread.
     */ 
    int id();

    /*
     * NOVO MÉTODO DESTE TRABALHO.
     * Daspachante (disptacher) de threads. 
     * Executa enquanto houverem threads do usuário.
     * Chama o escalonador para definir a próxima tarefa a ser executada.
     */
    static void dispatcher(); 

    /*
     * NOVO MÉTODO DESTE TRABALHO.
     * Realiza a inicialização da class Thread.
     * Cria as Threads main e dispatcher.
     */ 
    static void init(void (*main)(void *));


    /*
     * Devolve o processador para a thread dispatcher que irá escolher outra thread pronta
     * para ser executada.
     */
    static void yield() {
        db<Thread>(TRC) << "Thread iniciou processo de yield";
        
        //Validando a thread que está rodando
        if (Thread::_running->_state != State::FINISHING) {
            //Thread está terminando, não vamos colocar ela na fila
            // TODO.... (?)
        } else if (Thread::_running->id() == 0) {
            db<Thread>(TRC) << "[Thread Main] iniciou processo de yield";
            //Essa é a thread main, se ela deu yield quer dizer que nosso programa está terminando 
            // TODO.... (?)
        } else {
            //Inserir a thread que estava rodando novamente na lista de prontos e alterar o seu estado
            Thread * prev = Thread::_running;
            prev->_state = State::READY;
            
            //prev->_link = prev->_link(Thread::_running, (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()));
            //Ver como eu fiz na construção do método, como chamar ele no yield?
            
            //Remover a próxima thread da fila para colocá-la em execução, mudando seu estado no método switch_context
            //Na fila temos o tipo ELEMENT, precisamos retirar esse ELEMENT e pegar a thread de dentro dele, usando o object()
            Ready_Queue::Element* next_element = Thread::_ready.remove_head();
            Thread* next = next_element->object();
            //Atualizo o ponteiro _running para a thread que está executando
            Thread::_running = next;
            Thread::switch_context(prev, next);
        }
        
        // remove_head retorna um Element
        Ready_Queue::Element* next_element = _ready.remove_head();
        // Pego a thread de dentro do elemento
        Thread* next = next_element->object();

        

    }


    /*
     * Destrutor de uma thread. Realiza todo os procedimentos para manter a consistência da classe.
     */ 
    ~Thread();

    /*
     * Qualquer outro método que você achar necessário para a solução.
     */ 

private:
    int _id;
    Context * volatile _context;
    static Thread * _running;
    static unsigned int thread_count; 
    static Thread _main; 
    static CPU::Context _main_context;
    static Thread _dispatcher;
    static Ready_Queue _ready;
    Ready_Queue::Element _link;
    volatile State _state;

    /*
     * Qualquer outro atributo que você achar necessário para a solução.
     */ 

};

template<typename ... Tn>
inline Thread::Thread(void (* entry)(Tn ...), Tn ... an) : _link(this, (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()))
{
    //IMPLEMENTAÇÃO DO CONSTRUTOR
    //UPDATE: Chamada do debugger
    db<Thread>(TRC) << "Thread::Thread(): criou thread";
    //Criação do Contexto...
    this->_context = new CPU::Context(entry, an...);
    //... Outras inicializações
    // Incremento o valor de id para gerar um novo id para a thread (Update para não usar getter)
    this->_id = Thread::uid ++;
    //Alterar status para ready
    this->_state = State::READY;
    // Preciso realizar a atribuição de new (?) e adicionar o elemento na fila
    // Inserir a thread na fila de prontos
    Thread::Ready_Queue::insert(this->_link);
    
}
__END_API

#endif
