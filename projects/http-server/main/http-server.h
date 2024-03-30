#if !defined(HTTP_SERVER__H)
#define HTTP_SERVER__H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

/**
 * Messages for HTTP Monitor
*/
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT=0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_OTA_UPADTE_SUCCESSFULL,
    HTTP_MSG_OTA_UPADTE_FAIL,
    HTTP_MSG_OTA_UPADTE_INITALIZED,
} http_server_message_e;

/**
 * Structure for the message queue
*/
typedef struct http_server_queue_message
{
    http_server_message_e msg_id
} http_server_queue_message_t;

/**
 * Sends a message to the queue
 * @param msg_id message ID from the http_server_message_e enum.
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFAIL.
 * @note Expand the parameter list based on your requirement.
*/
BaseType_t http_server_monitor_send_message(http_server_message_e msg_id);

/**
 * Starts the HTTP server
*/
void http_server_start(void);

/**
 * Stops the HTTP server
*/
void http_server_stop(void);


#endif // HTTP_SERVER__H
