/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fanotify-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ধ্রুব সংরচনা fanotify_event_metadata **ret) {
        প্রতিজ্ঞা(i);
        প্রতিজ্ঞা(ret);

        যদি (i->বাম < পরিমাণ(সংরচনা fanotify_event_metadata))
                প্রত্যাগম false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        যদি (event_len < পরিমাণ(সংরচনা fanotify_event_metadata) ||
            event_len > i->বাম ||
            event_len > পরিমাণ(i->aligned.raw))
                প্রত্যাগম false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->বাম -= event_len;

        *ret = &i->aligned.em;
        প্রত্যাগম true;
}

স্থির ধ্রুব সংরচনা fanotify_event_info_header* fanotify_event_info_header_first(ধ্রুব সংরচনা fanotify_event_metadata *e) {
        প্রতিজ্ঞা(e);

        প্রত্যাগম (ধ্রুব সংরচনা fanotify_event_info_header*) ((ধ্রুব uint8_t*) e + e->metadata_len);
}

স্থির bool fanotify_event_info_header_is_valid(
                ধ্রুব সংরচনা fanotify_event_info_header *h,
                ধ্রুব সংরচনা fanotify_event_metadata *e) {

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(e);

        প্রত্যাগম (ধ্রুব uint8_t*) h + পরিমাণ(*h) <= (ধ্রুব uint8_t*) e + e->event_len &&
                h->len >= পরিমাণ(*h) &&
                (ধ্রুব uint8_t*) h + h->len <= (ধ্রুব uint8_t*) e + e->event_len;
}

স্থির ধ্রুব সংরচনা fanotify_event_info_header* fanotify_event_info_header_next(ধ্রুব সংরচনা fanotify_event_info_header *h) {
        প্রতিজ্ঞা(h);

        প্রত্যাগম (ধ্রুব সংরচনা fanotify_event_info_header*) ((ধ্রুব uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        প্রতি (ধ্রুব সংরচনা fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(সংরচনা fanotify_event_info_fid, handle) + offsetof(সংরচনা file_handle, f_handle))

পূর্ণাঙ্ক fanotify_event_pin_fd(
                ধ্রুব সংরচনা fanotify_event_metadata *e,
                পূর্ণাঙ্ক mount_fd,
                ধ্রুব সংরচনা statfs *sfs) {

        প্রতিজ্ঞা(e);
        প্রতিজ্ঞা(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                যদি (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        অনুবর্ত;

                যদি (h->len < FID_MIN_SIZE)
                        প্রত্যাগম -EBADMSG;

                ধ্রুব সংরচনা fanotify_event_info_fid *fid = (ধ্রুব সংরচনা fanotify_event_info_fid*) h;
                সংরচনা file_handle *fh = (সংরচনা file_handle*) fid->handle;

                যদি (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        প্রত্যাগম -EBADMSG;

                সংরচনা statfs our_sfs;
                যদি (!statfs_is_set(sfs)) {
                        যদি (fstatfs(mount_fd, &our_sfs) < 0)
                                প্রত্যাগম -errno;

                        sfs = &our_sfs;
                }

                যদি (memcmp(&fid->fsid, &sfs->f_fsid, পরিমাণ(fid->fsid)) != 0)
                        প্রত্যাগম -ESTALE;

                প্রত্যাগম RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        প্রত্যাগম -ENODATA;
}

পূর্ণাঙ্ক fanotify_mark_fd(পূর্ণাঙ্ক fanotify_fd, unsigned flags, uint64_t mask, পূর্ণাঙ্ক fd) {
        প্রতিজ্ঞা(fanotify_fd >= 0);
        প্রতিজ্ঞা(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        প্রত্যাগম RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
