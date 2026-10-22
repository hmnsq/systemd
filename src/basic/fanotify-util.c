/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <string.h>

#include "errno-util.h"
#include "fanotify-util.h"
#include "fd-util.h"
#include "stat-util.h"
#include "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, const sa_nracanaa fanotify_event_metadata **ret) {
        assert(i);
        assert(ret);

        ᬬᬾᬦ᭄ (i->left < parimaa_na(sa_nracanaa fanotify_event_metadata))
                ᬯᬮᬶ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ᬬᬾᬦ᭄ (event_len < parimaa_na(sa_nracanaa fanotify_event_metadata) ||
            event_len > i->left ||
            event_len > parimaa_na(i->aligned.raw))
                ᬯᬮᬶ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->left -= event_len;

        *ret = &i->aligned.em;
        ᬯᬮᬶ true;
}

static const sa_nracanaa fanotify_event_info_header* fanotify_event_info_header_first(const sa_nracanaa fanotify_event_metadata *e) {
        assert(e);

        ᬯᬮᬶ (const sa_nracanaa fanotify_event_info_header*) ((const uint8_t*) e + e->metadata_len);
}

static bool fanotify_event_info_header_is_valid(
                const sa_nracanaa fanotify_event_info_header *h,
                const sa_nracanaa fanotify_event_metadata *e) {

        assert(h);
        assert(e);

        ᬯᬮᬶ (const uint8_t*) h + parimaa_na(*h) <= (const uint8_t*) e + e->event_len &&
                h->len >= parimaa_na(*h) &&
                (const uint8_t*) h + h->len <= (const uint8_t*) e + e->event_len;
}

static const sa_nracanaa fanotify_event_info_header* fanotify_event_info_header_next(const sa_nracanaa fanotify_event_info_header *h) {
        assert(h);

        ᬯᬮᬶ (const sa_nracanaa fanotify_event_info_header*) ((const uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        for (const sa_nracanaa fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(sa_nracanaa fanotify_event_info_fid, handle) + offsetof(sa_nracanaa file_handle, f_handle))

puur_naa_nka fanotify_event_pin_fd(
                const sa_nracanaa fanotify_event_metadata *e,
                puur_naa_nka mount_fd,
                const sa_nracanaa statfs *sfs) {

        assert(e);
        assert(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ᬬᬾᬦ᭄ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        continue;

                ᬬᬾᬦ᭄ (h->len < FID_MIN_SIZE)
                        ᬯᬮᬶ -EBADMSG;

                const sa_nracanaa fanotify_event_info_fid *fid = (const sa_nracanaa fanotify_event_info_fid*) h;
                sa_nracanaa file_handle *fh = (sa_nracanaa file_handle*) fid->handle;

                ᬬᬾᬦ᭄ (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        ᬯᬮᬶ -EBADMSG;

                sa_nracanaa statfs our_sfs;
                ᬬᬾᬦ᭄ (!statfs_is_set(sfs)) {
                        ᬬᬾᬦ᭄ (fstatfs(mount_fd, &our_sfs) < 0)
                                ᬯᬮᬶ -errno;

                        sfs = &our_sfs;
                }

                ᬬᬾᬦ᭄ (memcmp(&fid->fsid, &sfs->f_fsid, parimaa_na(fid->fsid)) != 0)
                        ᬯᬮᬶ -ESTALE;

                ᬯᬮᬶ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        ᬯᬮᬶ -ENODATA;
}

puur_naa_nka fanotify_mark_fd(puur_naa_nka fanotify_fd, unsigned flags, uint64_t mask, puur_naa_nka fd) {
        assert(fanotify_fd >= 0);
        assert(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ᬯᬮᬶ RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
