#include "impl.h"
#include <iostream>


//void timer_handler(){

//}

//void IPI_handler(){

//}

void cpu::init(thread_startfunc_t func, void * arg){
	cpu::self()->impl_ptr = new cpu::impl();
	//(cpu::self()->impl_ptr)->running_context = new ucontext_t();
	// add interrupt handler
	//interrupt_vector_table[cpu::TIMER] = impl_ptr->timer_handler;
	//interrupt_vector_table[cpu::IPI] = impl_ptr->timer_handler;
	if(impl_ptr->first && func != nullptr){
			thread(func, arg);
			impl_ptr->first = false;
			guard.store(false);
			//std::cout<<"first cpu init with func, set guard to false"<<std::endl;
	}
	
	while(1) {
	    //assert_interrupts_enabled();
    	//cpu::interrupt_disable();
    	assert_interrupts_disabled();
    	while(guard.exchange(true)) {}

    	assert(guard.load());
    
		if(!impl_ptr->ready_queue.empty()) {

			ucontext_t* current_thread = (cpu::self()->impl_ptr)->running_context;
		
		//getcontext(current_thread);
	        //cpu::self()->impl_ptr->ready_queue.push(current_thread);   
	        ucontext_t* chosen_thread = cpu::self()->impl_ptr->ready_queue.front();
	        cpu::self()->impl_ptr->ready_queue.pop();    
	        (cpu::self()->impl_ptr)->running_context = chosen_thread;

	        swapcontext(current_thread,chosen_thread);
	        assert(guard.load());
	        assert_interrupts_disabled();
	        //(cpu::self()->impl_ptr)->running_context = current_thread;

		} else {
			guard.store(false);
			cpu::interrupt_enable_suspend();
		}

		while(cpu::self()->impl_ptr->finished_queue.empty() == false){
			ucontext_t* temp = cpu::self()->impl_ptr->finished_queue.front();
			cpu::self()->impl_ptr->finished_queue.pop();
			delete (char*)(temp->uc_stack.ss_sp);
			delete temp;
		}
		guard.store(false);

		//assert_interrupts_disabled();
		cpu::interrupt_enable();
		assert_interrupts_enabled();
		//cpu::interrupt_enable_suspend();
	}
}



