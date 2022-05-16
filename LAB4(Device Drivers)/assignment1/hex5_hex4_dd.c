#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>
#include <asm/io.h>

#include "address_map_arm.h"

#define LICENSE "KAU_EMBEDDED"
#define AUTHOR "THKIM"
#define DESCRIPTION "KAU EMBEDDED SYSTEMS LAB ASSIGNMENT 1"

#define HEX5_HEX4_MAJOR               0
#define HEX5_HEX4_NAME                "HEX5_HEX4"
#define HEX5_HEX4_MODULE_VERSION      "HEX5_HEX4 v0.1"

static int HEX5_HEX4_major = 0;
static volatile int* HEX5_HEX4_ptr;

int HEX5_HEX4_open(struct inode *minode, struct file *mfile){
    HEX5_HEX4_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + HEX5_HEX4
_BASE;

        printk(KERN_INFO "[HEX5_HEX4_open]\n");
    return 0;
}

int HEX5_HEX4_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[HEX5_HEX4 release]\n");
    return 0;
}

ssize_t HEX5_HEX4_write_byte(struct file *inode, const char *gdata, size_t lengt
h, loff_t *off_what){

    unsigned char a, b;
    get_user(a, gdata);
    get_user(b, gdata+1);

    *HEX5_HEX4_ptr = a | b<<8;
    printk(KERN_INFO "[HEX5_HEX4_write_byte] %d\n", b);
    return length;
}

static struct file_operations HEX5_HEX4_fops = {
        .owner          = THIS_MODULE,
        .write          = HEX5_HEX4_write_byte,
        .open           = HEX5_HEX4_open,
        .release         = HEX5_HEX4_release,
};

int HEX5_HEX4_init(void){
    int result = register_chrdev(HEX5_HEX4_MAJOR, HEX5_HEX4_NAME, &HEX5_HEX4_fop
s);
    if(result<0){
        printk(KERN_WARNING "Can't get any major\n");
        return result;
    }
    HEX5_HEX4_major = result;
    printk(KERN_INFO "[HEX5_HEX4_init] major number : %d\n", result);
    return 0;
}

void HEX5_HEX4_exit(void){
    printk(KERN_INFO "[HEX5_HEX4_exit]\n");
    unregister_chrdev(HEX5_HEX4_major, HEX5_HEX4_NAME);
}

module_init(HEX5_HEX4_init);
module_exit(HEX5_HEX4_exit);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
