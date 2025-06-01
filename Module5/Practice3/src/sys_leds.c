#include <linux/module.h>
#include <linux/init.h>
#include <linux/tty.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include <linux/console_struct.h>
#include <linux/vt_kern.h>
#include <linux/timer.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>

MODULE_DESCRIPTION("LEDs control module with sysfs interface for Eltex");
MODULE_AUTHOR("Vladislav Krovin");
MODULE_LICENSE("GPL");

// Для таймера
struct timer_list my_timer;
struct tty_driver *my_driver;

static int blink_enabled = 0;    // Флаг включения моргания
static int led_mask = 0x07;      // Маска светодиодов (по умолчанию все включены)
#define BLINK_DELAY HZ/5         // Частота моргания (5 раз в секунду)

// Структурf для sysfs
static struct kobject *leds_kobject;

// Таймер моргания
static void my_timer_func(struct timer_list *ptr)
{
    static int current_state = 0;
    
    if (blink_enabled) {
        // Переключаем состояние (вкл/выкл)
        current_state = !current_state;
        
        if (current_state) {
            // Включить светодиоды согласно маске
            if (my_driver->ops && my_driver->ops->ioctl)
                (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, 
                                      KDSETLED, led_mask);
        } else {
            // Выключить все светодиоды
            if (my_driver->ops && my_driver->ops->ioctl)
                (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, 
                                      KDSETLED, 0);
        }
        
        mod_timer(&my_timer, jiffies + BLINK_DELAY);
    }
}

// Чтение текущей маски светодиодов
static ssize_t mask_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf)
{
    return sprintf(buf, "%d\n", led_mask);
}

// Запись значения в маску светодиода
static ssize_t mask_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    int new_mask;
    sscanf(buf, "%du", &new_mask);
    
    if (new_mask >= 0 && new_mask <= 7) {
        led_mask = new_mask;
    }
    
    return count;
}

// Чтение состояния моргания
static ssize_t blink_show(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buf)
{
    return sprintf(buf, "%d\n", blink_enabled);
}

// Включить/выключить светодиоды
static ssize_t blink_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    int new_state;
    sscanf(buf, "%du", &new_state);
    
    blink_enabled = new_state ? 1 : 0;
    
    if (blink_enabled) {
        mod_timer(&my_timer, jiffies + BLINK_DELAY);
    } else {
        // Выключить все светодиоды при остановке
        if (my_driver->ops && my_driver->ops->ioctl)
            (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, 
                                  KDSETLED, 0);
    }
    
    return count;
}

// Атрибуты sysfs
static struct kobj_attribute mask_attribute = 
    __ATTR(mask, 0660, mask_show, mask_store);
static struct kobj_attribute blink_attribute = 
    __ATTR(blink, 0660, blink_show, blink_store);

static int __init kbleds_init(void)
{
    int error = 0;
    
    printk(KERN_INFO "kbleds: loading\n");
    
    // Инициализация tty драйвера
    if (!vc_cons[fg_console].d || !vc_cons[fg_console].d->port.tty) {
        printk(KERN_ERR "kbleds: could not get current tty\n");
        return -ENODEV;
    }
    
    my_driver = vc_cons[fg_console].d->port.tty->driver;
    printk(KERN_INFO "kbleds: tty driver name %s\n", my_driver->name);
    
    timer_setup(&my_timer, my_timer_func, 0);
    
    // Создание kobject для sysfs
    leds_kobject = kobject_create_and_add("kbleds", kernel_kobj);
    if (!leds_kobject)
        return -ENOMEM;
    
    // Создание файлов в sysfs
    error = sysfs_create_file(leds_kobject, &mask_attribute.attr);
    if (error)
        printk(KERN_ERR "kbleds: failed to create mask file\n");
    
    error = sysfs_create_file(leds_kobject, &blink_attribute.attr);
    if (error)
        printk(KERN_ERR "kbleds: failed to create blink file\n");
    
    return error;
}

static void __exit kbleds_cleanup(void)
{
    printk(KERN_INFO "kbleds: unloading...\n");
    
    del_timer_sync(&my_timer);
    
    // Восстановление исходного состояния светодиодов
    if (my_driver->ops && my_driver->ops->ioctl)
        (my_driver->ops->ioctl)(vc_cons[fg_console].d->port.tty, 
                              KDSETLED, 0);
    
    kobject_put(leds_kobject);
}

module_init(kbleds_init);
module_exit(kbleds_cleanup);