/* 
 * libDingaLing XMPP Jingle Library
 * Copyright (C) 2005/2006, Anthony Minessale II <anthmct@yahoo.com>
 *
 * Version: MPL 1.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is libDingaLing XMPP Jingle Library
 *
 * The Initial Developer of the Original Code is
 * Anthony Minessale II <anthmct@yahoo.com>
 * Portions created by the Initial Developer are Copyright (C)
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * 
 * Anthony Minessale II <anthmct@yahoo.com>
 *
 * libdingaling.h -- Main Header File
 *
 */
/*! \file libdingaling.h
    \brief Main Header File
*/

/*!
  \defgroup core1 libDingaLing Library
  \ingroup LIBDINGALING
  \{
*/
#ifndef LIBDINGALING_H
#define LIBDINGALING_H
#ifdef __cplusplus
extern "C" {
#endif
#ifdef __STUPIDFORMATBUG__
}
#endif

#define LDL_HANDLE_QLEN 2000
#define LDL_MAX_CANDIDATES 10
#define LDL_MAX_PAYLOADS 50
#define LDL_RETRY 3

/*! \brief A structure to store a jingle candidate */
struct ldl_candidate {
	/*! the transport id of the candidate */
	char *tid;
	/*! the name of the candidate */
	char *name;
	/*! the type of the candidate */
	char *type;
	/*! the protocol of the candidate */
	char *protocol;
	/*! the STUN username of the candidate */
	char *username;
	/*! the STUN password of the candidate */
	char *password;
	/*! the ip address of the candidate */
	char *address;
	/*! the stun/rtp port of the candidate */
	uint16_t port;
	/*! the preference level of the candidate */
	double pref;
};
typedef struct ldl_candidate ldl_candidate_t;

/*! \brief A structure to store a jingle payload */
struct ldl_payload {
	/*! the iana name of the payload type */
	char *name;
	/*! the iana id of the payload type */
	unsigned int id;
	/*! the transfer rate of the payload type */
	unsigned int rate;
	/*! the bits per second of the payload type */
	unsigned int bps;
};
typedef struct ldl_payload ldl_payload_t;

struct ldl_handle;
typedef struct ldl_handle ldl_handle_t;

struct ldl_session;
typedef struct ldl_session ldl_session_t;

typedef enum {
	LDL_STATUS_SUCCESS,
	LDL_STATUS_FALSE,
	LDL_STATUS_MEMERR,
} ldl_status;

typedef enum {
	LDL_FLAG_INIT = (1 << 0),
	LDL_FLAG_RUNNING = (1 << 1),
	LDL_FLAG_AUTHORIZED = (1 << 2),
	LDL_FLAG_READY = (1 << 3),
	LDL_FLAG_CONNECTED = (1 << 4),
	LDL_FLAG_QUEUE_RUNNING = (1 << 5),
} ldl_flag_t;

typedef enum {
	LDL_FLAG_TLS = (1 << 10),
	LDL_FLAG_SASL_PLAIN = (1 << 11),
	LDL_FLAG_SASL_MD5 = (1 << 12)
} ldl_user_flag_t;

typedef enum {
	LDL_SIGNAL_NONE,
	LDL_SIGNAL_INITIATE,
	LDL_SIGNAL_CANDIDATES,
	LDL_SIGNAL_MSG,
	LDL_SIGNAL_TERMINATE,
	LDL_SIGNAL_ERROR,
	LDL_SIGNAL_LOGIN_SUCCESS,
	LDL_SIGNAL_LOGIN_FAILURE,
	LDL_SIGNAL_CONNECTED
} ldl_signal_t;

typedef enum {
	LDL_REPLY_ACK,
	LDL_REPLY_NACK,
} ldl_reply_t;

typedef enum {
	LDL_STATE_NEW,
	LDL_STATE_ANSWERED,
	LDL_STATE_DESTROYED,
} ldl_state_t;

typedef enum {
	LDL_DESCRIPTION_INITIATE,
	LDL_DESCRIPTION_ACCEPT
} ldl_description_t;

#define DL_PRE __FILE__, __FUNCTION__, __LINE__
#define DL_LOG_DEBUG DL_PRE, 7
#define DL_LOG_INFO DL_PRE, 6
#define DL_LOG_NOTICE DL_PRE, 5
#define DL_LOG_WARNING DL_PRE, 4
#define DL_LOG_ERR DL_PRE, 3
#define DL_LOG_CRIT DL_PRE, 2
#define DL_LOG_ALERT DL_PRE, 1
#define DL_LOG_EMERG DL_PRE, 0

typedef ldl_status (*ldl_loop_callback_t)(ldl_handle_t *);
typedef ldl_status (*ldl_session_callback_t)(ldl_handle_t *, ldl_session_t *, ldl_signal_t, char *, char *, char *);
typedef ldl_status (*ldl_response_callback_t)(ldl_handle_t *, char *);
typedef void (*ldl_logger_t)(char *file, const char *func, int line, int level, char *fmt, ...);

#define ldl_yield(ms) apr_sleep(ms * 10); apr_thread_yield();

/*!
  \brief Test for the existance of a flag on an arbitary object
  \param obj the object to test
  \param flag the or'd list of flags to test
  \return true value if the object has the flags defined
*/
#define ldl_test_flag(obj, flag) ((obj)->flags & flag)


/*!
  \brief Set a flag on an arbitrary object
  \param obj the object to set the flags on
  \param flag the or'd list of flags to set
*/
#define ldl_set_flag(obj, flag) (obj)->flags |= (flag)

/*!
  \brief Clear a flag on an arbitrary object
  \param obj the object to test
  \param flag the or'd list of flags to clear
*/
#define ldl_clear_flag(obj, flag) (obj)->flags &= ~(flag)

/*!
  \brief Set a flag on an arbitrary object while locked
  \param obj the object to set the flags on
  \param flag the or'd list of flags to set
*/
#define ldl_set_flag_locked(obj, flag) assert(obj->flag_mutex != NULL);\
apr_thread_mutex_lock(obj->flag_mutex);\
(obj)->flags |= (flag);\
apr_thread_mutex_unlock(obj->flag_mutex);

/*!
  \brief Clear a flag on an arbitrary object
  \param obj the object to test
  \param flag the or'd list of flags to clear
*/
#define ldl_clear_flag_locked(obj, flag) apr_thread_mutex_lock(obj->flag_mutex); (obj)->flags &= ~(flag); apr_thread_mutex_unlock(obj->flag_mutex);

/*!
  \brief Copy flags from one arbitrary object to another
  \param dest the object to copy the flags to
  \param src the object to copy the flags from
  \param flags the flags to copy
*/
#define ldl_copy_flags(dest, src, flags) (dest)->flags &= ~(flags);	(dest)->flags |= ((src)->flags & (flags))

/*!
  \brief Test for NULL or zero length string
  \param s the string to test
  \return true value if the string is NULL or zero length
*/
#define ldl_strlen_zero(s) (s && *s != '\0') ? 0 : 1

/*!
  \brief Destroy a Jingle Session
  \param session_p the session to destroy
  \return SUCCESS OR FAILURE
*/
ldl_status ldl_session_destroy(ldl_session_t **session_p);

/*!
  \brief Get a value from a session
  \param session the session
  \param key the key to look up
  \return the value
*/
char *ldl_session_get_value(ldl_session_t *session, char *key);

/*!
  \brief Set a value on a session
  \param session the session
  \param key the key to set
  \param val the value of the key
*/
void ldl_session_set_value(ldl_session_t *session, char *key, char *val);

/*!
  \brief Create a Jingle Session
  \param session_p pointer to reference the session
  \param handle handle to associate the session with
  \param id the id to use for the session
  \param them the id of the other end of the call
  \param me the id of our end of the call
  \return SUCCESS OR FAILURE
*/
ldl_status ldl_session_create(ldl_session_t **session_p, ldl_handle_t *handle, char *id, char *them, char *me);

/*!
  \brief get the id of a session
  \param session the session to get the id of
  \return the requested id
*/
char *ldl_session_get_id(ldl_session_t *session);

/*!
  \brief Get the caller name of a session
  \param session the session to get the caller from
  \return the caller name
*/
char *ldl_session_get_caller(ldl_session_t *session);

/*!
  \brief Set the ip of a session
  \param session the session to set the ip on
  \param ip the ip
*/
void ldl_session_set_ip(ldl_session_t *session, char *ip);

/*!
  \brief Get the ip of a session
  \param session the session to get the ip from
  \return the ip
*/
char *ldl_session_get_ip(ldl_session_t *session);

/*!
  \brief Set a private pointer to associate with the session
  \param session the session to set the data pointer to
  \param private_data the data to associate
*/
void ldl_session_set_private(ldl_session_t *session, void *private_data);

/*!
  \brief Get a private pointer from a session
  \param session the session to get the data from
  \return the data
*/
void *ldl_session_get_private(ldl_session_t *session);

/*!
  \brief Accept a candidate
  \param session the session to accept on
  \param candidate the candidate to accept
*/
void ldl_session_accept_candidate(ldl_session_t *session, ldl_candidate_t *candidate);

/*!
  \brief Set a custom logger
  \param logger the logger function
*/
void ldl_global_set_logger(ldl_logger_t logger);

/*!
  \brief Perform a probe on a given id to resolve the proper Jingle Resource
  \param handle the connection handle to use.
  \param id the id to probe
  \param buf a string to store the result
  \param len the size in bytes of the string
  \return a pointer to buf if a successful lookup was made otherwise NULL
*/
char *ldl_handle_probe(ldl_handle_t *handle, char *id, char *buf, unsigned int len);

/*!
  \brief Signal a termination request on a given session
  \param session the session to terminate
  \return TRUE if the signal was sent.
*/
unsigned int ldl_session_terminate(ldl_session_t *session);

/*!
  \brief Get the private data of a connection handle
  \param handle the conection handle
  \return the requested data
*/
void *ldl_handle_get_private(ldl_handle_t *handle);

/*!
  \brief Send a message to a session
  \param session the session handle
  \param to the message recipiant
  \param subject optional subject
  \param body body of the message
*/
void ldl_session_send_msg(ldl_session_t *session, char *subject, char *body);

/*!
  \brief Send a message
  \param handle the conection handle
  \param to the message recipiant
  \param subject optional subject
  \param body body of the message
*/
void ldl_handle_send_msg(ldl_handle_t *handle, char *to, char *subject, char *body);

/*!
  \brief Offer candidates to a potential session
  \param session the session to send candidates on
  \param candidates an array of candidate description objects
  \param clen the number of elements in the candidates array
  \return the message_id of the generated xmpp request
*/
unsigned int ldl_session_candidates(ldl_session_t *session,
								  ldl_candidate_t *candidates,
								  unsigned int clen);

/*!
  \brief Initiate or Accept a new session and provide transport options
  \param session the session to initiate or accept
  \param payloads an array of payload description objects
  \param plen the number of elements in the payloads array
  \param description the type of description LDL_DESCRIPTION_INITIATE or LDL_DESCRIPTION_ACCEPT
  \return the message_id of the generated xmpp request
*/
unsigned int ldl_session_describe(ldl_session_t *session,
								ldl_payload_t *payloads,
								unsigned int plen,
								ldl_description_t description);


/*!
  \brief get a session's state
  \param session a session to get the state from
  \return the state
*/
ldl_state_t ldl_session_get_state(ldl_session_t *session);


/*!
  \brief get the candidates
  \param session the session
  \param candidates pointer to point at array of the candidates
  \param len the resulting len of the array pointer
  \return success or failure
*/
ldl_status ldl_session_get_candidates(ldl_session_t *session, ldl_candidate_t **candidates, unsigned int *len);

/*!
  \brief get the payloads
  \param session the session
  \param payloads pointer to point at array of the payloads
  \param len the resulting len of the array pointer
  \return success or failure
*/
ldl_status ldl_session_get_payloads(ldl_session_t *session, ldl_payload_t **payloads, unsigned int *len);

/*!
  \brief Initilize libDingaLing
  \param debug debug level
  \return success or failure
*/
ldl_status ldl_global_init(int debug);

/*!
  \brief Destroy libDingaLing
  \return success or failure
*/
ldl_status ldl_global_destroy(void);

/*!
  \brief Set the log stream
  \param log_stream the new log stream
*/
void ldl_global_set_log_stream(FILE *log_stream);

int8_t ldl_handle_ready(ldl_handle_t *handle);

/*!
  \brief Initilize a new libDingaLing handle
  \param handle the Dingaling handle to initialize
  \param login the xmpp login
  \param password the password
  \param status_msg status message to advertise
  \param loop_callback optional loop callback
  \param session_callback function to call on session signalling
  \param response_callback function to call on responses
  \param private_info optional pointer to private data
  \return success or failure
*/
ldl_status ldl_handle_init(ldl_handle_t **handle,
						   char *login,
						   char *password,
						   char *server,
						   ldl_user_flag_t flags,
						   char *status_msg,
						   ldl_loop_callback_t loop_callback,
						   ldl_session_callback_t session_callback,
						   ldl_response_callback_t response_callback,
						   void *private_info);

/*!
  \brief Run a libDingaLing handle
  \param handle the Dingaling handle to run
*/
void ldl_handle_run(ldl_handle_t *handle);

/*!
  \brief Stop a libDingaLing handle
  \param handle the Dingaling handle to stop
*/
void ldl_handle_stop(ldl_handle_t *handle);


/*!
  \brief Destroy a libDingaLing handle
  \param handle the Dingaling handle to destroy
  \return success or failure
*/
ldl_status ldl_handle_destroy(ldl_handle_t **handle);

/*!
  \brief Set the log stream on a handle
  \param handle the Dingaling handle
  \param log_stream the new log stream
*/
void ldl_handle_set_log_stream(ldl_handle_t *handle, FILE *log_stream);
///\}


#ifdef __cplusplus
}
#endif
/** \mainpage libDingaling
 * libDingaling - Cross Platform Jingle (Google Talk) voip signaling library

 * \section intro Introduction
 *
 * \section supports Supported Platforms
 * libDingaling has been built on the following platforms:
 *
 *  - Linux (x86, x86_64)
 *  - Windows (MSVC 2005)
 *  - Mac OS X (intel & ppc )
 *
 * \section depends Dependencies
 *  libDingaling makes use of the following external libraries.  
 *
 *		- APR (http://apr.apache.org)
 *		- iksemel (http://iksemel.jabberstudio.org/)
 *
 * \section license Licensing
 *
 * libDingaling is licensed under the terms of the MPL 1.1
 *
 */
#endif
