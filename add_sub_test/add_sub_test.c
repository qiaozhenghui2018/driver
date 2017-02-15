/*************************************************************************
	> File Name: add_sub_test.c
	> Author: 
	> Mail: 
	> Created Time: Wed 31 Aug 2016 06:49:36 PM CST
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>

#include "add_sub.h"

static long a = 0;
static long b = 0;

static int add_or_sub = 0;

static int __init add_sub_test_init(void)
{
    printk("add_sub_test_init add_or_sub=%d, a=%ld, b=%ld\n", add_or_sub, a, b);

    if(add_or_sub == 1)
    {
        printk("add_int=%ld\n", add_int(a, b));
    }
    else
    {
        printk("sub_int=%ld\n", sub_int(a, b));
    }

    return 0;
}

static void add_sub_test_exit(void)
{
    printk("add_sub_test_exit\n");
}

module_init(add_sub_test_init);
module_exit(add_sub_test_exit);

module_param(a, long, S_IRUGO);
module_param(b, long, S_IRUGO);

module_param(add_or_sub, int, S_IRUGO);

MODULE_LICENSE("GPL");
