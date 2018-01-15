<html>
<head>
<title>
scheduler.cpp
</title>
</head>
<body>
<h2>
scheduler.cpp
</h2>
<pre>
#include &quot;thread.h&quot;
#include&lt;iostream&gt;
#include&lt;stdlib.h&gt;
#include&lt;vector&gt;
#include &quot;disk.h&quot;
#include &lt;fstream&gt;
#include&lt;string&gt;
#include&lt;queue&gt;
//#include &lt;algorithm&gt;
using namespace std;

mutex mtx;

cv cv1;
cv cv2;
int n = 0;//argc
//cv cvrep;
char **argu = NULL;//argument pointer

long long count = 0;//giving the requester id
int track = 0;//current track  the scheduler is on
int total = 0;//amount of total request left which is not done yet
//int total_request = 0;
int thread_alive = 0;
struct Job{
	int req_id;
	int track;
	Job(int id, int track):req_id(id),track(track){}
};
vector&lt;Job&gt; job_queue;
void pin(void *a){
	
	vector&lt;int&gt; track_vec;	
	intptr_t arg = (intptr_t) a;
	int id = arg;
	string file = argu[id + 2];
	string line;
	ifstream myfile(file);
	
	if (myfile.is_open()){
    while (getline (myfile,line)){
    	//total ++;
    	track_vec.push_back(atoi(line.c_str()));
    }
    
    myfile.close();
	}
	
	unsigned int i = 0;
	mtx.lock();
		while(i &lt; track_vec.size() + 1){
			bool exist = false;
			for(unsigned int j = 0; j &lt; job_queue.size();j++){
				if(job_queue[j].req_id == id)
					exist = true;
			}
			if(!exist and i == track_vec.size()){
				break;
			}
			if((int)job_queue.size() &gt;= min(thread_alive,atoi(argu[1]))){
				
					cv2.broadcast();
					//full++;
		  			cv1.wait(mtx);
		  			
			}
			else if((int)job_queue.size() &lt; min(thread_alive,atoi(argu[1])) and !exist and i &lt; track_vec.size()){
				
		  		print_request(id,track_vec[i]);
		  		Job job(id,track_vec[i]);
		  		job_queue.push_back(job);
		  		if((int)job_queue.size() == min(thread_alive,atoi(argu[1]))){
		  			cv2.broadcast();
		  		}
		  		//total_request--;
		  		i++;
		  		cv1.wait(mtx);
		  		
		  	}
		  	else{
		  		
		  		//cv1.broadcast();
				cv1.wait(mtx);
				//cv1.broadcast();
			}
		}
	thread_alive--;
	cv2.broadcast();
	mtx.unlock();	
	//cv1.signal();
  	//mtx.lock();
	//cout&lt;&lt;&quot;here&quot;&lt;&lt;endl;
	//mtx.unlock();
	
}

void scheduler(void *a){
	//char **arguments = (char **) a;
	//vector&lt;thread&gt; requesters;
	//thread test((thread_startfunc_t)pin,(void*) 100);
	
		//requesters.push_back(test);
	for(int i = 0; i &lt; n - 2;i++){
		//mtx.lock();
		thread test((thread_startfunc_t)pin,(void*) count);

		//mtx.unlock();
		count = count + 1;
	}
	mtx.lock();
	while(total &gt;=1){
		
		if(((int)job_queue.size() &gt;= min(thread_alive,atoi(argu[1]))) and !job_queue.empty()  ){
		//mtx.unlock();
			int min_index = 0;
			for(unsigned int i = 0;i&lt;job_queue.size();i++){
				if(abs(job_queue[i].track - track) &lt; abs(job_queue[min_index].track - track)){
					min_index = i;
				}
			}
			track = job_queue[min_index].track;
			int id = job_queue[min_index].req_id;
			//mtx.lock();
			job_queue[min_index] = job_queue.back();
			
			job_queue.pop_back();
			
			print_service(id,track);
			total--;
			
			cv1.broadcast();
			//m.unlock()
			//sleep()
			cv2.wait(mtx);
			//wake up
			//m.lock()
			
		
		}
		
		else{
			
			 //if((int)job_queue.size() &lt; min(thread_alive,atoi(argu[1]))){
				cv1.broadcast();
				cv2.wait(mtx);
			//}
			
		}
		//mtx.unlock();
		//cout&lt;&lt;job_queue.size()&lt;&lt;endl;
	}
}
int main(int argc, char *argv[]){
	n = argc;
	argu = argv;
	thread_alive = argc -2;
	for(int i = 2; i &lt; argc;i++){
		string file = argv[i];
		string line;
		ifstream myfile(file);
	
		if (myfile.is_open()){
		while (getline (myfile,line)){
			total ++;
			
		}
		
		myfile.close();
		}
		
	}
	//total_request = total;
  cpu::boot((thread_startfunc_t)scheduler,(void*) argv,0);
return 0;
}
</pre>
</body>
</html>
