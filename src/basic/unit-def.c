/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "bus-label.h"
#شامل "glyph-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "unit-def.h"
#شامل "unit-name.h"

حرف* unit_dbus_راستہ_from_name(ثابت حرف *name) {
        _cleanup_free_ حرف *e = تہی;

        أكد(name);

        e = bus_label_escape(name);
        اگر (!e)
                واپس تہی;

        واپس strjoin("/org/freedesktop/systemd1/unit/", e);
}

صحیح unit_name_from_dbus_راستہ(ثابت حرف *راستہ, حرف **name) {
        ثابت حرف *e;
        حرف *n;

        أكد(name);

        e = startswith(راستہ, "/org/freedesktop/systemd1/unit/");
        اگر (!e)
                واپس -EINVAL;

        n = bus_label_unescape(e);
        اگر (!n)
                واپس -ENOMEM;

        *name = n;
        واپس 0;
}

ثابت حرف* unit_dbus_interface_from_type(UnitType t) {

        ساکن ثابت حرف *ثابت جدول[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_ہدف]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_تبادلہ]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_راستہ]      = "org.freedesktop.systemd1.Path",
                [UNIT_قاش]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        اگر (t < 0)
                واپس تہی;
        اگر (t >= _UNIT_TYPE_MAX)
                واپس تہی;

        واپس جدول[t];
}

ثابت حرف* unit_dbus_interface_from_name(ثابت حرف *name) {
        UnitType t;

        t = unit_name_مقصد_type(name);
        اگر (t < 0)
                واپس تہی;

        واپس unit_dbus_interface_from_type(t);
}

ثابت حرف* unit_type_مقصد_capitalized_string(UnitType t) {
        ثابت حرف *di = unit_dbus_interface_from_type(t);
        اگر (!di)
                واپس تہی;

        واپس ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

ساکن ثابت حرف* ثابت unit_type_جدول[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_ہدف]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_تبادلہ]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_راستہ]      = "path",
        [UNIT_قاش]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_جدول_LOOKUP(unit_type, UnitType);

ساکن ثابت حرف* ثابت unit_بھرو_state_جدول[UNIT_بھرو_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_ملا]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_جدول_LOOKUP(unit_بھرو_state, Unit_بھرو_State);

/* Keep in sync with man/unit-states.xml */
ساکن ثابت حرف* ثابت unit_فعال_state_جدول[UNIT_فعال_STATE_MAX] = {
        [UNIT_فعال]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_جدول_LOOKUP(unit_فعال_state, Unit_فعال_State);

ساکن ثابت حرف* ثابت freezer_state_جدول[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_والد] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_والد]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_جدول_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
ساکن ثابت FreezerState freezer_state_اختتام_جدول[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_والد] = FREEZER_FROZEN_BY_والد,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_والد]   = FREEZER_FROZEN_BY_والد,
};

FreezerState freezer_state_اختتام(FreezerState state) {
        أكد(state >= 0);
        أكد(state < _FREEZER_STATE_MAX);

        واپس freezer_state_اختتام_جدول[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_اختتام(state);
        اگر (objective == FREEZER_FROZEN_BY_والد)
                objective = FREEZER_FROZEN;

        واپس objective;
}

ساکن ثابت حرف* ثابت unit_marker_جدول[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_آغاز]   = "needs-start",
};

DEFINE_STRING_جدول_LOOKUP(unit_marker, UnitMarker);

ساکن ثابت حرف* ثابت automount_state_جدول[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_انتظار] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(automount_state, AutomountState);

ساکن ثابت حرف* ثابت device_state_جدول[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_جدول_LOOKUP(device_state, DeviceState);

ساکن ثابت حرف* ثابت mount_state_جدول[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_مکمل]      = "mounting-done",
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

DEFINE_STRING_جدول_LOOKUP(mount_state, MountState);

ساکن ثابت حرف* ثابت راستہ_state_جدول[راستہ_STATE_MAX] = {
        [راستہ_DEAD]    = "dead",
        [راستہ_انتظار] = "waiting",
        [راستہ_RUNNING] = "running",
        [راستہ_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(راستہ_state, راستہ_State);

ساکن ثابت حرف* ثابت scope_state_جدول[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_آغاز_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_جدول_LOOKUP(scope_state, ScopeState);

ساکن ثابت حرف* ثابت service_state_جدول[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_آغاز_PRE]                  = "start-pre",
        [SERVICE_آغاز]                      = "start",
        [SERVICE_آغاز_POST]                 = "start-post",
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

DEFINE_STRING_جدول_LOOKUP(service_state, ServiceState);

ساکن ثابت حرف* ثابت قاش_state_جدول[قاش_STATE_MAX] = {
        [قاش_DEAD]   = "dead",
        [قاش_فعال] = "active",
};

DEFINE_STRING_جدول_LOOKUP(قاش_state, قاش_State);

ساکن ثابت حرف* ثابت socket_state_جدول[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_آغاز_PRE]        = "start-pre",
        [SOCKET_آغاز_OPEN]       = "start-open",
        [SOCKET_آغاز_CHOWN]      = "start-chown",
        [SOCKET_آغاز_POST]       = "start-post",
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

DEFINE_STRING_جدول_LOOKUP(socket_state, SocketState);

ساکن ثابت حرف* ثابت تبادلہ_state_جدول[تبادلہ_STATE_MAX] = {
        [تبادلہ_DEAD]                 = "dead",
        [تبادلہ_ACTIVATING]           = "activating",
        [تبادلہ_ACTIVATING_مکمل]      = "activating-done",
        [تبادلہ_فعال]               = "active",
        [تبادلہ_DEACTIVATING]         = "deactivating",
        [تبادلہ_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [تبادلہ_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [تبادلہ_FAILED]               = "failed",
        [تبادلہ_CLEANING]             = "cleaning",
};

DEFINE_STRING_جدول_LOOKUP(تبادلہ_state, تبادلہ_State);

ساکن ثابت حرف* ثابت ہدف_state_جدول[ہدف_STATE_MAX] = {
        [ہدف_DEAD]   = "dead",
        [ہدف_فعال] = "active",
};

DEFINE_STRING_جدول_LOOKUP(ہدف_state, ہدف_State);

ساکن ثابت حرف* ثابت timer_state_جدول[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_انتظار] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(timer_state, TimerState);

ساکن ثابت حرف* ثابت unit_dependency_جدول[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_چاہتا]                  = "Wants",
        [UNIT_BINDS_مقصد]               = "BindsTo",
        [UNIT_PART_کا]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_کا]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_کا]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_کا]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_کا]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_مقصد]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_مقصد]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_کا]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_اندر_قاش]               = "InSlice",
        [UNIT_قاش_کا]               = "SliceOf",
};

DEFINE_STRING_جدول_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        واپس اندر_سیٹ(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_چاہتا,
                      UNIT_BINDS_مقصد,
                      UNIT_PART_کا,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_مقصد,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_مقصد,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_کا);
}

خالی unit_types_list(خالی) {
        DUMP_STRING_جدول(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

ساکن ثابت حرف* ثابت notify_access_جدول[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_جدول_LOOKUP(notify_access, NotifyAccess);

ساکن ثابت حرف* ثابت کام_منوال_جدول[کام_منوال_MAX] = {
        [کام_FAIL]                 = "fail",
        [کام_LENIENT]              = "lenient",
        [کام_REPLACE]              = "replace",
        [کام_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [کام_ISOLATE]              = "isolate",
        [کام_FLUSH]                = "flush",
        [کام_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [کام_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [کام_TRIGGERING]           = "triggering",
        [کام_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_جدول_LOOKUP(کام_منوال_2, کام_منوال);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
ساکن ثابت حرف* ثابت exec_directory_type_جدول[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_جدول_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
ساکن ثابت حرف* ثابت exec_directory_type_symlink_جدول[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_جدول_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_فعال_state_مقصد_glyph(Unit_فعال_State state) {
        ساکن ثابت Glyph map[UNIT_فعال_STATE_MAX] = {
                [UNIT_فعال]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        اگر (state < 0)
                واپس _GLYPH_INVALID;

        أكد(state < UNIT_فعال_STATE_MAX);
        واپس map[state];
}
