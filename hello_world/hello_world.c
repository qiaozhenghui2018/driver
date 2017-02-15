/*************************************************************************
	> File Name: hello_world.c
	> Author: 
	> Mail: 
	> Created Time: Wed 31 Aug 2016 01:42:00 PM CST
 ************************************************************************/

#include <linux/init.h>
#include <linux/module.h>

static int __init hello_world_init(void)
{
    printk("----->joe:hello kernel!\n");

    return 0;
}

static void __exit hello_world_exit(void)
{
    printk("----->joe:Good Bye kernel!\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");

