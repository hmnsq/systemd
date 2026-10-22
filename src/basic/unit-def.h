/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_ਅਦਲਾ_ਬਦਲੀ,
        UNIT_SOCKET,
        UNIT_ਨਿਸ਼ਾਨਾ,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_ਰਸਤਾ,
        UNIT_ਟੁਕੜਾ,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Unit_ਲੱਦੋ_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_ਮਿਲਿਆ,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_ਲੱਦੋ_STATE_MAX,
        UNIT_ਲੱਦੋ_STATE_INVALID = -EINVAL,
} Unit_ਲੱਦੋ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Unit_ਸਰਗਰਮ_State {
        UNIT_ਸਰਗਰਮ,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_ਸਰਗਰਮ_STATE_MAX,
        UNIT_ਸਰਗਰਮ_STATE_INVALID = -EINVAL,
} Unit_ਸਰਗਰਮ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_ਮਾਪਾ, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_ਮਾਪਾ,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_ਸ਼ੁਰੂ,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_ਉਡੀਕ,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_ਪੂਰਾ,          /* /usr/bin/mount is running, and the mount is done. */
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਰਸਤਾ_State {
        ਰਸਤਾ_DEAD,
        ਰਸਤਾ_ਉਡੀਕ,
        ਰਸਤਾ_RUNNING,
        ਰਸਤਾ_FAILED,
        ਰਸਤਾ_STATE_MAX,
        ਰਸਤਾ_STATE_INVALID = -EINVAL,
} ਰਸਤਾ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ScopeState {
        SCOPE_DEAD,
        SCOPE_ਸ਼ੁਰੂ_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_ਸ਼ੁਰੂ_PRE,
        SERVICE_ਸ਼ੁਰੂ,
        SERVICE_ਸ਼ੁਰੂ_POST,
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਟੁਕੜਾ_State {
        ਟੁਕੜਾ_DEAD,
        ਟੁਕੜਾ_ਸਰਗਰਮ,
        ਟੁਕੜਾ_STATE_MAX,
        ਟੁਕੜਾ_STATE_INVALID = -EINVAL,
} ਟੁਕੜਾ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ SocketState {
        SOCKET_DEAD,
        SOCKET_ਸ਼ੁਰੂ_PRE,
        SOCKET_ਸ਼ੁਰੂ_OPEN,
        SOCKET_ਸ਼ੁਰੂ_CHOWN,
        SOCKET_ਸ਼ੁਰੂ_POST,
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਅਦਲਾ_ਬਦਲੀ_State {
        ਅਦਲਾ_ਬਦਲੀ_DEAD,
        ਅਦਲਾ_ਬਦਲੀ_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        ਅਦਲਾ_ਬਦਲੀ_ACTIVATING_ਪੂਰਾ,          /* /sbin/swapon is running, and the swap is done. */
        ਅਦਲਾ_ਬਦਲੀ_ਸਰਗਰਮ,
        ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING,
        ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING_SIGTERM,
        ਅਦਲਾ_ਬਦਲੀ_DEACTIVATING_SIGKILL,
        ਅਦਲਾ_ਬਦਲੀ_FAILED,
        ਅਦਲਾ_ਬਦਲੀ_CLEANING,
        ਅਦਲਾ_ਬਦਲੀ_STATE_MAX,
        ਅਦਲਾ_ਬਦਲੀ_STATE_INVALID = -EINVAL,
} ਅਦਲਾ_ਬਦਲੀ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਨਿਸ਼ਾਨਾ_State {
        ਨਿਸ਼ਾਨਾ_DEAD,
        ਨਿਸ਼ਾਨਾ_ਸਰਗਰਮ,
        ਨਿਸ਼ਾਨਾ_STATE_MAX,
        ਨਿਸ਼ਾਨਾ_STATE_INVALID = -EINVAL,
} ਨਿਸ਼ਾਨਾ_State;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ TimerState {
        TIMER_DEAD,
        TIMER_ਉਡੀਕ,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_ਚਾਹੁੰਦਾ,
        UNIT_BINDS_ਮੰਜ਼ਿਲ,
        UNIT_PART_ਦਾ,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_ਦਾ,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_ਦਾ,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*ਕ੍ਰਮ*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_ਦਾ,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_ਦਾ,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_ਮੰਜ਼ਿਲ,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_ਮੰਜ਼ਿਲ,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_ਦਾ,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_ਵਿੱਚ_ਟੁਕੜਾ,
        UNIT_ਟੁਕੜਾ_ਦਾ,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ਕੰਮ_ਬਹੁਲਕ {
        ਕੰਮ_FAIL,                 /* Fail if a conflicting job is already queued */
        ਕੰਮ_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        ਕੰਮ_REPLACE,              /* Replace an existing conflicting job */
        ਕੰਮ_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        ਕੰਮ_ISOLATE,              /* Start a unit, and stop all others */
        ਕੰਮ_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        ਕੰਮ_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        ਕੰਮ_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        ਕੰਮ_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        ਕੰਮ_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        ਕੰਮ_ਬਹੁਲਕ_MAX,
        ਕੰਮ_ਬਹੁਲਕ_INVALID = -EINVAL,
} ਕੰਮ_ਬਹੁਲਕ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

ਅੱਖਰ* unit_dbus_ਰਸਤਾ_from_name(ਅਚਲ ਅੱਖਰ *name);
ਪੂਰਨ unit_name_from_dbus_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **name);

ਅਚਲ ਅੱਖਰ* unit_dbus_interface_from_type(UnitType t);
ਅਚਲ ਅੱਖਰ* unit_dbus_interface_from_name(ਅਚਲ ਅੱਖਰ *name);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(unit_type, UnitType);
ਖਾਲੀ unit_types_list(ਖਾਲੀ);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(unit_ਲੱਦੋ_state, Unit_ਲੱਦੋ_State);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(unit_ਸਰਗਰਮ_state, Unit_ਸਰਗਰਮ_State);
ਅਚਲ ਅੱਖਰ* unit_type_ਮੰਜ਼ਿਲ_capitalized_string(UnitType t);

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_ਅੰਤ(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(device_state, DeviceState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(mount_state, MountState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਰਸਤਾ_state, ਰਸਤਾ_State);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(service_state, ServiceState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਟੁਕੜਾ_state, ਟੁਕੜਾ_State);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(socket_state, SocketState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਅਦਲਾ_ਬਦਲੀ_state, ਅਦਲਾ_ਬਦਲੀ_State);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਨਿਸ਼ਾਨਾ_state, ਨਿਸ਼ਾਨਾ_State);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(timer_state, TimerState);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(ਕੰਮ_ਬਹੁਲਕ_2, ਕੰਮ_ਬਹੁਲਕ);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_ਸਰਗਰਮ_state_ਮੰਜ਼ਿਲ_glyph(Unit_ਸਰਗਰਮ_State state);
