Include(FetchContent)

FetchContent_Declare(
  ordered_map
  GIT_REPOSITORY https://github.com/Tessil/ordered-map.git
  GIT_TAG        v1.1.0 # or a later release
)

FetchContent_MakeAvailable(ordered_map)

add_library(INILib "src/INILib.cpp")
target_include_directories(INILib PRIVATE inc PRIVATE "C:/Users/Tashkent/Desktop/boost_1_86_0")
target_link_libraries(INILib PRIVATE ordered_map)

add_executable(test "test/test.cpp")
target_include_directories(test PRIVATE inc PRIVATE example_files)
target_link_libraries(test PRIVATE INILib)