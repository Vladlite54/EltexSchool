#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define MESSAGE_SIZE 64

static int len, temp;

static char* msg;

static ssize_t read_proc(struct file *filp, char *buf, size_t count, loff_t *offp) {
    if (count > temp) {
        count = temp;
    }
    temp = temp - count;
    if (copy_to_user(buf, msg, count) != 0) printk(KERN_ERR "copy_to_user error\n");
    if (count == 0) temp = len;

    return count;
}

static ssize_t write_proc(struct file *filp, const char *buf, size_t count, loff_t *offp) {
    if (copy_from_user(msg, buf, count) != 0) printk(KERN_ERR "copy_from_user error\n");
    len = count;
    temp = len;
    return count;
}

struct proc_ops proc_fops = {
proc_read:
    read_proc,
proc_write:
    write_proc
};

static void create_new_proc_entry(void) {
    proc_create("hello", 0, NULL, &proc_fops);
    msg = kmalloc(MESSAGE_SIZE * sizeof(char), GFP_KERNEL);
}

static int proc_init(void) {
    create_new_proc_entry();
    printk(KERN_INFO "Proc entry created\n");
    return 0;
}

static void proc_cleanup(void) {
    remove_proc_entry("hello", NULL);
    kfree(msg);
    printk(KERN_INFO "Proc entry removed\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vladislav Krovin");
MODULE_DESCRIPTION("Proc module for Eltex");

module_init(proc_init);
module_exit(proc_cleanup);
