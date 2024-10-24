# Build dependencies.

# MinHook - function hooking library.
set(SOURCES_MINHOOK
  "minhook/src/buffer.c"
  "minhook/src/hook.c"
  "minhook/src/trampoline.c"
  "minhook/src/hde/hde64.c"
)
add_library(minhook ${SOURCES_MINHOOK})
target_include_directories(minhook PUBLIC "minhook/include")

# Boost
set(BOOST_PATH "boost/libs/signals/src")
set(SOURCES_BOOST_SIGNALS
  "${BOOST_PATH}/connection.cpp" "${BOOST_PATH}/named_slot_map.cpp"
  "${BOOST_PATH}/signal_base.cpp" "${BOOST_PATH}/slot.cpp" "${BOOST_PATH}/trackable.cpp"
)

# MSVC seems to complain if it isn't named EXACTLY this.
add_library(libboost_signals-vc120-mt-1_55 ${SOURCES_BOOST_SIGNALS})
target_include_directories(libboost_signals-vc120-mt-1_55 PUBLIC "boost")