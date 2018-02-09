#include "impl.h"
#include <iostream>


int thread::impl::count = 0;
thread::impl::impl(){
	is_finished = false;
	can_delete_impl_ptr = false;
	id = count;
	count++;
}

void cpu::impl::clear_finish_queue(){
	while(!cpu::impl::finished_queue.empty()){
 		ucontext_t* temp = cpu::impl::finished_queue.front();
 		cpu::impl::finished_queue.pop();
 		delete [] (char*)(temp->uc_stack.ss_sp);
 		delete temp;
 	}
}

void cpu::impl::disable_interrupt_and_assertion(){
	assert_interrupts_enabled();
    cpu::interrupt_disable();
    assert_interrupts_disabled();
}

void cpu::impl::enable_interrupt_and_assertion(){
	assert_interrupts_disabled();
	cpu::interrupt_enable();
    assert_interrupts_enabled();
}

void cpu::impl::swapcontext_invariant(){
	assert(guard.load());
	assert_interrupts_disabled();
}

void cpu::impl::swap_and_set(ucontext_t* from, ucontext_t* to){
	cpu::self()->impl_ptr->running_context = to;
	swapcontext(from, to);
	cpu::self()->impl_ptr->running_context = from;
}

void cpu::impl::wake_up_suspended_cpu(){
	if (!cpu::impl::suspended_cpu_queue.empty()){
		cpu* suspended_cpu = cpu::impl::suspended_cpu_queue.front();
		cpu::impl::suspended_cpu_queue.pop();
		suspended_cpu->interrupt_send();
	}
}

void cpu::impl::func_wrapper(thread_startfunc_t func, void * arg, thread::impl* current) {
	assert_interrupts_disabled();
	assert(guard.load());

	// clear finish queue when swap to the beginning of func_wrapper
	cpu::self()->impl_ptr->clear_finish_queue();

	guard.store(false);
 	cpu::self()->impl_ptr->enable_interrupt_and_assertion();

 	// run user code
    (*func)(arg);

    // swith back to kernel code, set interrupt disable and guard to true
	cpu::self()->impl_ptr->disable_interrupt_and_assertion();
    while(guard.exchange(true)) {}
	assert(guard.load());

	cpu::self()->impl_ptr->clear_finish_queue();

    ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;  

    cpu::impl::finished_queue.push(current_thread);

    // make this thread's joiners ready to run
    while(!current->join_waiting_queue.empty()){		
		ucontext_t* join_waiting_context = current->join_waiting_queue.front();
		current->join_waiting_queue.pop();
		cpu::self()->impl_ptr->ready_queue.push(join_waiting_context);
		// wake suspended cpu
		cpu::self()->impl_ptr->wake_up_suspended_cpu();
	}
	// mark this context as finished
	current->is_finished = true;

	// delete the current impl_ptr if thread obj is out of scope, else tell the destructor to detele impl_ptr
	if (current->can_delete_impl_ptr){
		delete current;
	} else {
		current->can_delete_impl_ptr = true;
	}

	// switch to ready context if there is one, otherwise switch to cpu init context
	if(!cpu::impl::ready_queue.empty()) {
		ucontext_t* chosen_thread = cpu::impl::ready_queue.front();
	    cpu::impl::ready_queue.pop();

		cpu::self()->impl_ptr->running_context = chosen_thread;
		swapcontext(current_thread, chosen_thread);//set
		// never come back
		assert(0);
	} else { // ready queue empty
		cpu::self()->impl_ptr->running_context = cpu::self()->impl_ptr->cpu_context;
        swapcontext(current_thread, cpu::self()->impl_ptr->cpu_context); 
        // never come back
        assert(0); 
    } 
}

cpu::impl::impl() {
    cpu_context = new ucontext_t();
    running_context = cpu_context;
}
bool cpu::impl::first = true;
std::queue<ucontext_t*> cpu::impl::ready_queue;
std::queue<ucontext_t*> cpu::impl::finished_queue;
std::queue<cpu*> cpu::impl::suspended_cpu_queue;
void cpu::impl::timer_handler(){
	// do not put cpu context on ready queue
	if (cpu::self()->impl_ptr->running_context != cpu::self()->impl_ptr->cpu_context){
		thread::yield();
	}
}

void cpu::impl::IPI_handler(){
	
}

void mutex::impl::valid_lock_and_lock_holder(){
	// check if the lock is busy
	if(!is_busy){
		guard.store(false);
        cpu::self()->impl_ptr->enable_interrupt_and_assertion();
		throw std::runtime_error("lock not busy");
	}
	// check if the current thread is the lock holder by comparing the thread id with the context ss_flags
	if(lock_holder != cpu::self()->impl_ptr->running_context->uc_stack.ss_flags){
		guard.store(false);
        cpu::self()->impl_ptr->enable_interrupt_and_assertion();
		throw std::runtime_error("not lock holder");	 
	}
}

void mutex::impl::unlock_helper(){
	// free the lock
	is_busy = false;
	lock_holder = -1;

	// make the next context waiting for this lock ready to run
	// and handoff the lock to it
	if(!waiting_queue.empty()){
		assert(guard.load());
		ucontext_t* waiting_ptr = waiting_queue.front();

		waiting_queue.pop();
		cpu::impl::ready_queue.push(waiting_ptr);
		is_busy = true;
		lock_holder = waiting_ptr->uc_stack.ss_flags;

		// wake suspended cpu
		cpu::self()->impl_ptr->wake_up_suspended_cpu();
	}
}

void cv::impl::make_waiting_context_ready(){
	ucontext_t* signal_context = waiting_queue.front();
	waiting_queue.pop();
	// make context ready
	cpu::impl::ready_queue.push(signal_context);
	// wake suspended cpu
	cpu::self()->impl_ptr->wake_up_suspended_cpu();
}
