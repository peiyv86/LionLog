#pragma once
#include <string>

class GzipUtil {
public:
    // 将 src 文件压缩为 dst 文件，成功后删除 src 文件
    static void compress(const std::string& src, const std::string& dst);
};