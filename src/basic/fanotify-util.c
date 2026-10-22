/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fanotify-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

bool fanotify_event_iterator_ಮುಂದಿನ(FanotifyEventIterator *i, ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata **ret) {
        ಪ್ರತಿಜ್ಞಾ(i);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (i->ಎಡ < ಗಾತ್ರ(ರಚನೆ fanotify_event_metadata))
                ಹಿಂತಿರುಗಿ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ಆದರೆ (event_len < ಗಾತ್ರ(ರಚನೆ fanotify_event_metadata) ||
            event_len > i->ಎಡ ||
            event_len > ಗಾತ್ರ(i->aligned.raw))
                ಹಿಂತಿರುಗಿ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        ಸ್ಮರಣೆ_ನಕಲು(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ಎಡ -= event_len;

        *ret = &i->aligned.em;
        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header* fanotify_event_info_ಶೀರ್ಷಿಕೆ_ಮೊದಲ(ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata *e) {
        ಪ್ರತಿಜ್ಞಾ(e);

        ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header*) ((ಸ್ಥಿರಾಂಕ uint8_t*) e + e->metadata_len);
}

ಸ್ಥಿರ bool fanotify_event_info_ಶೀರ್ಷಿಕೆ_is_valid(
                ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header *h,
                ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata *e) {

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(e);

        ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ uint8_t*) h + ಗಾತ್ರ(*h) <= (ಸ್ಥಿರಾಂಕ uint8_t*) e + e->event_len &&
                h->len >= ಗಾತ್ರ(*h) &&
                (ಸ್ಥಿರಾಂಕ uint8_t*) h + h->len <= (ಸ್ಥಿರಾಂಕ uint8_t*) e + e->event_len;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header* fanotify_event_info_ಶೀರ್ಷಿಕೆ_ಮುಂದಿನ(ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header*) ((ಸ್ಥಿರಾಂಕ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_header *h = fanotify_event_info_ಶೀರ್ಷಿಕೆ_ಮೊದಲ(e); \
             fanotify_event_info_ಶೀರ್ಷಿಕೆ_is_valid(h, e);                 \
             h = fanotify_event_info_ಶೀರ್ಷಿಕೆ_ಮುಂದಿನ(h))

#ವ್ಯಾಖ್ಯಾನಿಸು FID_MIN_ಗಾತ್ರ (offsetof(ರಚನೆ fanotify_event_info_fid, handle) + offsetof(ರಚನೆ file_handle, f_handle))

ಪೂರ್ಣಾಂಕ fanotify_event_pin_fd(
                ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_metadata *e,
                ಪೂರ್ಣಾಂಕ mount_fd,
                ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *sfs) {

        ಪ್ರತಿಜ್ಞಾ(e);
        ಪ್ರತಿಜ್ಞಾ(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ಆದರೆ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ಮುಂದುವರಿ;

                ಆದರೆ (h->len < FID_MIN_ಗಾತ್ರ)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_fid *fid = (ಸ್ಥಿರಾಂಕ ರಚನೆ fanotify_event_info_fid*) h;
                ರಚನೆ file_handle *fh = (ರಚನೆ file_handle*) fid->handle;

                ಆದರೆ (h->len < FID_MIN_ಗಾತ್ರ + fh->handle_ಬೈಟುಗಳು)
                        ಹಿಂತಿರುಗಿ -EBADMSG;

                ರಚನೆ statfs our_sfs;
                ಆದರೆ (!statfs_is_ಗಣ(sfs)) {
                        ಆದರೆ (fstatfs(mount_fd, &our_sfs) < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        sfs = &our_sfs;
                }

                ಆದರೆ (memcmp(&fid->fsid, &sfs->f_fsid, ಗಾತ್ರ(fid->fsid)) != 0)
                        ಹಿಂತಿರುಗಿ -ESTALE;

                ಹಿಂತಿರುಗಿ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_ಮಾರ್ಗ));
        }

        ಹಿಂತಿರುಗಿ -ENODATA;
}

ಪೂರ್ಣಾಂಕ fanotify_mark_fd(ಪೂರ್ಣಾಂಕ fanotify_fd, ಚಿಹ್ನೆರಹಿತ flags, uint64_t ಮುಖವಾಡ_3, ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fanotify_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ಹಿಂತಿರುಗಿ RET_NERRNO(fanotify_mark(fanotify_fd, flags, ಮುಖವಾಡ_3, AT_FDCWD, FORMAT_PROC_FD_ಮಾರ್ಗ(fd)));
}
