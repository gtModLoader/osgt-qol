set(SOURCES
    src/signals/connection.cpp
    src/signals/named_slot_map.cpp
    src/signals/signal_base.cpp
    src/signals/slot.cpp
    src/signals/trackable.cpp
)

add_library(boost_signals STATIC ${SOURCES})
target_include_directories(boost_signals PUBLIC include)

# Set BOOST_ALL_NO_LIB to disable the boost auto-linking stuff
target_compile_definitions(boost_signals PUBLIC
    BOOST_ALL_NO_LIB
    BOOST_SIGNALS_NO_DEPRECATION_WARNING
    BOOST_SKIP_MSVC_VERSION_CHECK
)
