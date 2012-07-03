/*
 * Command-execute
 * Copyright (C) 2010 tymm
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#define PURPLE_PLUGINS

#include <glib.h>
#include <stdlib.h>
#include <string.h>

#include "notify.h"
#include "plugin.h"
#include "version.h"
#include "debug.h"
#include "prefs.h"

#define PLUGIN_ID "core-tymm-command-execute"

#ifndef _PIDGIN_CONVERSATION_H_
typedef enum
{
        PIDGIN_UNSEEN_NONE,
        PIDGIN_UNSEEN_EVENT,
        PIDGIN_UNSEEN_NO_LOG,
        PIDGIN_UNSEEN_TEXT,
        PIDGIN_UNSEEN_NICK
} PidginUnseenState;
#endif

/* cmdexe_conversation_updated is called everytime a message is received */
static void cmdexe_conversation_updated(PurpleConversation *conv, PurpleConvUpdateType type) {
	gboolean has_unseen_state=FALSE, has_unseen_count;
	if( type != PURPLE_CONV_UPDATE_UNSEEN) return;

	/* Due to the really bad API documentation I dont really know whats the difference between unseen-state and unseen-count */
	//has_unseen_state = (GPOINTER_TO_INT(purple_conversation_get_data(conv, "unseen-state")) >= PIDGIN_UNSEEN_TEXT);
	has_unseen_count = (GPOINTER_TO_INT(purple_conversation_get_data(conv, "unseen-count")) > 0);

	if(has_unseen_state || has_unseen_count) {
		const char *cmd = purple_prefs_get_string("/plugins/core/tymm-command-execute/command");

		if(strcmp(cmd,"") != 0) {
			/* Execute command */
			if(system(cmd) == 0) {
				purple_debug_info(PLUGIN_ID, "command executed\n");
			} else {
				purple_debug_warning(PLUGIN_ID, "executing the command failed\n");
			}
		} else {
			purple_debug_warning(PLUGIN_ID, "no command found\n");
		}
	}
}

static gboolean plugin_load(PurplePlugin *plugin) {
	/* Connect the conversation-updated signal to the callback function cmdexe_conversation_updated */
	purple_signal_connect(purple_conversations_get_handle(), "conversation-updated", plugin, PURPLE_CALLBACK(cmdexe_conversation_updated), NULL);
	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	/* Disconnect the signal if the plugin is getting unloaded */
	purple_signal_disconnect(purple_conversations_get_handle(), "conversation-updated", plugin, PURPLE_CALLBACK(cmdexe_conversation_updated));
	return TRUE;
}

/* UI */
static PurplePluginPrefFrame *plugin_config_frame(PurplePlugin *plugin) {
	PurplePluginPrefFrame *frame;
	PurplePluginPref *ppref;

	frame = purple_plugin_pref_frame_new();

	ppref = purple_plugin_pref_new_with_label("Insert Command:");
	purple_plugin_pref_frame_add(frame, ppref);

	ppref = purple_plugin_pref_new_with_name_and_label("/plugins/core/tymm-command-execute/command", "Command");
	purple_plugin_pref_frame_add(frame, ppref);

	return frame;
}

static PurplePluginUiInfo prefs_info = {
	plugin_config_frame,
	0,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,
	PLUGIN_ID,
	"Command execute",
	"0.1",
	"Command execution for pidgin and finch",
	"Takes a command which will be executed everytime a message is received",
	"tymm <tymm@gmx.de>",
	"",
	plugin_load,
	plugin_unload,
	NULL, // plugin_destroy
	NULL,
	NULL,
	&prefs_info,
	NULL, // plugin_actions function here
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin) {
	purple_prefs_add_none("/plugins/core/tymm-command-execute");
	purple_prefs_add_string("/plugins/core/tymm-command-execute/command", "");
}

PURPLE_INIT_PLUGIN(command-execute, init_plugin, info)
