project(PetraMarketMaker)

add_executable(MarketMaker 
    main.cpp 
    Engine.h 
    Strategy.h 
    Types.h
)

# Connect to the Calculation Engine
target_link_libraries(MarketMaker PRIVATE GreekCore)

# In a real app, we might add boost, zeromq, etc. here
