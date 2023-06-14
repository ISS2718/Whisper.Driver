#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isaac Soares");
MODULE_DESCRIPTION("Um simples exemplo de módulo de kernel");

static int __init hello_init(void)
{
    printk(KERN_INFO "Olá, mundo!\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk(KERN_INFO "Tchau, mundo!\n");
}

module_init(hello_init);
module_exit(hello_exit);