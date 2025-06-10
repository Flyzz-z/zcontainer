#ifndef UTILS_H
#define UTILS_H

#include "easylogging++.h"
#include <string>
#include <sys/stat.h>

namespace zcontainer {

class Utils {
public:
  bool static createDirectories(const std::string &dirPath) {
    const char separator = '/';
    std::string subPath;
    size_t pos = 0;

    // 逐级遍历路径，每次创建一个子目录
    while ((pos = dirPath.find(separator, pos + 1)) != std::string::npos) {
      subPath = dirPath.substr(0, pos);
      if (mkdir(subPath.c_str(), 0777) != 0 && errno != EEXIST) {
        LOG(ERROR) << "Failed to create directory: " << subPath;
        return false;
      }
    }
    // 创建最后一个目录
    if (mkdir(dirPath.c_str(), 0777) != 0 && errno != EEXIST) {
      LOG(ERROR) << "Failed to create directory: " << dirPath;
      return false;
    }
    return true;
  } // namespace zcontainer

  void static removeDirectories(const std::string &dirPath) {
    std::string command = "rm -rf " + dirPath;
    int status = system(command.c_str());
    if (!WIFEXITED(status)) {
      LOG(ERROR) << "Failed to remove directory: " << dirPath;
    }
  }
};
}; // namespace zcontainer
#endif // UTILS_H
