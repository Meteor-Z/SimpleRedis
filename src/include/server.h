#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include "file/file.h"

namespace SimpleRedis {
class Server {
public:
    
private:
    File m_file{};
    std::unordered_map<int,std::unique_ptr<>>
};
} // namespace SimpleRedis