#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include "address_map_arm.h"
#include "interrupt_ID.h"

#define LICENSE "KAU_EMBEDDED"
#define AUTHOR "THKIM"
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB EXAMPLE"

#define ISR_MAJOR               0
#define DEV_NAME                "ISR"

void* LW_virtual;
volatile int* LEDR_ptr;
volatile int* KEY_ptr;
static int ISR_major = 0;


irq_handler_t irq_handler(int irq, void* dev_id, struct pt_regs *regs)
{
    *LEDR_ptr = *LEDR_ptr + 1;
    *(KEY_ptr + 3) = 0xf;

    return (irq_handler_t) IRQ_HANDLED;
}

static int ISR_open(struct inode *minode, struct file *mfile){

    int value;

    LW_virtual = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN);

    LEDR_ptr = LW_virtual + LEDR_BASE;
    *LEDR_ptr = 0x0;

    KEY_ptr = LW_virtual + KEY_BASE;

    *(KEY_ptr + 3) = 0x1;
    *(KEY_ptr + 2) = 0x1;

    value = request_irq(KEYS_IRQ, (irq_handler_t) irq_handler, IRQF_SHARED, "pushbutton_irq_handler", (void*) (irq_handler));

    return value;
}

static int ISR_release(struct inode *minode, struct file *mfile){

    *LEDR_ptr = 0;
    free_irq(KEYS_IRQ, (void*) irq_handler);
    printk(KERN_INFO "[KEY release]\n");
}

static struct file_operations ISR_fops = {
        .owner          = THIS_MODULE,
        .open           = ISR_open,
        .release         = ISR_release,
};

static int __init initialize_pushbutton_handler(void)
{



    int result = register_chrdev(ISR_MAJOR, DEV_NAME, &ISR_fops);
    if(result<0){
        printk(KERN_WARNING "Can't get any major\n");
        return result;
    }
    ISR_major = result;
    printk(KERN_INFO "[ISR_init] major number : %d\n", result);

    return 0;

}

static void __exit cleanup_pushbutton_handler(void)
{
    printk(KERN_INFO "[ISR_exit]\n");
    unregister_chrdev(ISR_major, DEV_NAME);
}

module_init(initialize_pushbutton_handler);
module_exit(cleanup_pushbutton_handler);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);

