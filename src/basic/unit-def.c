/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "bus-label.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unit-def.h"
#अन्तर्भाव "unit-name.h"

वर्ण* unit_dbus_मार्गः_from_name(ध्रुव वर्ण *name) {
        _cleanup_free_ वर्ण *e = रिक्त;

        प्रतिज्ञा(name);

        e = bus_label_escape(name);
        यदि (!e)
                प्रत्यागम रिक्त;

        प्रत्यागम strjoin("/org/freedesktop/systemd1/unit/", e);
}

पूर्णाङ्क unit_name_from_dbus_मार्गः(ध्रुव वर्ण *मार्गः, वर्ण **name) {
        ध्रुव वर्ण *e;
        वर्ण *n;

        प्रतिज्ञा(name);

        e = startswith(मार्गः, "/org/freedesktop/systemd1/unit/");
        यदि (!e)
                प्रत्यागम -EINVAL;

        n = bus_label_unescape(e);
        यदि (!n)
                प्रत्यागम -ENOMEM;

        *name = n;
        प्रत्यागम 0;
}

ध्रुव वर्ण* unit_dbus_interface_from_type(UnitType t) {

        स्थिर ध्रुव वर्ण *ध्रुव सारणी[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_TARGET]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_SWAP]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_मार्गः]      = "org.freedesktop.systemd1.Path",
                [UNIT_खण्डिका]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        यदि (t < 0)
                प्रत्यागम रिक्त;
        यदि (t >= _UNIT_TYPE_MAX)
                प्रत्यागम रिक्त;

        प्रत्यागम सारणी[t];
}

ध्रुव वर्ण* unit_dbus_interface_from_name(ध्रुव वर्ण *name) {
        UnitType t;

        t = unit_name_गन्तव्यम्_type(name);
        यदि (t < 0)
                प्रत्यागम रिक्त;

        प्रत्यागम unit_dbus_interface_from_type(t);
}

ध्रुव वर्ण* unit_type_गन्तव्यम्_capitalized_string(UnitType t) {
        ध्रुव वर्ण *di = unit_dbus_interface_from_type(t);
        यदि (!di)
                प्रत्यागम रिक्त;

        प्रत्यागम ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

स्थिर ध्रुव वर्ण* ध्रुव unit_type_सारणी[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_TARGET]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_SWAP]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_मार्गः]      = "path",
        [UNIT_खण्डिका]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_सारणी_LOOKUP(unit_type, UnitType);

स्थिर ध्रुव वर्ण* ध्रुव unit_आरोपय_state_सारणी[UNIT_आरोपय_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_प्राप्तम्]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_सारणी_LOOKUP(unit_आरोपय_state, Unit_आरोपय_State);

/* Keep in sync with man/unit-states.xml */
स्थिर ध्रुव वर्ण* ध्रुव unit_सक्रिय_state_सारणी[UNIT_सक्रिय_STATE_MAX] = {
        [UNIT_सक्रिय]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_सारणी_LOOKUP(unit_सक्रिय_state, Unit_सक्रिय_State);

स्थिर ध्रुव वर्ण* ध्रुव freezer_state_सारणी[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_PARENT] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_PARENT]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_सारणी_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
स्थिर ध्रुव FreezerState freezer_state_finish_सारणी[_FREEZER_STATE_MAX] = {
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

        प्रत्यागम freezer_state_finish_सारणी[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_finish(state);
        यदि (objective == FREEZER_FROZEN_BY_PARENT)
                objective = FREEZER_FROZEN;

        प्रत्यागम objective;
}

स्थिर ध्रुव वर्ण* ध्रुव unit_marker_सारणी[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_START]   = "needs-start",
};

DEFINE_STRING_सारणी_LOOKUP(unit_marker, UnitMarker);

स्थिर ध्रुव वर्ण* ध्रुव automount_state_सारणी[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_प्रतीक्षा] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_सारणी_LOOKUP(automount_state, AutomountState);

स्थिर ध्रुव वर्ण* ध्रुव device_state_सारणी[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_सारणी_LOOKUP(device_state, DeviceState);

स्थिर ध्रुव वर्ण* ध्रुव mount_state_सारणी[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_समाप्तम्]      = "mounting-done",
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

DEFINE_STRING_सारणी_LOOKUP(mount_state, MountState);

स्थिर ध्रुव वर्ण* ध्रुव मार्गः_state_सारणी[मार्गः_STATE_MAX] = {
        [मार्गः_DEAD]    = "dead",
        [मार्गः_प्रतीक्षा] = "waiting",
        [मार्गः_RUNNING] = "running",
        [मार्गः_FAILED]  = "failed",
};

DEFINE_STRING_सारणी_LOOKUP(मार्गः_state, मार्गः_State);

स्थिर ध्रुव वर्ण* ध्रुव scope_state_सारणी[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_START_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_सारणी_LOOKUP(scope_state, ScopeState);

स्थिर ध्रुव वर्ण* ध्रुव service_state_सारणी[_SERVICE_STATE_MAX] = {
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

DEFINE_STRING_सारणी_LOOKUP(service_state, ServiceState);

स्थिर ध्रुव वर्ण* ध्रुव खण्डिका_state_सारणी[खण्डिका_STATE_MAX] = {
        [खण्डिका_DEAD]   = "dead",
        [खण्डिका_सक्रिय] = "active",
};

DEFINE_STRING_सारणी_LOOKUP(खण्डिका_state, खण्डिका_State);

स्थिर ध्रुव वर्ण* ध्रुव socket_state_सारणी[_SOCKET_STATE_MAX] = {
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

DEFINE_STRING_सारणी_LOOKUP(socket_state, SocketState);

स्थिर ध्रुव वर्ण* ध्रुव swap_state_सारणी[_SWAP_STATE_MAX] = {
        [SWAP_DEAD]                 = "dead",
        [SWAP_ACTIVATING]           = "activating",
        [SWAP_ACTIVATING_समाप्तम्]      = "activating-done",
        [SWAP_सक्रिय]               = "active",
        [SWAP_DEACTIVATING]         = "deactivating",
        [SWAP_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [SWAP_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [SWAP_FAILED]               = "failed",
        [SWAP_CLEANING]             = "cleaning",
};

DEFINE_STRING_सारणी_LOOKUP(swap_state, SwapState);

स्थिर ध्रुव वर्ण* ध्रुव target_state_सारणी[_TARGET_STATE_MAX] = {
        [TARGET_DEAD]   = "dead",
        [TARGET_सक्रिय] = "active",
};

DEFINE_STRING_सारणी_LOOKUP(target_state, TargetState);

स्थिर ध्रुव वर्ण* ध्रुव timer_state_सारणी[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_प्रतीक्षा] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_सारणी_LOOKUP(timer_state, TimerState);

स्थिर ध्रुव वर्ण* ध्रुव unit_dependency_सारणी[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_इच्छति]                  = "Wants",
        [UNIT_BINDS_गन्तव्यम्]               = "BindsTo",
        [UNIT_PART_सम्बन्धि]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_सम्बन्धि]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_सम्बन्धि]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_सम्बन्धि]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_सम्बन्धि]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_गन्तव्यम्]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_गन्तव्यम्]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_सम्बन्धि]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_अन्तर्_खण्डिका]               = "InSlice",
        [UNIT_खण्डिका_सम्बन्धि]               = "SliceOf",
};

DEFINE_STRING_सारणी_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        प्रत्यागम अन्तर्_समुच्चयः(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_इच्छति,
                      UNIT_BINDS_गन्तव्यम्,
                      UNIT_PART_सम्बन्धि,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_गन्तव्यम्,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_गन्तव्यम्,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_सम्बन्धि);
}

शून्य unit_types_list(शून्य) {
        DUMP_STRING_सारणी(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

स्थिर ध्रुव वर्ण* ध्रुव notify_access_सारणी[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_सारणी_LOOKUP(notify_access, NotifyAccess);

स्थिर ध्रुव वर्ण* ध्रुव job_बहुलकम्_सारणी[JOB_बहुलकम्_MAX] = {
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

DEFINE_STRING_सारणी_LOOKUP(job_बहुलकम्, Job_बहुलकम्);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
स्थिर ध्रुव वर्ण* ध्रुव exec_directory_type_सारणी[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_सारणी_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
स्थिर ध्रुव वर्ण* ध्रुव exec_directory_type_symlink_सारणी[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_सारणी_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_सक्रिय_state_गन्तव्यम्_glyph(Unit_सक्रिय_State state) {
        स्थिर ध्रुव Glyph map[UNIT_सक्रिय_STATE_MAX] = {
                [UNIT_सक्रिय]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        यदि (state < 0)
                प्रत्यागम _GLYPH_INVALID;

        प्रतिज्ञा(state < UNIT_सक्रिय_STATE_MAX);
        प्रत्यागम map[state];
}
