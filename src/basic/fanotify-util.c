/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fanotify-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, மாறிலி கட்டமைப்பு fanotify_event_metadata **ret) {
        ப்ரதிஜ்ஞா(i);
        ப்ரதிஜ்ஞா(ret);

        எனில் (i->இடது < அளவு(கட்டமைப்பு fanotify_event_metadata))
                திரும்பு false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        எனில் (event_len < அளவு(கட்டமைப்பு fanotify_event_metadata) ||
            event_len > i->இடது ||
            event_len > அளவு(i->aligned.raw))
                திரும்பு false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->இடது -= event_len;

        *ret = &i->aligned.em;
        திரும்பு true;
}

நிலையான மாறிலி கட்டமைப்பு fanotify_event_info_header* fanotify_event_info_header_first(மாறிலி கட்டமைப்பு fanotify_event_metadata *e) {
        ப்ரதிஜ்ஞா(e);

        திரும்பு (மாறிலி கட்டமைப்பு fanotify_event_info_header*) ((மாறிலி uint8_t*) e + e->metadata_len);
}

நிலையான bool fanotify_event_info_header_is_valid(
                மாறிலி கட்டமைப்பு fanotify_event_info_header *h,
                மாறிலி கட்டமைப்பு fanotify_event_metadata *e) {

        ப்ரதிஜ்ஞா(h);
        ப்ரதிஜ்ஞா(e);

        திரும்பு (மாறிலி uint8_t*) h + அளவு(*h) <= (மாறிலி uint8_t*) e + e->event_len &&
                h->len >= அளவு(*h) &&
                (மாறிலி uint8_t*) h + h->len <= (மாறிலி uint8_t*) e + e->event_len;
}

நிலையான மாறிலி கட்டமைப்பு fanotify_event_info_header* fanotify_event_info_header_next(மாறிலி கட்டமைப்பு fanotify_event_info_header *h) {
        ப்ரதிஜ்ஞா(h);

        திரும்பு (மாறிலி கட்டமைப்பு fanotify_event_info_header*) ((மாறிலி uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        க்கு (மாறிலி கட்டமைப்பு fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(கட்டமைப்பு fanotify_event_info_fid, handle) + offsetof(கட்டமைப்பு file_handle, f_handle))

முழு fanotify_event_pin_fd(
                மாறிலி கட்டமைப்பு fanotify_event_metadata *e,
                முழு mount_fd,
                மாறிலி கட்டமைப்பு statfs *sfs) {

        ப்ரதிஜ்ஞா(e);
        ப்ரதிஜ்ஞா(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                எனில் (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        தொடர்;

                எனில் (h->len < FID_MIN_SIZE)
                        திரும்பு -EBADMSG;

                மாறிலி கட்டமைப்பு fanotify_event_info_fid *fid = (மாறிலி கட்டமைப்பு fanotify_event_info_fid*) h;
                கட்டமைப்பு file_handle *fh = (கட்டமைப்பு file_handle*) fid->handle;

                எனில் (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        திரும்பு -EBADMSG;

                கட்டமைப்பு statfs our_sfs;
                எனில் (!statfs_is_set(sfs)) {
                        எனில் (fstatfs(mount_fd, &our_sfs) < 0)
                                திரும்பு -errno;

                        sfs = &our_sfs;
                }

                எனில் (memcmp(&fid->fsid, &sfs->f_fsid, அளவு(fid->fsid)) != 0)
                        திரும்பு -ESTALE;

                திரும்பு RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        திரும்பு -ENODATA;
}

முழு fanotify_mark_fd(முழு fanotify_fd, unsigned flags, uint64_t mask, முழு fd) {
        ப்ரதிஜ்ஞா(fanotify_fd >= 0);
        ப்ரதிஜ்ஞா(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        திரும்பு RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
