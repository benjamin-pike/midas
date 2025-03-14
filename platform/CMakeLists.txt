cmake_minimum_required(VERSION 3.10)
project(OrderBookSimulation)

set(CMAKE_CXX_STANDARD 23)

find_package(GTest REQUIRED)
find_package(SQLite3 REQUIRED)

find_path(CROW_INCLUDE_DIR crow.h PATHS /opt/homebrew/include)
include_directories(${CROW_INCLUDE_DIR})

add_library(orderbook_lib
    src/core/OrderBook.cpp

    src/core/services/ActiveOrderService.cpp
    src/core/services/ConditionalOrderService.cpp
    src/core/services/MarketService.cpp
    src/core/services/RiskService.cpp
    src/core/services/TraderService.cpp
    src/core/services/TradeService.cpp

    src/core/database/Database.cpp
    src/core/database/OrderMapping.cpp
    src/core/database/OrderRepository.cpp
    src/core/database/TradeMapping.cpp
    src/core/database/TradeRepository.cpp

    src/core/matcher/MatchingEngine.cpp
    src/core/matcher/DefaultMatchingStrategy.cpp
    src/core/matcher/IOCMatchingStrategy.cpp
    src/core/matcher/FOKMatchingStrategy.cpp

    src/core/models/Event.cpp
    src/core/models/Trader.cpp
    
    src/core/OrderQueueManager.cpp
    src/core/events/EventLogger.cpp
    
    src/api/Server.cpp
    src/api/Handlers.cpp
    
    src/lib/server-utils.cpp
    src/lib/order-utils.cpp
    src/lib/trader-utils.cpp
)

add_executable(OrderBookPlatform src/main.cpp)

target_include_directories(orderbook_lib PUBLIC include include/core include/lib)
target_link_libraries(orderbook_lib ${SQLite3_LIBRARIES})
target_link_libraries(OrderBookPlatform orderbook_lib)

enable_testing()

add_executable(ActiveOrderTest test/ActiveOrderTest.cpp)
target_link_libraries(ActiveOrderTest orderbook_lib GTest::GTest GTest::Main)
add_test(NAME ActiveOrderTest COMMAND ActiveOrderTest)

add_executable(ConditionalOrderTest test/ConditionalOrderTest.cpp)
target_link_libraries(ConditionalOrderTest orderbook_lib GTest::GTest GTest::Main)
add_test(NAME ConditionalOrderTest COMMAND ConditionalOrderTest)

add_executable(RiskTest test/RiskTest.cpp)
target_link_libraries(RiskTest orderbook_lib GTest::GTest GTest::Main)
add_test(NAME RiskTest COMMAND RiskTest)

add_executable(EventLoggerTest test/EventLoggerTest.cpp)
target_link_libraries(EventLoggerTest orderbook_lib GTest::GTest GTest::Main)
add_test(NAME EventLoggerTest COMMAND EventLoggerTest)