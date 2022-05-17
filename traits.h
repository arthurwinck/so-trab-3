#ifndef traits_h
#define traits_h

// UPDATE: Adicionar se quer ou não e quais o debugging são os níveis para cada classe do sistema
//Não alterar as 3 declarações abaixo

#define __BEGIN_API             namespace SOLUTION {
#define __END_API               }
#define __USING_API            using namespace SOLUTION;

__BEGIN_API

class CPU; //declaração das classes criadas nos trabalhos devem ser colocadas aqui
class Debug; //declaração do Debug para que possamos colocar os atributos

//declaração da classe Traits
template<typename T>
struct Traits {
};
// UPDATE: Fazer isso para todas as classes do sistema
template<> struct Traits<System> : public Traits<void> {
    static const bool debugged = true;
}

template<> struct Traits<Debug>: public Traits<void>
{
    static const bool error   = false;
    static const bool warning = false;
    static const bool info    = false;
    static const bool trace   = false;
};

template<> struct Traits<CPU>
{
    //Adicionar aqui a declaração do tamanho da Pilha
    static const int STACK_SIZE = 64000;
};

__END_API

#endif
