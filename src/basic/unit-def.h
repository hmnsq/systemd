/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
రకనిర్వచనం గణన UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_మార్పిడి,
        UNIT_SOCKET,
        UNIT_లక్ష్యం,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_మార్గం,
        UNIT_ముక్క,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

రకనిర్వచనం గణన Unit_ఎక్కించు_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_దొరికింది,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_ఎక్కించు_STATE_MAX,
        UNIT_ఎక్కించు_STATE_INVALID = -EINVAL,
} Unit_ఎక్కించు_State;

రకనిర్వచనం గణన Unit_చురుకైన_State {
        UNIT_చురుకైన,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_చురుకైన_STATE_MAX,
        UNIT_చురుకైన_STATE_INVALID = -EINVAL,
} Unit_చురుకైన_State;

రకనిర్వచనం గణన FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_మాతృ, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_మాతృ,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

రకనిర్వచనం గణన UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_ప్రారంభం,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

రకనిర్వచనం గణన AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_నిరీక్షణ,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
రకనిర్వచనం గణన DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

రకనిర్వచనం గణన MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_పూర్తి,          /* /usr/bin/mount is running, and the mount is done. */
        MOUNT_MOUNTED,
        MOUNT_REMOUNTING,
        MOUNT_UNMOUNTING,
        MOUNT_REMOUNTING_SIGTERM,
        MOUNT_REMOUNTING_SIGKILL,
        MOUNT_UNMOUNTING_SIGTERM,
        MOUNT_UNMOUNTING_SIGKILL,
        MOUNT_FAILED,
        MOUNT_CLEANING,
        _MOUNT_STATE_MAX,
        _MOUNT_STATE_INVALID = -EINVAL,
} MountState;

రకనిర్వచనం గణన మార్గం_State {
        మార్గం_DEAD,
        మార్గం_నిరీక్షణ,
        మార్గం_RUNNING,
        మార్గం_FAILED,
        మార్గం_STATE_MAX,
        మార్గం_STATE_INVALID = -EINVAL,
} మార్గం_State;

రకనిర్వచనం గణన ScopeState {
        SCOPE_DEAD,
        SCOPE_ప్రారంభం_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

రకనిర్వచనం గణన ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_ప్రారంభం_PRE,
        SERVICE_ప్రారంభం,
        SERVICE_ప్రారంభం_POST,
        SERVICE_RUNNING,
        SERVICE_RUNNING_REVALIDATING,   /* Temporarily waiting for the D-Bus name to reappear */
        SERVICE_EXITED,                 /* Nothing is running anymore, but RemainAfterExit is true hence this is OK */
        SERVICE_REFRESH_EXTENSIONS,     /* Refreshing extensions for a reload request */
        SERVICE_REFRESH_CREDENTIALS,    /* ditto, but for credentials */
        SERVICE_RELOAD,                 /* Reloading via ExecReload= */
        SERVICE_RELOAD_SIGNAL,          /* Reloading via SIGHUP requested */
        SERVICE_RELOAD_NOTIFY,          /* Waiting for READY=1 after RELOADING=1 notify */
        SERVICE_RELOAD_POST,
        SERVICE_MOUNTING,               /* Performing a live mount into the namespace of the service */
        SERVICE_STOP,                   /* No STOP_PRE state, instead just register multiple STOP executables */
        SERVICE_STOP_WATCHDOG,
        SERVICE_STOP_SIGTERM,
        SERVICE_STOP_SIGKILL,
        SERVICE_STOP_POST,
        SERVICE_FINAL_WATCHDOG,         /* In case the STOP_POST executable needs to be aborted. */
        SERVICE_FINAL_SIGTERM,          /* In case the STOP_POST executable hangs, we shoot that down, too */
        SERVICE_FINAL_SIGKILL,
        SERVICE_FAILED,
        SERVICE_DEAD_BEFORE_AUTO_RESTART,
        SERVICE_FAILED_BEFORE_AUTO_RESTART,
        SERVICE_DEAD_RESOURCES_PINNED,  /* Like SERVICE_DEAD, but with pinned resources */
        SERVICE_AUTO_RESTART,
        SERVICE_AUTO_RESTART_QUEUED,
        SERVICE_CLEANING,
        _SERVICE_STATE_MAX,
        _SERVICE_STATE_INVALID = -EINVAL,
} ServiceState;

రకనిర్వచనం గణన ముక్క_State {
        ముక్క_DEAD,
        ముక్క_చురుకైన,
        ముక్క_STATE_MAX,
        ముక్క_STATE_INVALID = -EINVAL,
} ముక్క_State;

రకనిర్వచనం గణన SocketState {
        SOCKET_DEAD,
        SOCKET_ప్రారంభం_PRE,
        SOCKET_ప్రారంభం_OPEN,
        SOCKET_ప్రారంభం_CHOWN,
        SOCKET_ప్రారంభం_POST,
        SOCKET_LISTENING,
        SOCKET_DEFERRED,
        SOCKET_RUNNING,
        SOCKET_STOP_PRE,
        SOCKET_STOP_PRE_SIGTERM,
        SOCKET_STOP_PRE_SIGKILL,
        SOCKET_STOP_POST,
        SOCKET_FINAL_SIGTERM,
        SOCKET_FINAL_SIGKILL,
        SOCKET_FAILED,
        SOCKET_CLEANING,
        _SOCKET_STATE_MAX,
        _SOCKET_STATE_INVALID = -EINVAL,
} SocketState;

రకనిర్వచనం గణన మార్పిడి_State {
        మార్పిడి_DEAD,
        మార్పిడి_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        మార్పిడి_ACTIVATING_పూర్తి,          /* /sbin/swapon is running, and the swap is done. */
        మార్పిడి_చురుకైన,
        మార్పిడి_DEACTIVATING,
        మార్పిడి_DEACTIVATING_SIGTERM,
        మార్పిడి_DEACTIVATING_SIGKILL,
        మార్పిడి_FAILED,
        మార్పిడి_CLEANING,
        మార్పిడి_STATE_MAX,
        మార్పిడి_STATE_INVALID = -EINVAL,
} మార్పిడి_State;

రకనిర్వచనం గణన లక్ష్యం_State {
        లక్ష్యం_DEAD,
        లక్ష్యం_చురుకైన,
        లక్ష్యం_STATE_MAX,
        లక్ష్యం_STATE_INVALID = -EINVAL,
} లక్ష్యం_State;

రకనిర్వచనం గణన TimerState {
        TIMER_DEAD,
        TIMER_నిరీక్షణ,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

రకనిర్వచనం గణన UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_కోరుతుంది,
        UNIT_BINDS_గమ్యం,
        UNIT_PART_యొక్క,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_యొక్క,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_యొక్క,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*క్రమం*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_యొక్క,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_యొక్క,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_గమ్యం,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_గమ్యం,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_యొక్క,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_లోపల_ముక్క,
        UNIT_ముక్క_యొక్క,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

రకనిర్వచనం గణన NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

రకనిర్వచనం గణన పని_బహుళకం {
        పని_FAIL,                 /* Fail if a conflicting job is already queued */
        పని_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        పని_REPLACE,              /* Replace an existing conflicting job */
        పని_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        పని_ISOLATE,              /* Start a unit, and stop all others */
        పని_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        పని_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        పని_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        పని_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        పని_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        పని_బహుళకం_MAX,
        పని_బహుళకం_INVALID = -EINVAL,
} పని_బహుళకం;

రకనిర్వచనం గణన ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

అక్షరం* unit_dbus_మార్గం_from_name(స్థిరరాశి అక్షరం *name);
పూర్ణాంకం unit_name_from_dbus_మార్గం(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **name);

స్థిరరాశి అక్షరం* unit_dbus_interface_from_type(UnitType t);
స్థిరరాశి అక్షరం* unit_dbus_interface_from_name(స్థిరరాశి అక్షరం *name);

DECLARE_STRING_పట్టిక_LOOKUP(unit_type, UnitType);
శూన్య unit_types_list(శూన్య);

DECLARE_STRING_పట్టిక_LOOKUP(unit_ఎక్కించు_state, Unit_ఎక్కించు_State);
DECLARE_STRING_పట్టిక_LOOKUP(unit_చురుకైన_state, Unit_చురుకైన_State);
స్థిరరాశి అక్షరం* unit_type_గమ్యం_capitalized_string(UnitType t);

DECLARE_STRING_పట్టిక_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_ముగింపు(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_పట్టిక_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_పట్టిక_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_పట్టిక_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_పట్టిక_LOOKUP(device_state, DeviceState);
DECLARE_STRING_పట్టిక_LOOKUP(mount_state, MountState);
DECLARE_STRING_పట్టిక_LOOKUP(మార్గం_state, మార్గం_State);
DECLARE_STRING_పట్టిక_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_పట్టిక_LOOKUP(service_state, ServiceState);
DECLARE_STRING_పట్టిక_LOOKUP(ముక్క_state, ముక్క_State);
DECLARE_STRING_పట్టిక_LOOKUP(socket_state, SocketState);
DECLARE_STRING_పట్టిక_LOOKUP(మార్పిడి_state, మార్పిడి_State);
DECLARE_STRING_పట్టిక_LOOKUP(లక్ష్యం_state, లక్ష్యం_State);
DECLARE_STRING_పట్టిక_LOOKUP(timer_state, TimerState);
DECLARE_STRING_పట్టిక_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_పట్టిక_LOOKUP(పని_బహుళకం_2, పని_బహుళకం);
DECLARE_STRING_పట్టిక_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_పట్టిక_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_చురుకైన_state_గమ్యం_glyph(Unit_చురుకైన_State state);
