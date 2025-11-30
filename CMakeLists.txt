cmake_minimum_required(VERSION 4.0)
project(traffic_forecast)

set(CMAKE_CXX_STANDARD 20)

add_library(
    dataset STATIC
        dataset/dataset/Dataset.h
        dataset/dataset/Dataset.cpp
        dataset/dataset_value/DatasetValue.h
        dataset/dataset_value/DatasetValue.cpp
)

target_include_directories(dataset PUBLIC
    dataset/dataset
    dataset/dataset_value
)

add_executable(traffic_forecast main.cpp)
target_link_libraries(
    traffic_forecast PRIVATE
        dataset
)

# --- GoogleTest for unit tests ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(dataset_value_test
        tests/test_dataset_value.cpp
)

add_executable(dataset_test
        tests/test_dataset.cpp
)

# Тесты и ведомые зависимости теперь ссылаются на единую библиотеку dataset
target_link_libraries(dataset_value_test PRIVATE dataset gtest gtest_main)
add_test(NAME dataset_value_test COMMAND dataset_value_test)

# Добавляем сборку и регистрацию нового теста dataset_test
target_link_libraries(dataset_test PRIVATE dataset gtest gtest_main)
add_test(NAME dataset_test COMMAND dataset_test)
