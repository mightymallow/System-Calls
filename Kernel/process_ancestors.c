#include "process_ancestors.h"
#include <linux/kernel.h>
#include <asm/errno.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uaccess.h>

asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled) {

    struct task_struct *currentProcess = current;
    struct process_info tempProcess;
    long i = 0;
    long numSiblings = 0;
    long numChildren = 0;
    struct list_head *theChildren = NULL;
    struct list_head *theSiblings = NULL;
    bool isFinished = false;

    //Checks size argument
    if (size <= 0) {
	return -EINVAL;
    }

    //Checks for NULL pointers
    if (num_filled == NULL || info_array == NULL) {
	return -EFAULT;
    }

    //Runs for entire array or until end is reached
    while (i < size && !isFinished) {

	//Checks if current process is the swapper
	if (currentProcess->real_parent == currentProcess) {
	    isFinished = true;
	}

	//Assign values to struct accordingly
	tempProcess.pid = currentProcess->pid;
	strncpy(tempProcess.name, currentProcess->comm, 16);
	tempProcess.state = currentProcess->state;
	tempProcess.uid = currentProcess->cred->uid.val;
	tempProcess.nvcsw = currentProcess->nvcsw;
	tempProcess.nivcsw = currentProcess->nivcsw;

	//Counts number of children
	if (list_empty(&(currentProcess->children)) == 0) {
	    list_for_each(theChildren, &(currentProcess->children)) {
		numChildren++;
	    }
	}

	//Counts number of siblings (assumes 1 if lone sibling, 0 if at swapper/root)
	if (list_empty(&(currentProcess->sibling)) == 0 && !isFinished) {
	    list_for_each(theSiblings, &(currentProcess->sibling)) {
		numSiblings++;
	    }
	} else {
	    numSiblings = 0;
	}

	tempProcess.num_children = numChildren;
	tempProcess.num_siblings = numSiblings;

	//Copies temporary struct into user level array
	if (copy_to_user(&info_array[i], &tempProcess, sizeof(struct process_info)) != 0) {
	    return -EFAULT;
	}

	//Iterates to parent process
	currentProcess = currentProcess->real_parent;
	i++;

    }

    //Copies number of structs created to num_filled
    if (copy_to_user(num_filled, &i, sizeof(long)) != 0) {
	return -EFAULT;
    }

    return 0;

}
