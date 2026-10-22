/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
تعريفنوع تعداد UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_بدلون,
        UNIT_SOCKET,
        UNIT_موخه,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_لار,
        UNIT_ټوټه,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

تعريفنوع تعداد Unit_بارول_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_وموندل,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_بارول_STATE_MAX,
        UNIT_بارول_STATE_INVALID = -EINVAL,
} Unit_بارول_State;

تعريفنوع تعداد Unit_فعال_State {
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

تعريفنوع تعداد FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_پلار, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_پلار,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

تعريفنوع تعداد UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_پیل,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

تعريفنوع تعداد AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_انتظار,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
تعريفنوع تعداد DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

تعريفنوع تعداد MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_بشپړ,          /* /usr/bin/mount is running, and the mount is done. */
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

تعريفنوع تعداد لار_State {
        لار_DEAD,
        لار_انتظار,
        لار_RUNNING,
        لار_FAILED,
        لار_STATE_MAX,
        لار_STATE_INVALID = -EINVAL,
} لار_State;

تعريفنوع تعداد ScopeState {
        SCOPE_DEAD,
        SCOPE_پیل_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

تعريفنوع تعداد ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_پیل_PRE,
        SERVICE_پیل,
        SERVICE_پیل_POST,
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

تعريفنوع تعداد ټوټه_State {
        ټوټه_DEAD,
        ټوټه_فعال,
        ټوټه_STATE_MAX,
        ټوټه_STATE_INVALID = -EINVAL,
} ټوټه_State;

تعريفنوع تعداد SocketState {
        SOCKET_DEAD,
        SOCKET_پیل_PRE,
        SOCKET_پیل_OPEN,
        SOCKET_پیل_CHOWN,
        SOCKET_پیل_POST,
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

تعريفنوع تعداد بدلون_State {
        بدلون_DEAD,
        بدلون_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        بدلون_ACTIVATING_بشپړ,          /* /sbin/swapon is running, and the swap is done. */
        بدلون_فعال,
        بدلون_DEACTIVATING,
        بدلون_DEACTIVATING_SIGTERM,
        بدلون_DEACTIVATING_SIGKILL,
        بدلون_FAILED,
        بدلون_CLEANING,
        بدلون_STATE_MAX,
        بدلون_STATE_INVALID = -EINVAL,
} بدلون_State;

تعريفنوع تعداد موخه_State {
        موخه_DEAD,
        موخه_فعال,
        موخه_STATE_MAX,
        موخه_STATE_INVALID = -EINVAL,
} موخه_State;

تعريفنوع تعداد TimerState {
        TIMER_DEAD,
        TIMER_انتظار,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

تعريفنوع تعداد UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_غواړي,
        UNIT_BINDS_منزل,
        UNIT_PART_د,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_د,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_د,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*ترتیب*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_د,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_د,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_منزل,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_منزل,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_د,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_دننه_ټوټه,
        UNIT_ټوټه_د,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

تعريفنوع تعداد NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

تعريفنوع تعداد دنده_ډېرپېښ {
        دنده_FAIL,                 /* Fail if a conflicting job is already queued */
        دنده_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        دنده_REPLACE,              /* Replace an existing conflicting job */
        دنده_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        دنده_ISOLATE,              /* Start a unit, and stop all others */
        دنده_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        دنده_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        دنده_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        دنده_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        دنده_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        دنده_ډېرپېښ_MAX,
        دنده_ډېرپېښ_INVALID = -EINVAL,
} دنده_ډېرپېښ;

تعريفنوع تعداد ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

حرف* unit_dbus_لار_from_name(ثابت حرف *name);
صحيح unit_name_from_dbus_لار(ثابت حرف *لار, حرف **name);

ثابت حرف* unit_dbus_interface_from_type(UnitType t);
ثابت حرف* unit_dbus_interface_from_name(ثابت حرف *name);

DECLARE_STRING_جدول_LOOKUP(unit_type, UnitType);
فراغ unit_types_list(فراغ);

DECLARE_STRING_جدول_LOOKUP(unit_بارول_state, Unit_بارول_State);
DECLARE_STRING_جدول_LOOKUP(unit_فعال_state, Unit_فعال_State);
ثابت حرف* unit_type_منزل_capitalized_string(UnitType t);

DECLARE_STRING_جدول_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_پای(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_جدول_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_جدول_LOOKUP(device_state, DeviceState);
DECLARE_STRING_جدول_LOOKUP(mount_state, MountState);
DECLARE_STRING_جدول_LOOKUP(لار_state, لار_State);
DECLARE_STRING_جدول_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_جدول_LOOKUP(service_state, ServiceState);
DECLARE_STRING_جدول_LOOKUP(ټوټه_state, ټوټه_State);
DECLARE_STRING_جدول_LOOKUP(socket_state, SocketState);
DECLARE_STRING_جدول_LOOKUP(بدلون_state, بدلون_State);
DECLARE_STRING_جدول_LOOKUP(موخه_state, موخه_State);
DECLARE_STRING_جدول_LOOKUP(timer_state, TimerState);
DECLARE_STRING_جدول_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_جدول_LOOKUP(دنده_ډېرپېښ_2, دنده_ډېرپېښ);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_فعال_state_منزل_glyph(Unit_فعال_State state);
