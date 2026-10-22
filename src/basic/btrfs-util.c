/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/btrfs.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "btrfs-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क btrfs_validate_subvolume_name(ध्रुव वर्ण *name) {

        यदि (!filename_is_valid(name))
                प्रत्यागम -EINVAL;

        यदि (वाक्यदैर्घ्यम्(name) > BTRFS_SUBVOL_NAME_MAX)
                प्रत्यागम -E2BIG;

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क extract_subvolume_name(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        _cleanup_free_ वर्ण *fn = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(ret);

        r = मार्गः_extract_filename(मार्गः, &fn);
        यदि (r < 0)
                प्रत्यागम r;

        r = btrfs_validate_subvolume_name(fn);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(fn);
        प्रत्यागम 0;
}

पूर्णाङ्क btrfs_subvol_make(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः) {
        संरचना btrfs_ioctl_vol_args तर्काः = {};
        _cleanup_free_ वर्ण *subvolume = रिक्त, *parent = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(!isempty(मार्गः));

        r = extract_subvolume_name(मार्गः, &subvolume);
        यदि (r < 0)
                प्रत्यागम r;

        r = मार्गः_extract_directory(मार्गः, &parent);
        यदि (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                प्रत्यागम r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम fd;

        strncpy(तर्काः.name, subvolume, परिमाण(तर्काः.name)-1);

        प्रत्यागम RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &तर्काः));
}

पूर्णाङ्क btrfs_subvol_make_fallback(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्) {
        mode_t old, combined;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~बहुलकम्);
        combined = old | ~बहुलकम्;
        यदि (combined != ~बहुलकम्)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, मार्गः);
        umask(old);

        यदि (r >= 0)
                प्रत्यागम 1; /* subvol worked */
        यदि (!ERRNO_IS_NOT_SUPPORTED(r))
                प्रत्यागम r;

        यदि (mkdirat(dir_fd, मार्गः, बहुलकम्) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0; /* plain directory */
}
