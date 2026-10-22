/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <string.h>

#include "errno-util.h"
#include "fanotify-util.h"
#include "fd-util.h"
#include "stat-util.h"
#include "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, const struct fanotify_event_metadata **ret) {
        assert(i);
        assert(ret);

        nếu (i->left < sizeof(struct fanotify_event_metadata))
                trả_về false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        nếu (event_len < sizeof(struct fanotify_event_metadata) ||
            event_len > i->left ||
            event_len > sizeof(i->aligned.raw))
                trả_về false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->left -= event_len;

        *ret = &i->aligned.em;
        trả_về true;
}

static const struct fanotify_event_info_header* fanotify_event_info_header_first(const struct fanotify_event_metadata *e) {
        assert(e);

        trả_về (const struct fanotify_event_info_header*) ((const uint8_t*) e + e->metadata_len);
}

static bool fanotify_event_info_header_is_valid(
                const struct fanotify_event_info_header *h,
                const struct fanotify_event_metadata *e) {

        assert(h);
        assert(e);

        trả_về (const uint8_t*) h + sizeof(*h) <= (const uint8_t*) e + e->event_len &&
                h->len >= sizeof(*h) &&
                (const uint8_t*) h + h->len <= (const uint8_t*) e + e->event_len;
}

static const struct fanotify_event_info_header* fanotify_event_info_header_next(const struct fanotify_event_info_header *h) {
        assert(h);

        trả_về (const struct fanotify_event_info_header*) ((const uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        cho (const struct fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(struct fanotify_event_info_fid, handle) + offsetof(struct file_handle, f_handle))

số_nguyên fanotify_event_pin_fd(
                const struct fanotify_event_metadata *e,
                số_nguyên mount_fd,
                const struct statfs *sfs) {

        assert(e);
        assert(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                nếu (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        continue;

                nếu (h->len < FID_MIN_SIZE)
                        trả_về -EBADMSG;

                const struct fanotify_event_info_fid *fid = (const struct fanotify_event_info_fid*) h;
                struct file_handle *fh = (struct file_handle*) fid->handle;

                nếu (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        trả_về -EBADMSG;

                struct statfs our_sfs;
                nếu (!statfs_is_set(sfs)) {
                        nếu (fstatfs(mount_fd, &our_sfs) < 0)
                                trả_về -errno;

                        sfs = &our_sfs;
                }

                nếu (memcmp(&fid->fsid, &sfs->f_fsid, sizeof(fid->fsid)) != 0)
                        trả_về -ESTALE;

                trả_về RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        trả_về -ENODATA;
}

số_nguyên fanotify_mark_fd(số_nguyên fanotify_fd, unsigned flags, uint64_t mask, số_nguyên fd) {
        assert(fanotify_fd >= 0);
        assert(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        trả_về RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
