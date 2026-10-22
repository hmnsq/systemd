/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-seq.h"
#അന്തര്ഭാവ "string-util.h"

ശൂന്യം ansi_seq_parser_done(AnsiSeqParser *p) {
        പ്രതിജ്ഞാ(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

സ്ഥിതം ശൂന്യം ansi_seq_parser_begin_string(AnsiSeqParser *p, അക്ഷരം introducer) {
        പ്രതിജ്ഞാ(p);

        p->string_len = 0;
        എങ്കിൽ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ansi_seq_parser_തള്ളുക_string(AnsiSeqParser *p, അക്ഷരം c) {
        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        എങ്കിൽ (p->string_len >= ANSI_SEQ_STRING_MAX)
                തിരികെ 0; /* too long */

        എങ്കിൽ (p->capture) {
                എങ്കിൽ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        തിരികെ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        തിരികെ 1; /* added */
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ansi_seq_parser_bad(AnsiSeqParser *p) {
        പ്രതിജ്ഞാ(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        തിരികെ ANSI_SEQ_EVENT_ABORT;
}

പൂർണ്ണസംഖ്യ ansi_seq_parser_feed(AnsiSeqParser *p, അക്ഷരം c) {
        uint8_t u = (uint8_t) c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);

        തിരഞ്ഞെടുപ്പ് (p->state) {

        അവസ്ഥ ANSI_SEQ_STATE_GROUND:
                എങ്കിൽ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        തിരികെ ANSI_SEQ_EVENT_SEQUENCE;
                }

                തിരികെ ANSI_SEQ_EVENT_TEXT;

        അവസ്ഥ ANSI_SEQ_STATE_ESC:
                തിരഞ്ഞെടുപ്പ് (c) {

                അവസ്ഥ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                അവസ്ഥ ']': /* OSC */
                അവസ്ഥ 'P': /* DCS */
                അവസ്ഥ 'X': /* SOS */
                അവസ്ഥ '^': /* PM */
                അവസ്ഥ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                അവസ്ഥ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                അവസ്ഥ 0x51 ... 0x57:
                അവസ്ഥ 0x59 ... 0x5a:
                അവസ്ഥ 0x5c:
                അവസ്ഥ 0x60 ... 0x7e: /* Fs sequence */
                അവസ്ഥ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        തിരികെ ANSI_SEQ_EVENT_END;

                അവസ്ഥ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                സ്ഥിരസ്ഥിതി:
                        /* Anything else is not a valid escape sequence, give up on it */
                        തിരികെ ansi_seq_parser_bad(p);
                }

        അവസ്ഥ ANSI_SEQ_STATE_CSI:
                എങ്കിൽ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_തള്ളുക_string(p, c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        തിരികെ ANSI_SEQ_EVENT_END;
                }

                എങ്കിൽ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_തള്ളുക_string(p, c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r > 0)
                                തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                തിരികെ ansi_seq_parser_bad(p);

        അവസ്ഥ ANSI_SEQ_STATE_STRING:
                എങ്കിൽ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        തിരികെ ANSI_SEQ_EVENT_END;
                }

                എങ്കിൽ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        തിരികെ ANSI_SEQ_EVENT_SEQUENCE;
                }

                എങ്കിൽ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_തള്ളുക_string(p, c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r > 0)
                                തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                തിരികെ ansi_seq_parser_bad(p);

        അവസ്ഥ ANSI_SEQ_STATE_STRING_ESC:
                എങ്കിൽ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        തിരികെ ANSI_SEQ_EVENT_END;
                }

                തിരികെ ansi_seq_parser_bad(p);

        അവസ്ഥ ANSI_SEQ_STATE_NF:
                എങ്കിൽ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_തള്ളുക_string(p, c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        തിരികെ ANSI_SEQ_EVENT_END;
                }

                എങ്കിൽ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_തള്ളുക_string(p, c);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r > 0)
                                തിരികെ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                തിരികെ ansi_seq_parser_bad(p);

        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}

പൂർണ്ണസംഖ്യ ansi_seq_parser_feed_harder(AnsiSeqParser *p, അക്ഷരം c) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        ചെയ്യുക {
                r = ansi_seq_parser_feed(p, c);
        } ആകുമ്പോൾ (r == ANSI_SEQ_EVENT_ABORT);

        തിരികെ r;
}

സ്ഥിരം അക്ഷരം* ansi_seq_parser_string(സ്ഥിരം AnsiSeqParser *p) {
        പ്രതിജ്ഞാ(p);

        എങ്കിൽ (!p->capture || p->bad || p->introducer == 0)
                തിരികെ ശൂന്യ;

        തിരികെ strempty(p->string);
}
