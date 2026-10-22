/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <string.h>

#समावेश "errno-util.h"
#समावेश "fanotify-util.h"
#समावेश "fd-util.h"
#समावेश "stat-util.h"
#समावेश "unaligned.h"

bool fanotify_event_iterator_अग्र(FanotifyEventIterator *i, स्थिर काष्ठा fanotify_event_metadata **ret) {
        निश्चित(i);
        निश्चित(ret);

        अगर (i->बायाँ < माप(काष्ठा fanotify_event_metadata))
                वापस false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        अगर (event_len < माप(काष्ठा fanotify_event_metadata) ||
            event_len > i->बायाँ ||
            event_len > माप(i->aligned.raw))
                वापस false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        स_नकल(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->बायाँ -= event_len;

        *ret = &i->aligned.em;
        वापस true;
}

अटल स्थिर काष्ठा fanotify_event_info_header* fanotify_event_info_header_first(स्थिर काष्ठा fanotify_event_metadata *e) {
        निश्चित(e);

        वापस (स्थिर काष्ठा fanotify_event_info_header*) ((स्थिर uint8_t*) e + e->metadata_len);
}

अटल bool fanotify_event_info_header_is_valid(
                स्थिर काष्ठा fanotify_event_info_header *h,
                स्थिर काष्ठा fanotify_event_metadata *e) {

        निश्चित(h);
        निश्चित(e);

        वापस (स्थिर uint8_t*) h + माप(*h) <= (स्थिर uint8_t*) e + e->event_len &&
                h->len >= माप(*h) &&
                (स्थिर uint8_t*) h + h->len <= (स्थिर uint8_t*) e + e->event_len;
}

अटल स्थिर काष्ठा fanotify_event_info_header* fanotify_event_info_header_अग्र(स्थिर काष्ठा fanotify_event_info_header *h) {
        निश्चित(h);

        वापस (स्थिर काष्ठा fanotify_event_info_header*) ((स्थिर uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#घोषणा FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        क्रम (स्थिर काष्ठा fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_अग्र(h))

#घोषणा FID_MIN_आकार (दुरत्व(काष्ठा fanotify_event_info_fid, handle) + दुरत्व(काष्ठा file_handle, f_handle))

पूर्णांक fanotify_event_pin_fd(
                स्थिर काष्ठा fanotify_event_metadata *e,
                पूर्णांक mount_fd,
                स्थिर काष्ठा statfs *sfs) {

        निश्चित(e);
        निश्चित(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                अगर (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        जारी;

                अगर (h->len < FID_MIN_आकार)
                        वापस -EBADMSG;

                स्थिर काष्ठा fanotify_event_info_fid *fid = (स्थिर काष्ठा fanotify_event_info_fid*) h;
                काष्ठा file_handle *fh = (काष्ठा file_handle*) fid->handle;

                अगर (h->len < FID_MIN_आकार + fh->handle_bytes)
                        वापस -EBADMSG;

                काष्ठा statfs our_sfs;
                अगर (!statfs_is_set(sfs)) {
                        अगर (fstatfs(mount_fd, &our_sfs) < 0)
                                वापस -त्रुटि_सं;

                        sfs = &our_sfs;
                }

                अगर (स_भेद(&fid->fsid, &sfs->f_fsid, माप(fid->fsid)) != 0)
                        वापस -ESTALE;

                वापस RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        वापस -ENODATA;
}

पूर्णांक fanotify_mark_fd(पूर्णांक fanotify_fd, अचिन्हित flags, uint64_t mask, पूर्णांक fd) {
        निश्चित(fanotify_fd >= 0);
        निश्चित(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        वापस RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
