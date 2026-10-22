/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* The enum order is used to order unit jobs in the job queue
 * when other criteria (cpu weight, nice level) are identical.
 * In this case service units have the highest priority. */
प्रकारनिर्देश गणना UnitType {
        UNIT_SERVICE,
        UNIT_MOUNT,
        UNIT_SWAP,
        UNIT_SOCKET,
        UNIT_TARGET,
        UNIT_DEVICE,
        UNIT_AUTOMOUNT,
        UNIT_TIMER,
        UNIT_मार्गः,
        UNIT_खण्डिका,
        UNIT_SCOPE,
        _UNIT_TYPE_MAX,
        _UNIT_TYPE_INVALID = -EINVAL,
        _UNIT_TYPE_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} UnitType;

प्रकारनिर्देश गणना Unit_आरोपय_State {
        UNIT_STUB,
        UNIT_LOADED,
        UNIT_NOT_प्राप्तम्,    /* error condition #1: unit file not found */
        UNIT_BAD_SETTING,  /* error condition #2: we couldn't parse some essential unit file setting */
        UNIT_ERROR,        /* error condition #3: other "system" error, catchall for the rest */
        UNIT_MERGED,
        UNIT_MASKED,
        UNIT_आरोपय_STATE_MAX,
        UNIT_आरोपय_STATE_INVALID = -EINVAL,
} Unit_आरोपय_State;

प्रकारनिर्देश गणना Unit_सक्रिय_State {
        UNIT_सक्रिय,
        UNIT_RELOADING,
        UNIT_INACTIVE,
        UNIT_FAILED,
        UNIT_ACTIVATING,
        UNIT_DEACTIVATING,
        UNIT_MAINTENANCE,
        UNIT_REFRESHING,
        UNIT_सक्रिय_STATE_MAX,
        UNIT_सक्रिय_STATE_INVALID = -EINVAL,
} Unit_सक्रिय_State;

प्रकारनिर्देश गणना FreezerState {
        FREEZER_RUNNING,
        FREEZER_FREEZING, /* freezing due to user request */
        FREEZER_FROZEN,
        FREEZER_FREEZING_BY_PARENT, /* freezing as a result of parent slice freezing */
        FREEZER_FROZEN_BY_PARENT,
        FREEZER_THAWING,
        _FREEZER_STATE_MAX,
        _FREEZER_STATE_INVALID = -EINVAL,
} FreezerState;

प्रकारनिर्देश गणना UnitMarker {
        UNIT_MARKER_NEEDS_RELOAD,
        UNIT_MARKER_NEEDS_RESTART,
        UNIT_MARKER_NEEDS_STOP,
        UNIT_MARKER_NEEDS_START,
        _UNIT_MARKER_MAX,
        _UNIT_MARKER_INVALID = -EINVAL,
} UnitMarker;

प्रकारनिर्देश गणना AutomountState {
        AUTOMOUNT_DEAD,
        AUTOMOUNT_प्रतीक्षा,
        AUTOMOUNT_RUNNING,
        AUTOMOUNT_FAILED,
        _AUTOMOUNT_STATE_MAX,
        _AUTOMOUNT_STATE_INVALID = -EINVAL,
} AutomountState;

/* We simply watch devices, we cannot plug/unplug them. That
 * simplifies the state engine greatly */
प्रकारनिर्देश गणना DeviceState {
        DEVICE_DEAD,
        DEVICE_TENTATIVE, /* mounted or swapped, but not (yet) announced by udev */
        DEVICE_PLUGGED,   /* announced by udev */
        _DEVICE_STATE_MAX,
        _DEVICE_STATE_INVALID = -EINVAL,
} DeviceState;

प्रकारनिर्देश गणना MountState {
        MOUNT_DEAD,
        MOUNT_MOUNTING,               /* /usr/bin/mount is running, but the mount is not done yet. */
        MOUNT_MOUNTING_समाप्तम्,          /* /usr/bin/mount is running, and the mount is done. */
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

प्रकारनिर्देश गणना मार्गः_State {
        मार्गः_DEAD,
        मार्गः_प्रतीक्षा,
        मार्गः_RUNNING,
        मार्गः_FAILED,
        मार्गः_STATE_MAX,
        मार्गः_STATE_INVALID = -EINVAL,
} मार्गः_State;

प्रकारनिर्देश गणना ScopeState {
        SCOPE_DEAD,
        SCOPE_START_CHOWN,
        SCOPE_RUNNING,
        SCOPE_ABANDONED,
        SCOPE_STOP_SIGTERM,
        SCOPE_STOP_SIGKILL,
        SCOPE_FAILED,
        _SCOPE_STATE_MAX,
        _SCOPE_STATE_INVALID = -EINVAL,
} ScopeState;

प्रकारनिर्देश गणना ServiceState {
        SERVICE_DEAD,
        SERVICE_CONDITION,
        SERVICE_START_PRE,
        SERVICE_START,
        SERVICE_START_POST,
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

प्रकारनिर्देश गणना खण्डिका_State {
        खण्डिका_DEAD,
        खण्डिका_सक्रिय,
        खण्डिका_STATE_MAX,
        खण्डिका_STATE_INVALID = -EINVAL,
} खण्डिका_State;

प्रकारनिर्देश गणना SocketState {
        SOCKET_DEAD,
        SOCKET_START_PRE,
        SOCKET_START_OPEN,
        SOCKET_START_CHOWN,
        SOCKET_START_POST,
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

प्रकारनिर्देश गणना SwapState {
        SWAP_DEAD,
        SWAP_ACTIVATING,               /* /sbin/swapon is running, but the swap not yet enabled. */
        SWAP_ACTIVATING_समाप्तम्,          /* /sbin/swapon is running, and the swap is done. */
        SWAP_सक्रिय,
        SWAP_DEACTIVATING,
        SWAP_DEACTIVATING_SIGTERM,
        SWAP_DEACTIVATING_SIGKILL,
        SWAP_FAILED,
        SWAP_CLEANING,
        _SWAP_STATE_MAX,
        _SWAP_STATE_INVALID = -EINVAL,
} SwapState;

प्रकारनिर्देश गणना TargetState {
        TARGET_DEAD,
        TARGET_सक्रिय,
        _TARGET_STATE_MAX,
        _TARGET_STATE_INVALID = -EINVAL,
} TargetState;

प्रकारनिर्देश गणना TimerState {
        TIMER_DEAD,
        TIMER_प्रतीक्षा,
        TIMER_RUNNING,
        TIMER_ELAPSED,
        TIMER_FAILED,
        _TIMER_STATE_MAX,
        _TIMER_STATE_INVALID = -EINVAL,
} TimerState;

प्रकारनिर्देश गणना UnitDependency {
        /* Positive dependencies */
        UNIT_REQUIRES,
        UNIT_REQUISITE,
        UNIT_इच्छति,
        UNIT_BINDS_गन्तव्यम्,
        UNIT_PART_सम्बन्धि,
        UNIT_UPHOLDS,

        /* Inverse of the above */
        UNIT_REQUIRED_BY,             /* inverse of 'requires' is 'required_by' */
        UNIT_REQUISITE_सम्बन्धि,            /* inverse of 'requisite' is 'requisite_of' */
        UNIT_WANTED_BY,               /* inverse of 'wants' */
        UNIT_BOUND_BY,                /* inverse of 'binds_to' */
        UNIT_CONSISTS_सम्बन्धि,             /* inverse of 'part_of' */
        UNIT_UPHELD_BY,               /* inverse of 'uphold' */

        /* Negative dependencies */
        UNIT_CONFLICTS,               /* inverse of 'conflicts' is 'conflicted_by' */
        UNIT_CONFLICTED_BY,

        /*क्रमः*/
        UNIT_BEFORE,                  /* inverse of 'before' is 'after' and vice versa */
        UNIT_AFTER,

        /* OnSuccess= + OnFailure= */
        UNIT_ON_SUCCESS,
        UNIT_ON_SUCCESS_सम्बन्धि,
        UNIT_ON_FAILURE,
        UNIT_ON_FAILURE_सम्बन्धि,

        /* Triggers (i.e. a socket triggers a service) */
        UNIT_TRIGGERS,
        UNIT_TRIGGERED_BY,

        /* Propagate reloads */
        UNIT_PROPAGATES_RELOAD_गन्तव्यम्,
        UNIT_RELOAD_PROPAGATED_FROM,

        /* Propagate stops */
        UNIT_PROPAGATES_STOP_गन्तव्यम्,
        UNIT_STOP_PROPAGATED_FROM,

        /* Joins namespace of */
        UNIT_JOINS_NAMESPACE_सम्बन्धि,

        /* Reference information for GC logic */
        UNIT_REFERENCES,              /* Inverse of 'references' is 'referenced_by' */
        UNIT_REFERENCED_BY,

        /* Slice= */
        UNIT_अन्तर्_खण्डिका,
        UNIT_खण्डिका_सम्बन्धि,

        _UNIT_DEPENDENCY_MAX,
        _UNIT_DEPENDENCY_INVALID = -EINVAL,
} UnitDependency;

प्रकारनिर्देश गणना NotifyAccess {
        NOTIFY_NONE,
        NOTIFY_ALL,
        NOTIFY_MAIN,
        NOTIFY_EXEC,
        _NOTIFY_ACCESS_MAX,
        _NOTIFY_ACCESS_INVALID = -EINVAL,
} NotifyAccess;

प्रकारनिर्देश गणना Job_बहुलकम् {
        JOB_FAIL,                 /* Fail if a conflicting job is already queued */
        JOB_LENIENT,              /* Fail if any conflicting unit is active (even weaker than JOB_FAIL) */
        JOB_REPLACE,              /* Replace an existing conflicting job */
        JOB_REPLACE_IRREVERSIBLY, /* Like JOB_REPLACE + produce irreversible jobs */
        JOB_ISOLATE,              /* Start a unit, and stop all others */
        JOB_FLUSH,                /* Flush out all other queued jobs when queueing this one */
        JOB_IGNORE_DEPENDENCIES,  /* Ignore both requirement and ordering dependencies */
        JOB_IGNORE_REQUIREMENTS,  /* Ignore requirement dependencies */
        JOB_TRIGGERING,           /* Adds TRIGGERED_BY dependencies to the same transaction */
        JOB_RESTART_DEPENDENCIES, /* A "start" job for the specified unit becomes "restart" for depending units */
        JOB_बहुलकम्_MAX,
        JOB_बहुलकम्_INVALID = -EINVAL,
} Job_बहुलकम्;

प्रकारनिर्देश गणना ExecDirectoryType {
        EXEC_DIRECTORY_RUNTIME,
        EXEC_DIRECTORY_STATE,
        EXEC_DIRECTORY_CACHE,
        EXEC_DIRECTORY_LOGS,
        EXEC_DIRECTORY_CONFIGURATION,
        _EXEC_DIRECTORY_TYPE_MAX,
        _EXEC_DIRECTORY_TYPE_INVALID = -EINVAL,
} ExecDirectoryType;

वर्ण* unit_dbus_मार्गः_from_name(ध्रुव वर्ण *name);
पूर्णाङ्क unit_name_from_dbus_मार्गः(ध्रुव वर्ण *मार्गः, वर्ण **name);

ध्रुव वर्ण* unit_dbus_interface_from_type(UnitType t);
ध्रुव वर्ण* unit_dbus_interface_from_name(ध्रुव वर्ण *name);

DECLARE_STRING_सारणी_LOOKUP(unit_type, UnitType);
शून्य unit_types_list(शून्य);

DECLARE_STRING_सारणी_LOOKUP(unit_आरोपय_state, Unit_आरोपय_State);
DECLARE_STRING_सारणी_LOOKUP(unit_सक्रिय_state, Unit_सक्रिय_State);
ध्रुव वर्ण* unit_type_गन्तव्यम्_capitalized_string(UnitType t);

DECLARE_STRING_सारणी_LOOKUP(freezer_state, FreezerState);
FreezerState freezer_state_finish(FreezerState state) _const_;
FreezerState freezer_state_objective(FreezerState state) _const_;

DECLARE_STRING_सारणी_LOOKUP(unit_dependency, UnitDependency);
bool unit_dependency_can_be_transient(UnitDependency d) _const_;

DECLARE_STRING_सारणी_LOOKUP(unit_marker, UnitMarker);
DECLARE_STRING_सारणी_LOOKUP(automount_state, AutomountState);
DECLARE_STRING_सारणी_LOOKUP(device_state, DeviceState);
DECLARE_STRING_सारणी_LOOKUP(mount_state, MountState);
DECLARE_STRING_सारणी_LOOKUP(मार्गः_state, मार्गः_State);
DECLARE_STRING_सारणी_LOOKUP(scope_state, ScopeState);
DECLARE_STRING_सारणी_LOOKUP(service_state, ServiceState);
DECLARE_STRING_सारणी_LOOKUP(खण्डिका_state, खण्डिका_State);
DECLARE_STRING_सारणी_LOOKUP(socket_state, SocketState);
DECLARE_STRING_सारणी_LOOKUP(swap_state, SwapState);
DECLARE_STRING_सारणी_LOOKUP(target_state, TargetState);
DECLARE_STRING_सारणी_LOOKUP(timer_state, TimerState);
DECLARE_STRING_सारणी_LOOKUP(notify_access, NotifyAccess);
DECLARE_STRING_सारणी_LOOKUP(job_बहुलकम्, Job_बहुलकम्);
DECLARE_STRING_सारणी_LOOKUP(exec_directory_type, ExecDirectoryType);
DECLARE_STRING_सारणी_LOOKUP(exec_directory_type_symlink, ExecDirectoryType);

Glyph unit_सक्रिय_state_गन्तव्यम्_glyph(Unit_सक्रिय_State state);
