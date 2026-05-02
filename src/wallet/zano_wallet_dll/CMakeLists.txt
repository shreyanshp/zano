# zano_wallet.dll — Windows-only target consumed by
# bitcoin-portal/mobile-news Flutter Windows runner.
#
# Linked against the existing `wallet` static lib (built from
# src/wallet/) plus the same dep chain Zano's mobile + simplewallet
# targets use: currency_core, crypto, common, zlibstatic, ethash,
# Boost, OpenSSL, miniupnpc.

if(NOT WIN32)
  message(STATUS "zano_wallet.dll target skipped (Windows-only).")
  return()
endif()

add_library(zano_wallet SHARED
  c_abi.cpp
)

set_target_properties(zano_wallet PROPERTIES
  OUTPUT_NAME "zano_wallet"
  CXX_VISIBILITY_PRESET hidden
  POSITION_INDEPENDENT_CODE ON
)

# plain_wallet_api requires the same flags as the iOS / Android mobile
# build path — the API is gated on MOBILE_WALLET_BUILD in some places.
target_compile_definitions(zano_wallet PRIVATE MOBILE_WALLET_BUILD)
target_compile_definitions(zano_wallet PRIVATE
  _CRT_SECURE_NO_WARNINGS
  _WIN32_WINNT=0x0600
  WIN32_LEAN_AND_MEAN
  GTEST_HAS_TR1_TUPLE=0)

target_include_directories(zano_wallet PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/contrib/epee/include
  ${OPENSSL_INCLUDE_DIR})

# Mirror the simplewallet link line — same dep tree, no GUI bits.
# `rpc` is required because src/wallet pulls in currency::core_rpc_server
# (LNK2019 on wallets_manager without it). simplewallet's link line in
# src/CMakeLists.txt also lists rpc — match it exactly.
target_link_libraries(zano_wallet PRIVATE
  wallet
  rpc
  currency_core
  crypto
  common
  zlibstatic
  ethash
  ${CMAKE_THREAD_LIBS_INIT}
  ${Boost_LIBRARIES}
  OpenSSL::SSL
  OpenSSL::Crypto
)

# Module def file forces clean exports without name-mangling. We
# generate it from the c_abi exports so the host's GetProcAddress
# always resolves the unmangled `zano_*` symbols.
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/zano_wallet.def.in
               ${CMAKE_CURRENT_BINARY_DIR}/zano_wallet.def @ONLY)
target_sources(zano_wallet PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}/zano_wallet.def)

set_property(TARGET zano_wallet PROPERTY FOLDER "libs")

install(TARGETS zano_wallet
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
