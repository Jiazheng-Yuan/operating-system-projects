#include "impl.h"
#include <new>

thread::thread(thread_startfunc_t func, void * arg) {
	if(!cpu::self()->impl_ptr->first){
        assert_interrupts_enabled();
        cpu::interrupt_disable();
        assert_interrupts_disabled();	   
	} else {
        cpu::self()->impl_ptr->first = false;
        assert_interrupts_disabled();
    }

    while(guard.exchange(true)) {}

    // try allocate space for new operation
    try{
        ucontext_t* ucontext_ptr = new ucontext_t();
        getcontext(ucontext_ptr);

        impl_ptr = new impl();

        char *stack = new char [STACK_SIZE];
        ucontext_ptr->uc_stack.ss_sp = stack;
        ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
        ucontext_ptr->uc_stack.ss_flags = impl_ptr->id;  // use id as an unique identifier
        ucontext_ptr->uc_link = nullptr;

        // make the context for this thread
        makecontext(ucontext_ptr, (void(*)()) &cpu::impl::func_wrapper, 3, func, arg, impl_ptr);
        cpu::impl::ready_queue.push(ucontext_ptr);
    } catch (std::bad_alloc& BadAlloc) { // unable to allocate space, throw error
        guard.store(false);
        cpu::self()->impl_ptr->enable_interrupt_and_assertion();
        throw(std::bad_alloc());
    }

    // wake suspended cpu
    cpu::self()->impl_ptr->wake_up_suspended_cpu();

    guard.store(false);
    cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

void thread::yield(){
    cpu::self()->impl_ptr->disable_interrupt_and_assertion();
    
    while(guard.exchange(true)) {}

    // put the current running context at the back of the ready queue
    // run the first context in the ready queue if there is one
    if(!cpu::impl::ready_queue.empty()) {
        assert(guard.load());

        ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;
        cpu::impl::ready_queue.push(current_thread);   
        ucontext_t* chosen_thread = cpu::impl::ready_queue.front();
        cpu::impl::ready_queue.pop();

        cpu::self()->impl_ptr->swap_and_set(current_thread, chosen_thread);   	
        cpu::self()->impl_ptr->swapcontext_invariant();
    } // if no ready context to run, run self

    assert(guard.load());

    cpu::self()->impl_ptr->clear_finish_queue();

    guard.store(false);
	cpu::self()->impl_ptr->enable_interrupt_and_assertion();
}

void thread::join(){
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();

    while(guard.exchange(true)) {}

    // if the joined thread context is not finished, add running context to joined thread's join waiting queue
    if (!impl_ptr->is_finished){
        ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;
        impl_ptr->join_waiting_queue.push(current_thread);
        if(!cpu::impl::ready_queue.empty()){
            ucontext_t* chosen_thread = cpu::impl::ready_queue.front();
            cpu::impl::ready_queue.pop();
            cpu::self()->impl_ptr->swap_and_set(current_thread, chosen_thread);
        } else {
            cpu::self()->impl_ptr->swap_and_set(current_thread, (cpu::self()->impl_ptr)->cpu_context);
        }
        cpu::self()->impl_ptr->clear_finish_queue();
    } // else, just go through

	guard.store(false);
    cpu::self()->impl_ptr->enable_interrupt_and_assertion();	
}

thread::~thread(){
    if (impl_ptr->can_delete_impl_ptr){
        delete impl_ptr;
    } else {
        impl_ptr->can_delete_impl_ptr = true;
    }
}
