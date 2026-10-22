/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fanotify-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, अचर संरचना fanotify_event_metadata **ret) {
        प्रतिज्ञा(i);
        प्रतिज्ञा(ret);

        यदि (i->बायाँ < परिमाण(संरचना fanotify_event_metadata))
                फर्काऊ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        यदि (event_len < परिमाण(संरचना fanotify_event_metadata) ||
            event_len > i->बायाँ ||
            event_len > परिमाण(i->aligned.raw))
                फर्काऊ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        स_नकल(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->बायाँ -= event_len;

        *ret = &i->aligned.em;
        फर्काऊ true;
}

स्थिर अचर संरचना fanotify_event_info_header* fanotify_event_info_header_first(अचर संरचना fanotify_event_metadata *e) {
        प्रतिज्ञा(e);

        फर्काऊ (अचर संरचना fanotify_event_info_header*) ((अचर uint8_t*) e + e->metadata_len);
}

स्थिर bool fanotify_event_info_header_is_valid(
                अचर संरचना fanotify_event_info_header *h,
                अचर संरचना fanotify_event_metadata *e) {

        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        फर्काऊ (अचर uint8_t*) h + परिमाण(*h) <= (अचर uint8_t*) e + e->event_len &&
                h->len >= परिमाण(*h) &&
                (अचर uint8_t*) h + h->len <= (अचर uint8_t*) e + e->event_len;
}

स्थिर अचर संरचना fanotify_event_info_header* fanotify_event_info_header_next(अचर संरचना fanotify_event_info_header *h) {
        प्रतिज्ञा(h);

        फर्काऊ (अचर संरचना fanotify_event_info_header*) ((अचर uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        लागि (अचर संरचना fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (दुरत्व(संरचना fanotify_event_info_fid, handle) + दुरत्व(संरचना file_handle, f_handle))

पूर्णाङ्क fanotify_event_pin_fd(
                अचर संरचना fanotify_event_metadata *e,
                पूर्णाङ्क mount_fd,
                अचर संरचना statfs *sfs) {

        प्रतिज्ञा(e);
        प्रतिज्ञा(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                यदि (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        जारी;

                यदि (h->len < FID_MIN_SIZE)
                        फर्काऊ -EBADMSG;

                अचर संरचना fanotify_event_info_fid *fid = (अचर संरचना fanotify_event_info_fid*) h;
                संरचना file_handle *fh = (संरचना file_handle*) fid->handle;

                यदि (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        फर्काऊ -EBADMSG;

                संरचना statfs our_sfs;
                यदि (!statfs_is_set(sfs)) {
                        यदि (fstatfs(mount_fd, &our_sfs) < 0)
                                फर्काऊ -त्रुटि_सं;

                        sfs = &our_sfs;
                }

                यदि (स_भेद(&fid->fsid, &sfs->f_fsid, परिमाण(fid->fsid)) != 0)
                        फर्काऊ -ESTALE;

                फर्काऊ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        फर्काऊ -ENODATA;
}

पूर्णाङ्क fanotify_mark_fd(पूर्णाङ्क fanotify_fd, अचिन्हित flags, uint64_t mask, पूर्णाङ्क fd) {
        प्रतिज्ञा(fanotify_fd >= 0);
        प्रतिज्ञा(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        फर्काऊ RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
