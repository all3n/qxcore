# DownloadNinja.cmake
function(download_ninja)
    set(options USE_GITHUB_PROXY)
    set(oneValueArgs VERSION DOWNLOAD_DIR)
    set(multiValueArgs "")
    cmake_parse_arguments(DN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT DN_VERSION)
        set(DN_VERSION "1.13.2")
    endif()
    
    if(NOT DN_DOWNLOAD_DIR)
        set(DN_DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/ninja")
    endif()
    
    # 检测系统
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
            set(ARCH "linux-aarch64")
        else()
            set(ARCH "linux")
        endif()
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(ARCH "mac")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "ARM64")
            set(ARCH "winarm64")
        else()
            set(ARCH "win")
        endif()
    else()
        message(FATAL_ERROR "Unsupported system: ${CMAKE_HOST_SYSTEM_NAME}")
    endif()
    
    # 确定 ninja 可执行文件路径
    if(WIN32)
        set(NINJA_PATH "${DN_DOWNLOAD_DIR}/ninja.exe")
    else()
        set(NINJA_PATH "${DN_DOWNLOAD_DIR}/ninja")
    endif()
    
    # 检查 ninja 是否已经存在
    if(EXISTS ${NINJA_PATH})
        message(STATUS "Ninja already exists at: ${NINJA_PATH}")
        set(NINJA_PATH ${NINJA_PATH} PARENT_SCOPE)
        return()
    endif()
    
    set(FILENAME "ninja-${ARCH}.zip")
    
    if(DN_USE_GITHUB_PROXY)
        set(URL "https://p.all3n.top/github.com/ninja-build/ninja/releases/download/v${DN_VERSION}/${FILENAME}")
    else()
        set(URL "https://github.com/ninja-build/ninja/releases/download/v${DN_VERSION}/${FILENAME}")
    endif()
    
    # 创建下载目录
    file(MAKE_DIRECTORY ${DN_DOWNLOAD_DIR})
    
    # 下载文件
    message(STATUS "Downloading Ninja from: ${URL}")
    file(DOWNLOAD
        ${URL}
        "${DN_DOWNLOAD_DIR}/${FILENAME}"
        STATUS DOWNLOAD_STATUS
        SHOW_PROGRESS
    )
    
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        list(GET DOWNLOAD_STATUS 1 ERROR_MSG)
        message(FATAL_ERROR "Failed to download Ninja: ${ERROR_MSG}")
    endif()
    
    # 解压文件
    message(STATUS "Extracting Ninja...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf "${DN_DOWNLOAD_DIR}/${FILENAME}"
        WORKING_DIRECTORY ${DN_DOWNLOAD_DIR}
        RESULT_VARIABLE EXTRACT_RESULT
    )
    
    if(NOT EXTRACT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to extract Ninja")
    endif()
    
    # 设置可执行权限（Unix系统）
    if(UNIX)
        execute_process(
            COMMAND chmod +x "${DN_DOWNLOAD_DIR}/ninja"
            RESULT_VARIABLE CHMOD_RESULT
        )
    endif()
    
    # 返回路径
    set(NINJA_PATH ${NINJA_PATH} PARENT_SCOPE)
    
    message(STATUS "Ninja downloaded to: ${NINJA_PATH}")
endfunction()
