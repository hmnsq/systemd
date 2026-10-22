/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/btrfs.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "btrfs-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం btrfs_validate_subvolume_name(స్థిరరాశి అక్షరం *name) {

        ఒకవేళ (!filename_is_valid(name))
                తిరిగి -EINVAL;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(name) > BTRFS_SUBVOL_NAME_MAX)
                తిరిగి -E2BIG;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం వెలికితీయి_subvolume_name(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *fn = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(ret);

        r = మార్గం_వెలికితీయి_filename(మార్గం, &fn);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = btrfs_validate_subvolume_name(fn);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(fn);
        తిరిగి 0;
}

పూర్ణాంకం btrfs_subvol_make(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం) {
        నిర్మాణం btrfs_ioctl_vol_args వాదనలు = {};
        _cleanup_free_ అక్షరం *subvolume = ఖాళీ, *మాతృ = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(!isempty(మార్గం));

        r = వెలికితీయి_subvolume_name(మార్గం, &subvolume);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = మార్గం_వెలికితీయి_directory(మార్గం, &మాతృ);
        ఒకవేళ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                తిరిగి r;

        fd = xopenat(dir_fd, మాతృ ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        strncpy(వాదనలు.name, subvolume, పరిమాణం(వాదనలు.name)-1);

        తిరిగి RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &వాదనలు));
}

పూర్ణాంకం btrfs_subvol_make_fallback(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        mode_t old, combined;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~బహుళకం);
        combined = old | ~బహుళకం;
        ఒకవేళ (combined != ~బహుళకం)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, మార్గం);
        umask(old);

        ఒకవేళ (r >= 0)
                తిరిగి 1; /* subvol worked */
        ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(r))
                తిరిగి r;

        ఒకవేళ (mkdirat(dir_fd, మార్గం, బహుళకం) < 0)
                తిరిగి -errno;

        తిరిగి 0; /* plain directory */
}
