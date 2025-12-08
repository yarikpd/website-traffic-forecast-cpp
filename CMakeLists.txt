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
target_link_libraries(
    dataset PRIVATE
        forecast_utils
)

add_library(
    forecast STATIC
        forecast/forecast.h
        forecast/forecast.cpp
)
target_include_directories(forecast PUBLIC
    forecast
)
target_link_libraries(
    forecast PRIVATE
        forecast_utils
)

add_library(
    forecast_utils STATIC
        forecast_utils/forecast_utils.h
        forecast_utils/forecast_utils.cpp
)
target_include_directories(
    forecast_utils PUBLIC
        forecast_utils
)
target_link_libraries(
    forecast_utils PUBLIC
        crypt
)

add_library(
    crypt STATIC
        crypt/crypt.h
        crypt/crypt.cpp
)
target_include_directories(
    crypt PUBLIC
        crypt
)

add_executable(traffic_forecast main.cpp)
target_link_libraries(
    traffic_forecast PRIVATE
        dataset
        forecast
        forecast_utils
        crypt
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

# Тесты для криптографии SEED
add_executable(crypt_test
        tests/test_crypt.cpp
)
target_link_libraries(crypt_test PRIVATE crypt gtest gtest_main)
add_test(NAME crypt_test COMMAND crypt_test)
