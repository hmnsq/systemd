/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
قسمتعریف شمار UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_تبادلہ,
        UNIT_SOCKET,
        UNIT_ہدف,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_راستہ,
        UNIT_قاش,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

قسمتعریف شمار Unit_بھرو_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_ملا,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_بھرو_STATE_MAX,
        UNIT_بھرو_STATE_INVALID = -EINVAL,
} Unit_بھرو_State;

قسمتعریف شمار Unit_فعال_State {
        UNIT_فعال,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_فعال_STATE_MAX,
        UNIT_فعال_STATE_INVALID = -EINVAL,
} Unit_فعال_State;

قسمتعریف شمار FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_والد, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_والد,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

قسمتعریف شمار UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_آغاز,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

قسمتعریف شمار AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_انتظار,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
قسمتعریف شمار DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

قسمتعریف شمار MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_مکمل,          /* /usr/bin/mount is running, and the mount is done. */
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

قسمتعریف شمار راستہ_State {
        راستہ_DEAD,
        راستہ_انتظار,
        راستہ_RUNNING,
        راستہ_FAILED,
        راستہ_STATE_MAX,
        راستہ_STATE_INVALID = -EINVAL,
} راستہ_State;

قسمتعریف شمار ScopeState {
        SCOPE_DEAD,
        SCOPE_آغاز_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

قسمتعریف شمار ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_آغاز_PRE,
        SERVICE_آغاز,
        SERVICE_آغاز_POST,
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

قسمتعریف شمار قاش_State {
        قاش_DEAD,
        قاش_فعال,
        قاش_STATE_MAX,
        قاش_STATE_INVALID = -EINVAL,
} قاش_State;

قسمتعریف شمار SocketState {
        SOCKET_DEAD,
        SOCKET_آغاز_PRE,
        SOCKET_آغاز_OPEN,
        SOCKET_آغاز_CHOWN,
        SOCKET_آغاز_POST,
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

قسمتعریف شمار تبادلہ_State {
        تبادلہ_DEAD,
        تبادلہ_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        تبادلہ_ACTIVATING_مکمل,          /* /sbin/swapon is running, and the swap is done. */
        تبادلہ_فعال,
        تبادلہ_DEACTIVATING,
        تبادلہ_DEACTIVATING_SIGTERM,
        تبادلہ_DEACTIVATING_SIGKILL,
        تبادلہ_FAILED,
        تبادلہ_CLEANING,
        تبادلہ_STATE_MAX,
        تبادلہ_STATE_INVALID = -EINVAL,
} تبادلہ_State;

قسمتعریف شمار ہدف_State {
        ہدف_DEAD,
        ہدف_فعال,
        ہدف_STATE_MAX,
        ہدف_STATE_INVALID = -EINVAL,
} ہدف_State;

قسمتعریف شمار TimerState {
        TIMER_DEAD,
        TIMER_انتظار,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

قسمتعریف شمار UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_چاہتا,
        UNIT_BINDS_مقصد,
        UNIT_PART_کا,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_کا,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_کا,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*ترتیب*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_کا,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_کا,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_مقصد,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_مقصد,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_کا,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_اندر_قاش,
        UNIT_قاش_کا,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

قسمتعریف شمار NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

قسمتعریف شمار کام_منوال {
        کام_FAIL,                 /* Fail if a conflicting job is already queued */
        کام_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        کام_REPLACE,              /* Replace an existing conflicting job */
        کام_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        کام_ISOLATE,              /* Start a unit, and stop all others */
        کام_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        کام_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        کام_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        کام_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        کام_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        کام_منوال_MAX,
        کام_منوال_INVALID = -EINVAL,
} کام_منوال;

قسمتعریف شمار ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

حرف* unit_dbus_راستہ_from_name(ثابت حرف *name);
صحیح unit_name_from_dbus_راستہ(ثابت حرف *راستہ, حرف **name);

ثابت حرف* unit_dbus_interface_from_type(UnitType t);
ثابت حرف* unit_dbus_interface_from_name(ثابت حرف *name);

DECLARE_STRING_جدول_LOOKUP(unit_type, UnitType);
خالی unit_types_list(خالی);

DECLARE_STRING_جدول_LOOKUP(unit_بھرو_state, Unit_بھرو_State);
DECLARE_STRING_جدول_LOOKUP(unit_فعال_state, Unit_فعال_State);
ثابت حرف* unit_type_مقصد_capitalized_string(UnitType t);

DECLARE_STRING_جدول_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_اختتام(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_جدول_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_جدول_LOOKUP(device_state, DeviceState);
DECLARE_STRING_جدول_LOOKUP(mount_state, MountState);
DECLARE_STRING_جدول_LOOKUP(راستہ_state, راستہ_State);
DECLARE_STRING_جدول_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_جدول_LOOKUP(service_state, ServiceState);
DECLARE_STRING_جدول_LOOKUP(قاش_state, قاش_State);
DECLARE_STRING_جدول_LOOKUP(socket_state, SocketState);
DECLARE_STRING_جدول_LOOKUP(تبادلہ_state, تبادلہ_State);
DECLARE_STRING_جدول_LOOKUP(ہدف_state, ہدف_State);
DECLARE_STRING_جدول_LOOKUP(timer_state, TimerState);
DECLARE_STRING_جدول_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_جدول_LOOKUP(کام_منوال_2, کام_منوال);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_فعال_state_مقصد_glyph(Unit_فعال_State state);
