#ifndef MSG_MAIN_BUBBLE_H_
#define MSG_MAIN_BUBBLE_H_

#include <stdint.h>
#include <zephyr/kernel.h>


enum main_to_ledrgb_cmd_type {
    MAIN_CMD_LEDRGB_STOMP,
    MAIN_CMD_LEDRGB_DEFAULT,
};

// Main -> LED RGB Message
struct main_to_ledrgb_msg_t {
    uint8_t type; // Values from "main_to_ledrgb_cmd_type"
};

int send_message_main_to_ledrgb(struct main_to_ledrgb_msg_t *msg);
int recv_message_main_to_ledrgb(struct main_to_ledrgb_msg_t *msg);

#endif
