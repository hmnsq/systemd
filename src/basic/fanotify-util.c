/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fanotify-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "unaligned.h"

bool fanotify_event_iterator_अग्र(FanotifyEventIterator *i, ध्रुव संरचना fanotify_event_metadata **ret) {
        प्रतिज्ञा(i);
        प्रतिज्ञा(ret);

        यदि (i->वामः < परिमाण(संरचना fanotify_event_metadata))
                प्रत्यागम false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        यदि (event_len < परिमाण(संरचना fanotify_event_metadata) ||
            event_len > i->वामः ||
            event_len > परिमाण(i->aligned.raw))
                प्रत्यागम false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        स्मृतिं_प्रतिलिख(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->वामः -= event_len;

        *ret = &i->aligned.em;
        प्रत्यागम true;
}

स्थिर ध्रुव संरचना fanotify_event_info_header* fanotify_event_info_शीर्षकम्_first(ध्रुव संरचना fanotify_event_metadata *e) {
        प्रतिज्ञा(e);

        प्रत्यागम (ध्रुव संरचना fanotify_event_info_header*) ((ध्रुव uint8_t*) e + e->metadata_len);
}

स्थिर bool fanotify_event_info_शीर्षकम्_is_valid(
                ध्रुव संरचना fanotify_event_info_header *h,
                ध्रुव संरचना fanotify_event_metadata *e) {

        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        प्रत्यागम (ध्रुव uint8_t*) h + परिमाण(*h) <= (ध्रुव uint8_t*) e + e->event_len &&
                h->len >= परिमाण(*h) &&
                (ध्रुव uint8_t*) h + h->len <= (ध्रुव uint8_t*) e + e->event_len;
}

स्थिर ध्रुव संरचना fanotify_event_info_header* fanotify_event_info_शीर्षकम्_अग्र(ध्रुव संरचना fanotify_event_info_header *h) {
        प्रतिज्ञा(h);

        प्रत्यागम (ध्रुव संरचना fanotify_event_info_header*) ((ध्रुव uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#परिभाषय FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        प्रति (ध्रुव संरचना fanotify_event_info_header *h = fanotify_event_info_शीर्षकम्_first(e); \
             fanotify_event_info_शीर्षकम्_is_valid(h, e);                 \
             h = fanotify_event_info_शीर्षकम्_अग्र(h))

#परिभाषय FID_MIN_आकार (offsetof(संरचना fanotify_event_info_fid, handle) + offsetof(संरचना file_handle, f_handle))

पूर्णाङ्क fanotify_event_pin_fd(
                ध्रुव संरचना fanotify_event_metadata *e,
                पूर्णाङ्क mount_fd,
                ध्रुव संरचना statfs *sfs) {

        प्रतिज्ञा(e);
        प्रतिज्ञा(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                यदि (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        अनुवर्त;

                यदि (h->len < FID_MIN_आकार)
                        प्रत्यागम -EBADMSG;

                ध्रुव संरचना fanotify_event_info_fid *fid = (ध्रुव संरचना fanotify_event_info_fid*) h;
                संरचना file_handle *fh = (संरचना file_handle*) fid->handle;

                यदि (h->len < FID_MIN_आकार + fh->handle_अष्टकानि)
                        प्रत्यागम -EBADMSG;

                संरचना statfs our_sfs;
                यदि (!statfs_is_समुच्चयः(sfs)) {
                        यदि (fstatfs(mount_fd, &our_sfs) < 0)
                                प्रत्यागम -errno;

                        sfs = &our_sfs;
                }

                यदि (memcmp(&fid->fsid, &sfs->f_fsid, परिमाण(fid->fsid)) != 0)
                        प्रत्यागम -ESTALE;

                प्रत्यागम RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_मार्गः));
        }

        प्रत्यागम -ENODATA;
}

पूर्णाङ्क fanotify_mark_fd(पूर्णाङ्क fanotify_fd, अचिह्नित flags, uint64_t आवरणम्_3, पूर्णाङ्क fd) {
        प्रतिज्ञा(fanotify_fd >= 0);
        प्रतिज्ञा(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        प्रत्यागम RET_NERRNO(fanotify_mark(fanotify_fd, flags, आवरणम्_3, AT_FDCWD, FORMAT_PROC_FD_मार्गः(fd)));
}
