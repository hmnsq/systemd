/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "inotify-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"

bool inotify_event_next(
                ସଂଘ inotify_event_buffer *buffer,
                size_t size,
                ସଂରଚନା inotify_event **iterator,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {

        ସଂରଚନା inotify_event *e;
        size_t offset = 0;

        ପ୍ରତିଜ୍ଞା(buffer);
        ପ୍ରତିଜ୍ଞା(iterator);

        ଯଦି (*iterator) {
                ପ୍ରତିଜ୍ଞା((uint8_t*) *iterator >= buffer->raw);
                offset = (uint8_t*) *iterator - buffer->raw;
                offset += offsetof(ସଂରଚନା inotify_event, name) + (*iterator)->len;
        }

        ଯଦି (size == offset)
                ଫେରନ୍ତୁ false; /* reached end of list */

        ଯଦି (size < offset ||
            size - offset < offsetof(ସଂରଚନା inotify_event, name)) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ଫେରନ୍ତୁ false;
        }

        e = CAST_ALIGN_PTR(ସଂରଚନା inotify_event, buffer->raw + offset);
        ଯଦି (size - offset - offsetof(ସଂରଚନା inotify_event, name) < e->len) {
                log_full(log_level, "Received invalid inotify event, ignoring.");
                ଫେରନ୍ତୁ false;
        }

        *iterator = e;
        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inotify_add_watch_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା what, uint32_t mask) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା wd;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(what >= 0);

        /* This is like inotify_add_watch(), except that the file to watch is not referenced by a path, but by an fd */
        wd = inotify_add_watch(fd, FORMAT_PROC_FD_PATH(what), mask);
        ଯଦି (wd < 0) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ proc_fd_enoent_errno();
        }

        ଫେରନ୍ତୁ wd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inotify_add_watch_and_warn(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *pathname, uint32_t mask) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା wd;

        wd = inotify_add_watch(fd, pathname, mask);
        ଯଦି (wd < 0) {
                ଯଦି (errno == ENOSPC)
                        ଫେରନ୍ତୁ log_error_errno(errno, "Failed to add a watch for %s: inotify watch limit reached", pathname);

                ଫେରନ୍ତୁ log_error_errno(errno, "Failed to add a watch for %s: %m", pathname);
        }

        ଫେରନ୍ତୁ wd;
}
