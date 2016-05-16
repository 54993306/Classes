// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: gate_protocol.proto

#ifndef PROTOBUF_gate_5fprotocol_2eproto__INCLUDED
#define PROTOBUF_gate_5fprotocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protos {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_gate_5fprotocol_2eproto();
void protobuf_AssignDesc_gate_5fprotocol_2eproto();
void protobuf_ShutdownFile_gate_5fprotocol_2eproto();

class ServerList;
class Server;

// ===================================================================

class ServerList : public ::google::protobuf::Message {
 public:
  ServerList();
  virtual ~ServerList();

  ServerList(const ServerList& from);

  inline ServerList& operator=(const ServerList& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ServerList& default_instance();

  void Swap(ServerList* other);

  // implements Message ----------------------------------------------

  ServerList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ServerList& from);
  void MergeFrom(const ServerList& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string game_version = 1;
  inline bool has_game_version() const;
  inline void clear_game_version();
  static const int kGameVersionFieldNumber = 1;
  inline const ::std::string& game_version() const;
  inline void set_game_version(const ::std::string& value);
  inline void set_game_version(const char* value);
  inline void set_game_version(const char* value, size_t size);
  inline ::std::string* mutable_game_version();
  inline ::std::string* release_game_version();
  inline void set_allocated_game_version(::std::string* game_version);

  // repeated .protos.Server server_list = 2;
  inline int server_list_size() const;
  inline void clear_server_list();
  static const int kServerListFieldNumber = 2;
  inline const ::protos::Server& server_list(int index) const;
  inline ::protos::Server* mutable_server_list(int index);
  inline ::protos::Server* add_server_list();
  inline const ::google::protobuf::RepeatedPtrField< ::protos::Server >&
      server_list() const;
  inline ::google::protobuf::RepeatedPtrField< ::protos::Server >*
      mutable_server_list();

  // optional string update_server = 3;
  inline bool has_update_server() const;
  inline void clear_update_server();
  static const int kUpdateServerFieldNumber = 3;
  inline const ::std::string& update_server() const;
  inline void set_update_server(const ::std::string& value);
  inline void set_update_server(const char* value);
  inline void set_update_server(const char* value, size_t size);
  inline ::std::string* mutable_update_server();
  inline ::std::string* release_update_server();
  inline void set_allocated_update_server(::std::string* update_server);

  // @@protoc_insertion_point(class_scope:protos.ServerList)
 private:
  inline void set_has_game_version();
  inline void clear_has_game_version();
  inline void set_has_update_server();
  inline void clear_has_update_server();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* game_version_;
  ::google::protobuf::RepeatedPtrField< ::protos::Server > server_list_;
  ::std::string* update_server_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_gate_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_gate_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_gate_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static ServerList* default_instance_;
};
// -------------------------------------------------------------------

class Server : public ::google::protobuf::Message {
 public:
  Server();
  virtual ~Server();

  Server(const Server& from);

  inline Server& operator=(const Server& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Server& default_instance();

  void Swap(Server* other);

  // implements Message ----------------------------------------------

  Server* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Server& from);
  void MergeFrom(const Server& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string server_name = 1;
  inline bool has_server_name() const;
  inline void clear_server_name();
  static const int kServerNameFieldNumber = 1;
  inline const ::std::string& server_name() const;
  inline void set_server_name(const ::std::string& value);
  inline void set_server_name(const char* value);
  inline void set_server_name(const char* value, size_t size);
  inline ::std::string* mutable_server_name();
  inline ::std::string* release_server_name();
  inline void set_allocated_server_name(::std::string* server_name);

  // required string server_host = 2;
  inline bool has_server_host() const;
  inline void clear_server_host();
  static const int kServerHostFieldNumber = 2;
  inline const ::std::string& server_host() const;
  inline void set_server_host(const ::std::string& value);
  inline void set_server_host(const char* value);
  inline void set_server_host(const char* value, size_t size);
  inline ::std::string* mutable_server_host();
  inline ::std::string* release_server_host();
  inline void set_allocated_server_host(::std::string* server_host);

  // required int32 server_port = 3;
  inline bool has_server_port() const;
  inline void clear_server_port();
  static const int kServerPortFieldNumber = 3;
  inline ::google::protobuf::int32 server_port() const;
  inline void set_server_port(::google::protobuf::int32 value);

  // optional int32 server_status = 4;
  inline bool has_server_status() const;
  inline void clear_server_status();
  static const int kServerStatusFieldNumber = 4;
  inline ::google::protobuf::int32 server_status() const;
  inline void set_server_status(::google::protobuf::int32 value);

  // optional bool selected = 5;
  inline bool has_selected() const;
  inline void clear_selected();
  static const int kSelectedFieldNumber = 5;
  inline bool selected() const;
  inline void set_selected(bool value);

  // @@protoc_insertion_point(class_scope:protos.Server)
 private:
  inline void set_has_server_name();
  inline void clear_has_server_name();
  inline void set_has_server_host();
  inline void clear_has_server_host();
  inline void set_has_server_port();
  inline void clear_has_server_port();
  inline void set_has_server_status();
  inline void clear_has_server_status();
  inline void set_has_selected();
  inline void clear_has_selected();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* server_name_;
  ::std::string* server_host_;
  ::google::protobuf::int32 server_port_;
  ::google::protobuf::int32 server_status_;
  bool selected_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_gate_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_gate_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_gate_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static Server* default_instance_;
};
// ===================================================================


// ===================================================================

// ServerList

// required string game_version = 1;
inline bool ServerList::has_game_version() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ServerList::set_has_game_version() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ServerList::clear_has_game_version() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ServerList::clear_game_version() {
  if (game_version_ != &::google::protobuf::internal::kEmptyString) {
    game_version_->clear();
  }
  clear_has_game_version();
}
inline const ::std::string& ServerList::game_version() const {
  return *game_version_;
}
inline void ServerList::set_game_version(const ::std::string& value) {
  set_has_game_version();
  if (game_version_ == &::google::protobuf::internal::kEmptyString) {
    game_version_ = new ::std::string;
  }
  game_version_->assign(value);
}
inline void ServerList::set_game_version(const char* value) {
  set_has_game_version();
  if (game_version_ == &::google::protobuf::internal::kEmptyString) {
    game_version_ = new ::std::string;
  }
  game_version_->assign(value);
}
inline void ServerList::set_game_version(const char* value, size_t size) {
  set_has_game_version();
  if (game_version_ == &::google::protobuf::internal::kEmptyString) {
    game_version_ = new ::std::string;
  }
  game_version_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ServerList::mutable_game_version() {
  set_has_game_version();
  if (game_version_ == &::google::protobuf::internal::kEmptyString) {
    game_version_ = new ::std::string;
  }
  return game_version_;
}
inline ::std::string* ServerList::release_game_version() {
  clear_has_game_version();
  if (game_version_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = game_version_;
    game_version_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ServerList::set_allocated_game_version(::std::string* game_version) {
  if (game_version_ != &::google::protobuf::internal::kEmptyString) {
    delete game_version_;
  }
  if (game_version) {
    set_has_game_version();
    game_version_ = game_version;
  } else {
    clear_has_game_version();
    game_version_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated .protos.Server server_list = 2;
inline int ServerList::server_list_size() const {
  return server_list_.size();
}
inline void ServerList::clear_server_list() {
  server_list_.Clear();
}
inline const ::protos::Server& ServerList::server_list(int index) const {
  return server_list_.Get(index);
}
inline ::protos::Server* ServerList::mutable_server_list(int index) {
  return server_list_.Mutable(index);
}
inline ::protos::Server* ServerList::add_server_list() {
  return server_list_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::protos::Server >&
ServerList::server_list() const {
  return server_list_;
}
inline ::google::protobuf::RepeatedPtrField< ::protos::Server >*
ServerList::mutable_server_list() {
  return &server_list_;
}

// optional string update_server = 3;
inline bool ServerList::has_update_server() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ServerList::set_has_update_server() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ServerList::clear_has_update_server() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ServerList::clear_update_server() {
  if (update_server_ != &::google::protobuf::internal::kEmptyString) {
    update_server_->clear();
  }
  clear_has_update_server();
}
inline const ::std::string& ServerList::update_server() const {
  return *update_server_;
}
inline void ServerList::set_update_server(const ::std::string& value) {
  set_has_update_server();
  if (update_server_ == &::google::protobuf::internal::kEmptyString) {
    update_server_ = new ::std::string;
  }
  update_server_->assign(value);
}
inline void ServerList::set_update_server(const char* value) {
  set_has_update_server();
  if (update_server_ == &::google::protobuf::internal::kEmptyString) {
    update_server_ = new ::std::string;
  }
  update_server_->assign(value);
}
inline void ServerList::set_update_server(const char* value, size_t size) {
  set_has_update_server();
  if (update_server_ == &::google::protobuf::internal::kEmptyString) {
    update_server_ = new ::std::string;
  }
  update_server_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* ServerList::mutable_update_server() {
  set_has_update_server();
  if (update_server_ == &::google::protobuf::internal::kEmptyString) {
    update_server_ = new ::std::string;
  }
  return update_server_;
}
inline ::std::string* ServerList::release_update_server() {
  clear_has_update_server();
  if (update_server_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = update_server_;
    update_server_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void ServerList::set_allocated_update_server(::std::string* update_server) {
  if (update_server_ != &::google::protobuf::internal::kEmptyString) {
    delete update_server_;
  }
  if (update_server) {
    set_has_update_server();
    update_server_ = update_server;
  } else {
    clear_has_update_server();
    update_server_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// Server

// required string server_name = 1;
inline bool Server::has_server_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Server::set_has_server_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Server::clear_has_server_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Server::clear_server_name() {
  if (server_name_ != &::google::protobuf::internal::kEmptyString) {
    server_name_->clear();
  }
  clear_has_server_name();
}
inline const ::std::string& Server::server_name() const {
  return *server_name_;
}
inline void Server::set_server_name(const ::std::string& value) {
  set_has_server_name();
  if (server_name_ == &::google::protobuf::internal::kEmptyString) {
    server_name_ = new ::std::string;
  }
  server_name_->assign(value);
}
inline void Server::set_server_name(const char* value) {
  set_has_server_name();
  if (server_name_ == &::google::protobuf::internal::kEmptyString) {
    server_name_ = new ::std::string;
  }
  server_name_->assign(value);
}
inline void Server::set_server_name(const char* value, size_t size) {
  set_has_server_name();
  if (server_name_ == &::google::protobuf::internal::kEmptyString) {
    server_name_ = new ::std::string;
  }
  server_name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Server::mutable_server_name() {
  set_has_server_name();
  if (server_name_ == &::google::protobuf::internal::kEmptyString) {
    server_name_ = new ::std::string;
  }
  return server_name_;
}
inline ::std::string* Server::release_server_name() {
  clear_has_server_name();
  if (server_name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = server_name_;
    server_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Server::set_allocated_server_name(::std::string* server_name) {
  if (server_name_ != &::google::protobuf::internal::kEmptyString) {
    delete server_name_;
  }
  if (server_name) {
    set_has_server_name();
    server_name_ = server_name;
  } else {
    clear_has_server_name();
    server_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required string server_host = 2;
inline bool Server::has_server_host() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Server::set_has_server_host() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Server::clear_has_server_host() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Server::clear_server_host() {
  if (server_host_ != &::google::protobuf::internal::kEmptyString) {
    server_host_->clear();
  }
  clear_has_server_host();
}
inline const ::std::string& Server::server_host() const {
  return *server_host_;
}
inline void Server::set_server_host(const ::std::string& value) {
  set_has_server_host();
  if (server_host_ == &::google::protobuf::internal::kEmptyString) {
    server_host_ = new ::std::string;
  }
  server_host_->assign(value);
}
inline void Server::set_server_host(const char* value) {
  set_has_server_host();
  if (server_host_ == &::google::protobuf::internal::kEmptyString) {
    server_host_ = new ::std::string;
  }
  server_host_->assign(value);
}
inline void Server::set_server_host(const char* value, size_t size) {
  set_has_server_host();
  if (server_host_ == &::google::protobuf::internal::kEmptyString) {
    server_host_ = new ::std::string;
  }
  server_host_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Server::mutable_server_host() {
  set_has_server_host();
  if (server_host_ == &::google::protobuf::internal::kEmptyString) {
    server_host_ = new ::std::string;
  }
  return server_host_;
}
inline ::std::string* Server::release_server_host() {
  clear_has_server_host();
  if (server_host_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = server_host_;
    server_host_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Server::set_allocated_server_host(::std::string* server_host) {
  if (server_host_ != &::google::protobuf::internal::kEmptyString) {
    delete server_host_;
  }
  if (server_host) {
    set_has_server_host();
    server_host_ = server_host;
  } else {
    clear_has_server_host();
    server_host_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required int32 server_port = 3;
inline bool Server::has_server_port() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Server::set_has_server_port() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Server::clear_has_server_port() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Server::clear_server_port() {
  server_port_ = 0;
  clear_has_server_port();
}
inline ::google::protobuf::int32 Server::server_port() const {
  return server_port_;
}
inline void Server::set_server_port(::google::protobuf::int32 value) {
  set_has_server_port();
  server_port_ = value;
}

// optional int32 server_status = 4;
inline bool Server::has_server_status() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Server::set_has_server_status() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Server::clear_has_server_status() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Server::clear_server_status() {
  server_status_ = 0;
  clear_has_server_status();
}
inline ::google::protobuf::int32 Server::server_status() const {
  return server_status_;
}
inline void Server::set_server_status(::google::protobuf::int32 value) {
  set_has_server_status();
  server_status_ = value;
}

// optional bool selected = 5;
inline bool Server::has_selected() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Server::set_has_selected() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Server::clear_has_selected() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Server::clear_selected() {
  selected_ = false;
  clear_has_selected();
}
inline bool Server::selected() const {
  return selected_;
}
inline void Server::set_selected(bool value) {
  set_has_selected();
  selected_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace protos

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_gate_5fprotocol_2eproto__INCLUDED