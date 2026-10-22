/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fanotify-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, സ്ഥിരം ഘടന fanotify_event_metadata **ret) {
        പ്രതിജ്ഞാ(i);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (i->ഇടത് < വലുപ്പം(ഘടന fanotify_event_metadata))
                തിരികെ false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        എങ്കിൽ (event_len < വലുപ്പം(ഘടന fanotify_event_metadata) ||
            event_len > i->ഇടത് ||
            event_len > വലുപ്പം(i->aligned.raw))
                തിരികെ false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        memcpy(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ഇടത് -= event_len;

        *ret = &i->aligned.em;
        തിരികെ true;
}

സ്ഥിതം സ്ഥിരം ഘടന fanotify_event_info_header* fanotify_event_info_header_first(സ്ഥിരം ഘടന fanotify_event_metadata *e) {
        പ്രതിജ്ഞാ(e);

        തിരികെ (സ്ഥിരം ഘടന fanotify_event_info_header*) ((സ്ഥിരം uint8_t*) e + e->metadata_len);
}

സ്ഥിതം bool fanotify_event_info_header_is_valid(
                സ്ഥിരം ഘടന fanotify_event_info_header *h,
                സ്ഥിരം ഘടന fanotify_event_metadata *e) {

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(e);

        തിരികെ (സ്ഥിരം uint8_t*) h + വലുപ്പം(*h) <= (സ്ഥിരം uint8_t*) e + e->event_len &&
                h->len >= വലുപ്പം(*h) &&
                (സ്ഥിരം uint8_t*) h + h->len <= (സ്ഥിരം uint8_t*) e + e->event_len;
}

സ്ഥിതം സ്ഥിരം ഘടന fanotify_event_info_header* fanotify_event_info_header_next(സ്ഥിരം ഘടന fanotify_event_info_header *h) {
        പ്രതിജ്ഞാ(h);

        തിരികെ (സ്ഥിരം ഘടന fanotify_event_info_header*) ((സ്ഥിരം uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        വേണ്ടി (സ്ഥിരം ഘടന fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (offsetof(ഘടന fanotify_event_info_fid, handle) + offsetof(ഘടന file_handle, f_handle))

പൂർണ്ണസംഖ്യ fanotify_event_pin_fd(
                സ്ഥിരം ഘടന fanotify_event_metadata *e,
                പൂർണ്ണസംഖ്യ mount_fd,
                സ്ഥിരം ഘടന statfs *sfs) {

        പ്രതിജ്ഞാ(e);
        പ്രതിജ്ഞാ(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                എങ്കിൽ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        തുടരുക;

                എങ്കിൽ (h->len < FID_MIN_SIZE)
                        തിരികെ -EBADMSG;

                സ്ഥിരം ഘടന fanotify_event_info_fid *fid = (സ്ഥിരം ഘടന fanotify_event_info_fid*) h;
                ഘടന file_handle *fh = (ഘടന file_handle*) fid->handle;

                എങ്കിൽ (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        തിരികെ -EBADMSG;

                ഘടന statfs our_sfs;
                എങ്കിൽ (!statfs_is_set(sfs)) {
                        എങ്കിൽ (fstatfs(mount_fd, &our_sfs) < 0)
                                തിരികെ -errno;

                        sfs = &our_sfs;
                }

                എങ്കിൽ (memcmp(&fid->fsid, &sfs->f_fsid, വലുപ്പം(fid->fsid)) != 0)
                        തിരികെ -ESTALE;

                തിരികെ RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        തിരികെ -ENODATA;
}

പൂർണ്ണസംഖ്യ fanotify_mark_fd(പൂർണ്ണസംഖ്യ fanotify_fd, unsigned flags, uint64_t mask, പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(fanotify_fd >= 0);
        പ്രതിജ്ഞാ(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        തിരികെ RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
