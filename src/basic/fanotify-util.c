/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fanotify-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ध्रुव संरचना fanotify_event_metadata **ret) {
        प्रतिज्ञा(i);
        प्रतिज्ञा(ret);

        सचे (i->वामः < परिमाण(संरचना fanotify_event_metadata))
                पटिनिवत्त false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        सचे (event_len < परिमाण(संरचना fanotify_event_metadata) ||
            event_len > i->वामः ||
            event_len > परिमाण(i->aligned.raw))
                पटिनिवत्त false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->वामः -= event_len;

        *ret = &i->aligned.em;
        पटिनिवत्त true;
}

स्थिर ध्रुव संरचना fanotify_event_info_header* fanotify_event_info_header_first(ध्रुव संरचना fanotify_event_metadata *e) {
        प्रतिज्ञा(e);

        पटिनिवत्त (ध्रुव संरचना fanotify_event_info_header*) ((ध्रुव uint8_t*) e + e->metadata_len);
}

स्थिर bool fanotify_event_info_header_is_valid(
                ध्रुव संरचना fanotify_event_info_header *h,
                ध्रुव संरचना fanotify_event_metadata *e) {

        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        पटिनिवत्त (ध्रुव uint8_t*) h + परिमाण(*h) <= (ध्रुव uint8_t*) e + e->event_len &&
                h->len >= परिमाण(*h) &&
                (ध्रुव uint8_t*) h + h->len <= (ध्रुव uint8_t*) e + e->event_len;
}

स्थिर ध्रुव संरचना fanotify_event_info_header* fanotify_event_info_header_next(ध्रुव संरचना fanotify_event_info_header *h) {
        प्रतिज्ञा(h);

        पटिनिवत्त (ध्रुव संरचना fanotify_event_info_header*) ((ध्रुव uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        अत्थाय (ध्रुव संरचना fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(संरचना fanotify_event_info_fid, handle) + offsetof(संरचना file_handle, f_handle))

पूर्णाङ्क fanotify_event_pin_fd(
                ध्रुव संरचना fanotify_event_metadata *e,
                पूर्णाङ्क mount_fd,
                ध्रुव संरचना statfs *sfs) {

        प्रतिज्ञा(e);
        प्रतिज्ञा(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                सचे (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        अनुवर्त;

                सचे (h->len < FID_MIN_SIZE)
                        पटिनिवत्त -EBADMSG;

                ध्रुव संरचना fanotify_event_info_fid *fid = (ध्रुव संरचना fanotify_event_info_fid*) h;
                संरचना file_handle *fh = (संरचना file_handle*) fid->handle;

                सचे (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        पटिनिवत्त -EBADMSG;

                संरचना statfs our_sfs;
                सचे (!statfs_is_set(sfs)) {
                        सचे (fstatfs(mount_fd, &our_sfs) < 0)
                                पटिनिवत्त -errno;

                        sfs = &our_sfs;
                }

                सचे (memcmp(&fid->fsid, &sfs->f_fsid, परिमाण(fid->fsid)) != 0)
                        पटिनिवत्त -ESTALE;

                पटिनिवत्त RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        पटिनिवत्त -ENODATA;
}

पूर्णाङ्क fanotify_mark_fd(पूर्णाङ्क fanotify_fd, unsigned flags, uint64_t mask, पूर्णाङ्क fd) {
        प्रतिज्ञा(fanotify_fd >= 0);
        प्रतिज्ञा(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        पटिनिवत्त RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
