/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <string.h>

#شامل "errno-util.h"
#شامل "fanotify-util.h"
#شامل "fd-util.h"
#شامل "stat-util.h"
#شامل "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ثابت ساختار fanotify_event_metadata **ret) {
        أكد(i);
        أكد(ret);

        اگر (i->چپ < اندازه(ساختار fanotify_event_metadata))
                بازگشت false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        اگر (event_len < اندازه(ساختار fanotify_event_metadata) ||
            event_len > i->چپ ||
            event_len > اندازه(i->aligned.raw))
                بازگشت false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->چپ -= event_len;

        *ret = &i->aligned.em;
        بازگشت true;
}

ایستا ثابت ساختار fanotify_event_info_header* fanotify_event_info_header_first(ثابت ساختار fanotify_event_metadata *e) {
        أكد(e);

        بازگشت (ثابت ساختار fanotify_event_info_header*) ((ثابت uint8_t*) e + e->metadata_len);
}

ایستا bool fanotify_event_info_header_is_valid(
                ثابت ساختار fanotify_event_info_header *h,
                ثابت ساختار fanotify_event_metadata *e) {

        أكد(h);
        أكد(e);

        بازگشت (ثابت uint8_t*) h + اندازه(*h) <= (ثابت uint8_t*) e + e->event_len &&
                h->len >= اندازه(*h) &&
                (ثابت uint8_t*) h + h->len <= (ثابت uint8_t*) e + e->event_len;
}

ایستا ثابت ساختار fanotify_event_info_header* fanotify_event_info_header_next(ثابت ساختار fanotify_event_info_header *h) {
        أكد(h);

        بازگشت (ثابت ساختار fanotify_event_info_header*) ((ثابت uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        برای (ثابت ساختار fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(ساختار fanotify_event_info_fid, handle) + offsetof(ساختار file_handle, f_handle))

صحیح fanotify_event_pin_fd(
                ثابت ساختار fanotify_event_metadata *e,
                صحیح mount_fd,
                ثابت ساختار statfs *sfs) {

        أكد(e);
        أكد(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                اگر (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ادامه;

                اگر (h->len < FID_MIN_SIZE)
                        بازگشت -EBADMSG;

                ثابت ساختار fanotify_event_info_fid *fid = (ثابت ساختار fanotify_event_info_fid*) h;
                ساختار file_handle *fh = (ساختار file_handle*) fid->handle;

                اگر (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        بازگشت -EBADMSG;

                ساختار statfs our_sfs;
                اگر (!statfs_is_set(sfs)) {
                        اگر (fstatfs(mount_fd, &our_sfs) < 0)
                                بازگشت -errno;

                        sfs = &our_sfs;
                }

                اگر (memcmp(&fid->fsid, &sfs->f_fsid, اندازه(fid->fsid)) != 0)
                        بازگشت -ESTALE;

                بازگشت RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        بازگشت -ENODATA;
}

صحیح fanotify_mark_fd(صحیح fanotify_fd, unsigned flags, uint64_t mask, صحیح fd) {
        أكد(fanotify_fd >= 0);
        أكد(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        بازگشت RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
