/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fanotify-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata **ret) {
        ಪ್ರತಿಜ್ಞಾ(i);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (i->ಎಡ < ಪರಿಮಾಣ(ಸಂರಚನಾ fanotify_event_metadata))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ಯದಿ (event_len < ಪರಿಮಾಣ(ಸಂರಚನಾ fanotify_event_metadata) ||
            event_len > i->ಎಡ ||
            event_len > ಪರಿಮಾಣ(i->aligned.raw))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ಎಡ -= event_len;

        *ret = &i->aligned.em;
        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header* fanotify_event_info_header_first(ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata *e) {
        ಪ್ರತಿಜ್ಞಾ(e);

        ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header*) ((ಧ್ರುವ uint8_t*) e + e->metadata_len);
}

ಸ್ಥಿರ bool fanotify_event_info_header_is_valid(
                ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header *h,
                ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata *e) {

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(e);

        ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ uint8_t*) h + ಪರಿಮಾಣ(*h) <= (ಧ್ರುವ uint8_t*) e + e->event_len &&
                h->len >= ಪರಿಮಾಣ(*h) &&
                (ಧ್ರುವ uint8_t*) h + h->len <= (ಧ್ರುವ uint8_t*) e + e->event_len;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header* fanotify_event_info_header_next(ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಪ್ರತ್ಯಾಗಮ (ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header*) ((ಧ್ರುವ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        ಪ್ರತಿ (ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(ಸಂರಚನಾ fanotify_event_info_fid, handle) + offsetof(ಸಂರಚನಾ file_handle, f_handle))

ಪೂರ್ಣಾಙ್ಕ fanotify_event_pin_fd(
                ಧ್ರುವ ಸಂರಚನಾ fanotify_event_metadata *e,
                ಪೂರ್ಣಾಙ್ಕ mount_fd,
                ಧ್ರುವ ಸಂರಚನಾ statfs *sfs) {

        ಪ್ರತಿಜ್ಞಾ(e);
        ಪ್ರತಿಜ್ಞಾ(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ಯದಿ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ಅನುವರ್ತ;

                ಯದಿ (h->len < FID_MIN_SIZE)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_fid *fid = (ಧ್ರುವ ಸಂರಚನಾ fanotify_event_info_fid*) h;
                ಸಂರಚನಾ file_handle *fh = (ಸಂರಚನಾ file_handle*) fid->handle;

                ಯದಿ (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                ಸಂರಚನಾ statfs our_sfs;
                ಯದಿ (!statfs_is_set(sfs)) {
                        ಯದಿ (fstatfs(mount_fd, &our_sfs) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        sfs = &our_sfs;
                }

                ಯದಿ (memcmp(&fid->fsid, &sfs->f_fsid, ಪರಿಮಾಣ(fid->fsid)) != 0)
                        ಪ್ರತ್ಯಾಗಮ -ESTALE;

                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        ಪ್ರತ್ಯಾಗಮ -ENODATA;
}

ಪೂರ್ಣಾಙ್ಕ fanotify_mark_fd(ಪೂರ್ಣಾಙ್ಕ fanotify_fd, unsigned flags, uint64_t mask, ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fanotify_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
