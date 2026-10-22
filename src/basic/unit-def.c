/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "bus-label.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-def.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unit-name.h"

ਅੱਖਰ* unit_dbus_ਰਸਤਾ_from_name(ਅਚਲ ਅੱਖਰ *name) {
        _cleanup_free_ ਅੱਖਰ *e = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        e = bus_label_escape(name);
        ਜੇ (!e)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ strjoin("/org/freedesktop/systemd1/unit/", e);
}

ਪੂਰਨ unit_name_from_dbus_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **name) {
        ਅਚਲ ਅੱਖਰ *e;
        ਅੱਖਰ *n;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        e = startswith(ਰਸਤਾ, "/org/freedesktop/systemd1/unit/");
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;

        n = bus_label_unescape(e);
        ਜੇ (!n)
                ਵਾਪਸ -ENOMEM;

        *name = n;
        ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ* unit_dbus_interface_from_type(UnitType t) {

        ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ ਸਾਰਣੀ[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_ਨਿਸ਼ਾਨਾ]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_ਅਦਲਾ_ਬਦਲੀ]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_ਰਸਤਾ]      = "org.freedesktop.systemd1.Path",
                [UNIT_ਟੁਕੜਾ]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        ਜੇ (t < 0)
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ (t >= _UNIT_TYPE_MAX)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ਸਾਰਣੀ[t];
}

ਅਚਲ ਅੱਖਰ* unit_dbus_interface_from_name(ਅਚਲ ਅੱਖਰ *name) {
        UnitType t;

        t = unit_name_ਮੰਜ਼ਿਲ_type(name);
        ਜੇ (t < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ unit_dbus_interface_from_type(t);
}

ਅਚਲ ਅੱਖਰ* unit_type_ਮੰਜ਼ਿਲ_capitalized_string(UnitType t) {
        ਅਚਲ ਅੱਖਰ *di = unit_dbus_interface_from_type(t);
        ਜੇ (!di)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ unit_type_ਸਾਰਣੀ[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_ਨਿਸ਼ਾਨਾ]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_ਅਦਲਾ_ਬਦਲੀ]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_ਰਸਤਾ]      = "path",
        [UNIT_ਟੁਕੜਾ]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(unit_type, UnitType);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ unit_ਲੱਦੋ_state_ਸਾਰਣੀ[UNIT_ਲੱਦੋ_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_ਮਿਲਿਆ]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(unit_ਲੱਦੋ_state, Unit_ਲੱਦੋ_State);

/* Keep in sync with man/unit-states.xml */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ unit_ਸਰਗਰਮ_state_ਸਾਰਣੀ[UNIT_ਸਰਗਰਮ_STATE_MAX] = {
        [UNIT_ਸਰਗਰਮ]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(unit_ਸਰਗਰਮ_state, Unit_ਸਰਗਰਮ_State);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ freezer_state_ਸਾਰਣੀ[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_ਮਾਪਾ] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_ਮਾਪਾ]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
ਸਥਿਰ ਅਚਲ FreezerState freezer_state_ਅੰਤ_ਸਾਰਣੀ[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_ਮਾਪਾ] = FREEZER_FROZEN_BY_ਮਾਪਾ,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_ਮਾਪਾ]   = FREEZER_FROZEN_BY_ਮਾਪਾ,
};

FreezerState freezer_state_ਅੰਤ(FreezerState state) {
        ਪ੍ਰਤਿਜ੍ਞਾ(state >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(state < _FREEZER_STATE_MAX);

        ਵਾਪਸ freezer_state_ਅੰਤ_ਸਾਰਣੀ[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_ਅੰਤ(state);
        ਜੇ (objective == FREEZER_FROZEN_BY_ਮਾਪਾ)
                objective = FREEZER_FROZEN;

        ਵਾਪਸ objective;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ unit_marker_ਸਾਰਣੀ[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_ਸ਼ੁਰੂ]   = "needs-start",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(unit_marker, UnitMarker);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ automount_state_ਸਾਰਣੀ[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_ਉਡੀਕ] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(automount_state, AutomountState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ device_state_ਸਾਰਣੀ[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(device_state, DeviceState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ mount_state_ਸਾਰਣੀ[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_ਪੂਰਾ]      = "mounting-done",
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(mount_state, MountState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ਰਸਤਾ_state_ਸਾਰਣੀ[ਰਸਤਾ_STATE_MAX] = {
        [ਰਸਤਾ_DEAD]    = "dead",
        [ਰਸਤਾ_ਉਡੀਕ] = "waiting",
        [ਰਸਤਾ_RUNNING] = "running",
        [ਰਸਤਾ_FAILED]  = "failed",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(ਰਸਤਾ_state, ਰਸਤਾ_State);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ scope_state_ਸਾਰਣੀ[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_ਸ਼ੁਰੂ_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(scope_state, ScopeState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ service_state_ਸਾਰਣੀ[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_ਸ਼ੁਰੂ_PRE]                  = "start-pre",
        [SERVICE_ਸ਼ੁਰੂ]                      = "start",
        [SERVICE_ਸ਼ੁਰੂ_POST]                 = "start-post",
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(service_state, ServiceState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ਟੁਕੜਾ_state_ਸਾਰਣੀ[ਟੁਕੜਾ_STATE_MAX] = {
        [ਟੁਕੜਾ_DEAD]   = "dead",
        [ਟੁਕੜਾ_ਸਰਗਰਮ] = "active",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(ਟੁਕੜਾ_state, ਟੁਕੜਾ_State);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ socket_state_ਸਾਰਣੀ[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_ਸ਼ੁਰੂ_PRE]        = "start-pre",
        [SOCKET_ਸ਼ੁਰੂ_OPEN]       = "start-open",
        [SOCKET_ਸ਼ੁਰੂ_CHOWN]      = "start-chown",
        [SOCKET_ਸ਼ੁਰੂ_POST]       = "start-post",
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

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(socket_state, SocketState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ਅਦਲਾ_ਬਦਲੀ_state_ਸਾਰਣੀ[ਅਦਲਾ_ਬਦਲੀ_STATE_MAX] = {
        [ਅਦਲਾ_ਬਦਲੀ_DEAD]                 = "dead",
        [ਅਦਲਾ_ਬਦਲੀ_ACTIVATING]           = "activating",
        [ਅਦਲਾ_ਬਦਲੀ_ACTIVATING_ਪੂਰਾ]      = "activating-done",
        [ਅਦਲਾ_ਬਦਲੀ_ਸਰਗਰਮ]               = "active",
        [ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING]         = "deactivating",
        [ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [ਅਦਲਾ_ਬਦਲੀ_FAILED]               = "failed",
        [ਅਦਲਾ_ਬਦਲੀ_CLEANING]             = "cleaning",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(ਅਦਲਾ_ਬਦਲੀ_state, ਅਦਲਾ_ਬਦਲੀ_State);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ਨਿਸ਼ਾਨਾ_state_ਸਾਰਣੀ[ਨਿਸ਼ਾਨਾ_STATE_MAX] = {
        [ਨਿਸ਼ਾਨਾ_DEAD]   = "dead",
        [ਨਿਸ਼ਾਨਾ_ਸਰਗਰਮ] = "active",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(ਨਿਸ਼ਾਨਾ_state, ਨਿਸ਼ਾਨਾ_State);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ timer_state_ਸਾਰਣੀ[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_ਉਡੀਕ] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(timer_state, TimerState);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ unit_dependency_ਸਾਰਣੀ[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_ਚਾਹੁੰਦਾ]                  = "Wants",
        [UNIT_BINDS_ਮੰਜ਼ਿਲ]               = "BindsTo",
        [UNIT_PART_ਦਾ]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_ਦਾ]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_ਦਾ]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_ਦਾ]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_ਦਾ]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_ਮੰਜ਼ਿਲ]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_ਮੰਜ਼ਿਲ]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_ਦਾ]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_ਵਿੱਚ_ਟੁਕੜਾ]               = "InSlice",
        [UNIT_ਟੁਕੜਾ_ਦਾ]               = "SliceOf",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_ਚਾਹੁੰਦਾ,
                      UNIT_BINDS_ਮੰਜ਼ਿਲ,
                      UNIT_PART_ਦਾ,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_ਮੰਜ਼ਿਲ,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_ਮੰਜ਼ਿਲ,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_ਦਾ);
}

ਖਾਲੀ unit_types_list(ਖਾਲੀ) {
        DUMP_STRING_ਸਾਰਣੀ(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ notify_access_ਸਾਰਣੀ[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(notify_access, NotifyAccess);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ ਕੰਮ_ਬਹੁਲਕ_ਸਾਰਣੀ[ਕੰਮ_ਬਹੁਲਕ_MAX] = {
        [ਕੰਮ_FAIL]                 = "fail",
        [ਕੰਮ_LENIENT]              = "lenient",
        [ਕੰਮ_REPLACE]              = "replace",
        [ਕੰਮ_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [ਕੰਮ_ISOLATE]              = "isolate",
        [ਕੰਮ_FLUSH]                = "flush",
        [ਕੰਮ_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [ਕੰਮ_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [ਕੰਮ_TRIGGERING]           = "triggering",
        [ਕੰਮ_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(ਕੰਮ_ਬਹੁਲਕ_2, ਕੰਮ_ਬਹੁਲਕ);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ exec_directory_type_ਸਾਰਣੀ[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ exec_directory_type_symlink_ਸਾਰਣੀ[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_ਸਰਗਰਮ_state_ਮੰਜ਼ਿਲ_glyph(Unit_ਸਰਗਰਮ_State state) {
        ਸਥਿਰ ਅਚਲ Glyph map[UNIT_ਸਰਗਰਮ_STATE_MAX] = {
                [UNIT_ਸਰਗਰਮ]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        ਜੇ (state < 0)
                ਵਾਪਸ _GLYPH_INVALID;

        ਪ੍ਰਤਿਜ੍ਞਾ(state < UNIT_ਸਰਗਰਮ_STATE_MAX);
        ਵਾਪਸ map[state];
}
