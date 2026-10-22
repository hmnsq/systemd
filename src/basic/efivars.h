/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ಅನ್ತರ್ಭಾವ "sd-id128.h"

#ಅನ್ತರ್ಭಾವ "../fundamental/efivars.h"        /* IWYU pragma: export */

#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_LOADER       SD_ID128_MAKE(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_LOADER_STR   SD_ID128_MAKE_UUID_STR(4a,67,b0,82,0a,4c,41,cf,b6,c7,44,0b,29,bb,8c,4f)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_GLOBAL       SD_ID128_MAKE(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_GLOBAL_STR   SD_ID128_MAKE_UUID_STR(8b,e4,df,61,93,ca,11,d2,aa,0d,00,e0,98,03,2b,8c)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_DATABASE     SD_ID128_MAKE(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_DATABASE_STR SD_ID128_MAKE_UUID_STR(d7,19,b2,cb,3d,3a,45,96,a3,bc,da,d0,0e,67,65,6f)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_SYSTEMD      SD_ID128_MAKE(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_SYSTEMD_STR  SD_ID128_MAKE_UUID_STR(8c,f2,64,4b,4b,0b,42,8f,93,87,6d,87,60,50,dc,67)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_SHIMLOCK     SD_ID128_MAKE(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_SHIMLOCK_STR SD_ID128_MAKE_UUID_STR(60,5d,ab,50,e0,46,43,00,ab,b6,3d,d8,10,dd,8b,23)

#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VARIABLE_NON_VOLATILE                          UINT32_C(0x00000001)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VARIABLE_BOOTSERVICE_ACCESS                    UINT32_C(0x00000002)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VARIABLE_RUNTIME_ACCESS                        UINT32_C(0x00000004)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VARIABLE_ಸಮಯ_BASED_AUTHENTICATED_WRITE_ACCESS UINT32_C(0x00000020)

/* Note that the <lowercaseuuid>-<varname> naming scheme is an efivarfs convention, i.e. part of the Linux
 * API file system implementation for EFI. EFI itself processes UIDS in binary form.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು EFI_VENDOR_VARIABLE_STR(vendor, name) name "-" vendor

#ವ್ಯಾಖ್ಯಾನಿಸು EFI_GLOBAL_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_GLOBAL_STR, name)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_LOADER_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_LOADER_STR, name)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_SYSTEMD_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SYSTEMD_STR, name)
#ವ್ಯಾಖ್ಯಾನಿಸು EFI_SHIMLOCK_VARIABLE_STR(name) EFI_VENDOR_VARIABLE_STR(EFI_VENDOR_SHIMLOCK_STR, name)

#ವ್ಯಾಖ್ಯಾನಿಸು EFIVAR_ಮಾರ್ಗ(variable) "/sys/firmware/efi/efivars/" variable
#ವ್ಯಾಖ್ಯಾನಿಸು EFIVAR_CACHE_ಮಾರ್ಗ(variable) "/run/systemd/efivars/" variable

#ಆದರೆ ENABLE_EFI

ಪೂರ್ಣಾಂಕ efi_get_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, uint32_t *attribute, ಶೂನ್ಯ **ret_ಮೌಲ್ಯ, size_t *ret_ಗಾತ್ರ);
ಪೂರ್ಣಾಂಕ efi_get_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ efi_get_variable_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2) _nonnull_if_nonzero_(2, 3);
ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

bool ಗಣ_efi_boot(bool b);
bool is_efi_boot(ಶೂನ್ಯ);
bool is_efi_secure_boot(ಶೂನ್ಯ);
Secure_Boot_ಬಹುಲಕ efi_get_secure_boot_ಬಹುಲಕ(ಶೂನ್ಯ);

#ಇಲ್ಲದಿದ್ದರೆ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ efi_get_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, uint32_t *attribute, ಶೂನ್ಯ **ಮೌಲ್ಯ, size_t *ಗಾತ್ರ_2) {
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ efi_get_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ efi_get_variable_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಮೌಲ್ಯ, size_t ಗಾತ್ರ_2) {
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ efi_ಗಣ_variable_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *variable, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಗಣ_efi_boot(bool b) {
        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_efi_boot(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool is_efi_secure_boot(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ Secure_Boot_ಬಹುಲಕ efi_get_secure_boot_ಬಹುಲಕ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ SECURE_BOOT_UNKNOWN;
}
#ಆದರೆ_ಅಂತ್ಯ

ಅಕ್ಷರ *efi_tilt_backslashes(ಅಕ್ಷರ *s);
