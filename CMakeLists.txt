cmake_minimum_required(VERSION 3.16)
project(sudo-therapist C)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "" FORCE)
endif()

if(CMAKE_INSTALL_SYSCONFDIR STREQUAL "etc")
    set(CMAKE_INSTALL_SYSCONFDIR "/etc" CACHE PATH "" FORCE)
endif()

include(GNUInstallDirs)

set(SUDO_THERAPIST_PAM_DIR "${CMAKE_INSTALL_LIBDIR}/security" CACHE PATH
    "PAM module directory, relative to install prefix unless overridden")

if(SUDO_THERAPIST_PAM_DIR STREQUAL "${CMAKE_INSTALL_LIBDIR}/security" AND NOT CMAKE_CROSSCOMPILING)
    set(_pam_dirs
        "/usr/lib/security" "/usr/lib64/security"
        "/lib/security" "/lib64/security"
    )

    file(GLOB _pam_glob_dirs LIST_DIRECTORIES true
        "/usr/lib/*/security"
        "/lib/*/security"
    )
    list(APPEND _pam_dirs ${_pam_glob_dirs})
    list(REMOVE_DUPLICATES _pam_dirs)

    find_file(_pam_ref_module
        NAMES pam_unix.so pam_deny.so
        PATHS ${_pam_dirs}
        NO_DEFAULT_PATH
    )

    if(_pam_ref_module)
        get_filename_component(_pam_security_dir "${_pam_ref_module}" DIRECTORY)
        get_filename_component(_pam_security_real "${_pam_security_dir}" REALPATH)

        if(_pam_security_real MATCHES "^/usr/(.*)$")
            set(SUDO_THERAPIST_PAM_DIR "${CMAKE_MATCH_1}" CACHE PATH "" FORCE)
        endif()
    endif()
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(pam_sudo_therapist SHARED
    src/pam_sudo_therapist.c
    src/st_config.c
    src/st_gate.c
    src/st_messages.c
    src/st_util.c
)

target_include_directories(pam_sudo_therapist PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_definitions(pam_sudo_therapist PRIVATE _GNU_SOURCE)
target_link_libraries(pam_sudo_therapist m pam)

set_target_properties(pam_sudo_therapist PROPERTIES
    PREFIX ""
    OUTPUT_NAME "pam_sudo_therapist"
)

install(TARGETS pam_sudo_therapist
    LIBRARY DESTINATION "${SUDO_THERAPIST_PAM_DIR}"
)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/packaging/pam-configs/sudo-therapist"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/pam-configs"
)

install(FILES "${CMAKE_CURRENT_LIST_DIR}/packaging/etc/sudo-therapist.conf"
    DESTINATION "${CMAKE_INSTALL_SYSCONFDIR}"
)
