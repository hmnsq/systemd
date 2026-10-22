/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "inotify-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "memory-util.h"

bool inotify_event_తదుపరి(
                సంఘం inotify_event_బఫరు *బఫరు,
                size_t కొలత,
                నిర్మాణం inotify_event **iterator,
                పూర్ణాంకం log_level) {

        నిర్మాణం inotify_event *e;
        size_t స్థానభ్రంశం = 0;

        ప్రతిజ్ఞా(బఫరు);
        ప్రతిజ్ఞా(iterator);

        ఒకవేళ (*iterator) {
                ప్రతిజ్ఞా((uint8_t*) *iterator >= బఫరు->raw);
                స్థానభ్రంశం = (uint8_t*) *iterator - బఫరు->raw;
                స్థానభ్రంశం += offsetof(నిర్మాణం inotify_event, name) + (*iterator)->len;
        }

        ఒకవేళ (కొలత == స్థానభ్రంశం)
                తిరిగి false; /* reached end of list */

        ఒకవేళ (కొలత < స్థానభ్రంశం ||
            కొలత - స్థానభ్రంశం < offsetof(నిర్మాణం inotify_event, name)) {
                log_నిండిన(log_level, "Received invalid inotify event, ignoring.");
                తిరిగి false;
        }

        e = CAST_ALIGN_PTR(నిర్మాణం inotify_event, బఫరు->raw + స్థానభ్రంశం);
        ఒకవేళ (కొలత - స్థానభ్రంశం - offsetof(నిర్మాణం inotify_event, name) < e->len) {
                log_నిండిన(log_level, "Received invalid inotify event, ignoring.");
                తిరిగి false;
        }

        *iterator = e;
        తిరిగి true;
}

పూర్ణాంకం inotify_కూడు_watch_fd(పూర్ణాంకం fd, పూర్ణాంకం what, uint32_t ముసుగు_3) {
        పూర్ణాంకం wd;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_మార్గం(what), ముసుగు_3);
        ఒకవేళ (wd < 0) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                తిరిగి proc_fd_enoent_errno();
        }

        తిరిగి wd;
}

పూర్ణాంకం inotify_కూడు_watch_and_warn(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *pathname, uint32_t ముసుగు_3) {
        పూర్ణాంకం wd;

        wd = inotify_add_watch(fd, pathname, ముసుగు_3);
        ఒకవేళ (wd < 0) {
                ఒకవేళ (errno == ENOSPC)
                        తిరిగి log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                తిరిగి log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        తిరిగి wd;
}
