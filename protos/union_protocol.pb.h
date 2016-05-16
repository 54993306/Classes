// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: union_protocol.proto

#ifndef PROTOBUF_union_5fprotocol_2eproto__INCLUDED
#define PROTOBUF_union_5fprotocol_2eproto__INCLUDED

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
#include "common/hero_common.pb.h"
// @@protoc_insertion_point(includes)

namespace protos {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_union_5fprotocol_2eproto();
void protobuf_AssignDesc_union_5fprotocol_2eproto();
void protobuf_ShutdownFile_union_5fprotocol_2eproto();

class UnionRequest;
class UnionResponse;

// ===================================================================

class UnionRequest : public ::google::protobuf::Message {
 public:
  UnionRequest();
  virtual ~UnionRequest();

  UnionRequest(const UnionRequest& from);

  inline UnionRequest& operator=(const UnionRequest& from) {
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
  static const UnionRequest& default_instance();

  void Swap(UnionRequest* other);

  // implements Message ----------------------------------------------

  UnionRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UnionRequest& from);
  void MergeFrom(const UnionRequest& from);
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

  // optional int32 stageId = 1;
  inline bool has_stageid() const;
  inline void clear_stageid();
  static const int kStageIdFieldNumber = 1;
  inline ::google::protobuf::int32 stageid() const;
  inline void set_stageid(::google::protobuf::int32 value);

  // optional bool boss = 2;
  inline bool has_boss() const;
  inline void clear_boss();
  static const int kBossFieldNumber = 2;
  inline bool boss() const;
  inline void set_boss(bool value);

  // @@protoc_insertion_point(class_scope:protos.UnionRequest)
 private:
  inline void set_has_stageid();
  inline void clear_has_stageid();
  inline void set_has_boss();
  inline void clear_has_boss();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 stageid_;
  bool boss_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_union_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_union_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_union_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static UnionRequest* default_instance_;
};
// -------------------------------------------------------------------

class UnionResponse : public ::google::protobuf::Message {
 public:
  UnionResponse();
  virtual ~UnionResponse();

  UnionResponse(const UnionResponse& from);

  inline UnionResponse& operator=(const UnionResponse& from) {
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
  static const UnionResponse& default_instance();

  void Swap(UnionResponse* other);

  // implements Message ----------------------------------------------

  UnionResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UnionResponse& from);
  void MergeFrom(const UnionResponse& from);
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

  // optional .protos.common.Hero hero1 = 1;
  inline bool has_hero1() const;
  inline void clear_hero1();
  static const int kHero1FieldNumber = 1;
  inline const ::protos::common::Hero& hero1() const;
  inline ::protos::common::Hero* mutable_hero1();
  inline ::protos::common::Hero* release_hero1();
  inline void set_allocated_hero1(::protos::common::Hero* hero1);

  // optional .protos.common.Hero hero2 = 2;
  inline bool has_hero2() const;
  inline void clear_hero2();
  static const int kHero2FieldNumber = 2;
  inline const ::protos::common::Hero& hero2() const;
  inline ::protos::common::Hero* mutable_hero2();
  inline ::protos::common::Hero* release_hero2();
  inline void set_allocated_hero2(::protos::common::Hero* hero2);

  // optional .protos.common.Hero hero3 = 3;
  inline bool has_hero3() const;
  inline void clear_hero3();
  static const int kHero3FieldNumber = 3;
  inline const ::protos::common::Hero& hero3() const;
  inline ::protos::common::Hero* mutable_hero3();
  inline ::protos::common::Hero* release_hero3();
  inline void set_allocated_hero3(::protos::common::Hero* hero3);

  // optional .protos.common.Hero hero4 = 4;
  inline bool has_hero4() const;
  inline void clear_hero4();
  static const int kHero4FieldNumber = 4;
  inline const ::protos::common::Hero& hero4() const;
  inline ::protos::common::Hero* mutable_hero4();
  inline ::protos::common::Hero* release_hero4();
  inline void set_allocated_hero4(::protos::common::Hero* hero4);

  // optional .protos.common.Hero hero5 = 5;
  inline bool has_hero5() const;
  inline void clear_hero5();
  static const int kHero5FieldNumber = 5;
  inline const ::protos::common::Hero& hero5() const;
  inline ::protos::common::Hero* mutable_hero5();
  inline ::protos::common::Hero* release_hero5();
  inline void set_allocated_hero5(::protos::common::Hero* hero5);

  // @@protoc_insertion_point(class_scope:protos.UnionResponse)
 private:
  inline void set_has_hero1();
  inline void clear_has_hero1();
  inline void set_has_hero2();
  inline void clear_has_hero2();
  inline void set_has_hero3();
  inline void clear_has_hero3();
  inline void set_has_hero4();
  inline void clear_has_hero4();
  inline void set_has_hero5();
  inline void clear_has_hero5();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::protos::common::Hero* hero1_;
  ::protos::common::Hero* hero2_;
  ::protos::common::Hero* hero3_;
  ::protos::common::Hero* hero4_;
  ::protos::common::Hero* hero5_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_union_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_union_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_union_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static UnionResponse* default_instance_;
};
// ===================================================================


// ===================================================================

// UnionRequest

// optional int32 stageId = 1;
inline bool UnionRequest::has_stageid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UnionRequest::set_has_stageid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UnionRequest::clear_has_stageid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UnionRequest::clear_stageid() {
  stageid_ = 0;
  clear_has_stageid();
}
inline ::google::protobuf::int32 UnionRequest::stageid() const {
  return stageid_;
}
inline void UnionRequest::set_stageid(::google::protobuf::int32 value) {
  set_has_stageid();
  stageid_ = value;
}

// optional bool boss = 2;
inline bool UnionRequest::has_boss() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UnionRequest::set_has_boss() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UnionRequest::clear_has_boss() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UnionRequest::clear_boss() {
  boss_ = false;
  clear_has_boss();
}
inline bool UnionRequest::boss() const {
  return boss_;
}
inline void UnionRequest::set_boss(bool value) {
  set_has_boss();
  boss_ = value;
}

// -------------------------------------------------------------------

// UnionResponse

// optional .protos.common.Hero hero1 = 1;
inline bool UnionResponse::has_hero1() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UnionResponse::set_has_hero1() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UnionResponse::clear_has_hero1() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UnionResponse::clear_hero1() {
  if (hero1_ != NULL) hero1_->::protos::common::Hero::Clear();
  clear_has_hero1();
}
inline const ::protos::common::Hero& UnionResponse::hero1() const {
  return hero1_ != NULL ? *hero1_ : *default_instance_->hero1_;
}
inline ::protos::common::Hero* UnionResponse::mutable_hero1() {
  set_has_hero1();
  if (hero1_ == NULL) hero1_ = new ::protos::common::Hero;
  return hero1_;
}
inline ::protos::common::Hero* UnionResponse::release_hero1() {
  clear_has_hero1();
  ::protos::common::Hero* temp = hero1_;
  hero1_ = NULL;
  return temp;
}
inline void UnionResponse::set_allocated_hero1(::protos::common::Hero* hero1) {
  delete hero1_;
  hero1_ = hero1;
  if (hero1) {
    set_has_hero1();
  } else {
    clear_has_hero1();
  }
}

// optional .protos.common.Hero hero2 = 2;
inline bool UnionResponse::has_hero2() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UnionResponse::set_has_hero2() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UnionResponse::clear_has_hero2() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UnionResponse::clear_hero2() {
  if (hero2_ != NULL) hero2_->::protos::common::Hero::Clear();
  clear_has_hero2();
}
inline const ::protos::common::Hero& UnionResponse::hero2() const {
  return hero2_ != NULL ? *hero2_ : *default_instance_->hero2_;
}
inline ::protos::common::Hero* UnionResponse::mutable_hero2() {
  set_has_hero2();
  if (hero2_ == NULL) hero2_ = new ::protos::common::Hero;
  return hero2_;
}
inline ::protos::common::Hero* UnionResponse::release_hero2() {
  clear_has_hero2();
  ::protos::common::Hero* temp = hero2_;
  hero2_ = NULL;
  return temp;
}
inline void UnionResponse::set_allocated_hero2(::protos::common::Hero* hero2) {
  delete hero2_;
  hero2_ = hero2;
  if (hero2) {
    set_has_hero2();
  } else {
    clear_has_hero2();
  }
}

// optional .protos.common.Hero hero3 = 3;
inline bool UnionResponse::has_hero3() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UnionResponse::set_has_hero3() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UnionResponse::clear_has_hero3() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UnionResponse::clear_hero3() {
  if (hero3_ != NULL) hero3_->::protos::common::Hero::Clear();
  clear_has_hero3();
}
inline const ::protos::common::Hero& UnionResponse::hero3() const {
  return hero3_ != NULL ? *hero3_ : *default_instance_->hero3_;
}
inline ::protos::common::Hero* UnionResponse::mutable_hero3() {
  set_has_hero3();
  if (hero3_ == NULL) hero3_ = new ::protos::common::Hero;
  return hero3_;
}
inline ::protos::common::Hero* UnionResponse::release_hero3() {
  clear_has_hero3();
  ::protos::common::Hero* temp = hero3_;
  hero3_ = NULL;
  return temp;
}
inline void UnionResponse::set_allocated_hero3(::protos::common::Hero* hero3) {
  delete hero3_;
  hero3_ = hero3;
  if (hero3) {
    set_has_hero3();
  } else {
    clear_has_hero3();
  }
}

// optional .protos.common.Hero hero4 = 4;
inline bool UnionResponse::has_hero4() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void UnionResponse::set_has_hero4() {
  _has_bits_[0] |= 0x00000008u;
}
inline void UnionResponse::clear_has_hero4() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void UnionResponse::clear_hero4() {
  if (hero4_ != NULL) hero4_->::protos::common::Hero::Clear();
  clear_has_hero4();
}
inline const ::protos::common::Hero& UnionResponse::hero4() const {
  return hero4_ != NULL ? *hero4_ : *default_instance_->hero4_;
}
inline ::protos::common::Hero* UnionResponse::mutable_hero4() {
  set_has_hero4();
  if (hero4_ == NULL) hero4_ = new ::protos::common::Hero;
  return hero4_;
}
inline ::protos::common::Hero* UnionResponse::release_hero4() {
  clear_has_hero4();
  ::protos::common::Hero* temp = hero4_;
  hero4_ = NULL;
  return temp;
}
inline void UnionResponse::set_allocated_hero4(::protos::common::Hero* hero4) {
  delete hero4_;
  hero4_ = hero4;
  if (hero4) {
    set_has_hero4();
  } else {
    clear_has_hero4();
  }
}

// optional .protos.common.Hero hero5 = 5;
inline bool UnionResponse::has_hero5() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void UnionResponse::set_has_hero5() {
  _has_bits_[0] |= 0x00000010u;
}
inline void UnionResponse::clear_has_hero5() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void UnionResponse::clear_hero5() {
  if (hero5_ != NULL) hero5_->::protos::common::Hero::Clear();
  clear_has_hero5();
}
inline const ::protos::common::Hero& UnionResponse::hero5() const {
  return hero5_ != NULL ? *hero5_ : *default_instance_->hero5_;
}
inline ::protos::common::Hero* UnionResponse::mutable_hero5() {
  set_has_hero5();
  if (hero5_ == NULL) hero5_ = new ::protos::common::Hero;
  return hero5_;
}
inline ::protos::common::Hero* UnionResponse::release_hero5() {
  clear_has_hero5();
  ::protos::common::Hero* temp = hero5_;
  hero5_ = NULL;
  return temp;
}
inline void UnionResponse::set_allocated_hero5(::protos::common::Hero* hero5) {
  delete hero5_;
  hero5_ = hero5;
  if (hero5) {
    set_has_hero5();
  } else {
    clear_has_hero5();
  }
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

#endif  // PROTOBUF_union_5fprotocol_2eproto__INCLUDED