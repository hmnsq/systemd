/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fanotify-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "unaligned.h"

bool fanotify_event_iterator_next(FanotifyEventIterator *i, स्थिरांक संरचना fanotify_event_metadata **ret) {
        प्रतिज्ञा(i);
        प्रतिज्ञा(ret);

        जर (i->डावा < आकार(संरचना fanotify_event_metadata))
                परत false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        जर (event_len < आकार(संरचना fanotify_event_metadata) ||
            event_len > i->डावा ||
            event_len > आकार(i->aligned.raw))
                परत false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        स_नकल(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->डावा -= event_len;

        *ret = &i->aligned.em;
        परत true;
}

स्थिर स्थिरांक संरचना fanotify_event_info_header* fanotify_event_info_header_first(स्थिरांक संरचना fanotify_event_metadata *e) {
        प्रतिज्ञा(e);

        परत (स्थिरांक संरचना fanotify_event_info_header*) ((स्थिरांक uint8_t*) e + e->metadata_len);
}

स्थिर bool fanotify_event_info_header_is_valid(
                स्थिरांक संरचना fanotify_event_info_header *h,
                स्थिरांक संरचना fanotify_event_metadata *e) {

        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        परत (स्थिरांक uint8_t*) h + आकार(*h) <= (स्थिरांक uint8_t*) e + e->event_len &&
                h->len >= आकार(*h) &&
                (स्थिरांक uint8_t*) h + h->len <= (स्थिरांक uint8_t*) e + e->event_len;
}

स्थिर स्थिरांक संरचना fanotify_event_info_header* fanotify_event_info_header_next(स्थिरांक संरचना fanotify_event_info_header *h) {
        प्रतिज्ञा(h);

        परत (स्थिरांक संरचना fanotify_event_info_header*) ((स्थिरांक uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#define FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        साठी (स्थिरांक संरचना fanotify_event_info_header *h = fanotify_event_info_header_first(e); \
             fanotify_event_info_header_is_valid(h, e);                 \
             h = fanotify_event_info_header_next(h))

#define FID_MIN_SIZE (दुरत्व(संरचना fanotify_event_info_fid, handle) + दुरत्व(संरचना file_handle, f_handle))

पूर्णांक fanotify_event_pin_fd(
                स्थिरांक संरचना fanotify_event_metadata *e,
                पूर्णांक mount_fd,
                स्थिरांक संरचना statfs *sfs) {

        प्रतिज्ञा(e);
        प्रतिज्ञा(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                जर (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        सुरूठेवा;

                जर (h->len < FID_MIN_SIZE)
                        परत -EBADMSG;

                स्थिरांक संरचना fanotify_event_info_fid *fid = (स्थिरांक संरचना fanotify_event_info_fid*) h;
                संरचना file_handle *fh = (संरचना file_handle*) fid->handle;

                जर (h->len < FID_MIN_SIZE + fh->handle_bytes)
                        परत -EBADMSG;

                संरचना statfs our_sfs;
                जर (!statfs_is_set(sfs)) {
                        जर (fstatfs(mount_fd, &our_sfs) < 0)
                                परत -त्रुटि_सं;

                        sfs = &our_sfs;
                }

                जर (स_भेद(&fid->fsid, &sfs->f_fsid, आकार(fid->fsid)) != 0)
                        परत -ESTALE;

                परत RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_PATH));
        }

        परत -ENODATA;
}

पूर्णांक fanotify_mark_fd(पूर्णांक fanotify_fd, अचिन्हित flags, uint64_t mask, पूर्णांक fd) {
        प्रतिज्ञा(fanotify_fd >= 0);
        प्रतिज्ञा(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        परत RET_NERRNO(fanotify_mark(fanotify_fd, flags, mask, AT_FDCWD, FORMAT_PROC_FD_PATH(fd)));
}
