/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-seq.h"
#அந்தர்பாவ "string-util.h"

வெறுமை ansi_seq_parser_done(AnsiSeqParser *p) {
        ப்ரதிஜ்ஞா(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

நிலையான வெறுமை ansi_seq_parser_begin_string(AnsiSeqParser *p, எழுத்து introducer) {
        ப்ரதிஜ்ஞா(p);

        p->string_len = 0;
        எனில் (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

நிலையான முழு ansi_seq_parser_தள்ளு_string(AnsiSeqParser *p, எழுத்து c) {
        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        எனில் (p->string_len >= ANSI_SEQ_STRING_MAX)
                திரும்பு 0; /* too long */

        எனில் (p->capture) {
                எனில் (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        திரும்பு -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        திரும்பு 1; /* added */
}

நிலையான முழு ansi_seq_parser_bad(AnsiSeqParser *p) {
        ப்ரதிஜ்ஞா(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        திரும்பு ANSI_SEQ_EVENT_ABORT;
}

முழு ansi_seq_parser_feed(AnsiSeqParser *p, எழுத்து c) {
        uint8_t u = (uint8_t) c;
        முழு r;

        ப்ரதிஜ்ஞா(p);

        தேர்வு (p->state) {

        நிலை ANSI_SEQ_STATE_GROUND:
                எனில் (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        திரும்பு ANSI_SEQ_EVENT_SEQUENCE;
                }

                திரும்பு ANSI_SEQ_EVENT_TEXT;

        நிலை ANSI_SEQ_STATE_ESC:
                தேர்வு (c) {

                நிலை '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                நிலை ']': /* OSC */
                நிலை 'P': /* DCS */
                நிலை 'X': /* SOS */
                நிலை '^': /* PM */
                நிலை '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                நிலை 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                நிலை 0x51 ... 0x57:
                நிலை 0x59 ... 0x5a:
                நிலை 0x5c:
                நிலை 0x60 ... 0x7e: /* Fs sequence */
                நிலை 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        திரும்பு ANSI_SEQ_EVENT_END;

                நிலை 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                இயல்பு:
                        /* Anything else is not a valid escape sequence, give up on it */
                        திரும்பு ansi_seq_parser_bad(p);
                }

        நிலை ANSI_SEQ_STATE_CSI:
                எனில் (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_தள்ளு_string(p, c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        திரும்பு ANSI_SEQ_EVENT_END;
                }

                எனில் (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_தள்ளு_string(p, c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r > 0)
                                திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                திரும்பு ansi_seq_parser_bad(p);

        நிலை ANSI_SEQ_STATE_STRING:
                எனில் (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        திரும்பு ANSI_SEQ_EVENT_END;
                }

                எனில் (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        திரும்பு ANSI_SEQ_EVENT_SEQUENCE;
                }

                எனில் (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_தள்ளு_string(p, c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r > 0)
                                திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                திரும்பு ansi_seq_parser_bad(p);

        நிலை ANSI_SEQ_STATE_STRING_ESC:
                எனில் (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        திரும்பு ANSI_SEQ_EVENT_END;
                }

                திரும்பு ansi_seq_parser_bad(p);

        நிலை ANSI_SEQ_STATE_NF:
                எனில் (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_தள்ளு_string(p, c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        திரும்பு ANSI_SEQ_EVENT_END;
                }

                எனில் (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_தள்ளு_string(p, c);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r > 0)
                                திரும்பு ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                திரும்பு ansi_seq_parser_bad(p);

        இயல்பு:
                assert_not_reached();
        }
}

முழு ansi_seq_parser_feed_harder(AnsiSeqParser *p, எழுத்து c) {
        முழு r;

        ப்ரதிஜ்ஞா(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        செய் {
                r = ansi_seq_parser_feed(p, c);
        } போது (r == ANSI_SEQ_EVENT_ABORT);

        திரும்பு r;
}

மாறிலி எழுத்து* ansi_seq_parser_string(மாறிலி AnsiSeqParser *p) {
        ப்ரதிஜ்ஞா(p);

        எனில் (!p->capture || p->bad || p->introducer == 0)
                திரும்பு வெற்று;

        திரும்பு strempty(p->string);
}
