/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
تعريفنوع تعداد UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_مٽا_سٽا,
        UNIT_SOCKET,
        UNIT_هدف,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_رستو,
        UNIT_ٽڪرو,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

تعريفنوع تعداد Unit_ڀريو_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_مليو,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_ڀريو_STATE_MAX,
        UNIT_ڀريو_STATE_INVALID = -EINVAL,
} Unit_ڀريو_State;

تعريفنوع تعداد Unit_سرگرم_State {
        UNIT_سرگرم,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_سرگرم_STATE_MAX,
        UNIT_سرگرم_STATE_INVALID = -EINVAL,
} Unit_سرگرم_State;

تعريفنوع تعداد FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_والد, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_والد,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

تعريفنوع تعداد UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_شروعات,
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
        MOUNT_MOUNTING_پورو,          /* /usr/bin/mount is running, and the mount is done. */
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

تعريفنوع تعداد رستو_State {
        رستو_DEAD,
        رستو_انتظار,
        رستو_RUNNING,
        رستو_FAILED,
        رستو_STATE_MAX,
        رستو_STATE_INVALID = -EINVAL,
} رستو_State;

تعريفنوع تعداد ScopeState {
        SCOPE_DEAD,
        SCOPE_شروعات_CHOWN,
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
        SERVICE_شروعات_PRE,
        SERVICE_شروعات,
        SERVICE_شروعات_POST,
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

تعريفنوع تعداد ٽڪرو_State {
        ٽڪرو_DEAD,
        ٽڪرو_سرگرم,
        ٽڪرو_STATE_MAX,
        ٽڪرو_STATE_INVALID = -EINVAL,
} ٽڪرو_State;

تعريفنوع تعداد SocketState {
        SOCKET_DEAD,
        SOCKET_شروعات_PRE,
        SOCKET_شروعات_OPEN,
        SOCKET_شروعات_CHOWN,
        SOCKET_شروعات_POST,
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

تعريفنوع تعداد مٽا_سٽا_State {
        مٽا_سٽا_DEAD,
        مٽا_سٽا_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        مٽا_سٽا_ACTIVATING_پورو,          /* /sbin/swapon is running, and the swap is done. */
        مٽا_سٽا_سرگرم,
        مٽا_سٽا_DEACTIVATING,
        مٽا_سٽا_DEACTIVATING_SIGTERM,
        مٽا_سٽا_DEACTIVATING_SIGKILL,
        مٽا_سٽا_FAILED,
        مٽا_سٽا_CLEANING,
        مٽا_سٽا_STATE_MAX,
        مٽا_سٽا_STATE_INVALID = -EINVAL,
} مٽا_سٽا_State;

تعريفنوع تعداد هدف_State {
        هدف_DEAD,
        هدف_سرگرم,
        هدف_STATE_MAX,
        هدف_STATE_INVALID = -EINVAL,
} هدف_State;

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
        UNIT_چاهي,
        UNIT_BINDS_منزل,
        UNIT_PART_جو,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_جو,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_جو,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*ترتيب*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_جو,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_جو,

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
        UNIT_JOINS_NAMESPACE_جو,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_اندريون_ٽڪرو,
        UNIT_ٽڪرو_جو,

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

تعريفنوع تعداد ڪم_گهڻائي {
        ڪم_FAIL,                 /* Fail if a conflicting job is already queued */
        ڪم_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        ڪم_REPLACE,              /* Replace an existing conflicting job */
        ڪم_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        ڪم_ISOLATE,              /* Start a unit, and stop all others */
        ڪم_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        ڪم_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        ڪم_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        ڪم_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        ڪم_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        ڪم_گهڻائي_MAX,
        ڪم_گهڻائي_INVALID = -EINVAL,
} ڪم_گهڻائي;

تعريفنوع تعداد ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

حرف* unit_dbus_رستو_from_name(ثابت حرف *name);
صحيح unit_name_from_dbus_رستو(ثابت حرف *رستو, حرف **name);

ثابت حرف* unit_dbus_interface_from_type(UnitType t);
ثابت حرف* unit_dbus_interface_from_name(ثابت حرف *name);

DECLARE_STRING_جدول_LOOKUP(unit_type, UnitType);
فراغ unit_types_list(فراغ);

DECLARE_STRING_جدول_LOOKUP(unit_ڀريو_state, Unit_ڀريو_State);
DECLARE_STRING_جدول_LOOKUP(unit_سرگرم_state, Unit_سرگرم_State);
ثابت حرف* unit_type_منزل_capitalized_string(UnitType t);

DECLARE_STRING_جدول_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_پڄاڻي(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_جدول_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_جدول_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_جدول_LOOKUP(device_state, DeviceState);
DECLARE_STRING_جدول_LOOKUP(mount_state, MountState);
DECLARE_STRING_جدول_LOOKUP(رستو_state, رستو_State);
DECLARE_STRING_جدول_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_جدول_LOOKUP(service_state, ServiceState);
DECLARE_STRING_جدول_LOOKUP(ٽڪرو_state, ٽڪرو_State);
DECLARE_STRING_جدول_LOOKUP(socket_state, SocketState);
DECLARE_STRING_جدول_LOOKUP(مٽا_سٽا_state, مٽا_سٽا_State);
DECLARE_STRING_جدول_LOOKUP(هدف_state, هدف_State);
DECLARE_STRING_جدول_LOOKUP(timer_state, TimerState);
DECLARE_STRING_جدول_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_جدول_LOOKUP(ڪم_گهڻائي_2, ڪم_گهڻائي);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_جدول_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_سرگرم_state_منزل_glyph(Unit_سرگرم_State state);
