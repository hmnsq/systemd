/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
ಎಣಿಕೆ pcapng_ಖಂಡ_types {
        PCAPNG_INTERFACE_ಖಂಡ = 1,
        PCAPNG_PACKET_ಖಂಡ, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_ಖಂಡ,
        PCAPNG_NAME_RESOLUTION_ಖಂಡ,
        PCAPNG_INTERFACE_STATS_ಖಂಡ,
        PCAPNG_ENHANCED_PACKET_ಖಂಡ,

        PCAPNG_SECTION_ಖಂಡ   = 0x0A0D0D0A,
};

ರಚನೆ pcapng_option {
        uint16_t ಸಂಕೇತ_2;
        uint16_t length;
        uint8_t ದತ್ತಾಂಶ[];
};

#ವ್ಯಾಖ್ಯಾನಿಸು PCAPNG_BYTE_ಕ್ರಮ_MAGIC 0x1A2B3C4D
#ವ್ಯಾಖ್ಯಾನಿಸು PCAPNG_MAJOR_VERS 1
#ವ್ಯಾಖ್ಯಾನಿಸು PCAPNG_MINOR_VERS 0

ಎಣಿಕೆ pcapng_opt {
        PCAPNG_OPT_ಕೊನೆ     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

ರಚನೆ pcapng_section {
        uint32_t ಖಂಡ_type;
        uint32_t ಖಂಡ_ಉದ್ದಳತೆ;
        uint32_t byte_ಕ್ರಮ_magic;
        uint16_t major_ಆವೃತ್ತಿ;
        uint16_t minor_ಆವೃತ್ತಿ;
        uint64_t section_ಉದ್ದಳತೆ;
};

ಎಣಿಕೆ pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

ರಚನೆ pcapng_interface_ಖಂಡ {
        uint32_t ಖಂಡ_type;   /* 1 */
        uint32_t ಖಂಡ_ಉದ್ದಳತೆ;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

ಎಣಿಕೆ pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_ವೇಗ,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

ರಚನೆ pcapng_enhance_packet_ಖಂಡ {
        uint32_t ಖಂಡ_type; /* 6 */
        uint32_t ಖಂಡ_ಉದ್ದಳತೆ;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_ಉದ್ದಳತೆ;
        uint32_t original_ಉದ್ದಳತೆ;
};

/* Flags values */
#ವ್ಯಾಖ್ಯಾನಿಸು PCAPNG_IFB_INBOUND   0b01
#ವ್ಯಾಖ್ಯಾನಿಸು PCAPNG_IFB_OUTBOUND  0b10

ಎಣಿಕೆ pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_ಸರತಿ,
        PCAPNG_EPB_VERDICT,
};

ರಚನೆ pcapng_statistics_ಖಂಡ {
        uint32_t ಖಂಡ_type; /* 5 */
        uint32_t ಖಂಡ_ಉದ್ದಳತೆ;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

ಎಣಿಕೆ pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
