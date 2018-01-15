
#include "impl.h"
#include <iostream>

thread::thread(thread_startfunc_t func, void * arg) {
    ucontext_t* ucontext_ptr = new ucontext_t;
    getcontext(ucontext_ptr);
    char *stack = new char [STACK_SIZE];
    ucontext_ptr->uc_stack.ss_sp = stack;
    ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
    ucontext_ptr->uc_stack.ss_flags = 0;
    ucontext_ptr->uc_link = nullptr;
    impl_ptr = new impl();
    //makecontext(ucontext_ptr, ((void(*)()) impl_ptr->func_wrapper), 2, func, arg);
    //makecontext(ucontext_ptr,(void(*)())func, 1, arg);
    makecontext(ucontext_ptr, (void(*)()) &impl::func_wrapper, 2, func, arg);
    (cpu::self()->impl_ptr)->ready_queue.push(ucontext_ptr);
}

void thread::yield(){
    assert_interrupts_enabled();
    cpu::interrupt_disable();
    assert_interrupts_disabled();
    assert(!guard.load());

    while(guard.exchange(true)) {}

    if(!cpu::self()->impl_ptr->ready_queue.empty()) {
        assert(guard.load());

        ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;
        cpu::self()->impl_ptr->ready_queue.push(current_thread);   
        ucontext_t* chosen_thread = cpu::self()->impl_ptr->ready_queue.front();
        cpu::self()->impl_ptr->ready_queue.pop();
    	(cpu::self()->impl_ptr)->running_context = chosen_thread;
        swapcontext(current_thread,chosen_thread);
        assert(guard.load());
        assert_interrupts_disabled();
    	(cpu::self()->impl_ptr)->running_context = current_thread;
    }

    assert(guard.load());
    guard.store(false);
    
    // delete?
    
    //assert_interrupts_disabled();
    cpu::interrupt_enable();
 	//std::cout << "thread yield enabled interrupt --------- "<<std::endl;
    assert_interrupts_enabled();
}

thread::~thread(){delete impl_ptr;}
