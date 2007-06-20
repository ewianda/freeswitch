/* 
 * FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
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
 * The Original Code is FreeSWITCH Modular Media Switching Software Library / Soft-Switch Application
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
 *
 * mod_console.c -- Console Logger
 *
 */
#include <switch.h>

SWITCH_MODULE_LOAD_FUNCTION(mod_console_load);
SWITCH_MODULE_DEFINITION(mod_console, mod_console_load, NULL, NULL);

static const uint8_t STATIC_LEVELS[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
static int COLORIZE = 0;
#ifdef WIN32
static HANDLE hStdout;
static WORD wOldColorAttrs;
static CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
static WORD COLORS[] = { FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_RED | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	FOREGROUND_GREEN | FOREGROUND_INTENSITY
};
#else
static const char *COLORS[] = { SWITCH_SEQ_FRED, SWITCH_SEQ_FRED, SWITCH_SEQ_FRED, SWITCH_SEQ_FRED, SWITCH_SEQ_FMAGEN, SWITCH_SEQ_FCYAN,
	SWITCH_SEQ_FGREEN, SWITCH_SEQ_FYELLOW, ""
};
#endif

static switch_memory_pool_t *module_pool = NULL;
static switch_hash_t *log_hash = NULL;
static switch_hash_t *name_hash = NULL;
static int8_t all_level = -1;

static void del_mapping(char *var)
{
	if (!strcasecmp(var, "all")) {
		all_level = -1;
		return;
	}
	switch_core_hash_insert(log_hash, var, NULL);
}

static void add_mapping(char *var, char *val)
{
	char *name;

	if (!strcasecmp(var, "all")) {
		all_level = (int8_t) switch_log_str2level(val);
		return;
	}

	if (!(name = switch_core_hash_find(name_hash, var))) {
		name = switch_core_strdup(module_pool, var);
		switch_core_hash_insert(name_hash, name, name);
	}

	del_mapping(name);
	switch_core_hash_insert(log_hash, name, (void *) &STATIC_LEVELS[(uint8_t) switch_log_str2level(val)]);
}

static switch_status_t config_logger(void)
{
	char *cf = "console.conf";
	switch_xml_t cfg, xml, settings, param;

	if (!(xml = switch_xml_open_cfg(cf, &cfg, NULL))) {
		switch_log_printf(SWITCH_CHANNEL_LOG, SWITCH_LOG_ERROR, "open of %s failed\n", cf);
		return SWITCH_STATUS_TERM;
	}

	switch_core_hash_init(&log_hash, module_pool);
	switch_core_hash_init(&name_hash, module_pool);

	if ((settings = switch_xml_child(cfg, "mappings"))) {
		for (param = switch_xml_child(settings, "param"); param; param = param->next) {
			char *var = (char *) switch_xml_attr_soft(param, "name");
			char *val = (char *) switch_xml_attr_soft(param, "value");

			add_mapping(var, val);
		}
	}

	if ((settings = switch_xml_child(cfg, "settings"))) {
		for (param = switch_xml_child(settings, "param"); param; param = param->next) {
			char *var = (char *) switch_xml_attr_soft(param, "name");
			char *val = (char *) switch_xml_attr_soft(param, "value");

			if (!strcasecmp(var, "colorize") && switch_true(val)) {
#ifdef WIN32
				hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
				if (switch_core_get_console() == stdout && hStdout != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) {
					wOldColorAttrs = csbiInfo.wAttributes;
					COLORIZE = 1;
				}
#else
				COLORIZE = 1;
#endif
			}
		}
	}

	switch_xml_free(xml);

	return SWITCH_STATUS_SUCCESS;
}

static switch_status_t switch_console_logger(const switch_log_node_t *node, switch_log_level_t level)
{
	FILE *handle;

	if ((handle = switch_core_data_channel(SWITCH_CHANNEL_ID_LOG))) {
		uint8_t *lookup = NULL;
		switch_log_level_t level = SWITCH_LOG_DEBUG;

		if (log_hash) {
			lookup = switch_core_hash_find(log_hash, node->file);

			if (!lookup) {
				lookup = switch_core_hash_find(log_hash, node->func);
			}
		}

		if (lookup) {
			level = (switch_log_level_t) *lookup;
		} else if (all_level > -1) {
			level = (switch_log_level_t) all_level;
		}

		if (!log_hash || (((all_level > -1) || lookup) && level >= node->level)) {
			if (COLORIZE) {
#ifdef WIN32
				SetConsoleTextAttribute(hStdout, COLORS[node->level]);
				WriteFile(hStdout, node->data, (DWORD) strlen(node->data), NULL, NULL);
				SetConsoleTextAttribute(hStdout, wOldColorAttrs);
#else
				fprintf(handle, "%s%s%s", COLORS[node->level], node->data, SWITCH_SEQ_DEFAULT_COLOR);
#endif
			} else {
				fprintf(handle, "%s", node->data);
			}
		}
	} else {
		fprintf(stderr, "HELP I HAVE NO CONSOLE TO LOG TO!\n");
		fflush(stderr);
	}

	return SWITCH_STATUS_SUCCESS;
}

SWITCH_MODULE_LOAD_FUNCTION(mod_console_load)
{
	module_pool = pool;

	/* connect my internal structure to the blank pointer passed to me */
	*module_interface = switch_loadable_module_create_module_interface(pool, modname);

	/* setup my logger function */
	switch_log_bind_logger(switch_console_logger, SWITCH_LOG_DEBUG);

	config_logger();

	/* indicate that the module should continue to be loaded */
	return SWITCH_STATUS_SUCCESS;
}

/* For Emacs:
 * Local Variables:
 * mode:c
 * indent-tabs-mode:t
 * tab-width:4
 * c-basic-offset:4
 * End:
 * For VIM:
 * vim:set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
 */
