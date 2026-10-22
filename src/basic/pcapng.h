/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/*
 * For details about the file format see RFC:
 *   https://www.ietf.org/id/draft-tuexen-opsawg-pcapng-03.html
 *  and
 *    https://github.com/pcapng/pcapng/
 */
ਗਿਣਤੀ pcapng_ਖੰਡ_types {
        PCAPNG_INTERFACE_ਖੰਡ = 1,
        PCAPNG_PACKET_ਖੰਡ, /* Obsolete */
        PCAPNG_SIMPLE_PACKET_ਖੰਡ,
        PCAPNG_NAME_RESOLUTION_ਖੰਡ,
        PCAPNG_INTERFACE_STATS_ਖੰਡ,
        PCAPNG_ENHANCED_PACKET_ਖੰਡ,

        PCAPNG_SECTION_ਖੰਡ   = 0x0A0D0D0A,
};

ਬਣਤਰ pcapng_option {
        uint16_t ਕੋਡ;
        uint16_t length;
        uint8_t ਅੰਕੜੇ[];
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCAPNG_BYTE_ਕ੍ਰਮ_MAGIC 0x1A2B3C4D
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCAPNG_MAJOR_VERS 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCAPNG_MINOR_VERS 0

ਗਿਣਤੀ pcapng_opt {
        PCAPNG_OPT_ਸਿਰਾ     = 0,
        PCAPNG_OPT_COMMENT = 1,
};

ਬਣਤਰ pcapng_section {
        uint32_t ਖੰਡ_type;
        uint32_t ਖੰਡ_ਲੰਮਾਈ;
        uint32_t byte_ਕ੍ਰਮ_magic;
        uint16_t major_ਸੰਸਕਰਣ;
        uint16_t minor_ਸੰਸਕਰਣ;
        uint64_t section_ਲੰਮਾਈ;
};

ਗਿਣਤੀ pcapng_section_opt {
        PCAPNG_SHB_HARDWARE = 2,
        PCAPNG_SHB_OS       = 3,
        PCAPNG_SHB_USERAPPL = 4,
};

ਬਣਤਰ pcapng_interface_ਖੰਡ {
        uint32_t ਖੰਡ_type;   /* 1 */
        uint32_t ਖੰਡ_ਲੰਮਾਈ;
        uint16_t link_type;
        uint16_t reserved;
        uint32_t snap_len;
};

ਗਿਣਤੀ pcapng_interface_options {
        PCAPNG_IFB_NAME = 2,
        PCAPNG_IFB_DESCRIPTION,
        PCAPNG_IFB_IPV4ADDR,
        PCAPNG_IFB_IPV6ADDR,
        PCAPNG_IFB_MACADDR,
        PCAPNG_IFB_EUIADDR,
        PCAPNG_IFB_ਰਫਤਾਰ,
        PCAPNG_IFB_TSRESOL,
        PCAPNG_IFB_TZONE,
        PCAPNG_IFB_FILTER,
        PCAPNG_IFB_OS,
        PCAPNG_IFB_FCSLEN,
        PCAPNG_IFB_TSOFFSET,
        PCAPNG_IFB_HARDWARE,
};

ਬਣਤਰ pcapng_enhance_packet_ਖੰਡ {
        uint32_t ਖੰਡ_type; /* 6 */
        uint32_t ਖੰਡ_ਲੰਮਾਈ;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
        uint32_t capture_ਲੰਮਾਈ;
        uint32_t original_ਲੰਮਾਈ;
};

/* Flags values */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCAPNG_IFB_INBOUND   0b01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PCAPNG_IFB_OUTBOUND  0b10

ਗਿਣਤੀ pcapng_epb_options {
        PCAPNG_EPB_FLAGS = 2,
        PCAPNG_EPB_HASH,
        PCAPNG_EPB_DROPCOUNT,
        PCAPNG_EPB_PACKETID,
        PCAPNG_EPB_ਕਤਾਰ,
        PCAPNG_EPB_VERDICT,
};

ਬਣਤਰ pcapng_statistics_ਖੰਡ {
        uint32_t ਖੰਡ_type; /* 5 */
        uint32_t ਖੰਡ_ਲੰਮਾਈ;
        uint32_t interface_id;
        uint32_t timestamp_hi;
        uint32_t timestamp_lo;
};

ਗਿਣਤੀ pcapng_isb_options {
        PCAPNG_ISB_STARTTIME = 2,
        PCAPNG_ISB_ENDTIME,
        PCAPNG_ISB_IFRECV,
        PCAPNG_ISB_IFDROP,
        PCAPNG_ISB_FILTERACCEPT,
        PCAPNG_ISB_OSDROP,
        PCAPNG_ISB_USRDELIV,
};
