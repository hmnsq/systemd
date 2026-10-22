/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "bus-label.h"
#ضمن "glyph-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "unit-def.h"
#ضمن "unit-name.h"

حرف* unit_dbus_لار_from_name(ثابت حرف *name) {
        _cleanup_free_ حرف *e = عدم;

        أكد(name);

        e = bus_label_escape(name);
        إذا (!e)
                أرجع عدم;

        أرجع strjoin("/org/freedesktop/systemd1/unit/", e);
}

صحيح unit_name_from_dbus_لار(ثابت حرف *لار, حرف **name) {
        ثابت حرف *e;
        حرف *n;

        أكد(name);

        e = startswith(لار, "/org/freedesktop/systemd1/unit/");
        إذا (!e)
                أرجع -EINVAL;

        n = bus_label_unescape(e);
        إذا (!n)
                أرجع -ENOMEM;

        *name = n;
        أرجع 0;
}

ثابت حرف* unit_dbus_interface_from_type(UnitType t) {

        ساكن ثابت حرف *ثابت جدول[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_موخه]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_بدلون]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_لار]      = "org.freedesktop.systemd1.Path",
                [UNIT_ټوټه]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        إذا (t < 0)
                أرجع عدم;
        إذا (t >= _UNIT_TYPE_MAX)
                أرجع عدم;

        أرجع جدول[t];
}

ثابت حرف* unit_dbus_interface_from_name(ثابت حرف *name) {
        UnitType t;

        t = unit_name_منزل_type(name);
        إذا (t < 0)
                أرجع عدم;

        أرجع unit_dbus_interface_from_type(t);
}

ثابت حرف* unit_type_منزل_capitalized_string(UnitType t) {
        ثابت حرف *di = unit_dbus_interface_from_type(t);
        إذا (!di)
                أرجع عدم;

        أرجع ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

ساكن ثابت حرف* ثابت unit_type_جدول[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_موخه]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_بدلون]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_لار]      = "path",
        [UNIT_ټوټه]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_جدول_LOOKUP(unit_type, UnitType);

ساكن ثابت حرف* ثابت unit_بارول_state_جدول[UNIT_بارول_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_وموندل]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_جدول_LOOKUP(unit_بارول_state, Unit_بارول_State);

/* Keep in sync with man/unit-states.xml */
ساكن ثابت حرف* ثابت unit_فعال_state_جدول[UNIT_فعال_STATE_MAX] = {
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

ساكن ثابت حرف* ثابت freezer_state_جدول[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_پلار] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_پلار]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_جدول_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
ساكن ثابت FreezerState freezer_state_پای_جدول[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_پلار] = FREEZER_FROZEN_BY_پلار,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_پلار]   = FREEZER_FROZEN_BY_پلار,
};

FreezerState freezer_state_پای(FreezerState state) {
        أكد(state >= 0);
        أكد(state < _FREEZER_STATE_MAX);

        أرجع freezer_state_پای_جدول[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_پای(state);
        إذا (objective == FREEZER_FROZEN_BY_پلار)
                objective = FREEZER_FROZEN;

        أرجع objective;
}

ساكن ثابت حرف* ثابت unit_marker_جدول[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_پیل]   = "needs-start",
};

DEFINE_STRING_جدول_LOOKUP(unit_marker, UnitMarker);

ساكن ثابت حرف* ثابت automount_state_جدول[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_انتظار] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(automount_state, AutomountState);

ساكن ثابت حرف* ثابت device_state_جدول[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_جدول_LOOKUP(device_state, DeviceState);

ساكن ثابت حرف* ثابت mount_state_جدول[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_بشپړ]      = "mounting-done",
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

ساكن ثابت حرف* ثابت لار_state_جدول[لار_STATE_MAX] = {
        [لار_DEAD]    = "dead",
        [لار_انتظار] = "waiting",
        [لار_RUNNING] = "running",
        [لار_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(لار_state, لار_State);

ساكن ثابت حرف* ثابت scope_state_جدول[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_پیل_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_جدول_LOOKUP(scope_state, ScopeState);

ساكن ثابت حرف* ثابت service_state_جدول[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_پیل_PRE]                  = "start-pre",
        [SERVICE_پیل]                      = "start",
        [SERVICE_پیل_POST]                 = "start-post",
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

ساكن ثابت حرف* ثابت ټوټه_state_جدول[ټوټه_STATE_MAX] = {
        [ټوټه_DEAD]   = "dead",
        [ټوټه_فعال] = "active",
};

DEFINE_STRING_جدول_LOOKUP(ټوټه_state, ټوټه_State);

ساكن ثابت حرف* ثابت socket_state_جدول[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_پیل_PRE]        = "start-pre",
        [SOCKET_پیل_OPEN]       = "start-open",
        [SOCKET_پیل_CHOWN]      = "start-chown",
        [SOCKET_پیل_POST]       = "start-post",
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

ساكن ثابت حرف* ثابت بدلون_state_جدول[بدلون_STATE_MAX] = {
        [بدلون_DEAD]                 = "dead",
        [بدلون_ACTIVATING]           = "activating",
        [بدلون_ACTIVATING_بشپړ]      = "activating-done",
        [بدلون_فعال]               = "active",
        [بدلون_DEACTIVATING]         = "deactivating",
        [بدلون_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [بدلون_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [بدلون_FAILED]               = "failed",
        [بدلون_CLEANING]             = "cleaning",
};

DEFINE_STRING_جدول_LOOKUP(بدلون_state, بدلون_State);

ساكن ثابت حرف* ثابت موخه_state_جدول[موخه_STATE_MAX] = {
        [موخه_DEAD]   = "dead",
        [موخه_فعال] = "active",
};

DEFINE_STRING_جدول_LOOKUP(موخه_state, موخه_State);

ساكن ثابت حرف* ثابت timer_state_جدول[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_انتظار] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_جدول_LOOKUP(timer_state, TimerState);

ساكن ثابت حرف* ثابت unit_dependency_جدول[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_غواړي]                  = "Wants",
        [UNIT_BINDS_منزل]               = "BindsTo",
        [UNIT_PART_د]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_د]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_د]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_د]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_د]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_منزل]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_منزل]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_د]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_دننه_ټوټه]               = "InSlice",
        [UNIT_ټوټه_د]               = "SliceOf",
};

DEFINE_STRING_جدول_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        أرجع دننه_سېټ(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_غواړي,
                      UNIT_BINDS_منزل,
                      UNIT_PART_د,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_منزل,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_منزل,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_د);
}

فراغ unit_types_list(فراغ) {
        DUMP_STRING_جدول(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

ساكن ثابت حرف* ثابت notify_access_جدول[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_جدول_LOOKUP(notify_access, NotifyAccess);

ساكن ثابت حرف* ثابت دنده_ډېرپېښ_جدول[دنده_ډېرپېښ_MAX] = {
        [دنده_FAIL]                 = "fail",
        [دنده_LENIENT]              = "lenient",
        [دنده_REPLACE]              = "replace",
        [دنده_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [دنده_ISOLATE]              = "isolate",
        [دنده_FLUSH]                = "flush",
        [دنده_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [دنده_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [دنده_TRIGGERING]           = "triggering",
        [دنده_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_جدول_LOOKUP(دنده_ډېرپېښ_2, دنده_ډېرپېښ);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
ساكن ثابت حرف* ثابت exec_directory_type_جدول[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_جدول_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
ساكن ثابت حرف* ثابت exec_directory_type_symlink_جدول[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_جدول_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_فعال_state_منزل_glyph(Unit_فعال_State state) {
        ساكن ثابت Glyph map[UNIT_فعال_STATE_MAX] = {
                [UNIT_فعال]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        إذا (state < 0)
                أرجع _GLYPH_INVALID;

        أكد(state < UNIT_فعال_STATE_MAX);
        أرجع map[state];
}
