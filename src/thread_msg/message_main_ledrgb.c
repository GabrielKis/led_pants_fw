/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include "message_main_ledrgb.h"

K_MSGQ_DEFINE(main_to_ledrgb_msgq, sizeof(struct main_to_ledrgb_msg_t), 5, 4);


// Main -> LED RGB Message
int send_message_main_to_ledrgb(struct main_to_ledrgb_msg_t *msg) {
    return k_msgq_put(&main_to_ledrgb_msgq, msg, K_NO_WAIT);
}
int recv_message_main_to_ledrgb(struct main_to_ledrgb_msg_t *msg) {
    return k_msgq_get(&main_to_ledrgb_msgq, msg, K_NO_WAIT);
}
