/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fanotify-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "unaligned.h"

bool fanotify_event_iterator_తదుపరి(FanotifyEventIterator *i, స్థిరరాశి నిర్మాణం fanotify_event_metadata **ret) {
        ప్రతిజ్ఞా(i);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (i->ఎడమ < పరిమాణం(నిర్మాణం fanotify_event_metadata))
                తిరిగి false;

        /* Read event_len without going through struct fanotify_event_metadata: the struct requires 64-bit
         * alignment (due to the __aligned_u64 mask field), but 'ptr' may only be 4-byte aligned, as the
         * kernel pads events to FANOTIFY_EVENT_ALIGN (4). */
        uint32_t event_len = unaligned_read_ne32(i->ptr);

        ఒకవేళ (event_len < పరిమాణం(నిర్మాణం fanotify_event_metadata) ||
            event_len > i->ఎడమ ||
            event_len > పరిమాణం(i->aligned.raw))
                తిరిగి false;

        /* Copy the whole event (metadata + info records) into an 8-byte aligned buffer, so the metadata and
         * the trailing records can be dereferenced safely. */
        జ్ఞాపకం_నకలు(&i->aligned, i->ptr, event_len);

        i->ptr += event_len;
        i->ఎడమ -= event_len;

        *ret = &i->aligned.em;
        తిరిగి true;
}

స్థిర స్థిరరాశి నిర్మాణం fanotify_event_info_header* fanotify_event_info_శీర్షిక_మొదటి(స్థిరరాశి నిర్మాణం fanotify_event_metadata *e) {
        ప్రతిజ్ఞా(e);

        తిరిగి (స్థిరరాశి నిర్మాణం fanotify_event_info_header*) ((స్థిరరాశి uint8_t*) e + e->metadata_len);
}

స్థిర bool fanotify_event_info_శీర్షిక_is_valid(
                స్థిరరాశి నిర్మాణం fanotify_event_info_header *h,
                స్థిరరాశి నిర్మాణం fanotify_event_metadata *e) {

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(e);

        తిరిగి (స్థిరరాశి uint8_t*) h + పరిమాణం(*h) <= (స్థిరరాశి uint8_t*) e + e->event_len &&
                h->len >= పరిమాణం(*h) &&
                (స్థిరరాశి uint8_t*) h + h->len <= (స్థిరరాశి uint8_t*) e + e->event_len;
}

స్థిర స్థిరరాశి నిర్మాణం fanotify_event_info_header* fanotify_event_info_శీర్షిక_తదుపరి(స్థిరరాశి నిర్మాణం fanotify_event_info_header *h) {
        ప్రతిజ్ఞా(h);

        తిరిగి (స్థిరరాశి నిర్మాణం fanotify_event_info_header*) ((స్థిరరాశి uint8_t*) h + h->len);
}

/* This evaluates arguments multiple times */
#నిర్వచించు FOREACH_FANOTIFY_EVENT_INFO(h, e)                               \
        కొరకు (స్థిరరాశి నిర్మాణం fanotify_event_info_header *h = fanotify_event_info_శీర్షిక_మొదటి(e); \
             fanotify_event_info_శీర్షిక_is_valid(h, e);                 \
             h = fanotify_event_info_శీర్షిక_తదుపరి(h))

#నిర్వచించు FID_MIN_కొలత (offsetof(నిర్మాణం fanotify_event_info_fid, handle) + offsetof(నిర్మాణం file_handle, f_handle))

పూర్ణాంకం fanotify_event_pin_fd(
                స్థిరరాశి నిర్మాణం fanotify_event_metadata *e,
                పూర్ణాంకం mount_fd,
                స్థిరరాశి నిర్మాణం statfs *sfs) {

        ప్రతిజ్ఞా(e);
        ప్రతిజ్ఞా(mount_fd >= 0);

        FOREACH_FANOTIFY_EVENT_INFO(h, e) {
                ఒకవేళ (h->info_type != FAN_EVENT_INFO_TYPE_FID)
                        కొనసాగు;

                ఒకవేళ (h->len < FID_MIN_కొలత)
                        తిరిగి -EBADMSG;

                స్థిరరాశి నిర్మాణం fanotify_event_info_fid *fid = (స్థిరరాశి నిర్మాణం fanotify_event_info_fid*) h;
                నిర్మాణం file_handle *fh = (నిర్మాణం file_handle*) fid->handle;

                ఒకవేళ (h->len < FID_MIN_కొలత + fh->handle_బైట్లు)
                        తిరిగి -EBADMSG;

                నిర్మాణం statfs our_sfs;
                ఒకవేళ (!statfs_is_సమితి(sfs)) {
                        ఒకవేళ (fstatfs(mount_fd, &our_sfs) < 0)
                                తిరిగి -errno;

                        sfs = &our_sfs;
                }

                ఒకవేళ (memcmp(&fid->fsid, &sfs->f_fsid, పరిమాణం(fid->fsid)) != 0)
                        తిరిగి -ESTALE;

                తిరిగి RET_NERRNO(open_by_handle_at(mount_fd, fh, O_CLOEXEC | O_మార్గం));
        }

        తిరిగి -ENODATA;
}

పూర్ణాంకం fanotify_mark_fd(పూర్ణాంకం fanotify_fd, గుర్తులేని flags, uint64_t ముసుగు_3, పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(fanotify_fd >= 0);
        ప్రతిజ్ఞా(fd >= 0);

        /* This also works for O_PATH fds, which fanotify_mark() rejects as dirfd. */
        తిరిగి RET_NERRNO(fanotify_mark(fanotify_fd, flags, ముసుగు_3, AT_FDCWD, FORMAT_PROC_FD_మార్గం(fd)));
}
