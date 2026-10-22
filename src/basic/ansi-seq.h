/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* A minimal, generic state engine that tracks ANSI escape sequence boundaries in a byte stream. It does not
 * interpret sequences, it only determines where they begin and end, so that callers can find "safe"
 * positions in a stream at which generated sequences may be inserted without corrupting a sequence already
 * in progress. Optionally, the payload of sequences is captured, so that callers may act on specific
 * sequences: for CSI and nF sequences that's the parameter, intermediate and final bytes, for "string"
 * sequences (OSC and friends) it's the string itself. */

تعريفنوع تعداد AnsiSeqState {
        ANSI_SEQ_STATE_GROUND,     /* regular characters */
        ANSI_SEQ_STATE_ESC,        /* ESC seen, or within an escape sequence that is neither CSI nor a string sequence */
        ANSI_SEQ_STATE_CSI,        /* within a CSI sequence */
        ANSI_SEQ_STATE_STRING,     /* within a string sequence (OSC, DCS, SOS, PM, APC) */
        ANSI_SEQ_STATE_STRING_ESC, /* within a string sequence, ESC seen (presumably the beginning of ST) */
        ANSI_SEQ_STATE_NF,        /* within an NF sequence */
        _ANSI_SEQ_STATE_MAX,
        _ANSI_SEQ_STATE_INVALID = -EINVAL,
} AnsiSeqState;

تعريفنوع تعداد AnsiSeqEvent {
        ANSI_SEQ_EVENT_متن,       /* a regular character, not part of any sequence */
        ANSI_SEQ_EVENT_SEQUENCE,   /* a character belonging to a sequence that is not complete yet */
        ANSI_SEQ_EVENT_پڇاڙي,        /* the character completes a sequence, its payload may be queried */
        ANSI_SEQ_EVENT_ABORT,      /* the character aborted a sequence and was not consumed: feed it again! */
        _ANSI_SEQ_EVENT_MAX,
        _ANSI_SEQ_EVENT_INVALID = -EINVAL,
} AnsiSeqEvent;

/* Maximum length of a sequence payload we are willing to process. A sequence that grows beyond this is
 * considered invalid and aborted. */
#وصف_ڪريو ANSI_SEQ_STRING_MAX 192U

تعريفنوع بنية AnsiSeqParser {
        AnsiSeqState state;
        حرف introducer;    /* for string sequences: the character that introduced it, e.g. ']' for OSC */
        حرف *string;       /* the accumulated payload of the most recent string sequence */
        size_t string_len;
        bool bad;           /* the current string sequence grew beyond ANSI_SEQ_STRING_MAX or is otherwise invalid */
        bool capture;       /* if true we'll capture the sequence */
} AnsiSeqParser;

فراغ ansi_seq_parser_پورو(AnsiSeqParser *p);

/* Feeds a single character, returns an AnsiSeqEvent, or a negative errno on capture allocation failure */
صحيح ansi_seq_parser_feed(AnsiSeqParser *p, حرف c);

/* Just like this, but eat up errors, and immediately submit character again */
صحيح ansi_seq_parser_feed_harder(AnsiSeqParser *p, حرف c);

/* After ANSI_SEQ_EVENT_END: the captured payload as NUL terminated string, or NULL if capturing is
 * off or the payload overflowed */
ثابت حرف* ansi_seq_parser_string(ثابت AnsiSeqParser *p);
