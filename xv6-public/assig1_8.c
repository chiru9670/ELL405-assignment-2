#include "types.h"
#include "stat.h"
#include "user.h"

int partial_sum(short arr[], int start_index, int step, int size_of_arr) {
	int ret_sum = 0;
	int i = start_index;
	while(i < size_of_arr) {
		ret_sum += arr[i];
		i += step;
	}
	return ret_sum;
}

int
main(int argc, char *argv[])
{
	if(argc< 2){
		printf(1,"Need type and input filename\n");
		exit();
	}
	char *filename;
	filename=argv[2];
	int type = atoi(argv[1]);
	printf(1,"Type is %d and filename is %s\n",type, filename);

	int tot_sum = 0;	

	int size=1000;
	short arr[size];
	char c;
	int fd = open(filename, 0);
	for(int i=0; i<size; i++){
		read(fd, &c, 1);
		arr[i]=c-'0';
		read(fd, &c, 1);
	}	
  	close(fd);
  	// this is to supress warning
  	printf(1,"first elem %d\n", arr[0]);
  
  	//----FILL THE CODE HERE for unicast sum
	const int num_child_processes = 7;
	const int parent_pid = getpid();
	int pid = parent_pid; // initialized with parent's pid(which should be !=0)
	for(int i = 0; i < num_child_processes && pid != 0; ++i) {
		pid = fork();
		if(pid==0) {
			int p_sum = partial_sum(arr, i, num_child_processes, size);
			int mypid = getpid();
			send(mypid, parent_pid, &p_sum);
			exit();
		}
	}

	for(int i = 0; i < num_child_processes; ++i) {
		char child_result[8];
		recv(child_result);
		// printf(2, "child result is %d\n", *((int *)child_result));
		tot_sum += *((int *)child_result);
	}
	for (int i = 0; i < num_child_processes; ++i)
	{
		wait();
	}
	
  	//------------------

  	if(type==0){ //unicast sum
		printf(1,"Sum of array for file %s is %d\n", filename,tot_sum);
	}
	exit();
}
