#ifndef _SERVER_H_
#define _SERVER_H_
#include "ncml.h"
#include "featuredef.h"
#include "base.h"

typedef struct {
	char *key;
	char *value;
} two_strings;

typedef enum { CONFIG_UNSET, CONFIG_DOCUMENT_ROOT } config_var_t;


extern SERVICE_CONF_STRUCT g_serviceconf;
extern CoreFeatures_T      g_corefeatures;
int config_read(server *srv, const char *fn);
int config_set_defaults(server *srv);
buffer *config_get_value_buffer(server *srv, connection *con, config_var_t field);
int  ReInitialiseSSL(server *srv);
#endif
