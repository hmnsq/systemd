/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
تعداد pcapng_بلاڪ_types {
        PCAPNG_INTERFACE_بلاڪ = 1,
        PCAPNG_PACKET_بلاڪ, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_بلاڪ,
        PCAPNG_NAME_RESOLUTION_بلاڪ,
        PCAPNG_INTERFACE_STATS_بلاڪ,
        PCAPNG_ENHANCED_PACKET_بلاڪ,

        PCAPNG_SECTION_بلاڪ   = 0x0A0D0D0A,
};

بنية pcapng_option {
        uint16_t ڪوڊ;
        uint16_t length;
        uint8_t معلومات[];
};

#وصف_ڪريو PCAPNG_BYTE_ترتيب_MAGIC 0x1A2B3C4D
#وصف_ڪريو PCAPNG_MAJOR_VERS 1
#وصف_ڪريو PCAPNG_MINOR_VERS 0

تعداد pcapng_opt {
        PCAPNG_OPT_پڇاڙي     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

بنية pcapng_section {
        uint32_t بلاڪ_type;
        uint32_t بلاڪ_ڊيگهائي;
        uint32_t byte_ترتيب_magic;
        uint16_t major_نسخو;
        uint16_t minor_نسخو;
        uint64_t section_ڊيگهائي;
};

تعداد pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

بنية pcapng_interface_بلاڪ {
        uint32_t بلاڪ_type;   /* 1 */
        uint32_t بلاڪ_ڊيگهائي;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

تعداد pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_رفتار,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

بنية pcapng_enhance_packet_بلاڪ {
        uint32_t بلاڪ_type; /* 6 */
        uint32_t بلاڪ_ڊيگهائي;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_ڊيگهائي;
        uint32_t original_ڊيگهائي;
};

/* Flags values */
#وصف_ڪريو PCAPNG_IFB_INBOUND   0b01
#وصف_ڪريو PCAPNG_IFB_OUTBOUND  0b10

تعداد pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_قطار,
        PCAPNG_EPB_VERDICT,
};

بنية pcapng_statistics_بلاڪ {
        uint32_t بلاڪ_type; /* 5 */
        uint32_t بلاڪ_ڊيگهائي;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

تعداد pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
