cmake_minimum_required(VERSION 3.10)  # 必须
set(CMAKE_CXX_STANDARD 20)	# C++20

project(SimpleRedis)  #工程名
set(CMAKE_CXX_STANDARD_REQUIRED True)



# 测试相关
add_executable(test main.cc)
add_executable(avl_test test/avl_test.cc)
add_executable(heap_test test/heap_test.cc)
add_executable(threadpoll_test test/threadpool_test.cc)
include_directories(${CMAKE_SOURCE_DIR}/src/include) # 加入头文件
# target_link_libraries(${PROJECT_NAME} pthread) # 链接库

# 下面使用install作为项目打包使用
# set(CMAKE_INSTALL_PREFIX ./dist)  # 自定义安装目录，打包使用
# install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin) # 打包二进制文件

# set(CONFIGS ${PROJECT_SOURCE_DIR}/hello.cf) 
# install(FILES ${CONFIGS} DESTINATION config) # 打包配置文件
