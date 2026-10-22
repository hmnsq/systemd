/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fanotify-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ধ্রুৱ সংরচনা fanotify_event_metadata **ret) {
        প্রতিজ্ঞা(i);
        প্রতিজ্ঞা(ret);

        যদি (i->বাম < পরিমাণ(সংরচনা fanotify_event_metadata))
                ঘূৰাই false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        যদি (event_len < পরিমাণ(সংরচনা fanotify_event_metadata) ||
            event_len > i->বাম ||
            event_len > পরিমাণ(i->aligned.raw))
                ঘূৰাই false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->বাম -= event_len;

        *ret = &i->aligned.em;
        ঘূৰাই true;
}

স্থির ধ্রুৱ সংরচনা fanotify_event_info_header* fanotify_event_info_header_first(ধ্রুৱ সংরচনা fanotify_event_metadata *e) {
        প্রতিজ্ঞা(e);

        ঘূৰাই (ধ্রুৱ সংরচনা fanotify_event_info_header*) ((ধ্রুৱ uint8_t*) e + e->metadata_len);
}

স্থির bool fanotify_event_info_header_is_valid(
                ধ্রুৱ সংরচনা fanotify_event_info_header *h,
                ধ্রুৱ সংরচনা fanotify_event_metadata *e) {

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(e);

        ঘূৰাই (ধ্রুৱ uint8_t*) h + পরিমাণ(*h) <= (ধ্রুৱ uint8_t*) e + e->event_len &&
                h->len >= পরিমাণ(*h) &&
                (ধ্রুৱ uint8_t*) h + h->len <= (ধ্রুৱ uint8_t*) e + e->event_len;
}

স্থির ধ্রুৱ সংরচনা fanotify_event_info_header* fanotify_event_info_header_next(ধ্রুৱ সংরচনা fanotify_event_info_header *h) {
        প্রতিজ্ঞা(h);

        ঘূৰাই (ধ্রুৱ সংরচনা fanotify_event_info_header*) ((ধ্রুৱ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        বাবে (ধ্রুৱ সংরচনা fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(সংরচনা fanotify_event_info_fid, handle) + offsetof(সংরচনা file_handle, f_handle))

পূর্ণাঙ্ক fanotify_event_pin_fd(
                ধ্রুৱ সংরচনা fanotify_event_metadata *e,
                পূর্ণাঙ্ক mount_fd,
                ধ্রুৱ সংরচনা statfs *sfs) {

        প্রতিজ্ঞা(e);
        প্রতিজ্ঞা(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                যদি (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        অনুৱর্ত;

                যদি (h->len < FID_MIN_SIZE)
                        ঘূৰাই -EBADMSG;

                ধ্রুৱ সংরচনা fanotify_event_info_fid *fid = (ধ্রুৱ সংরচনা fanotify_event_info_fid*) h;
                সংরচনা file_handle *fh = (সংরচনা file_handle*) fid->handle;

                যদি (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        ঘূৰাই -EBADMSG;

                সংরচনা statfs our_sfs;
                যদি (!statfs_is_set(sfs)) {
                        যদি (fstatfs(mount_fd, &our_sfs) < 0)
                                ঘূৰাই -errno;

                        sfs = &our_sfs;
                }

                যদি (memcmp(&fid->fsid, &sfs->f_fsid, পরিমাণ(fid->fsid)) != 0)
                        ঘূৰাই -ESTALE;

                ঘূৰাই RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        ঘূৰাই -ENODATA;
}

পূর্ণাঙ্ক fanotify_mark_fd(পূর্ণাঙ্ক fanotify_fd, unsigned flags, uint64_t mask, পূর্ণাঙ্ক fd) {
        প্রতিজ্ঞা(fanotify_fd >= 0);
        প্রতিজ্ঞা(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ঘূৰাই RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
