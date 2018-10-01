project("Common")

add_library(${PROJECT_NAME} src/common.cpp src/userProfile.cpp src/address.cpp)
add_library(commonLib ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    nlohmann_json
)

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)