/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/btrfs.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "btrfs-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ btrfs_validate_subvolume_name(സ്ഥിരം അക്ഷരം *name) {

        എങ്കിൽ (!filename_is_valid(name))
                തിരികെ -EINVAL;

        എങ്കിൽ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                തിരികെ -E2BIG;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ extract_subvolume_name(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *fn = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        r = path_extract_filename(path, &fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = btrfs_validate_subvolume_name(fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(fn);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ btrfs_subvol_make(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path) {
        ഘടന btrfs_ioctl_vol_args args = {};
        _cleanup_free_ അക്ഷരം *subvolume = ശൂന്യ, *parent = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));
        പ്രതിജ്ഞാ(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = path_extract_directory(path, &parent);
        എങ്കിൽ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                തിരികെ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        strncpy(args.name, subvolume, വലുപ്പം(args.name)-1);

        തിരികെ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

പൂർണ്ണസംഖ്യ btrfs_subvol_make_fallback(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        mode_t old, combined;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        എങ്കിൽ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        എങ്കിൽ (r >= 0)
                തിരികെ 1; /* subvol worked */
        എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(r))
                തിരികെ r;

        എങ്കിൽ (mkdirat(dir_fd, path, mode) < 0)
                തിരികെ -errno;

        തിരികെ 0; /* plain directory */
}
