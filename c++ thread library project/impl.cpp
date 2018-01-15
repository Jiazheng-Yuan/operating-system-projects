//#include <queue>
////#include <ucontext.h>
//#include "thread.h"
#include "impl.h"
#include <iostream>
//using namespace std;
void thread::impl::func_wrapper(thread_startfunc_t func, void * arg) {
    assert_interrupts_disabled();
    cpu::interrupt_enable();
    assert_interrupts_enabled();
    assert(guard.load());
    guard.store(false);

    (*func)(arg);

	assert_interrupts_enabled();
    cpu::interrupt_disable();
    assert_interrupts_disabled();
    while(guard.exchange(true)) {}

    ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;   
    cpu::self()->impl_ptr->finished_queue.push(current_thread);
	if(!cpu::self()->impl_ptr->ready_queue.empty()) {
		
		ucontext_t* chosen_thread = cpu::self()->impl_ptr->ready_queue.front();
	    cpu::self()->impl_ptr->ready_queue.pop();
		swapcontext(current_thread,chosen_thread);
	    assert_interrupts_disabled();
	}


	guard.store(false);
	assert_interrupts_disabled();
	cpu::interrupt_enable();
	assert_interrupts_enabled();
}

cpu::impl::impl() {
    running_context = new ucontext_t();
}
bool cpu::impl::first = true;
std::queue<ucontext_t*> cpu::impl::ready_queue;
std::queue<ucontext_t*> cpu::impl::finished_queue;
/*void cpu::impl::timer_handler(){
	assert_interrupts_enabled();
    cpu::interrupt_disable();
   	assert_interrupts_disabled();
    if(!impl_ptr->ready_queue.empty()) {
			ucontext_t* current_thread = (impl_ptr)->running_context;
			impl_ptr->ready_queue.push(current_thread);   //add lock in the future
			ucontext_t* chosen_thread = impl_ptr->ready_queue.front();
			impl_ptr->ready_queue.pop();    // unlock
			swapcontext(current_thread,chosen_thread);
    }
    
    // delete?
    
    assert_interrupts_disabled();
    cpu::interrupt_enable();
    assert_interrupts_enabled();
}*/

