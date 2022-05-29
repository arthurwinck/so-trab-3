#include "cpu.h"
#include <iostream>
#include "ucontext.h"

__BEGIN_API

void CPU::Context::save()
{
    //adicionar implementação
    getcontext(&this->_context);
    //utiliza o método getcontext para salvar o contexto atual do objeto apontado por _context
}

void CPU::Context::load()
{
    //adicionar implementação
    setcontext(&this->_context);
    //restaura o contexto salvo apontado pelo _context
}

CPU::Context::~Context()
{
    //adicionar implementação
    //chamado quando o contexto é deletado no final de main
    //free(_context.uc_stack.ss_sp); //remove alocação para stack
    //free(_context.uc_link); //remove valor para executar ao retornar de makecontext

    //Update para a destruição da stack
    if (this->_stack) {
        delete this->_stack;
    }
}

__END_API
