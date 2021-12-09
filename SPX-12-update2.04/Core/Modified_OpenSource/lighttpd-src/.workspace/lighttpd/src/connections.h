#ifndef _CONNECTIONS_H_
#define _CONNECTIONS_H_

#include "server.h"
#include "fdevent.h"

#define HTTP_MEDIA_ERROR     401
#define HTTP_SESSION_ERROR   405
#define HTTP_FLASH_ERROR     503

connection *connection_init(server *srv);
int connection_reset(server *srv, connection *con);
void connections_free(server *srv);

connection * connection_accept(server *srv, server_socket *srv_sock);
int connection_close(server *srv, connection *con);

int connection_set_state(server *srv, connection *con, connection_state_t state);
const char * connection_get_state(connection_state_t state);
const char * connection_get_short_state(connection_state_t state);
int connection_state_machine(server *srv, connection *con);

#endif
