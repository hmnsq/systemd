/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bus-label.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unit-def.h"
#अन्तर्भाव "unit-name.h"

अक्षर* unit_dbus_path_from_name(स्थिरांक अक्षर *name) {
        _cleanup_free_ अक्षर *e = रिक्त;

        प्रतिज्ञा(name);

        e = bus_label_escape(name);
        जर (!e)
                परत रिक्त;

        परत strjoin("/org/freedesktop/systemd1/unit/", e);
}

पूर्णांक unit_name_from_dbus_path(स्थिरांक अक्षर *path, अक्षर **name) {
        स्थिरांक अक्षर *e;
        अक्षर *n;

        प्रतिज्ञा(name);

        e = startswith(path, "/org/freedesktop/systemd1/unit/");
        जर (!e)
                परत -EINVAL;

        n = bus_label_unescape(e);
        जर (!n)
                परत -ENOMEM;

        *name = n;
        परत 0;
}

स्थिरांक अक्षर* unit_dbus_interface_from_type(UnitType t) {

        स्थिर स्थिरांक अक्षर *स्थिरांक table[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_TARGET]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_SWAP]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_PATH]      = "org.freedesktop.systemd1.Path",
                [UNIT_SLICE]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        जर (t < 0)
                परत रिक्त;
        जर (t >= _UNIT_TYPE_MAX)
                परत रिक्त;

        परत table[t];
}

स्थिरांक अक्षर* unit_dbus_interface_from_name(स्थिरांक अक्षर *name) {
        UnitType t;

        t = unit_name_to_type(name);
        जर (t < 0)
                परत रिक्त;

        परत unit_dbus_interface_from_type(t);
}

स्थिरांक अक्षर* unit_type_to_capitalized_string(UnitType t) {
        स्थिरांक अक्षर *di = unit_dbus_interface_from_type(t);
        जर (!di)
                परत रिक्त;

        परत ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

स्थिर स्थिरांक अक्षर* स्थिरांक unit_type_table[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_TARGET]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_SWAP]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_PATH]      = "path",
        [UNIT_SLICE]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_TABLE_LOOKUP(unit_type, UnitType);

स्थिर स्थिरांक अक्षर* स्थिरांक unit_load_state_table[_UNIT_LOAD_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_FOUND]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_TABLE_LOOKUP(unit_load_state, UnitLoadState);

/* Keep in sync with man/unit-states.xml */
स्थिर स्थिरांक अक्षर* स्थिरांक unit_active_state_table[_UNIT_ACTIVE_STATE_MAX] = {
        [UNIT_ACTIVE]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_TABLE_LOOKUP(unit_active_state, UnitActiveState);

स्थिर स्थिरांक अक्षर* स्थिरांक freezer_state_table[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_PARENT] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_PARENT]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_TABLE_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
स्थिर स्थिरांक FreezerState freezer_state_finish_table[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_PARENT] = FREEZER_FROZEN_BY_PARENT,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_PARENT]   = FREEZER_FROZEN_BY_PARENT,
};

FreezerState freezer_state_finish(FreezerState state) {
        प्रतिज्ञा(state >= 0);
        प्रतिज्ञा(state < _FREEZER_STATE_MAX);

        परत freezer_state_finish_table[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_finish(state);
        जर (objective == FREEZER_FROZEN_BY_PARENT)
                objective = FREEZER_FROZEN;

        परत objective;
}

स्थिर स्थिरांक अक्षर* स्थिरांक unit_marker_table[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_START]   = "needs-start",
};

DEFINE_STRING_TABLE_LOOKUP(unit_marker, UnitMarker);

स्थिर स्थिरांक अक्षर* स्थिरांक automount_state_table[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_WAITING] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_TABLE_LOOKUP(automount_state, AutomountState);

स्थिर स्थिरांक अक्षर* स्थिरांक device_state_table[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_TABLE_LOOKUP(device_state, DeviceState);

स्थिर स्थिरांक अक्षर* स्थिरांक mount_state_table[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_DONE]      = "mounting-done",
        [MOUNT_MOUNTED]            = "mounted",
        [MOUNT_REMOUNTING]         = "remounting",
        [MOUNT_UNMOUNTING]         = "unmounting",
        [MOUNT_REMOUNTING_SIGTERM] = "remounting-sigterm",
        [MOUNT_REMOUNTING_SIGKILL] = "remounting-sigkill",
        [MOUNT_UNMOUNTING_SIGTERM] = "unmounting-sigterm",
        [MOUNT_UNMOUNTING_SIGKILL] = "unmounting-sigkill",
        [MOUNT_FAILED]             = "failed",
        [MOUNT_CLEANING]           = "cleaning",
};

DEFINE_STRING_TABLE_LOOKUP(mount_state, MountState);

स्थिर स्थिरांक अक्षर* स्थिरांक path_state_table[_PATH_STATE_MAX] = {
        [PATH_DEAD]    = "dead",
        [PATH_WAITING] = "waiting",
        [PATH_RUNNING] = "running",
        [PATH_FAILED]  = "failed",
};

DEFINE_STRING_TABLE_LOOKUP(path_state, PathState);

स्थिर स्थिरांक अक्षर* स्थिरांक scope_state_table[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_START_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_TABLE_LOOKUP(scope_state, ScopeState);

स्थिर स्थिरांक अक्षर* स्थिरांक service_state_table[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_START_PRE]                  = "start-pre",
        [SERVICE_START]                      = "start",
        [SERVICE_START_POST]                 = "start-post",
        [SERVICE_RUNNING]                    = "running",
        [SERVICE_RUNNING_REVALIDATING]       = "running-revalidating",
        [SERVICE_EXITED]                     = "exited",
        [SERVICE_REFRESH_EXTENSIONS]         = "refresh-extensions",
        [SERVICE_REFRESH_CREDENTIALS]        = "refresh-credentials",
        [SERVICE_RELOAD]                     = "reload",
        [SERVICE_RELOAD_SIGNAL]              = "reload-signal",
        [SERVICE_RELOAD_NOTIFY]              = "reload-notify",
        [SERVICE_RELOAD_POST]                = "reload-post",
        [SERVICE_STOP]                       = "stop",
        [SERVICE_STOP_WATCHDOG]              = "stop-watchdog",
        [SERVICE_STOP_SIGTERM]               = "stop-sigterm",
        [SERVICE_STOP_SIGKILL]               = "stop-sigkill",
        [SERVICE_STOP_POST]                  = "stop-post",
        [SERVICE_FINAL_WATCHDOG]             = "final-watchdog",
        [SERVICE_FINAL_SIGTERM]              = "final-sigterm",
        [SERVICE_FINAL_SIGKILL]              = "final-sigkill",
        [SERVICE_FAILED]                     = "failed",
        [SERVICE_DEAD_BEFORE_AUTO_RESTART]   = "dead-before-auto-restart",
        [SERVICE_FAILED_BEFORE_AUTO_RESTART] = "failed-before-auto-restart",
        [SERVICE_DEAD_RESOURCES_PINNED]      = "dead-resources-pinned",
        [SERVICE_AUTO_RESTART]               = "auto-restart",
        [SERVICE_AUTO_RESTART_QUEUED]        = "auto-restart-queued",
        [SERVICE_CLEANING]                   = "cleaning",
        [SERVICE_MOUNTING]                   = "mounting",
};

DEFINE_STRING_TABLE_LOOKUP(service_state, ServiceState);

स्थिर स्थिरांक अक्षर* स्थिरांक slice_state_table[_SLICE_STATE_MAX] = {
        [SLICE_DEAD]   = "dead",
        [SLICE_ACTIVE] = "active",
};

DEFINE_STRING_TABLE_LOOKUP(slice_state, SliceState);

स्थिर स्थिरांक अक्षर* स्थिरांक socket_state_table[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_START_PRE]        = "start-pre",
        [SOCKET_START_OPEN]       = "start-open",
        [SOCKET_START_CHOWN]      = "start-chown",
        [SOCKET_START_POST]       = "start-post",
        [SOCKET_LISTENING]        = "listening",
        [SOCKET_DEFERRED]         = "deferred",
        [SOCKET_RUNNING]          = "running",
        [SOCKET_STOP_PRE]         = "stop-pre",
        [SOCKET_STOP_PRE_SIGTERM] = "stop-pre-sigterm",
        [SOCKET_STOP_PRE_SIGKILL] = "stop-pre-sigkill",
        [SOCKET_STOP_POST]        = "stop-post",
        [SOCKET_FINAL_SIGTERM]    = "final-sigterm",
        [SOCKET_FINAL_SIGKILL]    = "final-sigkill",
        [SOCKET_FAILED]           = "failed",
        [SOCKET_CLEANING]         = "cleaning",
};

DEFINE_STRING_TABLE_LOOKUP(socket_state, SocketState);

स्थिर स्थिरांक अक्षर* स्थिरांक swap_state_table[_SWAP_STATE_MAX] = {
        [SWAP_DEAD]                 = "dead",
        [SWAP_ACTIVATING]           = "activating",
        [SWAP_ACTIVATING_DONE]      = "activating-done",
        [SWAP_ACTIVE]               = "active",
        [SWAP_DEACTIVATING]         = "deactivating",
        [SWAP_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [SWAP_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [SWAP_FAILED]               = "failed",
        [SWAP_CLEANING]             = "cleaning",
};

DEFINE_STRING_TABLE_LOOKUP(swap_state, SwapState);

स्थिर स्थिरांक अक्षर* स्थिरांक target_state_table[_TARGET_STATE_MAX] = {
        [TARGET_DEAD]   = "dead",
        [TARGET_ACTIVE] = "active",
};

DEFINE_STRING_TABLE_LOOKUP(target_state, TargetState);

स्थिर स्थिरांक अक्षर* स्थिरांक timer_state_table[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_WAITING] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_TABLE_LOOKUP(timer_state, TimerState);

स्थिर स्थिरांक अक्षर* स्थिरांक unit_dependency_table[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_WANTS]                  = "Wants",
        [UNIT_BINDS_TO]               = "BindsTo",
        [UNIT_PART_OF]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_OF]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_OF]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_OF]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_OF]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_TO]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_TO]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_OF]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_IN_SLICE]               = "InSlice",
        [UNIT_SLICE_OF]               = "SliceOf",
};

DEFINE_STRING_TABLE_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        परत IN_SET(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_WANTS,
                      UNIT_BINDS_TO,
                      UNIT_PART_OF,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_TO,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_TO,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_OF);
}

शून्य unit_types_list(शून्य) {
        DUMP_STRING_TABLE(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

स्थिर स्थिरांक अक्षर* स्थिरांक notify_access_table[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_TABLE_LOOKUP(notify_access, NotifyAccess);

स्थिर स्थिरांक अक्षर* स्थिरांक job_mode_table[_JOB_MODE_MAX] = {
        [JOB_FAIL]                 = "fail",
        [JOB_LENIENT]              = "lenient",
        [JOB_REPLACE]              = "replace",
        [JOB_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [JOB_ISOLATE]              = "isolate",
        [JOB_FLUSH]                = "flush",
        [JOB_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [JOB_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [JOB_TRIGGERING]           = "triggering",
        [JOB_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_TABLE_LOOKUP(job_mode, JobMode);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
स्थिर स्थिरांक अक्षर* स्थिरांक exec_directory_type_table[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_TABLE_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
स्थिर स्थिरांक अक्षर* स्थिरांक exec_directory_type_symlink_table[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_TABLE_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_active_state_to_glyph(UnitActiveState state) {
        स्थिर स्थिरांक Glyph map[_UNIT_ACTIVE_STATE_MAX] = {
                [UNIT_ACTIVE]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        जर (state < 0)
                परत _GLYPH_INVALID;

        प्रतिज्ञा(state < _UNIT_ACTIVE_STATE_MAX);
        परत map[state];
}
