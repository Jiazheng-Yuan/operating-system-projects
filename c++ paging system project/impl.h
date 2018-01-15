#ifndef _IMPL_H_
#define _IMPL_H_

#include "vm_arena.h"
#include "vm_app.h"
#include "vm_pager.h"
#include <sys/types.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <utility>
#include <string>


extern size_t total_virtual_pages;
extern size_t swap_block_num;
extern size_t used_swap_blocks;

class pageInfo { 
public:
    pageInfo(std::string, size_t);   
    unsigned int read: 1;       
    unsigned int write: 1;       
	  int pageNumber;            
    bool dirty;
    bool resident;
    bool ref;
    std::string filename_string;
    bool is_unnamed_fbpage;
    size_t block;
};

struct fileblock
{
  std::string file_name_string;
  size_t block;
  bool is_file_back_page;

  fileblock(std::string, size_t);

  bool operator<(const fileblock&) const;
};

class processInfo {
public:
   processInfo(pid_t);
   pid_t pid;
   page_table_t* pageTable;
   void* baseAddr;
   size_t num_of_swap;
   char* currentBase;
   std::vector<pageInfo*> virtual_page_info_directory;
   ~processInfo();
};

extern std::set<size_t> unused_swapfile;

extern std::map<pid_t, processInfo*> pid_to_processInfo;

extern std::vector<int> unused_pages;

extern std::deque<pageInfo*> page_clock;


extern std::map<fileblock, pageInfo*> fileblock_pageInfo;

extern pageInfo* pinned_page;

extern pid_t current_process;

extern std::map<pageInfo*, std::vector<std::pair<processInfo*, int> > >  pageInfo_virtualPageNoPerProcess;    //  <physical page number, <process, virtual page number> >

extern char copy_buffer[VM_PAGESIZE];

// evict a victim and return the victim's physical page number
extern int evict_page(); 

extern int copy_on_write(processInfo* current_process_Info, pageInfo* oldPageInfo,int virtual_page_number);

extern void update_pageTables(pageInfo*, bool, bool, int);

extern void clear_pageTables(pageInfo*);

extern bool is_swap_backed_page(pageInfo*);

extern void clear_memory_page(int pageNumber);

//set the read bit for a clean physical page and the virtual pages mapped to it in case of a read fault
extern void set_read_enable(pageInfo*);

//return true when ref is 0, dirty is 1, resident is 1
extern bool judge_write_enable(pageInfo*);

extern int non_resident(pageInfo*);
#endif /* IMPL_H */
