/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <string.h>

#شامل "errno-util.h"
#شامل "fanotify-util.h"
#شامل "fd-util.h"
#شامل "stat-util.h"
#شامل "unaligned.h"

bool fanotify_event_iterator_اگلا(FanotifyEventIterator *i, ثابت ساخت fanotify_event_metadata **ret) {
        أكد(i);
        أكد(ret);

        اگر (i->بایاں < حجم(ساخت fanotify_event_metadata))
                واپس false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        اگر (event_len < حجم(ساخت fanotify_event_metadata) ||
            event_len > i->بایاں ||
            event_len > حجم(i->aligned.raw))
                واپس false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        حافظہ_نقل(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->بایاں -= event_len;

        *ret = &i->aligned.em;
        واپس true;
}

ساکن ثابت ساخت fanotify_event_info_header* fanotify_event_info_سرخی_پہلا(ثابت ساخت fanotify_event_metadata *e) {
        أكد(e);

        واپس (ثابت ساخت fanotify_event_info_header*) ((ثابت uint8_t*) e + e->metadata_len);
}

ساکن bool fanotify_event_info_سرخی_is_valid(
                ثابت ساخت fanotify_event_info_header *h,
                ثابت ساخت fanotify_event_metadata *e) {

        أكد(h);
        أكد(e);

        واپس (ثابت uint8_t*) h + حجم(*h) <= (ثابت uint8_t*) e + e->event_len &&
                h->len >= حجم(*h) &&
                (ثابت uint8_t*) h + h->len <= (ثابت uint8_t*) e + e->event_len;
}

ساکن ثابت ساخت fanotify_event_info_header* fanotify_event_info_سرخی_اگلا(ثابت ساخت fanotify_event_info_header *h) {
        أكد(h);

        واپس (ثابت ساخت fanotify_event_info_header*) ((ثابت uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#تعریف_کرو FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        برائے (ثابت ساخت fanotify_event_info_header *h = fanotify_event_info_سرخی_پہلا(e); \
             fanotify_event_info_سرخی_is_valid(h, e);                 \
             h = fanotify_event_info_سرخی_اگلا(h))

#تعریف_کرو FID_MIN_سائز (offsetof(ساخت fanotify_event_info_fid, handle) + offsetof(ساخت file_handle, f_handle))

صحیح fanotify_event_pin_fd(
                ثابت ساخت fanotify_event_metadata *e,
                صحیح mount_fd,
                ثابت ساخت statfs *sfs) {

        أكد(e);
        أكد(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                اگر (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        جاری;

                اگر (h->len < FID_MIN_سائز)
                        واپس -EBADMSG;

                ثابت ساخت fanotify_event_info_fid *fid = (ثابت ساخت fanotify_event_info_fid*) h;
                ساخت file_handle *fh = (ساخت file_handle*) fid->handle;

                اگر (h->len < FID_MIN_سائز + fh->handle_بائٹیں)
                        واپس -EBADMSG;

                ساخت statfs our_sfs;
                اگر (!statfs_is_سیٹ(sfs)) {
                        اگر (fstatfs(mount_fd, &our_sfs) < 0)
                                واپس -errno;

                        sfs = &our_sfs;
                }

                اگر (memcmp(&fid->fsid, &sfs->f_fsid, حجم(fid->fsid)) != 0)
                        واپس -ESTALE;

                واپس RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_راستہ));
        }

        واپس -ENODATA;
}

صحیح fanotify_mark_fd(صحیح fanotify_fd, غیرعلامتی flags, uint64_t نقاب_3, صحیح fd) {
        أكد(fanotify_fd >= 0);
        أكد(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        واپس RET_NERRNO(fanotify_mark(fanotify_fd, flags, نقاب_3, AT_FDCWD, FORMAT_PROC_FD_راستہ(fd)));
}
