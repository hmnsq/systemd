/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fanotify-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, ଅଚଳ ସଂରଚନା fanotify_event_metadata **ret) {
        ପ୍ରତିଜ୍ଞା(i);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (i->ବାମ < ଆକାର(ସଂରଚନା fanotify_event_metadata))
                ଫେରନ୍ତୁ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ଯଦି (event_len < ଆକାର(ସଂରଚନା fanotify_event_metadata) ||
            event_len > i->ବାମ ||
            event_len > ଆକାର(i->aligned.raw))
                ଫେରନ୍ତୁ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ବାମ -= event_len;

        *ret = &i->aligned.em;
        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fanotify_event_info_header* fanotify_event_info_header_first(ଅଚଳ ସଂରଚନା fanotify_event_metadata *e) {
        ପ୍ରତିଜ୍ଞା(e);

        ଫେରନ୍ତୁ (ଅଚଳ ସଂରଚନା fanotify_event_info_header*) ((ଅଚଳ uint8_t*) e + e->metadata_len);
}

ସ୍ଥିର bool fanotify_event_info_header_is_valid(
                ଅଚଳ ସଂରଚନା fanotify_event_info_header *h,
                ଅଚଳ ସଂରଚନା fanotify_event_metadata *e) {

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(e);

        ଫେରନ୍ତୁ (ଅଚଳ uint8_t*) h + ଆକାର(*h) <= (ଅଚଳ uint8_t*) e + e->event_len &&
                h->len >= ଆକାର(*h) &&
                (ଅଚଳ uint8_t*) h + h->len <= (ଅଚଳ uint8_t*) e + e->event_len;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା fanotify_event_info_header* fanotify_event_info_header_next(ଅଚଳ ସଂରଚନା fanotify_event_info_header *h) {
        ପ୍ରତିଜ୍ଞା(h);

        ଫେରନ୍ତୁ (ଅଚଳ ସଂରଚନା fanotify_event_info_header*) ((ଅଚଳ uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        ପାଇଁ (ଅଚଳ ସଂରଚନା fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(ସଂରଚନା fanotify_event_info_fid, handle) + offsetof(ସଂରଚନା file_handle, f_handle))

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_event_pin_fd(
                ଅଚଳ ସଂରଚନା fanotify_event_metadata *e,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_fd,
                ଅଚଳ ସଂରଚନା statfs *sfs) {

        ପ୍ରତିଜ୍ଞା(e);
        ପ୍ରତିଜ୍ଞା(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ଯଦି (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        ଜାରି;

                ଯଦି (h->len < FID_MIN_SIZE)
                        ଫେରନ୍ତୁ -EBADMSG;

                ଅଚଳ ସଂରଚନା fanotify_event_info_fid *fid = (ଅଚଳ ସଂରଚନା fanotify_event_info_fid*) h;
                ସଂରଚନା file_handle *fh = (ସଂରଚନା file_handle*) fid->handle;

                ଯଦି (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        ଫେରନ୍ତୁ -EBADMSG;

                ସଂରଚନା statfs our_sfs;
                ଯଦି (!statfs_is_set(sfs)) {
                        ଯଦି (fstatfs(mount_fd, &our_sfs) < 0)
                                ଫେରନ୍ତୁ -errno;

                        sfs = &our_sfs;
                }

                ଯଦି (memcmp(&fid->fsid, &sfs->f_fsid, ଆକାର(fid->fsid)) != 0)
                        ଫେରନ୍ତୁ -ESTALE;

                ଫେରନ୍ତୁ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        ଫେରନ୍ତୁ -ENODATA;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_mark_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fanotify_fd, unsigned flags, uint64_t mask, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(fanotify_fd >= 0);
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        ଫେରନ୍ତୁ RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
