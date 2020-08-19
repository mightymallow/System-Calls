#include "array_stats.h"
#include <linux/kernel.h>
#include <asm/errno.h>
#include <linux/uaccess.h>

asmlinkage long sys_array_stats(struct array_stats *stats, long data[], long size) {
    struct array_stats manageStats;
    long min;
    long max;
    long sum;
    long check;
    long i;

    //Checks size for correct value
    if (size <= 0) {
	return -EINVAL;
    }

    //Checks for NULL pointers
    if (data == NULL || stats == NULL) {
	return -EFAULT;
    }

    //Checks and copies first value in array if valid
    if (copy_from_user(&check, &data[0], sizeof(long)) != 0) {
	return -EFAULT;
    }

    min = check;
    max = check;
    sum = check;

    //Checks each value in array and updates
    for (i=1; i<size; i++) {
	if (copy_from_user(&check, &data[i], sizeof(long)) != 0) {
	    return -EFAULT;
	}

	if (check < min) {
	    min = check;
	}

	if (check > max) {
	    max = check;
	}

	sum += check;

    }

    manageStats.min = min;
    manageStats.max = max;
    manageStats.sum = sum;

    //Copies temporary struct into user level array
    if (copy_to_user(stats, &manageStats, sizeof(struct array_stats)) != 0) {
	return -EFAULT;
    }

    return 0;

}
