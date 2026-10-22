/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "bus-label.h"
#ಅನ್ತರ್ಭಾವ "glyph-util.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unit-def.h"
#ಅನ್ತರ್ಭಾವ "unit-name.h"

ಅಕ್ಷರ* unit_dbus_ಮಾರ್ಗ_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        _cleanup_free_ ಅಕ್ಷರ *e = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(name);

        e = bus_label_escape(name);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ strjoin("/org/freedesktop/systemd1/unit/", e);
}

ಪೂರ್ಣಾಂಕ unit_name_from_dbus_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **name) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಅಕ್ಷರ *n;

        ಪ್ರತಿಜ್ಞಾ(name);

        e = startswith(ಮಾರ್ಗ, "/org/freedesktop/systemd1/unit/");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;

        n = bus_label_unescape(e);
        ಆದರೆ (!n)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *name = n;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_dbus_interface_from_type(UnitType t) {

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ ಕೋಷ್ಟಕ[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_ಗುರಿ]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_ಅದಲುಬದಲು]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_ಮಾರ್ಗ]      = "org.freedesktop.systemd1.Path",
                [UNIT_ತುಂಡು]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        ಆದರೆ (t < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಆದರೆ (t >= _UNIT_TYPE_MAX)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ಕೋಷ್ಟಕ[t];
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_dbus_interface_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        UnitType t;

        t = unit_name_ಗಮ್ಯ_type(name);
        ಆದರೆ (t < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ unit_dbus_interface_from_type(t);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_type_ಗಮ್ಯ_capitalized_string(UnitType t) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *di = unit_dbus_interface_from_type(t);
        ಆದರೆ (!di)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ unit_type_ಕೋಷ್ಟಕ[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_ಗುರಿ]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_ಅದಲುಬದಲು]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_ಮಾರ್ಗ]      = "path",
        [UNIT_ತುಂಡು]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_type, UnitType);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ unit_ತುಂಬು_state_ಕೋಷ್ಟಕ[UNIT_ತುಂಬು_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_ಸಿಕ್ಕಿತು]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_ತುಂಬು_state, Unit_ತುಂಬು_State);

/* Keep in sync with man/unit-states.xml */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ unit_ಸಕ್ರಿಯ_state_ಕೋಷ್ಟಕ[UNIT_ಸಕ್ರಿಯ_STATE_MAX] = {
        [UNIT_ಸಕ್ರಿಯ]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_ಸಕ್ರಿಯ_state, Unit_ಸಕ್ರಿಯ_State);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ freezer_state_ಕೋಷ್ಟಕ[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_ಪೋಷಕ] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_ಪೋಷಕ]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ FreezerState freezer_state_ಮುಕ್ತಾಯ_ಕೋಷ್ಟಕ[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_ಪೋಷಕ] = FREEZER_FROZEN_BY_ಪೋಷಕ,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_ಪೋಷಕ]   = FREEZER_FROZEN_BY_ಪೋಷಕ,
};

FreezerState freezer_state_ಮುಕ್ತಾಯ(FreezerState state) {
        ಪ್ರತಿಜ್ಞಾ(state >= 0);
        ಪ್ರತಿಜ್ಞಾ(state < _FREEZER_STATE_MAX);

        ಹಿಂತಿರುಗಿ freezer_state_ಮುಕ್ತಾಯ_ಕೋಷ್ಟಕ[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_ಮುಕ್ತಾಯ(state);
        ಆದರೆ (objective == FREEZER_FROZEN_BY_ಪೋಷಕ)
                objective = FREEZER_FROZEN;

        ಹಿಂತಿರುಗಿ objective;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ unit_marker_ಕೋಷ್ಟಕ[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_ಆರಂಭ]   = "needs-start",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_marker, UnitMarker);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ automount_state_ಕೋಷ್ಟಕ[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_ಕಾಯುವಿಕೆ] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(automount_state, AutomountState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ device_state_ಕೋಷ್ಟಕ[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(device_state, DeviceState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ mount_state_ಕೋಷ್ಟಕ[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_ಮುಗಿದ]      = "mounting-done",
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(mount_state, MountState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಮಾರ್ಗ_state_ಕೋಷ್ಟಕ[ಮಾರ್ಗ_STATE_MAX] = {
        [ಮಾರ್ಗ_DEAD]    = "dead",
        [ಮಾರ್ಗ_ಕಾಯುವಿಕೆ] = "waiting",
        [ಮಾರ್ಗ_RUNNING] = "running",
        [ಮಾರ್ಗ_FAILED]  = "failed",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಮಾರ್ಗ_state, ಮಾರ್ಗ_State);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ scope_state_ಕೋಷ್ಟಕ[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_ಆರಂಭ_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(scope_state, ScopeState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ service_state_ಕೋಷ್ಟಕ[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_ಆರಂಭ_PRE]                  = "start-pre",
        [SERVICE_ಆರಂಭ]                      = "start",
        [SERVICE_ಆರಂಭ_POST]                 = "start-post",
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(service_state, ServiceState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ತುಂಡು_state_ಕೋಷ್ಟಕ[ತುಂಡು_STATE_MAX] = {
        [ತುಂಡು_DEAD]   = "dead",
        [ತುಂಡು_ಸಕ್ರಿಯ] = "active",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ತುಂಡು_state, ತುಂಡು_State);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ socket_state_ಕೋಷ್ಟಕ[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_ಆರಂಭ_PRE]        = "start-pre",
        [SOCKET_ಆರಂಭ_OPEN]       = "start-open",
        [SOCKET_ಆರಂಭ_CHOWN]      = "start-chown",
        [SOCKET_ಆರಂಭ_POST]       = "start-post",
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

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(socket_state, SocketState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಅದಲುಬದಲು_state_ಕೋಷ್ಟಕ[ಅದಲುಬದಲು_STATE_MAX] = {
        [ಅದಲುಬದಲು_DEAD]                 = "dead",
        [ಅದಲುಬದಲು_ACTIVATING]           = "activating",
        [ಅದಲುಬದಲು_ACTIVATING_ಮುಗಿದ]      = "activating-done",
        [ಅದಲುಬದಲು_ಸಕ್ರಿಯ]               = "active",
        [ಅದಲುಬದಲು_DEACTIVATING]         = "deactivating",
        [ಅದಲುಬದಲು_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [ಅದಲುಬದಲು_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [ಅದಲುಬದಲು_FAILED]               = "failed",
        [ಅದಲುಬದಲು_CLEANING]             = "cleaning",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಅದಲುಬದಲು_state, ಅದಲುಬದಲು_State);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಗುರಿ_state_ಕೋಷ್ಟಕ[ಗುರಿ_STATE_MAX] = {
        [ಗುರಿ_DEAD]   = "dead",
        [ಗುರಿ_ಸಕ್ರಿಯ] = "active",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಗುರಿ_state, ಗುರಿ_State);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ timer_state_ಕೋಷ್ಟಕ[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_ಕಾಯುವಿಕೆ] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(timer_state, TimerState);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ unit_dependency_ಕೋಷ್ಟಕ[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_ಬಯಸುತ್ತದೆ]                  = "Wants",
        [UNIT_BINDS_ಗಮ್ಯ]               = "BindsTo",
        [UNIT_PART_ದ]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_ದ]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_ದ]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_ದ]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_ದ]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_ಗಮ್ಯ]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_ಗಮ್ಯ]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_ದ]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_ಒಳ_ತುಂಡು]               = "InSlice",
        [UNIT_ತುಂಡು_ದ]               = "SliceOf",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_ಬಯಸುತ್ತದೆ,
                      UNIT_BINDS_ಗಮ್ಯ,
                      UNIT_PART_ದ,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_ಗಮ್ಯ,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_ಗಮ್ಯ,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_ದ);
}

ಶೂನ್ಯ unit_types_list(ಶೂನ್ಯ) {
        DUMP_STRING_ಕೋಷ್ಟಕ(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ notify_access_ಕೋಷ್ಟಕ[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(notify_access, NotifyAccess);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ ಕೆಲಸ_ಬಹುಲಕ_ಕೋಷ್ಟಕ[ಕೆಲಸ_ಬಹುಲಕ_MAX] = {
        [ಕೆಲಸ_FAIL]                 = "fail",
        [ಕೆಲಸ_LENIENT]              = "lenient",
        [ಕೆಲಸ_REPLACE]              = "replace",
        [ಕೆಲಸ_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [ಕೆಲಸ_ISOLATE]              = "isolate",
        [ಕೆಲಸ_FLUSH]                = "flush",
        [ಕೆಲಸ_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [ಕೆಲಸ_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [ಕೆಲಸ_TRIGGERING]           = "triggering",
        [ಕೆಲಸ_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಕೆಲಸ_ಬಹುಲಕ_2, ಕೆಲಸ_ಬಹುಲಕ);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ exec_directory_type_ಕೋಷ್ಟಕ[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ exec_directory_type_symlink_ಕೋಷ್ಟಕ[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_ಸಕ್ರಿಯ_state_ಗಮ್ಯ_glyph(Unit_ಸಕ್ರಿಯ_State state) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Glyph map[UNIT_ಸಕ್ರಿಯ_STATE_MAX] = {
                [UNIT_ಸಕ್ರಿಯ]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        ಆದರೆ (state < 0)
                ಹಿಂತಿರುಗಿ _GLYPH_INVALID;

        ಪ್ರತಿಜ್ಞಾ(state < UNIT_ಸಕ್ರಿಯ_STATE_MAX);
        ಹಿಂತಿರುಗಿ map[state];
}
