/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "bus-label.h"
#అన్తర్భావ "glyph-util.h"
#అన్తర్భావ "string-table.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unit-def.h"
#అన్తర్భావ "unit-name.h"

అక్షరం* unit_dbus_మార్గం_from_name(స్థిరరాశి అక్షరం *name) {
        _cleanup_free_ అక్షరం *e = ఖాళీ;

        ప్రతిజ్ఞా(name);

        e = bus_label_escape(name);
        ఒకవేళ (!e)
                తిరిగి ఖాళీ;

        తిరిగి strjoin("/org/freedesktop/systemd1/unit/", e);
}

పూర్ణాంకం unit_name_from_dbus_మార్గం(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **name) {
        స్థిరరాశి అక్షరం *e;
        అక్షరం *n;

        ప్రతిజ్ఞా(name);

        e = startswith(మార్గం, "/org/freedesktop/systemd1/unit/");
        ఒకవేళ (!e)
                తిరిగి -EINVAL;

        n = bus_label_unescape(e);
        ఒకవేళ (!n)
                తిరిగి -ENOMEM;

        *name = n;
        తిరిగి 0;
}

స్థిరరాశి అక్షరం* unit_dbus_interface_from_type(UnitType t) {

        స్థిర స్థిరరాశి అక్షరం *స్థిరరాశి పట్టిక[_UNIT_TYPE_MAX] = {
                [UNIT_SERVICE]   = "org.freedesktop.systemd1.Service",
                [UNIT_SOCKET]    = "org.freedesktop.systemd1.Socket",
                [UNIT_లక్ష్యం]    = "org.freedesktop.systemd1.Target",
                [UNIT_DEVICE]    = "org.freedesktop.systemd1.Device",
                [UNIT_MOUNT]     = "org.freedesktop.systemd1.Mount",
                [UNIT_AUTOMOUNT] = "org.freedesktop.systemd1.Automount",
                [UNIT_మార్పిడి]      = "org.freedesktop.systemd1.Swap",
                [UNIT_TIMER]     = "org.freedesktop.systemd1.Timer",
                [UNIT_మార్గం]      = "org.freedesktop.systemd1.Path",
                [UNIT_ముక్క]     = "org.freedesktop.systemd1.Slice",
                [UNIT_SCOPE]     = "org.freedesktop.systemd1.Scope",
        };

        ఒకవేళ (t < 0)
                తిరిగి ఖాళీ;
        ఒకవేళ (t >= _UNIT_TYPE_MAX)
                తిరిగి ఖాళీ;

        తిరిగి పట్టిక[t];
}

స్థిరరాశి అక్షరం* unit_dbus_interface_from_name(స్థిరరాశి అక్షరం *name) {
        UnitType t;

        t = unit_name_గమ్యం_type(name);
        ఒకవేళ (t < 0)
                తిరిగి ఖాళీ;

        తిరిగి unit_dbus_interface_from_type(t);
}

స్థిరరాశి అక్షరం* unit_type_గమ్యం_capitalized_string(UnitType t) {
        స్థిరరాశి అక్షరం *di = unit_dbus_interface_from_type(t);
        ఒకవేళ (!di)
                తిరిగి ఖాళీ;

        తిరిగి ASSERT_PTR(startswith(di, "org.freedesktop.systemd1."));
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి unit_type_పట్టిక[_UNIT_TYPE_MAX] = {
        [UNIT_SERVICE]   = "service",
        [UNIT_SOCKET]    = "socket",
        [UNIT_లక్ష్యం]    = "target",
        [UNIT_DEVICE]    = "device",
        [UNIT_MOUNT]     = "mount",
        [UNIT_AUTOMOUNT] = "automount",
        [UNIT_మార్పిడి]      = "swap",
        [UNIT_TIMER]     = "timer",
        [UNIT_మార్గం]      = "path",
        [UNIT_ముక్క]     = "slice",
        [UNIT_SCOPE]     = "scope",
};

DEFINE_STRING_పట్టిక_LOOKUP(unit_type, UnitType);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి unit_ఎక్కించు_state_పట్టిక[UNIT_ఎక్కించు_STATE_MAX] = {
        [UNIT_STUB]        = "stub",
        [UNIT_LOADED]      = "loaded",
        [UNIT_NOT_దొరికింది]   = "not-found",
        [UNIT_BAD_SETTING] = "bad-setting",
        [UNIT_ERROR]       = "error",
        [UNIT_MERGED]      = "merged",
        [UNIT_MASKED]      = "masked",
};

DEFINE_STRING_పట్టిక_LOOKUP(unit_ఎక్కించు_state, Unit_ఎక్కించు_State);

/* Keep in sync with man/unit-states.xml */
స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి unit_చురుకైన_state_పట్టిక[UNIT_చురుకైన_STATE_MAX] = {
        [UNIT_చురుకైన]       = "active",
        [UNIT_RELOADING]    = "reloading",
        [UNIT_INACTIVE]     = "inactive",
        [UNIT_FAILED]       = "failed",
        [UNIT_ACTIVATING]   = "activating",
        [UNIT_DEACTIVATING] = "deactivating",
        [UNIT_MAINTENANCE]  = "maintenance",
        [UNIT_REFRESHING]   = "refreshing",
};

DEFINE_STRING_పట్టిక_LOOKUP(unit_చురుకైన_state, Unit_చురుకైన_State);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి freezer_state_పట్టిక[_FREEZER_STATE_MAX] = {
        [FREEZER_RUNNING]            = "running",
        [FREEZER_FREEZING]           = "freezing",
        [FREEZER_FREEZING_BY_మాతృ] = "freezing-by-parent",
        [FREEZER_FROZEN]             = "frozen",
        [FREEZER_FROZEN_BY_మాతృ]   = "frozen-by-parent",
        [FREEZER_THAWING]            = "thawing",
};

DEFINE_STRING_పట్టిక_LOOKUP(freezer_state, FreezerState);

/* Maps in-progress freezer states to the corresponding finished state */
స్థిర స్థిరరాశి FreezerState freezer_state_ముగింపు_పట్టిక[_FREEZER_STATE_MAX] = {
        [FREEZER_FREEZING]           = FREEZER_FROZEN,
        [FREEZER_FREEZING_BY_మాతృ] = FREEZER_FROZEN_BY_మాతృ,
        [FREEZER_THAWING]            = FREEZER_RUNNING,

        /* Finished states trivially map to themselves */
        [FREEZER_RUNNING]            = FREEZER_RUNNING,
        [FREEZER_FROZEN]             = FREEZER_FROZEN,
        [FREEZER_FROZEN_BY_మాతృ]   = FREEZER_FROZEN_BY_మాతృ,
};

FreezerState freezer_state_ముగింపు(FreezerState state) {
        ప్రతిజ్ఞా(state >= 0);
        ప్రతిజ్ఞా(state < _FREEZER_STATE_MAX);

        తిరిగి freezer_state_ముగింపు_పట్టిక[state];
}

FreezerState freezer_state_objective(FreezerState state) {
        FreezerState objective;

        objective = freezer_state_ముగింపు(state);
        ఒకవేళ (objective == FREEZER_FROZEN_BY_మాతృ)
                objective = FREEZER_FROZEN;

        తిరిగి objective;
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి unit_marker_పట్టిక[_UNIT_MARKER_MAX] = {
        [UNIT_MARKER_NEEDS_RELOAD]  = "needs-reload",
        [UNIT_MARKER_NEEDS_RESTART] = "needs-restart",
        [UNIT_MARKER_NEEDS_STOP]    = "needs-stop",
        [UNIT_MARKER_NEEDS_ప్రారంభం]   = "needs-start",
};

DEFINE_STRING_పట్టిక_LOOKUP(unit_marker, UnitMarker);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి automount_state_పట్టిక[_AUTOMOUNT_STATE_MAX] = {
        [AUTOMOUNT_DEAD]    = "dead",
        [AUTOMOUNT_నిరీక్షణ] = "waiting",
        [AUTOMOUNT_RUNNING] = "running",
        [AUTOMOUNT_FAILED]  = "failed",
};

DEFINE_STRING_పట్టిక_LOOKUP(automount_state, AutomountState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి device_state_పట్టిక[_DEVICE_STATE_MAX] = {
        [DEVICE_DEAD]      = "dead",
        [DEVICE_TENTATIVE] = "tentative",
        [DEVICE_PLUGGED]   = "plugged",
};

DEFINE_STRING_పట్టిక_LOOKUP(device_state, DeviceState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి mount_state_పట్టిక[_MOUNT_STATE_MAX] = {
        [MOUNT_DEAD]               = "dead",
        [MOUNT_MOUNTING]           = "mounting",
        [MOUNT_MOUNTING_పూర్తి]      = "mounting-done",
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

DEFINE_STRING_పట్టిక_LOOKUP(mount_state, MountState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి మార్గం_state_పట్టిక[మార్గం_STATE_MAX] = {
        [మార్గం_DEAD]    = "dead",
        [మార్గం_నిరీక్షణ] = "waiting",
        [మార్గం_RUNNING] = "running",
        [మార్గం_FAILED]  = "failed",
};

DEFINE_STRING_పట్టిక_LOOKUP(మార్గం_state, మార్గం_State);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి scope_state_పట్టిక[_SCOPE_STATE_MAX] = {
        [SCOPE_DEAD]         = "dead",
        [SCOPE_ప్రారంభం_CHOWN]  = "start-chown",
        [SCOPE_RUNNING]      = "running",
        [SCOPE_ABANDONED]    = "abandoned",
        [SCOPE_STOP_SIGTERM] = "stop-sigterm",
        [SCOPE_STOP_SIGKILL] = "stop-sigkill",
        [SCOPE_FAILED]       = "failed",
};

DEFINE_STRING_పట్టిక_LOOKUP(scope_state, ScopeState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి service_state_పట్టిక[_SERVICE_STATE_MAX] = {
        [SERVICE_DEAD]                       = "dead",
        [SERVICE_CONDITION]                  = "condition",
        [SERVICE_ప్రారంభం_PRE]                  = "start-pre",
        [SERVICE_ప్రారంభం]                      = "start",
        [SERVICE_ప్రారంభం_POST]                 = "start-post",
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

DEFINE_STRING_పట్టిక_LOOKUP(service_state, ServiceState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి ముక్క_state_పట్టిక[ముక్క_STATE_MAX] = {
        [ముక్క_DEAD]   = "dead",
        [ముక్క_చురుకైన] = "active",
};

DEFINE_STRING_పట్టిక_LOOKUP(ముక్క_state, ముక్క_State);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి socket_state_పట్టిక[_SOCKET_STATE_MAX] = {
        [SOCKET_DEAD]             = "dead",
        [SOCKET_ప్రారంభం_PRE]        = "start-pre",
        [SOCKET_ప్రారంభం_OPEN]       = "start-open",
        [SOCKET_ప్రారంభం_CHOWN]      = "start-chown",
        [SOCKET_ప్రారంభం_POST]       = "start-post",
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

DEFINE_STRING_పట్టిక_LOOKUP(socket_state, SocketState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి మార్పిడి_state_పట్టిక[మార్పిడి_STATE_MAX] = {
        [మార్పిడి_DEAD]                 = "dead",
        [మార్పిడి_ACTIVATING]           = "activating",
        [మార్పిడి_ACTIVATING_పూర్తి]      = "activating-done",
        [మార్పిడి_చురుకైన]               = "active",
        [మార్పిడి_DEACTIVATING]         = "deactivating",
        [మార్పిడి_DEACTIVATING_SIGTERM] = "deactivating-sigterm",
        [మార్పిడి_DEACTIVATING_SIGKILL] = "deactivating-sigkill",
        [మార్పిడి_FAILED]               = "failed",
        [మార్పిడి_CLEANING]             = "cleaning",
};

DEFINE_STRING_పట్టిక_LOOKUP(మార్పిడి_state, మార్పిడి_State);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి లక్ష్యం_state_పట్టిక[లక్ష్యం_STATE_MAX] = {
        [లక్ష్యం_DEAD]   = "dead",
        [లక్ష్యం_చురుకైన] = "active",
};

DEFINE_STRING_పట్టిక_LOOKUP(లక్ష్యం_state, లక్ష్యం_State);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి timer_state_పట్టిక[_TIMER_STATE_MAX] = {
        [TIMER_DEAD]    = "dead",
        [TIMER_నిరీక్షణ] = "waiting",
        [TIMER_RUNNING] = "running",
        [TIMER_ELAPSED] = "elapsed",
        [TIMER_FAILED]  = "failed",
};

DEFINE_STRING_పట్టిక_LOOKUP(timer_state, TimerState);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి unit_dependency_పట్టిక[_UNIT_DEPENDENCY_MAX] = {
        [UNIT_REQUIRES]               = "Requires",
        [UNIT_REQUISITE]              = "Requisite",
        [UNIT_కోరుతుంది]                  = "Wants",
        [UNIT_BINDS_గమ్యం]               = "BindsTo",
        [UNIT_PART_యొక్క]                = "PartOf",
        [UNIT_UPHOLDS]                = "Upholds",
        [UNIT_REQUIRED_BY]            = "RequiredBy",
        [UNIT_REQUISITE_యొక్క]           = "RequisiteOf",
        [UNIT_WANTED_BY]              = "WantedBy",
        [UNIT_BOUND_BY]               = "BoundBy",
        [UNIT_UPHELD_BY]              = "UpheldBy",
        [UNIT_CONSISTS_యొక్క]            = "ConsistsOf",
        [UNIT_CONFLICTS]              = "Conflicts",
        [UNIT_CONFLICTED_BY]          = "ConflictedBy",
        [UNIT_BEFORE]                 = "Before",
        [UNIT_AFTER]                  = "After",
        [UNIT_ON_SUCCESS]             = "OnSuccess",
        [UNIT_ON_SUCCESS_యొక్క]          = "OnSuccessOf",
        [UNIT_ON_FAILURE]             = "OnFailure",
        [UNIT_ON_FAILURE_యొక్క]          = "OnFailureOf",
        [UNIT_TRIGGERS]               = "Triggers",
        [UNIT_TRIGGERED_BY]           = "TriggeredBy",
        [UNIT_PROPAGATES_RELOAD_గమ్యం]   = "PropagatesReloadTo",
        [UNIT_RELOAD_PROPAGATED_FROM] = "ReloadPropagatedFrom",
        [UNIT_PROPAGATES_STOP_గమ్యం]     = "PropagatesStopTo",
        [UNIT_STOP_PROPAGATED_FROM]   = "StopPropagatedFrom",
        [UNIT_JOINS_NAMESPACE_యొక్క]     = "JoinsNamespaceOf",
        [UNIT_REFERENCES]             = "References",
        [UNIT_REFERENCED_BY]          = "ReferencedBy",
        [UNIT_లోపల_ముక్క]               = "InSlice",
        [UNIT_ముక్క_యొక్క]               = "SliceOf",
};

DEFINE_STRING_పట్టిక_LOOKUP(unit_dependency, UnitDependency);

bool unit_dependency_can_be_transient(UnitDependency d) {
        తిరిగి లోపల_సమితి(d,
                      UNIT_REQUIRES,
                      UNIT_REQUISITE,
                      UNIT_కోరుతుంది,
                      UNIT_BINDS_గమ్యం,
                      UNIT_PART_యొక్క,
                      UNIT_UPHOLDS,
                      UNIT_CONFLICTS,
                      UNIT_BEFORE,
                      UNIT_AFTER,
                      UNIT_ON_SUCCESS,
                      UNIT_ON_FAILURE,
                      UNIT_PROPAGATES_RELOAD_గమ్యం,
                      UNIT_RELOAD_PROPAGATED_FROM,
                      UNIT_PROPAGATES_STOP_గమ్యం,
                      UNIT_STOP_PROPAGATED_FROM,
                      UNIT_JOINS_NAMESPACE_యొక్క);
}

శూన్య unit_types_list(శూన్య) {
        DUMP_STRING_పట్టిక(unit_dependency, UnitDependency, _UNIT_DEPENDENCY_MAX);
}

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి notify_access_పట్టిక[_NOTIFY_ACCESS_MAX] = {
        [NOTIFY_NONE] = "none",
        [NOTIFY_MAIN] = "main",
        [NOTIFY_EXEC] = "exec",
        [NOTIFY_ALL]  = "all",
};

DEFINE_STRING_పట్టిక_LOOKUP(notify_access, NotifyAccess);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి పని_బహుళకం_పట్టిక[పని_బహుళకం_MAX] = {
        [పని_FAIL]                 = "fail",
        [పని_LENIENT]              = "lenient",
        [పని_REPLACE]              = "replace",
        [పని_REPLACE_IRREVERSIBLY] = "replace-irreversibly",
        [పని_ISOLATE]              = "isolate",
        [పని_FLUSH]                = "flush",
        [పని_IGNORE_DEPENDENCIES]  = "ignore-dependencies",
        [పని_IGNORE_REQUIREMENTS]  = "ignore-requirements",
        [పని_TRIGGERING]           = "triggering",
        [పని_RESTART_DEPENDENCIES] = "restart-dependencies",
};

DEFINE_STRING_పట్టిక_LOOKUP(పని_బహుళకం_2, పని_బహుళకం);

/* This table maps ExecDirectoryType to the setting it is configured with in the unit */
స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి exec_directory_type_పట్టిక[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectory",
        [EXEC_DIRECTORY_STATE]         = "StateDirectory",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectory",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectory",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectory",
};

DEFINE_STRING_పట్టిక_LOOKUP(exec_directory_type, ExecDirectoryType);

/* D-Bus properties carrying the symlink destinations and flags for each directory type. */
స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి exec_directory_type_symlink_పట్టిక[_EXEC_DIRECTORY_TYPE_MAX] = {
        [EXEC_DIRECTORY_RUNTIME]       = "RuntimeDirectorySymlink",
        [EXEC_DIRECTORY_STATE]         = "StateDirectorySymlink",
        [EXEC_DIRECTORY_CACHE]         = "CacheDirectorySymlink",
        [EXEC_DIRECTORY_LOGS]          = "LogsDirectorySymlink",
        [EXEC_DIRECTORY_CONFIGURATION] = "ConfigurationDirectorySymlink",
};

DEFINE_STRING_పట్టిక_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_చురుకైన_state_గమ్యం_glyph(Unit_చురుకైన_State state) {
        స్థిర స్థిరరాశి Glyph map[UNIT_చురుకైన_STATE_MAX] = {
                [UNIT_చురుకైన]       = GLYPH_BLACK_CIRCLE,
                [UNIT_RELOADING]    = GLYPH_CIRCLE_ARROW,
                [UNIT_REFRESHING]   = GLYPH_CIRCLE_ARROW,
                [UNIT_INACTIVE]     = GLYPH_WHITE_CIRCLE,
                [UNIT_FAILED]       = GLYPH_MULTIPLICATION_SIGN,
                [UNIT_ACTIVATING]   = GLYPH_BLACK_CIRCLE,
                [UNIT_DEACTIVATING] = GLYPH_BLACK_CIRCLE,
                [UNIT_MAINTENANCE]  = GLYPH_WHITE_CIRCLE,
        };

        ఒకవేళ (state < 0)
                తిరిగి _GLYPH_INVALID;

        ప్రతిజ్ఞా(state < UNIT_చురుకైన_STATE_MAX);
        తిరిగి map[state];
}
