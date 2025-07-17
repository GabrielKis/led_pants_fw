#ifndef MSG_HMI_MAIN_H_
#define MSG_HMI_MAIN_H_

#include <stdint.h>
#include <zephyr/kernel.h>

enum hmi_cmd_type {
    HMI_CMD_STOMP_INC,
    HMI_CMD_ON_SIGNAL,
    HMI_CMD_OFF_SIGNAL,
    HMI_CMD_RST_STOMP,
};

// HMI -> Main Message
struct hmi_msg_t {
    uint8_t type;   // enum hmi_cmd_type
};

int send_message_hmi_to_main(struct hmi_msg_t *hmi_data);
int recv_message_hmi_to_main(struct hmi_msg_t *hmi_data);

#endif
