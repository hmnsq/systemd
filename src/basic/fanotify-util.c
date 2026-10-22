/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fanotify-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, અચલ સંરચના fanotify_event_metadata **ret) {
        પ્રતિજ્ઞા(i);
        પ્રતિજ્ઞા(ret);

        જો (i->ડાબું < કદ(સંરચના fanotify_event_metadata))
                પાછા false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        જો (event_len < કદ(સંરચના fanotify_event_metadata) ||
            event_len > i->ડાબું ||
            event_len > કદ(i->aligned.raw))
                પાછા false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ડાબું -= event_len;

        *ret = &i->aligned.em;
        પાછા true;
}

સ્થિર અચલ સંરચના fanotify_event_info_header* fanotify_event_info_header_first(અચલ સંરચના fanotify_event_metadata *e) {
        પ્રતિજ્ઞા(e);

        પાછા (અચલ સંરચના fanotify_event_info_header*) ((અચલ uint8_t*) e + e->metadata_len);
}

સ્થિર bool fanotify_event_info_header_is_valid(
                અચલ સંરચના fanotify_event_info_header *h,
                અચલ સંરચના fanotify_event_metadata *e) {

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(e);

        પાછા (અચલ uint8_t*) h + કદ(*h) <= (અચલ uint8_t*) e + e->event_len &&
                h->len >= કદ(*h) &&
                (અચલ uint8_t*) h + h->len <= (અચલ uint8_t*) e + e->event_len;
}

સ્થિર અચલ સંરચના fanotify_event_info_header* fanotify_event_info_header_next(અચલ સંરચના fanotify_event_info_header *h) {
        પ્રતિજ્ઞા(h);

        પાછા (અચલ સંરચના fanotify_event_info_header*) ((અચલ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        માટે (અચલ સંરચના fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(સંરચના fanotify_event_info_fid, handle) + offsetof(સંરચના file_handle, f_handle))

પૂર્ણાંક fanotify_event_pin_fd(
                અચલ સંરચના fanotify_event_metadata *e,
                પૂર્ણાંક mount_fd,
                અચલ સંરચના statfs *sfs) {

        પ્રતિજ્ઞા(e);
        પ્રતિજ્ઞા(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                જો (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ચાલુ;

                જો (h->len < FID_MIN_SIZE)
                        પાછા -EBADMSG;

                અચલ સંરચના fanotify_event_info_fid *fid = (અચલ સંરચના fanotify_event_info_fid*) h;
                સંરચના file_handle *fh = (સંરચના file_handle*) fid->handle;

                જો (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        પાછા -EBADMSG;

                સંરચના statfs our_sfs;
                જો (!statfs_is_set(sfs)) {
                        જો (fstatfs(mount_fd, &our_sfs) < 0)
                                પાછા -errno;

                        sfs = &our_sfs;
                }

                જો (memcmp(&fid->fsid, &sfs->f_fsid, કદ(fid->fsid)) != 0)
                        પાછા -ESTALE;

                પાછા RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        પાછા -ENODATA;
}

પૂર્ણાંક fanotify_mark_fd(પૂર્ણાંક fanotify_fd, unsigned flags, uint64_t mask, પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(fanotify_fd >= 0);
        પ્રતિજ્ઞા(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        પાછા RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
