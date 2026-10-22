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

        만약 (i->left < sizeof(struct fanotify_event_metadata))
                반환 false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        만약 (event_len < sizeof(struct fanotify_event_metadata) ||
            event_len > i->left ||
            event_len > sizeof(i->aligned.raw))
                반환 false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->left -= event_len;

        *ret = &i->aligned.em;
        반환 true;
}

static const struct fanotify_event_info_header* fanotify_event_info_header_first(const struct fanotify_event_metadata *e) {
        assert(e);

        반환 (const struct fanotify_event_info_header*) ((const uint8_t*) e + e->metadata_len);
}

static bool fanotify_event_info_header_is_valid(
                const struct fanotify_event_info_header *h,
                const struct fanotify_event_metadata *e) {

        assert(h);
        assert(e);

        반환 (const uint8_t*) h + sizeof(*h) <= (const uint8_t*) e + e->event_len &&
                h->len >= sizeof(*h) &&
                (const uint8_t*) h + h->len <= (const uint8_t*) e + e->event_len;
}

static const struct fanotify_event_info_header* fanotify_event_info_header_next(const struct fanotify_event_info_header *h) {
        assert(h);

        반환 (const struct fanotify_event_info_header*) ((const uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        위해 (const struct fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(struct fanotify_event_info_fid, handle) + offsetof(struct file_handle, f_handle))

정수 fanotify_event_pin_fd(
                const struct fanotify_event_metadata *e,
                정수 mount_fd,
                const struct statfs *sfs) {

        assert(e);
        assert(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                만약 (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        continue;

                만약 (h->len < FID_MIN_SIZE)
                        반환 -EBADMSG;

                const struct fanotify_event_info_fid *fid = (const struct fanotify_event_info_fid*) h;
                struct file_handle *fh = (struct file_handle*) fid->handle;

                만약 (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        반환 -EBADMSG;

                struct statfs our_sfs;
                만약 (!statfs_is_set(sfs)) {
                        만약 (fstatfs(mount_fd, &our_sfs) < 0)
                                반환 -errno;

                        sfs = &our_sfs;
                }

                만약 (memcmp(&fid->fsid, &sfs->f_fsid, sizeof(fid->fsid)) != 0)
                        반환 -ESTALE;

                반환 RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        반환 -ENODATA;
}

정수 fanotify_mark_fd(정수 fanotify_fd, unsigned flags, uint64_t mask, 정수 fd) {
        assert(fanotify_fd >= 0);
        assert(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        반환 RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
