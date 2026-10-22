/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

పూర్ణాంకం parse_env_filev(FILE *f, స్థిరరాశి అక్షరం *fname, va_list ap);
పూర్ణాంకం parse_env_file_fdv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fname, va_list ap);
పూర్ణాంకం parse_env_file_sentinel(FILE *f, స్థిరరాశి అక్షరం *fname, ...) _sentinel_;
#నిర్వచించు parse_env_file(f, fname, ...) parse_env_file_sentinel(f, fname, __VA_ARGS__, ఖాళీ)
పూర్ణాంకం parse_env_file_fd_sentinel(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fname, ...) _sentinel_;
#నిర్వచించు parse_env_file_fd(fd, fname, ...) parse_env_file_fd_sentinel(fd, fname, __VA_ARGS__, ఖాళీ)

పూర్ణాంకం parse_env_datav(స్థిరరాశి అక్షరం *దత్తాంశం, size_t కొలత, స్థిరరాశి అక్షరం *fname, va_list ap);
పూర్ణాంకం parse_env_దత్తాంశం_sentinel(స్థిరరాశి అక్షరం *దత్తాంశం, size_t కొలత, స్థిరరాశి అక్షరం *fname, ...) _sentinel_;
#నిర్వచించు parse_env_దత్తాంశం(పాఠ్యం, కొలత, fname, ...) parse_env_దత్తాంశం_sentinel(పాఠ్యం, కొలత, fname, __VA_ARGS__, ఖాళీ)

పూర్ణాంకం ఎక్కించు_env_file(FILE *f, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret);
పూర్ణాంకం ఎక్కించు_env_file_జతలు(FILE *f, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret);
పూర్ణాంకం ఎక్కించు_env_file_జతలు_fd(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fname, అక్షరం ***ret);

పూర్ణాంకం విలీనం_env_file(అక్షరం ***env, FILE *f, స్థిరరాశి అక్షరం *fname);

రకనిర్వచనం గణన WriteEnvFileFlags {
        WRITE_ENV_FILE_LABEL = 1 << 0,
} WriteEnvFileFlags;

పూర్ణాంకం write_env_file_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fname, అక్షరం **శీర్షికలు, అక్షరం **l, WriteEnvFileFlags flags, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం write_env_file(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fname, అక్షరం **శీర్షికలు, అక్షరం **l, WriteEnvFileFlags flags) {
        తిరిగి write_env_file_label(dir_fd, fname, శీర్షికలు, l, flags, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం write_vconsole_conf(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fname, అక్షరం **l);

శూన్య env_file_fputs_assignment(FILE *f, స్థిరరాశి అక్షరం *k, స్థిరరాశి అక్షరం *v);
