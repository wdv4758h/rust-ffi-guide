#include "wrappers.hpp"
#include "client.hpp"
#include <cassert>
#include <string>
#include <vector>

Request::~Request() { ffi::request_destroy(raw); }

Request::Request(const std::string &url) {
  raw = ffi::request_create(url.c_str());
  if (raw == nullptr) {
    throw new WrapperException("Invalid URL");
  }
}

Response Request::send() {
  ffi::Response *raw_response = ffi::request_send(raw);

  if (raw_response == nullptr) {
    throw new WrapperException("Request failed");
  }

  return Response(raw_response);
}

Response::~Response() { ffi::response_destroy(raw); }

std::vector<char> Response::read_body() {
  int length = ffi::response_body_length(raw);
  assert(length >= 0);

  std::vector<char> buffer(length);

  int bytes_written = ffi::response_body(raw, buffer.data(), buffer.size());
  assert(bytes_written == length);

  return buffer;
}

PluginManager::PluginManager() { raw = ffi::plugin_manager_new(); }

PluginManager::~PluginManager() { ffi::plugin_manager_destroy(raw); }

void PluginManager::pre_send(Request &req) {
  ffi::plugin_manager_pre_send(raw, req.raw);
}

void PluginManager::unload() { ffi::plugin_manager_unload(raw); }

void PluginManager::post_receive(Response &res) {
  ffi::plugin_manager_post_receive(raw, res.raw);
}

void PluginManager::load_plugin(const std::string& filename) {
  int ret = ffi::plugin_manager_load_plugin(raw, filename.c_str());

  if (ret != 0) {
    throw "Couldn't load the plugin";
  }
}