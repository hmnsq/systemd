/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_ಅದಲುಬದಲು,
        UNIT_SOCKET,
        UNIT_ಗುರಿ,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_ಮಾರ್ಗ,
        UNIT_ತುಂಡು,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Unit_ತುಂಬು_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_ಸಿಕ್ಕಿತು,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_ತುಂಬು_STATE_MAX,
        UNIT_ತುಂಬು_STATE_INVALID = -EINVAL,
} Unit_ತುಂಬು_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Unit_ಸಕ್ರಿಯ_State {
        UNIT_ಸಕ್ರಿಯ,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_ಸಕ್ರಿಯ_STATE_MAX,
        UNIT_ಸಕ್ರಿಯ_STATE_INVALID = -EINVAL,
} Unit_ಸಕ್ರಿಯ_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_ಪೋಷಕ, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_ಪೋಷಕ,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_ಆರಂಭ,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_ಕಾಯುವಿಕೆ,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_ಮುಗಿದ,          /* /usr/bin/mount is running, and the mount is done. */
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಮಾರ್ಗ_State {
        ಮಾರ್ಗ_DEAD,
        ಮಾರ್ಗ_ಕಾಯುವಿಕೆ,
        ಮಾರ್ಗ_RUNNING,
        ಮಾರ್ಗ_FAILED,
        ಮಾರ್ಗ_STATE_MAX,
        ಮಾರ್ಗ_STATE_INVALID = -EINVAL,
} ಮಾರ್ಗ_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ScopeState {
        SCOPE_DEAD,
        SCOPE_ಆರಂಭ_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_ಆರಂಭ_PRE,
        SERVICE_ಆರಂಭ,
        SERVICE_ಆರಂಭ_POST,
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ತುಂಡು_State {
        ತುಂಡು_DEAD,
        ತುಂಡು_ಸಕ್ರಿಯ,
        ತುಂಡು_STATE_MAX,
        ತುಂಡು_STATE_INVALID = -EINVAL,
} ತುಂಡು_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ SocketState {
        SOCKET_DEAD,
        SOCKET_ಆರಂಭ_PRE,
        SOCKET_ಆರಂಭ_OPEN,
        SOCKET_ಆರಂಭ_CHOWN,
        SOCKET_ಆರಂಭ_POST,
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಅದಲುಬದಲು_State {
        ಅದಲುಬದಲು_DEAD,
        ಅದಲುಬದಲು_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        ಅದಲುಬದಲು_ACTIVATING_ಮುಗಿದ,          /* /sbin/swapon is running, and the swap is done. */
        ಅದಲುಬದಲು_ಸಕ್ರಿಯ,
        ಅದಲುಬದಲು_DEACTIVATING,
        ಅದಲುಬದಲು_DEACTIVATING_SIGTERM,
        ಅದಲುಬದಲು_DEACTIVATING_SIGKILL,
        ಅದಲುಬದಲು_FAILED,
        ಅದಲುಬದಲು_CLEANING,
        ಅದಲುಬದಲು_STATE_MAX,
        ಅದಲುಬದಲು_STATE_INVALID = -EINVAL,
} ಅದಲುಬದಲು_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಗುರಿ_State {
        ಗುರಿ_DEAD,
        ಗುರಿ_ಸಕ್ರಿಯ,
        ಗುರಿ_STATE_MAX,
        ಗುರಿ_STATE_INVALID = -EINVAL,
} ಗುರಿ_State;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ TimerState {
        TIMER_DEAD,
        TIMER_ಕಾಯುವಿಕೆ,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_ಬಯಸುತ್ತದೆ,
        UNIT_BINDS_ಗಮ್ಯ,
        UNIT_PART_ದ,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_ದ,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_ದ,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*ಕ್ರಮ*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_ದ,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_ದ,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_ಗಮ್ಯ,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_ಗಮ್ಯ,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_ದ,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_ಒಳ_ತುಂಡು,
        UNIT_ತುಂಡು_ದ,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ಕೆಲಸ_ಬಹುಲಕ {
        ಕೆಲಸ_FAIL,                 /* Fail if a conflicting job is already queued */
        ಕೆಲಸ_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        ಕೆಲಸ_REPLACE,              /* Replace an existing conflicting job */
        ಕೆಲಸ_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        ಕೆಲಸ_ISOLATE,              /* Start a unit, and stop all others */
        ಕೆಲಸ_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        ಕೆಲಸ_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        ಕೆಲಸ_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        ಕೆಲಸ_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        ಕೆಲಸ_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        ಕೆಲಸ_ಬಹುಲಕ_MAX,
        ಕೆಲಸ_ಬಹುಲಕ_INVALID = -EINVAL,
} ಕೆಲಸ_ಬಹುಲಕ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

ಅಕ್ಷರ* unit_dbus_ಮಾರ್ಗ_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಪೂರ್ಣಾಂಕ unit_name_from_dbus_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **name);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_dbus_interface_from_type(UnitType t);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_dbus_interface_from_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_type, UnitType);
ಶೂನ್ಯ unit_types_list(ಶೂನ್ಯ);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_ತುಂಬು_state, Unit_ತುಂಬು_State);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_ಸಕ್ರಿಯ_state, Unit_ಸಕ್ರಿಯ_State);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* unit_type_ಗಮ್ಯ_capitalized_string(UnitType t);

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_ಮುಕ್ತಾಯ(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(device_state, DeviceState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(mount_state, MountState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಮಾರ್ಗ_state, ಮಾರ್ಗ_State);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(service_state, ServiceState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ತುಂಡು_state, ತುಂಡು_State);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(socket_state, SocketState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಅದಲುಬದಲು_state, ಅದಲುಬದಲು_State);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಗುರಿ_state, ಗುರಿ_State);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(timer_state, TimerState);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(ಕೆಲಸ_ಬಹುಲಕ_2, ಕೆಲಸ_ಬಹುಲಕ);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_ಸಕ್ರಿಯ_state_ಗಮ್ಯ_glyph(Unit_ಸಕ್ರಿಯ_State state);
