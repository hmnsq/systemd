/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fanotify-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"

bool fanotify_event_iterator_ਅਗਲਾ(FanotifyEventIterator *i, ਅਚਲ ਬਣਤਰ fanotify_event_metadata **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(i);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (i->ਖੱਬਾ < ਆਕਾਰ(ਬਣਤਰ fanotify_event_metadata))
                ਵਾਪਸ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ਜੇ (event_len < ਆਕਾਰ(ਬਣਤਰ fanotify_event_metadata) ||
            event_len > i->ਖੱਬਾ ||
            event_len > ਆਕਾਰ(i->aligned.raw))
                ਵਾਪਸ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ਖੱਬਾ -= event_len;

        *ret = &i->aligned.em;
        ਵਾਪਸ true;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fanotify_event_info_header* fanotify_event_info_ਸਿਰਲੇਖ_ਪਹਿਲਾ(ਅਚਲ ਬਣਤਰ fanotify_event_metadata *e) {
        ਪ੍ਰਤਿਜ੍ਞਾ(e);

        ਵਾਪਸ (ਅਚਲ ਬਣਤਰ fanotify_event_info_header*) ((ਅਚਲ uint8_t*) e + e->metadata_len);
}

ਸਥਿਰ bool fanotify_event_info_ਸਿਰਲੇਖ_is_valid(
                ਅਚਲ ਬਣਤਰ fanotify_event_info_header *h,
                ਅਚਲ ਬਣਤਰ fanotify_event_metadata *e) {

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(e);

        ਵਾਪਸ (ਅਚਲ uint8_t*) h + ਆਕਾਰ(*h) <= (ਅਚਲ uint8_t*) e + e->event_len &&
                h->len >= ਆਕਾਰ(*h) &&
                (ਅਚਲ uint8_t*) h + h->len <= (ਅਚਲ uint8_t*) e + e->event_len;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ fanotify_event_info_header* fanotify_event_info_ਸਿਰਲੇਖ_ਅਗਲਾ(ਅਚਲ ਬਣਤਰ fanotify_event_info_header *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        ਵਾਪਸ (ਅਚਲ ਬਣਤਰ fanotify_event_info_header*) ((ਅਚਲ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        ਲਈ (ਅਚਲ ਬਣਤਰ fanotify_event_info_header *h = fanotify_event_info_ਸਿਰਲੇਖ_ਪਹਿਲਾ(e); \
             fanotify_event_info_ਸਿਰਲੇਖ_is_valid(h, e);                 \
             h = fanotify_event_info_ਸਿਰਲੇਖ_ਅਗਲਾ(h))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FID_MIN_ਮਾਪ (offsetof(ਬਣਤਰ fanotify_event_info_fid, handle) + offsetof(ਬਣਤਰ file_handle, f_handle))

ਪੂਰਨ fanotify_event_pin_fd(
                ਅਚਲ ਬਣਤਰ fanotify_event_metadata *e,
                ਪੂਰਨ mount_fd,
                ਅਚਲ ਬਣਤਰ statfs *sfs) {

        ਪ੍ਰਤਿਜ੍ਞਾ(e);
        ਪ੍ਰਤਿਜ੍ਞਾ(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ਜੇ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ਜਾਰੀ;

                ਜੇ (h->len < FID_MIN_ਮਾਪ)
                        ਵਾਪਸ -EBADMSG;

                ਅਚਲ ਬਣਤਰ fanotify_event_info_fid *fid = (ਅਚਲ ਬਣਤਰ fanotify_event_info_fid*) h;
                ਬਣਤਰ file_handle *fh = (ਬਣਤਰ file_handle*) fid->handle;

                ਜੇ (h->len < FID_MIN_ਮਾਪ + fh->handle_ਬਾਈਟਾਂ)
                        ਵਾਪਸ -EBADMSG;

                ਬਣਤਰ statfs our_sfs;
                ਜੇ (!statfs_is_ਸੈੱਟ(sfs)) {
                        ਜੇ (fstatfs(mount_fd, &our_sfs) < 0)
                                ਵਾਪਸ -errno;

                        sfs = &our_sfs;
                }

                ਜੇ (memcmp(&fid->fsid, &sfs->f_fsid, ਆਕਾਰ(fid->fsid)) != 0)
                        ਵਾਪਸ -ESTALE;

                ਵਾਪਸ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_ਰਸਤਾ));
        }

        ਵਾਪਸ -ENODATA;
}

ਪੂਰਨ fanotify_mark_fd(ਪੂਰਨ fanotify_fd, ਬਿਨਾਨਿਸ਼ਾਨੀ flags, uint64_t ਨਕਾਬ_3, ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fanotify_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ਵਾਪਸ RET_NERRNO(fanotify_mark(fanotify_fd, flags, ਨਕਾਬ_3, AT_FDCWD, FORMAT_PROC_FD_ਰਸਤਾ(fd)));
}
