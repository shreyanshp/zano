// C ABI shim for the Windows zano_wallet.dll, consumed by the
// bitcoin-portal/mobile-news Flutter Windows runner at
// `app/windows/runner/zano_wallet_handler.cpp`.
//
// Wraps `plain_wallet::*` (src/wallet/plain_wallet_api.h) — the same
// surface the iOS Swift handler uses through the
// `zano_native_lib_package_ios` SPM and the Android JNI layer. Returns
// caller-owned `char*` allocated via `new char[]`; the caller frees
// each pointer via `zano_free_string`.
//
// The call surface mirrors `bitcoin_news/zano_wallet` on the Flutter
// side: 17 methods, all UTF-8 strings, integer wallet handles.

#include "../plain_wallet_api.h"

#include <cstdint>
#include <cstring>
#include <string>

#if defined(_WIN32)
#define ZANO_DLL_EXPORT __declspec(dllexport)
#else
#define ZANO_DLL_EXPORT __attribute__((visibility("default")))
#endif

namespace {

// Allocate a heap-owned UTF-8 string from a std::string. The caller
// must release it via zano_free_string. Returns nullptr if the input
// is empty (saves an allocation; the Flutter side treats nullptr as
// an empty result).
const char* clone(const std::string& s) {
  if (s.empty()) return nullptr;
  char* p = new char[s.size() + 1];
  std::memcpy(p, s.data(), s.size());
  p[s.size()] = '\0';
  return p;
}

std::string sv(const char* p) { return p ? std::string(p) : std::string(); }

}  // namespace

extern "C" {

ZANO_DLL_EXPORT void zano_free_string(const char* str) {
  if (str) delete[] str;
}

// init / lifecycle ----------------------------------------------------

ZANO_DLL_EXPORT const char* zano_test_init_ip_port(
    const char* ip, const char* port,
    const char* working_dir, int log_level) {
  return clone(plain_wallet::init(sv(ip), sv(port), sv(working_dir),
                                    log_level));
}

ZANO_DLL_EXPORT const char* zano_get_version() {
  return clone(plain_wallet::get_version());
}

ZANO_DLL_EXPORT const char* zano_delete_wallet(const char* file_name) {
  return clone(plain_wallet::delete_wallet(sv(file_name)));
}

ZANO_DLL_EXPORT const char* zano_get_address_info(const char* addr) {
  return clone(plain_wallet::get_address_info(sv(addr)));
}

ZANO_DLL_EXPORT const char* zano_get_connectivity_status() {
  return clone(plain_wallet::get_connectivity_status());
}

// wallet open / restore / generate -----------------------------------

ZANO_DLL_EXPORT const char* zano_open(const char* path,
                                       const char* password) {
  return clone(plain_wallet::open(sv(path), sv(password)));
}

ZANO_DLL_EXPORT const char* zano_restore(const char* seed,
                                          const char* path,
                                          const char* password,
                                          const char* seed_password) {
  return clone(plain_wallet::restore(sv(seed), sv(path), sv(password),
                                       sv(seed_password)));
}

ZANO_DLL_EXPORT const char* zano_generate(const char* path,
                                           const char* password) {
  return clone(plain_wallet::generate(sv(path), sv(password)));
}

// wallet info / status / RPC -----------------------------------------

ZANO_DLL_EXPORT const char* zano_get_wallet_status(uint64_t handle) {
  return clone(plain_wallet::get_wallet_status(
      static_cast<plain_wallet::hwallet>(handle)));
}

ZANO_DLL_EXPORT const char* zano_close_wallet(uint64_t handle) {
  return clone(plain_wallet::close_wallet(
      static_cast<plain_wallet::hwallet>(handle)));
}

ZANO_DLL_EXPORT const char* zano_invoke(uint64_t handle,
                                         const char* params) {
  return clone(plain_wallet::invoke(
      static_cast<plain_wallet::hwallet>(handle), sv(params)));
}

ZANO_DLL_EXPORT const char* zano_async_call(const char* method_name,
                                              uint64_t instance_id,
                                              const char* params) {
  return clone(plain_wallet::async_call(sv(method_name), instance_id,
                                          sv(params)));
}

ZANO_DLL_EXPORT const char* zano_try_pull_result(uint64_t job_id) {
  return clone(plain_wallet::try_pull_result(job_id));
}

ZANO_DLL_EXPORT const char* zano_sync_call(const char* method_name,
                                             uint64_t instance_id,
                                             const char* params) {
  return clone(plain_wallet::sync_call(sv(method_name), instance_id,
                                         sv(params)));
}

// extras --------------------------------------------------------------

ZANO_DLL_EXPORT bool zano_is_wallet_exist(const char* path) {
  return plain_wallet::is_wallet_exist(sv(path));
}

ZANO_DLL_EXPORT const char* zano_get_wallet_info(uint64_t handle) {
  return clone(plain_wallet::get_wallet_info(
      static_cast<plain_wallet::hwallet>(handle)));
}

ZANO_DLL_EXPORT uint64_t zano_get_current_tx_fee(uint64_t priority) {
  return plain_wallet::get_current_tx_fee(priority);
}

}  // extern "C"
