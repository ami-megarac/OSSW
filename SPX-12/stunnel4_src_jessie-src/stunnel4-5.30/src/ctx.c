/*
 *   stunnel       TLS offloading and load-balancing proxy
 *   Copyright (C) 1998-2016 Michal Trojnara <Michal.Trojnara@mirt.net>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation; either version 2 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, see <http://www.gnu.org/licenses>.
 *
 *   Linking stunnel statically or dynamically with other modules is making
 *   a combined work based on stunnel. Thus, the terms and conditions of
 *   the GNU General Public License cover the whole combination.
 *
 *   In addition, as a special exception, the copyright holder of stunnel
 *   gives you permission to combine stunnel with free software programs or
 *   libraries that are released under the GNU LGPL and with code included
 *   in the standard release of OpenSSL under the OpenSSL License (or
 *   modified versions of such code, with unchanged license). You may copy
 *   and distribute such a system following the terms of the GNU GPL for
 *   stunnel and the licenses of the other code concerned.
 *
 *   Note that people who make modified versions of stunnel are not obligated
 *   to grant this special exception for their modified versions; it is their
 *   choice whether to do so. The GNU General Public License gives permission
 *   to release a modified version without this exception; this exception
 *   also makes it possible to release a modified version which carries
 *   forward this exception.
 */

#include "common.h"
#include "prototypes.h"

#ifndef OPENSSL_NO_DH
DH *dh_params=NULL;
int dh_needed=0;
#endif /* OPENSSL_NO_DH */

/**************************************** prototypes */

/* SNI */
#ifndef OPENSSL_NO_TLSEXT
NOEXPORT int servername_cb(SSL *, int *, void *);
NOEXPORT int matches_wildcard(char *, char *);
#endif

/* DH/ECDH */
#ifndef OPENSSL_NO_DH
NOEXPORT int dh_init(SERVICE_OPTIONS *);
NOEXPORT DH *dh_read(char *);
#endif /* OPENSSL_NO_DH */
#ifndef OPENSSL_NO_ECDH
NOEXPORT int ecdh_init(SERVICE_OPTIONS *);
#endif /* USE_ECDH */

/* configuration commands */
NOEXPORT int conf_init(SERVICE_OPTIONS *section);

/* authentication */
NOEXPORT int auth_init(SERVICE_OPTIONS *);
#ifndef OPENSSL_NO_PSK
NOEXPORT unsigned psk_client_callback(SSL *, const char *,
    char *, unsigned, unsigned char *, unsigned);
NOEXPORT unsigned psk_server_callback(SSL *, const char *,
    unsigned char *, unsigned);
#endif /* !defined(OPENSSL_NO_PSK) */
NOEXPORT int load_cert_file(SERVICE_OPTIONS *);
NOEXPORT int load_key_file(SERVICE_OPTIONS *);
#ifndef OPENSSL_NO_ENGINE
NOEXPORT int load_cert_engine(SERVICE_OPTIONS *);
NOEXPORT int load_key_engine(SERVICE_OPTIONS *);
#endif
NOEXPORT int password_cb(char *, int, int, void *);

/* session callbacks */
NOEXPORT int sess_new_cb(SSL *, SSL_SESSION *);
NOEXPORT SSL_SESSION *sess_get_cb(SSL *, unsigned char *, int, int *);
NOEXPORT void sess_remove_cb(SSL_CTX *, SSL_SESSION *);

/* sessiond interface */
NOEXPORT void cache_new(SSL *, SSL_SESSION *);
NOEXPORT SSL_SESSION *cache_get(SSL *, unsigned char *, int);
NOEXPORT void cache_remove(SSL_CTX *, SSL_SESSION *);
NOEXPORT void cache_transfer(SSL_CTX *, const u_char, const long,
    const u_char *, const size_t,
    const u_char *, const size_t,
    unsigned char **, size_t *);

/* info callbacks */
NOEXPORT void info_callback(const SSL *, int, int);

NOEXPORT void sslerror_queue(void);
NOEXPORT void sslerror_log(unsigned long, char *);

/**************************************** initialize section->ctx */

#if OPENSSL_VERSION_NUMBER<0x10100000L
typedef long SSL_OPTIONS_TYPE;
#else /* OpenSSL >= 1.1.0 */
typedef long unsigned SSL_OPTIONS_TYPE;
#endif

int context_init(SERVICE_OPTIONS *section) { /* init SSL context */
    /* create SSL context */
    if(section->option.client)
        section->ctx=SSL_CTX_new(section->client_method);
    else /* server mode */
        section->ctx=SSL_CTX_new(section->server_method);
    if(!section->ctx) {
        sslerror("SSL_CTX_new");
        return 1; /* FAILED */
    }
    SSL_CTX_set_ex_data(section->ctx, index_opt, section); /* for callbacks */

    /* load certificate and private key to be verified by the peer server */
#if !defined(OPENSSL_NO_ENGINE) && OPENSSL_VERSION_NUMBER>=0x0090809fL
    /* SSL_CTX_set_client_cert_engine() was introduced in OpenSSL 0.9.8i */
    if(section->option.client && section->engine) {
        if(SSL_CTX_set_client_cert_engine(section->ctx, section->engine))
            s_log(LOG_INFO, "Client certificate engine (%s) enabled",
                ENGINE_get_id(section->engine));
        else /* no client certificate functionality in this engine */
            sslerror("SSL_CTX_set_client_cert_engine"); /* ignore error */
    }
#endif
    if(auth_init(section))
        return 1; /* FAILED */

    /* initialize verification of the peer server certificate */
    if(verify_init(section))
        return 1; /* FAILED */

    /* initialize DH/ECDH server mode */
    if(!section->option.client) {
#ifndef OPENSSL_NO_TLSEXT
        SSL_CTX_set_tlsext_servername_arg(section->ctx, section);
        SSL_CTX_set_tlsext_servername_callback(section->ctx, servername_cb);
#endif /* OPENSSL_NO_TLSEXT */
#ifndef OPENSSL_NO_DH
        dh_init(section); /* ignore the result (errors are not critical) */
#endif /* OPENSSL_NO_DH */
#ifndef OPENSSL_NO_ECDH
        ecdh_init(section); /* ignore the result (errors are not critical) */
#endif /* OPENSSL_NO_ECDH */
    }

    /* setup session cache */
    if(!section->option.client) {
        unsigned servname_len=(unsigned)strlen(section->servname);
        if(servname_len>SSL_MAX_SSL_SESSION_ID_LENGTH)
            servname_len=SSL_MAX_SSL_SESSION_ID_LENGTH;
        if(!SSL_CTX_set_session_id_context(section->ctx,
                (unsigned char *)section->servname, servname_len)) {
            sslerror("SSL_CTX_set_session_id_context");
            return 1; /* FAILED */
        }
    }
#ifdef SSL_SESS_CACHE_NO_INTERNAL_STORE
    /* the default cache mode is just SSL_SESS_CACHE_SERVER */
    SSL_CTX_set_session_cache_mode(section->ctx,
        SSL_SESS_CACHE_SERVER|SSL_SESS_CACHE_NO_INTERNAL_STORE);
#endif
    SSL_CTX_sess_set_cache_size(section->ctx, section->session_size);
    SSL_CTX_set_timeout(section->ctx, section->session_timeout);
    SSL_CTX_sess_set_new_cb(section->ctx, sess_new_cb);
    SSL_CTX_sess_set_get_cb(section->ctx, sess_get_cb);
    SSL_CTX_sess_set_remove_cb(section->ctx, sess_remove_cb);

    /* set info callback */
    SSL_CTX_set_info_callback(section->ctx, info_callback);

    /* ciphers, options, mode */
    if(section->cipher_list)
        if(!SSL_CTX_set_cipher_list(section->ctx, section->cipher_list)) {
            sslerror("SSL_CTX_set_cipher_list");
            return 1; /* FAILED */
        }
    SSL_CTX_set_options(section->ctx,
        (SSL_OPTIONS_TYPE)(section->ssl_options_set));
#if OPENSSL_VERSION_NUMBER>=0x009080dfL
    SSL_CTX_clear_options(section->ctx,
        (SSL_OPTIONS_TYPE)(section->ssl_options_clear));
    s_log(LOG_DEBUG, "SSL options: 0x%08lX (+0x%08lX, -0x%08lX)",
        SSL_CTX_get_options(section->ctx),
        section->ssl_options_set, section->ssl_options_clear);
#else /* OpenSSL older than 0.9.8m */
    s_log(LOG_DEBUG, "SSL options: 0x%08lX (+0x%08lX)",
        SSL_CTX_get_options(section->ctx),
        section->ssl_options_set);
#endif /* OpenSSL 0.9.8m or later */

    /* initialize OpenSSL CONF options */
    if(conf_init(section))
        return 1; /* FAILED */

#ifdef SSL_MODE_RELEASE_BUFFERS
    SSL_CTX_set_mode(section->ctx,
        SSL_MODE_ENABLE_PARTIAL_WRITE |
        SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER |
        SSL_MODE_RELEASE_BUFFERS);
#else
    SSL_CTX_set_mode(section->ctx,
        SSL_MODE_ENABLE_PARTIAL_WRITE |
        SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
#endif
    return 0; /* OK */
}

/**************************************** SNI callback */

#ifndef OPENSSL_NO_TLSEXT

NOEXPORT int servername_cb(SSL *ssl, int *ad, void *arg) {
    SERVICE_OPTIONS *section=(SERVICE_OPTIONS *)arg;
    const char *servername=SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    SERVERNAME_LIST *list;
    CLI *c;
#ifdef USE_LIBWRAP
    char *accepted_address;
#endif /* USE_LIBWRAP */

    /* leave the alert type at SSL_AD_UNRECOGNIZED_NAME */
    (void)ad; /* squash the unused parameter warning */
    if(!section->servername_list_head) {
        s_log(LOG_DEBUG, "SNI: no virtual services defined");
        return SSL_TLSEXT_ERR_OK;
    }
    if(!servername) {
        s_log(LOG_NOTICE, "SNI: no servername received");
        return SSL_TLSEXT_ERR_NOACK;
    }
    s_log(LOG_INFO, "SNI: requested servername: %s", servername);

    for(list=section->servername_list_head; list; list=list->next)
        if(matches_wildcard((char *)servername, list->servername)) {
            s_log(LOG_DEBUG, "SNI: matched pattern: %s", list->servername);
            c=SSL_get_ex_data(ssl, index_cli);
            c->opt=list->opt;
            SSL_set_SSL_CTX(ssl, c->opt->ctx);
            SSL_set_verify(ssl, SSL_CTX_get_verify_mode(c->opt->ctx),
                SSL_CTX_get_verify_callback(c->opt->ctx));
            s_log(LOG_NOTICE, "SNI: switched to service [%s]",
                c->opt->servname);
#ifdef USE_LIBWRAP
            accepted_address=s_ntop(&c->peer_addr, c->peer_addr_len);
            libwrap_auth(c, accepted_address); /* retry on a service switch */
            str_free(accepted_address);
#endif /* USE_LIBWRAP */
            return SSL_TLSEXT_ERR_OK;
        }
    s_log(LOG_ERR, "SNI: no pattern matched servername: %s", servername);
    return SSL_TLSEXT_ERR_ALERT_FATAL;
}
/* TLSEXT callback return codes:
 *  - SSL_TLSEXT_ERR_OK
 *  - SSL_TLSEXT_ERR_ALERT_WARNING
 *  - SSL_TLSEXT_ERR_ALERT_FATAL
 *  - SSL_TLSEXT_ERR_NOACK */

NOEXPORT int matches_wildcard(char *servername, char *pattern) {
    ssize_t diff;

    if(!servername || !pattern)
        return 0;
    if(*pattern=='*') { /* wildcard comparison */
        diff=(ssize_t)strlen(servername)-(ssize_t)strlen(++pattern);
        if(diff<0) /* pattern longer than servername */
            return 0;
        servername+=diff;
    }
    return !strcasecmp(servername, pattern);
}

#endif /* OPENSSL_NO_TLSEXT */

/**************************************** DH initialization */

#ifndef OPENSSL_NO_DH

NOEXPORT int dh_init(SERVICE_OPTIONS *section) {
    DH *dh=NULL;

    s_log(LOG_DEBUG, "DH initialization");
#ifndef OPENSSL_NO_ENGINE
    if(!section->engine) /* cert is a file and not an identifier */
#endif
        dh=dh_read(section->cert);
    if(dh) {
        SSL_CTX_set_tmp_dh(section->ctx, dh);
        s_log(LOG_INFO, "%d-bit DH parameters loaded", 8*DH_size(dh));
        DH_free(dh);
        return 0; /* OK */
    }
    CRYPTO_r_lock(stunnel_locks[LOCK_DH]);
    SSL_CTX_set_tmp_dh(section->ctx, dh_params);
    CRYPTO_r_unlock(stunnel_locks[LOCK_DH]);
    dh_needed=1; /* generate temporary DH parameters in cron */
    section->option.dh_needed=1; /* update this context */
    s_log(LOG_INFO, "Using dynamic DH parameters");
    return 0; /* OK */
}

NOEXPORT DH *dh_read(char *cert) {
    DH *dh;
    BIO *bio;

    if(!cert) {
        s_log(LOG_DEBUG, "No certificate available to load DH parameters");
        return NULL; /* FAILED */
    }
    bio=BIO_new_file(cert, "r");
    if(!bio) {
        sslerror("BIO_new_file");
        return NULL; /* FAILED */
    }
    dh=PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
    BIO_free(bio);
    if(!dh) {
        while(ERR_get_error())
            ; /* OpenSSL error queue cleanup */
        s_log(LOG_DEBUG, "Could not load DH parameters from %s", cert);
        return NULL; /* FAILED */
    }
    s_log(LOG_DEBUG, "Using DH parameters from %s", cert);
    return dh;
}

#endif /* OPENSSL_NO_DH */

/**************************************** ECDH initialization */

#ifndef OPENSSL_NO_ECDH
NOEXPORT int ecdh_init(SERVICE_OPTIONS *section) {
    EC_KEY *ecdh;

    s_log(LOG_DEBUG, "ECDH initialization");
    ecdh=EC_KEY_new_by_curve_name(section->curve);
    if(!ecdh) {
        sslerror("EC_KEY_new_by_curve_name");
        s_log(LOG_ERR, "Cannot create curve %s",
            OBJ_nid2ln(section->curve));
        return 1; /* FAILED */
    }
    SSL_CTX_set_tmp_ecdh(section->ctx, ecdh);
    EC_KEY_free(ecdh);
    s_log(LOG_DEBUG, "ECDH initialized with curve %s",
        OBJ_nid2ln(section->curve));
    return 0; /* OK */
}
#endif /* OPENSSL_NO_ECDH */

/**************************************** initialize OpenSSL CONF */

NOEXPORT int conf_init(SERVICE_OPTIONS *section) {
#if OPENSSL_VERSION_NUMBER>=0x10002000L
    SSL_CONF_CTX *cctx;
    NAME_LIST *curr;
    char *cmd, *param;

    if(!section->config)
        return 0; /* OK */
    cctx=SSL_CONF_CTX_new();
    if(!cctx) {
        sslerror("SSL_CONF_CTX_new");
        return 1; /* FAILED */
    }
    SSL_CONF_CTX_set_ssl_ctx(cctx, section->ctx);
    SSL_CONF_CTX_set_flags(cctx, SSL_CONF_FLAG_FILE);
    SSL_CONF_CTX_set_flags(cctx, section->option.client ?
        SSL_CONF_FLAG_CLIENT : SSL_CONF_FLAG_SERVER);
    SSL_CONF_CTX_set_flags(cctx, SSL_CONF_FLAG_CERTIFICATE);

    for(curr=section->config; curr; curr=curr->next) {
        cmd=str_dup(curr->name);
        param=strchr(cmd, ':');
        if(param)
            *param++='\0';
        switch(SSL_CONF_cmd(cctx, cmd, param)) {
        case 2:
            s_log(LOG_DEBUG, "OpenSSL config \"%s\" set to \"%s\"", cmd, param);
            break;
        case 1:
            s_log(LOG_DEBUG, "OpenSSL config command \"%s\" executed", cmd);
            break;
        case -2:
            s_log(LOG_ERR,
                "OpenSSL config command \"%s\" was not recognised", cmd);
            str_free(cmd);
            SSL_CONF_CTX_free(cctx);
            return 1; /* FAILED */
        case -3:
            s_log(LOG_ERR,
                "OpenSSL config command \"%s\" requires a parameter", cmd);
            str_free(cmd);
            SSL_CONF_CTX_free(cctx);
            return 1; /* FAILED */
        default:
            sslerror("SSL_CONF_cmd");
            str_free(cmd);
            SSL_CONF_CTX_free(cctx);
            return 1; /* FAILED */
        }
        str_free(cmd);
    }

    if(!SSL_CONF_CTX_finish(cctx)) {
        sslerror("SSL_CONF_CTX_finish");
        SSL_CONF_CTX_free(cctx);
        return 1; /* FAILED */
    }
    SSL_CONF_CTX_free(cctx);
#else /* OpenSSL earlier than 1.0.2 */
    (void)section; /* squash the unused parameter warning */
#endif /* OpenSSL 1.0.2 or later */
    return 0; /* OK */
}

/**************************************** initialize authentication */

NOEXPORT int auth_init(SERVICE_OPTIONS *section) {
    int cert_needed=1, key_needed=1;

#ifndef OPENSSL_NO_PSK
    if(section->psk_keys) {
        if(section->option.client)
            SSL_CTX_set_psk_client_callback(section->ctx, psk_client_callback);
        else
            SSL_CTX_set_psk_server_callback(section->ctx, psk_server_callback);
    }
#endif /* !defined(OPENSSL_NO_PSK) */

    /* load the certificate and private key */
    if(!section->cert || !section->key) {
        s_log(LOG_DEBUG, "No certificate or private key specified");
        return 0; /* OK */
    }
#ifndef OPENSSL_NO_ENGINE
    if(section->engine) { /* try to use the engine first */
        cert_needed=load_cert_engine(section);
        key_needed=load_key_engine(section);
    }
#endif
    if(cert_needed && load_cert_file(section))
        return 1; /* FAILED */
    if(key_needed && load_key_file(section))
        return 1; /* FAILED */

    /* validate the private key against the certificate */
    if(!SSL_CTX_check_private_key(section->ctx)) {
        sslerror("Private key does not match the certificate");
        return 1; /* FAILED */
    }
    s_log(LOG_DEBUG, "Private key check succeeded");
    return 0; /* OK */
}

#ifndef OPENSSL_NO_PSK

NOEXPORT unsigned psk_client_callback(SSL *ssl, const char *hint,
    char *identity, unsigned max_identity_len,
    unsigned char *psk, unsigned max_psk_len) {
    CLI *c;
    size_t identity_len;

    (void)hint; /* squash the unused parameter warning */
    c=SSL_get_ex_data(ssl, index_cli);
    if(!c->opt->psk_selected) {
        s_log(LOG_ERR, "INTERNAL ERROR: No PSK identity selected");
        return 0;
    }
    /* the source seems to have its buffer large enough for
     * the trailing null character, but the manual page says
     * nothing about it -- lets play safe */
    identity_len=strlen(c->opt->psk_selected->identity)+1;
    if(identity_len>max_identity_len) {
        s_log(LOG_ERR, "PSK identity too long (%lu>%d bytes)",
            (long unsigned)identity_len, max_psk_len);
        return 0;
    }
    if(c->opt->psk_selected->key_len>max_psk_len) {
        s_log(LOG_ERR, "PSK too long (%lu>%d bytes)",
            (long unsigned)c->opt->psk_selected->key_len, max_psk_len);
        return 0;
    }
    strcpy(identity, c->opt->psk_selected->identity);
    memcpy(psk, c->opt->psk_selected->key_val, c->opt->psk_selected->key_len);
    s_log(LOG_INFO, "PSK client configured for identity \"%s\"", identity);
    return (unsigned)(c->opt->psk_selected->key_len);
}

NOEXPORT unsigned psk_server_callback(SSL *ssl, const char *identity,
    unsigned char *psk, unsigned max_psk_len) {
    CLI *c;
    PSK_KEYS *found;
    size_t len;

    c=SSL_get_ex_data(ssl, index_cli);
    found=psk_find(&c->opt->psk_sorted, identity);
    if(found) {
        len=found->key_len;
    } else {
        s_log(LOG_ERR, "No key found for PSK identity \"%s\"", identity);
        len=0;
    }
    if(len>max_psk_len) {
        s_log(LOG_ERR, "PSK too long (%lu>%d bytes)",
            (long unsigned)len, max_psk_len);
        len=0;
    }
    if(len) {
        memcpy(psk, found->key_val, len);
        s_log(LOG_NOTICE, "Key configured for PSK identity \"%s\"", identity);
    } else { /* block identity probes if possible */
        if(max_psk_len>=32 && RAND_bytes(psk, 32)>0) {
            len=32; /* 256 random bits */
            s_log(LOG_ERR, "Configured random PSK");
        } else {
            s_log(LOG_ERR, "Rejecting with unknown_psk_identity alert");
        }
    }
    return (unsigned)len;
}

NOEXPORT int psk_compar(const void *a, const void *b) {
    PSK_KEYS *x=*(PSK_KEYS **)a, *y=*(PSK_KEYS **)b;

#if 0
    s_log(LOG_DEBUG, "PSK cmp: %s %s", x->identity, y->identity);
#endif
    return strcmp(x->identity, y->identity);
}

void psk_sort(PSK_TABLE *table, PSK_KEYS *head) {
    PSK_KEYS *curr;
    size_t i;

    table->num=0;
    for(curr=head; curr; curr=curr->next)
        ++table->num;
    s_log(LOG_INFO, "PSK identities: %lu retrieved",
        (long unsigned)table->num);
    table->val=str_alloc(table->num*sizeof(PSK_KEYS *));
    for(curr=head, i=0; i<table->num; ++i) {
        table->val[i]=curr;
        curr=curr->next;
    }
    qsort(table->val, table->num, sizeof(PSK_KEYS *), psk_compar);
#if 0
    for(i=0; i<table->num; ++i)
        s_log(LOG_DEBUG, "PSK table: %s", table->val[i]->identity);
#endif
}

PSK_KEYS *psk_find(const PSK_TABLE *table, const char *identity) {
    PSK_KEYS key, *ptr=&key, **ret;

    key.identity=(char *)identity;
    ret=bsearch(&ptr,
        table->val, table->num, sizeof(PSK_KEYS *), psk_compar);
    return ret ? *ret : NULL;
}

#endif /* !defined(OPENSSL_NO_PSK) */

NOEXPORT int load_cert_file(SERVICE_OPTIONS *section) {
    s_log(LOG_INFO, "Loading certificate from file: %s", section->cert);
    if(!SSL_CTX_use_certificate_chain_file(section->ctx, section->cert)) {
        sslerror("SSL_CTX_use_certificate_chain_file");
        return 1; /* FAILED */
    }
    s_log(LOG_INFO, "Certificate loaded from file: %s", section->cert);
    return 0; /* OK */
}

static int cache_initialized=0;

NOEXPORT int load_key_file(SERVICE_OPTIONS *section) {
    int i, reason;
    UI_DATA ui_data;

    s_log(LOG_INFO, "Loading private key from file: %s", section->key);
    if(file_permissions(section->key))
        return 1; /* FAILED */

    ui_data.section=section; /* setup current section for callbacks */
    SSL_CTX_set_default_passwd_cb(section->ctx, password_cb);

    for(i=0; i<=3; i++) {
        if(!i && !cache_initialized)
            continue; /* there is no cached value */
        SSL_CTX_set_default_passwd_cb_userdata(section->ctx,
            i ? &ui_data : NULL); /* try the cached password first */
        if(SSL_CTX_use_PrivateKey_file(section->ctx, section->key,
                SSL_FILETYPE_PEM))
            break;
        reason=ERR_GET_REASON(ERR_peek_error());
        if(i<=2 && reason==EVP_R_BAD_DECRYPT) {
            sslerror_queue(); /* dump the error queue */
            s_log(LOG_ERR, "Wrong pass phrase: retrying");
            continue;
        }
        sslerror("SSL_CTX_use_PrivateKey_file");
        return 1; /* FAILED */
    }
    s_log(LOG_INFO, "Private key loaded from file: %s", section->key);
    return 0; /* OK */
}

#ifndef OPENSSL_NO_ENGINE

NOEXPORT int load_cert_engine(SERVICE_OPTIONS *section) {
    struct {
        const char *id;
        X509 *cert;
    } parms;

    s_log(LOG_INFO, "Loading certificate from engine ID: %s", section->cert);
    parms.id=section->cert;
    parms.cert=NULL;
    ENGINE_ctrl_cmd(section->engine, "LOAD_CERT_CTRL", 0, &parms, NULL, 1);
    if(!parms.cert) {
        sslerror("ENGINE_ctrl_cmd");
        return 1; /* FAILED */
    }
    if(!SSL_CTX_use_certificate(section->ctx, parms.cert)) {
        sslerror("SSL_CTX_use_certificate");
        return 1; /* FAILED */
    }
    s_log(LOG_INFO, "Certificate loaded from engine ID: %s", section->cert);
    return 0; /* OK */
}

NOEXPORT int load_key_engine(SERVICE_OPTIONS *section) {
    int i, reason;
    UI_DATA ui_data;
    EVP_PKEY *pkey;
    UI_METHOD *ui_method;

    s_log(LOG_INFO, "Initializing private key on engine ID: %s", section->key);

    ui_data.section=section; /* setup current section for callbacks */
    SSL_CTX_set_default_passwd_cb(section->ctx, password_cb);

#ifdef USE_WIN32
    ui_method=UI_create_method("stunnel WIN32 UI");
    UI_method_set_reader(ui_method, pin_cb);
#else /* USE_WIN32 */
    ui_method=UI_OpenSSL();
    /* workaround for broken engines */
    /* ui_data.section=NULL; */
#endif /* USE_WIN32 */
    for(i=1; i<=3; i++) {
        pkey=ENGINE_load_private_key(section->engine, section->key,
            ui_method, &ui_data);
        if(!pkey) {
            reason=ERR_GET_REASON(ERR_peek_error());
            if(i<=2 && (reason==7 || reason==160)) { /* wrong PIN */
                sslerror_queue(); /* dump the error queue */
                s_log(LOG_ERR, "Wrong PIN: retrying");
                continue;
            }
            sslerror("ENGINE_load_private_key");
            return 1; /* FAILED */
        }
        if(SSL_CTX_use_PrivateKey(section->ctx, pkey))
            break; /* success */
        sslerror("SSL_CTX_use_PrivateKey");
        return 1; /* FAILED */
    }
    s_log(LOG_INFO, "Private key initialized on engine ID: %s", section->key);
    return 0; /* OK */
}

#endif /* !defined(OPENSSL_NO_ENGINE) */

NOEXPORT int password_cb(char *buf, int size, int rwflag, void *userdata) {
    static char cache[PEM_BUFSIZE];
    int len;

    if(size>PEM_BUFSIZE)
        size=PEM_BUFSIZE;

    if(userdata) { /* prompt the user */
#ifdef USE_WIN32
        len=passwd_cb(buf, size, rwflag, userdata);
#else
        len=PEM_def_callback(buf, size, rwflag, NULL);
#endif
        memcpy(cache, buf, (size_t)size); /* save in cache */
        cache_initialized=1;
    } else { /* try the cached value */
        strncpy(buf, cache, (size_t)size);
        buf[size-1]='\0';
        len=(int)strlen(buf);
    }
    return len;
}

/**************************************** session callbacks */

NOEXPORT int sess_new_cb(SSL *ssl, SSL_SESSION *sess) {
    CLI *c;

    s_log(LOG_DEBUG, "New session callback");
    c=SSL_get_ex_data(ssl, index_cli);
    if(c->opt->option.sessiond)
        cache_new(ssl, sess);
    return 1; /* leave the session in local cache for reuse */
}

NOEXPORT SSL_SESSION *sess_get_cb(SSL *ssl,
        unsigned char *key, int key_len, int *do_copy) {
    CLI *c;

    s_log(LOG_DEBUG, "Get session callback");
    *do_copy=0; /* allow the session to be freed automatically */
    c=SSL_get_ex_data(ssl, index_cli);
    if(c->opt->option.sessiond)
        return cache_get(ssl, key, key_len);
    return NULL; /* no session to resume */
}

NOEXPORT void sess_remove_cb(SSL_CTX *ctx, SSL_SESSION *sess) {
    SERVICE_OPTIONS *opt;

    s_log(LOG_DEBUG, "Remove session callback");
    opt=SSL_CTX_get_ex_data(ctx, index_opt);
    if(opt->option.sessiond)
        cache_remove(ctx, sess);
}

/**************************************** sessiond functionality */

#define CACHE_CMD_NEW     0x00
#define CACHE_CMD_GET     0x01
#define CACHE_CMD_REMOVE  0x02
#define CACHE_RESP_ERR    0x80
#define CACHE_RESP_OK     0x81

NOEXPORT void cache_new(SSL *ssl, SSL_SESSION *sess) {
    unsigned char *val, *val_tmp;
    ssize_t val_len;
    const unsigned char *session_id;
    unsigned int session_id_length;

    val_len=i2d_SSL_SESSION(sess, NULL);
    val_tmp=val=str_alloc((size_t)val_len);
    i2d_SSL_SESSION(sess, &val_tmp);

#if OPENSSL_VERSION_NUMBER>=0x0090800fL
    session_id=SSL_SESSION_get_id(sess, &session_id_length);
#else
    session_id=(const unsigned char *)sess->session_id;
    session_id_length=sess->session_id_length;
#endif
    cache_transfer(SSL_get_SSL_CTX(ssl), CACHE_CMD_NEW,
        SSL_SESSION_get_timeout(sess),
        session_id, session_id_length, val, (size_t)val_len, NULL, NULL);
    str_free(val);
}

NOEXPORT SSL_SESSION *cache_get(SSL *ssl,
        unsigned char *key, int key_len) {
    unsigned char *val, *val_tmp=NULL;
    ssize_t val_len=0;
    SSL_SESSION *sess;

    cache_transfer(SSL_get_SSL_CTX(ssl), CACHE_CMD_GET, 0,
        key, (size_t)key_len, NULL, 0, &val, (size_t *)&val_len);
    if(!val)
        return NULL;
    val_tmp=val;
    sess=d2i_SSL_SESSION(NULL,
#if OPENSSL_VERSION_NUMBER>=0x0090800fL
        (const unsigned char **)
#endif /* OpenSSL version >= 0.8.0 */
        &val_tmp, (long)val_len);
    str_free(val);
    return sess;
}

NOEXPORT void cache_remove(SSL_CTX *ctx, SSL_SESSION *sess) {
    const unsigned char *session_id;
    unsigned int session_id_length;

#if OPENSSL_VERSION_NUMBER>=0x0090800fL
    session_id=SSL_SESSION_get_id(sess, &session_id_length);
#else
    session_id=(const unsigned char *)sess->session_id;
    session_id_length=sess->session_id_length;
#endif
    cache_transfer(ctx, CACHE_CMD_REMOVE, 0,
        session_id, session_id_length, NULL, 0, NULL, NULL);
}

#define MAX_VAL_LEN 512
typedef struct {
    u_char version, type;
    u_short timeout;
    u_char key[SSL_MAX_SSL_SESSION_ID_LENGTH];
    u_char val[MAX_VAL_LEN];
} CACHE_PACKET;

NOEXPORT void cache_transfer(SSL_CTX *ctx, const u_char type,
        const long timeout,
        const u_char *key, const size_t key_len,
        const u_char *val, const size_t val_len,
        unsigned char **ret, size_t *ret_len) {
    char session_id_txt[2*SSL_MAX_SSL_SESSION_ID_LENGTH+1];
    const char hex[16]="0123456789ABCDEF";
    const char *type_description[]={"new", "get", "remove"};
    unsigned i;
    SOCKET s;
    ssize_t len;
    struct timeval t;
    CACHE_PACKET *packet;
    SERVICE_OPTIONS *section;

    if(ret) /* set error as the default result if required */
        *ret=NULL;

    /* log the request information */
    for(i=0; i<key_len && i<SSL_MAX_SSL_SESSION_ID_LENGTH; ++i) {
        session_id_txt[2*i]=hex[key[i]>>4];
        session_id_txt[2*i+1]=hex[key[i]&0x0f];
    }
    session_id_txt[2*i]='\0';
    s_log(LOG_INFO,
        "cache_transfer: request=%s, timeout=%ld, id=%s, length=%lu",
        type_description[type], timeout, session_id_txt, (long unsigned)val_len);

    /* allocate UDP packet buffer */
    if(key_len>SSL_MAX_SSL_SESSION_ID_LENGTH) {
        s_log(LOG_ERR, "cache_transfer: session id too big (%lu bytes)",
            (unsigned long)key_len);
        return;
    }
    if(val_len>MAX_VAL_LEN) {
        s_log(LOG_ERR, "cache_transfer: encoded session too big (%lu bytes)",
            (unsigned long)key_len);
        return;
    }
    packet=str_alloc(sizeof(CACHE_PACKET));

    /* setup packet */
    packet->version=1;
    packet->type=type;
    packet->timeout=htons((u_short)(timeout<64800?timeout:64800));/* 18 hours */
    memcpy(packet->key, key, key_len);
    memcpy(packet->val, val, val_len);

    /* create the socket */
    s=s_socket(AF_INET, SOCK_DGRAM, 0, 0, "cache_transfer: socket");
    if(s==INVALID_SOCKET) {
        str_free(packet);
        return;
    }

    /* retrieve pointer to the section structure of this ctx */
    section=SSL_CTX_get_ex_data(ctx, index_opt);
    if(sendto(s, (void *)packet,
#ifdef USE_WIN32
            (int)
#endif
            (sizeof(CACHE_PACKET)-MAX_VAL_LEN+val_len),
            0, &section->sessiond_addr.sa,
            addr_len(&section->sessiond_addr))<0) {
        sockerror("cache_transfer: sendto");
        closesocket(s);
        str_free(packet);
        return;
    }

    if(!ret || !ret_len) { /* no response is required */
        closesocket(s);
        str_free(packet);
        return;
    }

    /* set recvfrom timeout to 200ms */
    t.tv_sec=0;
    t.tv_usec=200;
    if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (void *)&t, sizeof t)<0) {
        sockerror("cache_transfer: setsockopt SO_RCVTIMEO");
        closesocket(s);
        str_free(packet);
        return;
    }

    /* retrieve response */
    len=recv(s, (void *)packet, sizeof(CACHE_PACKET), 0);
    closesocket(s);
    if(len<0) {
        if(get_last_socket_error()==S_EWOULDBLOCK ||
                get_last_socket_error()==S_EAGAIN)
            s_log(LOG_INFO, "cache_transfer: recv timeout");
        else
            sockerror("cache_transfer: recv");
        str_free(packet);
        return;
    }

    /* parse results */
    if(len<(int)sizeof(CACHE_PACKET)-MAX_VAL_LEN || /* too short */
            packet->version!=1 || /* wrong version */
            safe_memcmp(packet->key, key, key_len)) { /* wrong session id */
        s_log(LOG_DEBUG, "cache_transfer: malformed packet received");
        str_free(packet);
        return;
    }
    if(packet->type!=CACHE_RESP_OK) {
        s_log(LOG_INFO, "cache_transfer: session not found");
        str_free(packet);
        return;
    }
    *ret_len=(size_t)len-(sizeof(CACHE_PACKET)-MAX_VAL_LEN);
    *ret=str_alloc(*ret_len);
    s_log(LOG_INFO, "cache_transfer: session found");
    memcpy(*ret, packet->val, *ret_len);
    str_free(packet);
}

/**************************************** informational callback */

NOEXPORT void info_callback(const SSL *ssl, int where, int ret) {
    CLI *c;
    SSL_CTX *ctx;
    const char *state_string;

    c=SSL_get_ex_data((SSL *)ssl, index_cli);
    if(c) {
        if((where&SSL_CB_HANDSHAKE_DONE)
                && c->reneg_state==RENEG_INIT) {
            /* first (initial) handshake was completed, remember this,
             * so that further renegotiation attempts can be detected */
            c->reneg_state=RENEG_ESTABLISHED;
        } else if((where&SSL_CB_ACCEPT_LOOP)
                && c->reneg_state==RENEG_ESTABLISHED) {
            int state=SSL_get_state((SSL *)ssl);

#ifndef SSL3_ST_SR_CLNT_HELLO_A
            if(state==TLS_ST_SR_CLNT_HELLO
                    || state==TLS_ST_SR_CLNT_HELLO) {
#else
            if(state==SSL3_ST_SR_CLNT_HELLO_A
                    || state==SSL23_ST_SR_CLNT_HELLO_A) {
#endif
                /* client hello received after initial handshake,
                 * this means renegotiation -> mark it */
                c->reneg_state=RENEG_DETECTED;
            }
        }
        if(c->opt->log_level<LOG_DEBUG) /* performance optimization */
            return;
    }

    if(where & SSL_CB_LOOP) {
        state_string=SSL_state_string_long(ssl);
        if(strcmp(state_string, "unknown state"))
            s_log(LOG_DEBUG, "SSL state (%s): %s",
                where & SSL_ST_CONNECT ? "connect" :
                where & SSL_ST_ACCEPT ? "accept" :
                "undefined", state_string);
    } else if(where & SSL_CB_ALERT) {
        s_log(LOG_DEBUG, "SSL alert (%s): %s: %s",
            where & SSL_CB_READ ? "read" : "write",
            SSL_alert_type_string_long(ret),
            SSL_alert_desc_string_long(ret));
    } else if(where==SSL_CB_HANDSHAKE_DONE) {
        ctx=SSL_get_SSL_CTX((SSL *)ssl);
        if(c->opt->option.client) {
            s_log(LOG_DEBUG, "%6ld client connect(s) requested",
                SSL_CTX_sess_connect(ctx));
            s_log(LOG_DEBUG, "%6ld client connect(s) succeeded",
                SSL_CTX_sess_connect_good(ctx));
            s_log(LOG_DEBUG, "%6ld client renegotiation(s) requested",
                SSL_CTX_sess_connect_renegotiate(ctx));
        } else {
            s_log(LOG_DEBUG, "%6ld server accept(s) requested",
                SSL_CTX_sess_accept(ctx));
            s_log(LOG_DEBUG, "%6ld server accept(s) succeeded",
                SSL_CTX_sess_accept_good(ctx));
            s_log(LOG_DEBUG, "%6ld server renegotiation(s) requested",
                SSL_CTX_sess_accept_renegotiate(ctx));
        }
        /* according to the source it not only includes internal
           and external session caches, but also session tickets */
        s_log(LOG_DEBUG, "%6ld session reuse(s)",
            SSL_CTX_sess_hits(ctx));
        if(!c->opt->option.client) { /* server session cache stats */
            s_log(LOG_DEBUG, "%6ld internal session cache item(s)",
                SSL_CTX_sess_number(ctx));
            s_log(LOG_DEBUG, "%6ld internal session cache fill-up(s)",
                SSL_CTX_sess_cache_full(ctx));
            s_log(LOG_DEBUG, "%6ld internal session cache miss(es)",
                SSL_CTX_sess_misses(ctx));
            s_log(LOG_DEBUG, "%6ld external session cache hit(s)",
                SSL_CTX_sess_cb_hits(ctx));
            s_log(LOG_DEBUG, "%6ld expired session(s) retrieved",
                SSL_CTX_sess_timeouts(ctx));
        }
    }
}

/**************************************** SSL error reporting */

void sslerror(char *txt) { /* OpenSSL error handler */
    unsigned long err;

    err=ERR_get_error();
    if(err) {
        sslerror_queue();
        sslerror_log(err, txt);
    } else {
        s_log(LOG_ERR, "%s: Peer suddenly disconnected", txt);
    }
}

NOEXPORT void sslerror_queue(void) { /* recursive dump of the error queue */
    unsigned long err;

    err=ERR_get_error();
    if(err) {
        sslerror_queue();
        sslerror_log(err, "error queue");
    }
}

NOEXPORT void sslerror_log(unsigned long err, char *txt) {
    char *error_string;

    error_string=str_alloc(256);
    ERR_error_string_n(err, error_string, 256);
    s_log(LOG_ERR, "%s: %lX: %s", txt, err, error_string);
    str_free(error_string);
}

/* end of ctx.c */
