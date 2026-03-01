#include "GzipUtil.h"

#include <zlib.h>    // ZLIB 核心头文件
#include <cstdio>    // fopen, fclose
#include <filesystem> // 用于删除源文件

void GzipUtil::compress(const std::string& src, const std::string& dst) {
    // 1. 打开源文件 (二进制读)
    FILE* infile = fopen(src.c_str(), "rb");
    if (!infile) return;

    // 2. 打开目标压缩文件 (二进制写, wb9 表示最大压缩率)
    gzFile outfile = gzopen(dst.c_str(), "wb9");
    if (!outfile) {
        fclose(infile);
        return;
    }

    // 3. 读写循环
    char buffer[128 * 1024]; // 128KB 缓冲区
    int num_read = 0;
    while ((num_read = fread(buffer, 1, sizeof(buffer), infile)) > 0) {
        gzwrite(outfile, buffer, num_read);
    }

    // 4. 关闭文件
    gzclose(outfile);
    fclose(infile);

    // 5. 压缩成功后，删除源文件 (模拟 gzip 命令的行为)
    std::filesystem::remove(src);
}