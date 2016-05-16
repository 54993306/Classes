// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common/trap_common.proto

#ifndef PROTOBUF_common_2ftrap_5fcommon_2eproto__INCLUDED
#define PROTOBUF_common_2ftrap_5fcommon_2eproto__INCLUDED

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
#include "skill_common.pb.h"
// @@protoc_insertion_point(includes)

namespace protos {
namespace common {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_common_2ftrap_5fcommon_2eproto();
void protobuf_AssignDesc_common_2ftrap_5fcommon_2eproto();
void protobuf_ShutdownFile_common_2ftrap_5fcommon_2eproto();

class Trap;

// ===================================================================

class Trap : public ::google::protobuf::Message {
 public:
  Trap();
  virtual ~Trap();

  Trap(const Trap& from);

  inline Trap& operator=(const Trap& from) {
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
  static const Trap& default_instance();

  void Swap(Trap* other);

  // implements Message ----------------------------------------------

  Trap* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Trap& from);
  void MergeFrom(const Trap& from);
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

  // optional int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // optional int32 trapId = 2;
  inline bool has_trapid() const;
  inline void clear_trapid();
  static const int kTrapIdFieldNumber = 2;
  inline ::google::protobuf::int32 trapid() const;
  inline void set_trapid(::google::protobuf::int32 value);

  // optional string name = 3;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 3;
  inline const ::std::string& name() const;
  inline void set_name(const ::std::string& value);
  inline void set_name(const char* value);
  inline void set_name(const char* value, size_t size);
  inline ::std::string* mutable_name();
  inline ::std::string* release_name();
  inline void set_allocated_name(::std::string* name);

  // optional int32 type = 4;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 4;
  inline ::google::protobuf::int32 type() const;
  inline void set_type(::google::protobuf::int32 value);

  // optional int32 damage = 5;
  inline bool has_damage() const;
  inline void clear_damage();
  static const int kDamageFieldNumber = 5;
  inline ::google::protobuf::int32 damage() const;
  inline void set_damage(::google::protobuf::int32 value);

  // optional int32 round = 6;
  inline bool has_round() const;
  inline void clear_round();
  static const int kRoundFieldNumber = 6;
  inline ::google::protobuf::int32 round() const;
  inline void set_round(::google::protobuf::int32 value);

  // optional int32 posX = 7;
  inline bool has_posx() const;
  inline void clear_posx();
  static const int kPosXFieldNumber = 7;
  inline ::google::protobuf::int32 posx() const;
  inline void set_posx(::google::protobuf::int32 value);

  // optional int32 posY = 8;
  inline bool has_posy() const;
  inline void clear_posy();
  static const int kPosYFieldNumber = 8;
  inline ::google::protobuf::int32 posy() const;
  inline void set_posy(::google::protobuf::int32 value);

  // optional int32 pro_type = 9;
  inline bool has_pro_type() const;
  inline void clear_pro_type();
  static const int kProTypeFieldNumber = 9;
  inline ::google::protobuf::int32 pro_type() const;
  inline void set_pro_type(::google::protobuf::int32 value);

  // optional sint32 pro_rate = 10;
  inline bool has_pro_rate() const;
  inline void clear_pro_rate();
  static const int kProRateFieldNumber = 10;
  inline ::google::protobuf::int32 pro_rate() const;
  inline void set_pro_rate(::google::protobuf::int32 value);

  // optional int32 touch = 11;
  inline bool has_touch() const;
  inline void clear_touch();
  static const int kTouchFieldNumber = 11;
  inline ::google::protobuf::int32 touch() const;
  inline void set_touch(::google::protobuf::int32 value);

  // optional .protos.common.Buff buff = 12;
  inline bool has_buff() const;
  inline void clear_buff();
  static const int kBuffFieldNumber = 12;
  inline const ::protos::common::Buff& buff() const;
  inline ::protos::common::Buff* mutable_buff();
  inline ::protos::common::Buff* release_buff();
  inline void set_allocated_buff(::protos::common::Buff* buff);

  // optional int32 batch = 13;
  inline bool has_batch() const;
  inline void clear_batch();
  static const int kBatchFieldNumber = 13;
  inline ::google::protobuf::int32 batch() const;
  inline void set_batch(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.common.Trap)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_trapid();
  inline void clear_has_trapid();
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_damage();
  inline void clear_has_damage();
  inline void set_has_round();
  inline void clear_has_round();
  inline void set_has_posx();
  inline void clear_has_posx();
  inline void set_has_posy();
  inline void clear_has_posy();
  inline void set_has_pro_type();
  inline void clear_has_pro_type();
  inline void set_has_pro_rate();
  inline void clear_has_pro_rate();
  inline void set_has_touch();
  inline void clear_has_touch();
  inline void set_has_buff();
  inline void clear_has_buff();
  inline void set_has_batch();
  inline void clear_has_batch();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 trapid_;
  ::std::string* name_;
  ::google::protobuf::int32 type_;
  ::google::protobuf::int32 damage_;
  ::google::protobuf::int32 round_;
  ::google::protobuf::int32 posx_;
  ::google::protobuf::int32 posy_;
  ::google::protobuf::int32 pro_type_;
  ::google::protobuf::int32 pro_rate_;
  ::google::protobuf::int32 touch_;
  ::protos::common::Buff* buff_;
  ::google::protobuf::int32 batch_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(13 + 31) / 32];

  friend void  protobuf_AddDesc_common_2ftrap_5fcommon_2eproto();
  friend void protobuf_AssignDesc_common_2ftrap_5fcommon_2eproto();
  friend void protobuf_ShutdownFile_common_2ftrap_5fcommon_2eproto();

  void InitAsDefaultInstance();
  static Trap* default_instance_;
};
// ===================================================================


// ===================================================================

// Trap

// optional int32 id = 1;
inline bool Trap::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Trap::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Trap::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Trap::clear_id() {
  id_ = 0;
  clear_has_id();
}
inline ::google::protobuf::int32 Trap::id() const {
  return id_;
}
inline void Trap::set_id(::google::protobuf::int32 value) {
  set_has_id();
  id_ = value;
}

// optional int32 trapId = 2;
inline bool Trap::has_trapid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Trap::set_has_trapid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Trap::clear_has_trapid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Trap::clear_trapid() {
  trapid_ = 0;
  clear_has_trapid();
}
inline ::google::protobuf::int32 Trap::trapid() const {
  return trapid_;
}
inline void Trap::set_trapid(::google::protobuf::int32 value) {
  set_has_trapid();
  trapid_ = value;
}

// optional string name = 3;
inline bool Trap::has_name() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Trap::set_has_name() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Trap::clear_has_name() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Trap::clear_name() {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    name_->clear();
  }
  clear_has_name();
}
inline const ::std::string& Trap::name() const {
  return *name_;
}
inline void Trap::set_name(const ::std::string& value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Trap::set_name(const char* value) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(value);
}
inline void Trap::set_name(const char* value, size_t size) {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Trap::mutable_name() {
  set_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    name_ = new ::std::string;
  }
  return name_;
}
inline ::std::string* Trap::release_name() {
  clear_has_name();
  if (name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = name_;
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Trap::set_allocated_name(::std::string* name) {
  if (name_ != &::google::protobuf::internal::kEmptyString) {
    delete name_;
  }
  if (name) {
    set_has_name();
    name_ = name;
  } else {
    clear_has_name();
    name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional int32 type = 4;
inline bool Trap::has_type() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Trap::set_has_type() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Trap::clear_has_type() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Trap::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::google::protobuf::int32 Trap::type() const {
  return type_;
}
inline void Trap::set_type(::google::protobuf::int32 value) {
  set_has_type();
  type_ = value;
}

// optional int32 damage = 5;
inline bool Trap::has_damage() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Trap::set_has_damage() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Trap::clear_has_damage() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Trap::clear_damage() {
  damage_ = 0;
  clear_has_damage();
}
inline ::google::protobuf::int32 Trap::damage() const {
  return damage_;
}
inline void Trap::set_damage(::google::protobuf::int32 value) {
  set_has_damage();
  damage_ = value;
}

// optional int32 round = 6;
inline bool Trap::has_round() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void Trap::set_has_round() {
  _has_bits_[0] |= 0x00000020u;
}
inline void Trap::clear_has_round() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void Trap::clear_round() {
  round_ = 0;
  clear_has_round();
}
inline ::google::protobuf::int32 Trap::round() const {
  return round_;
}
inline void Trap::set_round(::google::protobuf::int32 value) {
  set_has_round();
  round_ = value;
}

// optional int32 posX = 7;
inline bool Trap::has_posx() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void Trap::set_has_posx() {
  _has_bits_[0] |= 0x00000040u;
}
inline void Trap::clear_has_posx() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void Trap::clear_posx() {
  posx_ = 0;
  clear_has_posx();
}
inline ::google::protobuf::int32 Trap::posx() const {
  return posx_;
}
inline void Trap::set_posx(::google::protobuf::int32 value) {
  set_has_posx();
  posx_ = value;
}

// optional int32 posY = 8;
inline bool Trap::has_posy() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void Trap::set_has_posy() {
  _has_bits_[0] |= 0x00000080u;
}
inline void Trap::clear_has_posy() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void Trap::clear_posy() {
  posy_ = 0;
  clear_has_posy();
}
inline ::google::protobuf::int32 Trap::posy() const {
  return posy_;
}
inline void Trap::set_posy(::google::protobuf::int32 value) {
  set_has_posy();
  posy_ = value;
}

// optional int32 pro_type = 9;
inline bool Trap::has_pro_type() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void Trap::set_has_pro_type() {
  _has_bits_[0] |= 0x00000100u;
}
inline void Trap::clear_has_pro_type() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void Trap::clear_pro_type() {
  pro_type_ = 0;
  clear_has_pro_type();
}
inline ::google::protobuf::int32 Trap::pro_type() const {
  return pro_type_;
}
inline void Trap::set_pro_type(::google::protobuf::int32 value) {
  set_has_pro_type();
  pro_type_ = value;
}

// optional sint32 pro_rate = 10;
inline bool Trap::has_pro_rate() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void Trap::set_has_pro_rate() {
  _has_bits_[0] |= 0x00000200u;
}
inline void Trap::clear_has_pro_rate() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void Trap::clear_pro_rate() {
  pro_rate_ = 0;
  clear_has_pro_rate();
}
inline ::google::protobuf::int32 Trap::pro_rate() const {
  return pro_rate_;
}
inline void Trap::set_pro_rate(::google::protobuf::int32 value) {
  set_has_pro_rate();
  pro_rate_ = value;
}

// optional int32 touch = 11;
inline bool Trap::has_touch() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void Trap::set_has_touch() {
  _has_bits_[0] |= 0x00000400u;
}
inline void Trap::clear_has_touch() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void Trap::clear_touch() {
  touch_ = 0;
  clear_has_touch();
}
inline ::google::protobuf::int32 Trap::touch() const {
  return touch_;
}
inline void Trap::set_touch(::google::protobuf::int32 value) {
  set_has_touch();
  touch_ = value;
}

// optional .protos.common.Buff buff = 12;
inline bool Trap::has_buff() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void Trap::set_has_buff() {
  _has_bits_[0] |= 0x00000800u;
}
inline void Trap::clear_has_buff() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void Trap::clear_buff() {
  if (buff_ != NULL) buff_->::protos::common::Buff::Clear();
  clear_has_buff();
}
inline const ::protos::common::Buff& Trap::buff() const {
  return buff_ != NULL ? *buff_ : *default_instance_->buff_;
}
inline ::protos::common::Buff* Trap::mutable_buff() {
  set_has_buff();
  if (buff_ == NULL) buff_ = new ::protos::common::Buff;
  return buff_;
}
inline ::protos::common::Buff* Trap::release_buff() {
  clear_has_buff();
  ::protos::common::Buff* temp = buff_;
  buff_ = NULL;
  return temp;
}
inline void Trap::set_allocated_buff(::protos::common::Buff* buff) {
  delete buff_;
  buff_ = buff;
  if (buff) {
    set_has_buff();
  } else {
    clear_has_buff();
  }
}

// optional int32 batch = 13;
inline bool Trap::has_batch() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void Trap::set_has_batch() {
  _has_bits_[0] |= 0x00001000u;
}
inline void Trap::clear_has_batch() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void Trap::clear_batch() {
  batch_ = 0;
  clear_has_batch();
}
inline ::google::protobuf::int32 Trap::batch() const {
  return batch_;
}
inline void Trap::set_batch(::google::protobuf::int32 value) {
  set_has_batch();
  batch_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace common
}  // namespace protos

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_common_2ftrap_5fcommon_2eproto__INCLUDED