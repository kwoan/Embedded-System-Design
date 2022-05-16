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

#define HEX3_HEX0_MAJOR               0
#define HEX3_HEX0_NAME                "HEX3_HEX0"
#define HEX3_HEX0_MODULE_VERSION      "HEX3_HEX0 v0.1"

static int HEX3_HEX0_major = 0;
static volatile int* HEX3_HEX0_ptr;

int HEX3_HEX0_open(struct inode *minode, struct file *mfile){
    HEX3_HEX0_ptr = ioremap_nocache (LW_BRIDGE_BASE, LW_BRIDGE_SPAN) + HEX3_HEX0
_BASE;

        printk(KERN_INFO "[HEX3_HEX0_open]\n");
    return 0;
}

int HEX3_HEX0_release(struct inode *minode, struct file *mfile){
    printk(KERN_INFO "[HEX3_HEX0 release]\n");
    return 0;
}

ssize_t HEX3_HEX0_write_byte(struct file *inode, const char *gdata, size_t lengt
h, loff_t *off_what){

    unsigned char a, b, c, d;
    get_user(a, gdata);
    get_user(b, gdata+1);
    get_user(c, gdata+2);
    get_user(d, gdata+3);
    *HEX3_HEX0_ptr = a | b<<8 | c<<16 | d<<24;
    printk(KERN_INFO "[HEX3_HEX0_write_byte] %d\n", c);
    return length;
}

static struct file_operations HEX3_HEX0_fops = {
        .owner          = THIS_MODULE,
        .write          = HEX3_HEX0_write_byte,
        .open           = HEX3_HEX0_open,
        .release         = HEX3_HEX0_release,
};

int HEX3_HEX0_init(void){
    int result = register_chrdev(HEX3_HEX0_MAJOR, HEX3_HEX0_NAME, &HEX3_HEX0_fop
s);
    if(result<0){
        printk(KERN_WARNING "Can't get any major\n");
        return result;
    }
    HEX3_HEX0_major = result;
    printk(KERN_INFO "[HEX3_HEX0_init] major number : %d\n", result);
    return 0;
}

void HEX3_HEX0_exit(void){
    printk(KERN_INFO "[HEX3_HEX0_exit]\n");
    unregister_chrdev(HEX3_HEX0_major, HEX3_HEX0_NAME);
}

module_init(HEX3_HEX0_init);
module_exit(HEX3_HEX0_exit);

MODULE_LICENSE(LICENSE);
MODULE_AUTHOR(AUTHOR);
MODULE_DESCRIPTION(DESCRIPTION);
