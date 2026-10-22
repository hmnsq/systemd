/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <string.h>

#ضمن "errno-util.h"
#ضمن "fanotify-util.h"
#ضمن "fd-util.h"
#ضمن "stat-util.h"
#ضمن "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ثابت بنية fanotify_event_metadata **ret) {
        أكد(i);
        أكد(ret);

        إذا (i->يسار < حجم(بنية fanotify_event_metadata))
                أرجع false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        إذا (event_len < حجم(بنية fanotify_event_metadata) ||
            event_len > i->يسار ||
            event_len > حجم(i->aligned.raw))
                أرجع false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->يسار -= event_len;

        *ret = &i->aligned.em;
        أرجع true;
}

ساكن ثابت بنية fanotify_event_info_header* fanotify_event_info_header_first(ثابت بنية fanotify_event_metadata *e) {
        أكد(e);

        أرجع (ثابت بنية fanotify_event_info_header*) ((ثابت uint8_t*) e + e->metadata_len);
}

ساكن bool fanotify_event_info_header_is_valid(
                ثابت بنية fanotify_event_info_header *h,
                ثابت بنية fanotify_event_metadata *e) {

        أكد(h);
        أكد(e);

        أرجع (ثابت uint8_t*) h + حجم(*h) <= (ثابت uint8_t*) e + e->event_len &&
                h->len >= حجم(*h) &&
                (ثابت uint8_t*) h + h->len <= (ثابت uint8_t*) e + e->event_len;
}

ساكن ثابت بنية fanotify_event_info_header* fanotify_event_info_header_next(ثابت بنية fanotify_event_info_header *h) {
        أكد(h);

        أرجع (ثابت بنية fanotify_event_info_header*) ((ثابت uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        لكل (ثابت بنية fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(بنية fanotify_event_info_fid, handle) + offsetof(بنية file_handle, f_handle))

صحيح fanotify_event_pin_fd(
                ثابت بنية fanotify_event_metadata *e,
                صحيح mount_fd,
                ثابت بنية statfs *sfs) {

        أكد(e);
        أكد(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                إذا (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        استمر;

                إذا (h->len < FID_MIN_SIZE)
                        أرجع -EBADMSG;

                ثابت بنية fanotify_event_info_fid *fid = (ثابت بنية fanotify_event_info_fid*) h;
                بنية file_handle *fh = (بنية file_handle*) fid->handle;

                إذا (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        أرجع -EBADMSG;

                بنية statfs our_sfs;
                إذا (!statfs_is_set(sfs)) {
                        إذا (fstatfs(mount_fd, &our_sfs) < 0)
                                أرجع -errno;

                        sfs = &our_sfs;
                }

                إذا (memcmp(&fid->fsid, &sfs->f_fsid, حجم(fid->fsid)) != 0)
                        أرجع -ESTALE;

                أرجع RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        أرجع -ENODATA;
}

صحيح fanotify_mark_fd(صحيح fanotify_fd, unsigned flags, uint64_t mask, صحيح fd) {
        أكد(fanotify_fd >= 0);
        أكد(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        أرجع RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
