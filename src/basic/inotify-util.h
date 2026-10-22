/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/inotify.h>        /* IWYU pragma: export */
#అన్తర్భావ <syslog.h>

#అన్తర్భావ "forward.h"

#నిర్వచించు INOTIFY_EVENT_MAX (offsetof(నిర్మాణం inotify_event, name) + NAME_MAX + 1)

/* This evaluates arguments multiple times */
#నిర్వచించు FOREACH_INOTIFY_EVENT_నిండిన(e, బఫరు, sz, log_level)            \
        కొరకు (నిర్మాణం inotify_event *e = ఖాళీ;                            \
             inotify_event_తదుపరి(&బఫరు, sz, &e, log_level); )

#నిర్వచించు FOREACH_INOTIFY_EVENT(e, బఫరు, sz)                    \
        FOREACH_INOTIFY_EVENT_నిండిన(e, బఫరు, sz, LOG_DEBUG)

#నిర్వచించు FOREACH_INOTIFY_EVENT_WARN(e, బఫరు, sz)               \
        FOREACH_INOTIFY_EVENT_నిండిన(e, బఫరు, sz, LOG_WARNING)

సంఘం inotify_event_బఫరు {
        నిర్మాణం inotify_event ev;
        uint8_t raw[INOTIFY_EVENT_MAX];
};

bool inotify_event_తదుపరి(
                సంఘం inotify_event_బఫరు *బఫరు,
                size_t కొలత,
                నిర్మాణం inotify_event **iterator,
                పూర్ణాంకం log_level);

పూర్ణాంకం inotify_కూడు_watch_fd(పూర్ణాంకం fd, పూర్ణాంకం what, uint32_t ముసుగు_3);
పూర్ణాంకం inotify_కూడు_watch_and_warn(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *pathname, uint32_t ముసుగు_3);
