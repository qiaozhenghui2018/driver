/*************************************************************************
	> File Name: add_sub.c
	> Author: 
	> Mail: 
	> Created Time: Wed 31 Aug 2016 06:33:03 PM CST
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>

#include "add_sub.h"

static int __init add_sub_init(void)
{
    printk("add_sub init\n");

    return 0;
}

static void __exit add_sub_exit(void)
{
    printk("add_sub_exit\n");
}

long add_int(int a, int b)
{
    return a+b;
}

long sub_int(int a, int b)
{
    return a-b;
}

MODULE_LICENSE("GPL");

module_init(add_sub_init);
module_exit(add_sub_exit);

EXPORT_SYMBOL(add_int);
EXPORT_SYMBOL(sub_int);

