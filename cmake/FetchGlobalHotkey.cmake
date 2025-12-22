include(FetchContent)
FetchContent_Declare(
    global_hotkey
    GIT_REPOSITORY https://github.com/JaderoChan/global_hotkey.git
    GIT_TAG main
)
set(GLOBAL_HOTKEY_DISABLE_HOOK ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(global_hotkey)
