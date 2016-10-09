// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: atlas_protocol.proto

#ifndef PROTOBUF_atlas_5fprotocol_2eproto__INCLUDED
#define PROTOBUF_atlas_5fprotocol_2eproto__INCLUDED

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
#include "common/prize_common.pb.h"
// @@protoc_insertion_point(includes)

namespace protos {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

class AtlasRequest;
class AtlasResponse;
class AtlasPrizeReq;
class AtlasPrizeRes;
class AtlasIns;
class AData;

// ===================================================================

class AtlasRequest : public ::google::protobuf::Message {
 public:
  AtlasRequest();
  virtual ~AtlasRequest();

  AtlasRequest(const AtlasRequest& from);

  inline AtlasRequest& operator=(const AtlasRequest& from) {
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
  static const AtlasRequest& default_instance();

  void Swap(AtlasRequest* other);

  // implements Message ----------------------------------------------

  AtlasRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AtlasRequest& from);
  void MergeFrom(const AtlasRequest& from);
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

  // required int32 atlas_type = 1;
  inline bool has_atlas_type() const;
  inline void clear_atlas_type();
  static const int kAtlasTypeFieldNumber = 1;
  inline ::google::protobuf::int32 atlas_type() const;
  inline void set_atlas_type(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.AtlasRequest)
 private:
  inline void set_has_atlas_type();
  inline void clear_has_atlas_type();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 atlas_type_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AtlasRequest* default_instance_;
};
// -------------------------------------------------------------------

class AtlasResponse : public ::google::protobuf::Message {
 public:
  AtlasResponse();
  virtual ~AtlasResponse();

  AtlasResponse(const AtlasResponse& from);

  inline AtlasResponse& operator=(const AtlasResponse& from) {
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
  static const AtlasResponse& default_instance();

  void Swap(AtlasResponse* other);

  // implements Message ----------------------------------------------

  AtlasResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AtlasResponse& from);
  void MergeFrom(const AtlasResponse& from);
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

  // repeated .protos.AtlasIns atlas_list = 1;
  inline int atlas_list_size() const;
  inline void clear_atlas_list();
  static const int kAtlasListFieldNumber = 1;
  inline const ::protos::AtlasIns& atlas_list(int index) const;
  inline ::protos::AtlasIns* mutable_atlas_list(int index);
  inline ::protos::AtlasIns* add_atlas_list();
  inline const ::google::protobuf::RepeatedPtrField< ::protos::AtlasIns >&
      atlas_list() const;
  inline ::google::protobuf::RepeatedPtrField< ::protos::AtlasIns >*
      mutable_atlas_list();

  // @@protoc_insertion_point(class_scope:protos.AtlasResponse)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::protos::AtlasIns > atlas_list_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AtlasResponse* default_instance_;
};
// -------------------------------------------------------------------

class AtlasPrizeReq : public ::google::protobuf::Message {
 public:
  AtlasPrizeReq();
  virtual ~AtlasPrizeReq();

  AtlasPrizeReq(const AtlasPrizeReq& from);

  inline AtlasPrizeReq& operator=(const AtlasPrizeReq& from) {
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
  static const AtlasPrizeReq& default_instance();

  void Swap(AtlasPrizeReq* other);

  // implements Message ----------------------------------------------

  AtlasPrizeReq* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AtlasPrizeReq& from);
  void MergeFrom(const AtlasPrizeReq& from);
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

  // required int32 atlas_type = 1;
  inline bool has_atlas_type() const;
  inline void clear_atlas_type();
  static const int kAtlasTypeFieldNumber = 1;
  inline ::google::protobuf::int32 atlas_type() const;
  inline void set_atlas_type(::google::protobuf::int32 value);

  // required int32 atlas_id = 2;
  inline bool has_atlas_id() const;
  inline void clear_atlas_id();
  static const int kAtlasIdFieldNumber = 2;
  inline ::google::protobuf::int32 atlas_id() const;
  inline void set_atlas_id(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.AtlasPrizeReq)
 private:
  inline void set_has_atlas_type();
  inline void clear_has_atlas_type();
  inline void set_has_atlas_id();
  inline void clear_has_atlas_id();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 atlas_type_;
  ::google::protobuf::int32 atlas_id_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AtlasPrizeReq* default_instance_;
};
// -------------------------------------------------------------------

class AtlasPrizeRes : public ::google::protobuf::Message {
 public:
  AtlasPrizeRes();
  virtual ~AtlasPrizeRes();

  AtlasPrizeRes(const AtlasPrizeRes& from);

  inline AtlasPrizeRes& operator=(const AtlasPrizeRes& from) {
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
  static const AtlasPrizeRes& default_instance();

  void Swap(AtlasPrizeRes* other);

  // implements Message ----------------------------------------------

  AtlasPrizeRes* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AtlasPrizeRes& from);
  void MergeFrom(const AtlasPrizeRes& from);
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

  // required int32 result = 1;
  inline bool has_result() const;
  inline void clear_result();
  static const int kResultFieldNumber = 1;
  inline ::google::protobuf::int32 result() const;
  inline void set_result(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.AtlasPrizeRes)
 private:
  inline void set_has_result();
  inline void clear_has_result();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 result_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AtlasPrizeRes* default_instance_;
};
// -------------------------------------------------------------------

class AtlasIns : public ::google::protobuf::Message {
 public:
  AtlasIns();
  virtual ~AtlasIns();

  AtlasIns(const AtlasIns& from);

  inline AtlasIns& operator=(const AtlasIns& from) {
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
  static const AtlasIns& default_instance();

  void Swap(AtlasIns* other);

  // implements Message ----------------------------------------------

  AtlasIns* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AtlasIns& from);
  void MergeFrom(const AtlasIns& from);
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

  // repeated .protos.AData data_list = 1;
  inline int data_list_size() const;
  inline void clear_data_list();
  static const int kDataListFieldNumber = 1;
  inline const ::protos::AData& data_list(int index) const;
  inline ::protos::AData* mutable_data_list(int index);
  inline ::protos::AData* add_data_list();
  inline const ::google::protobuf::RepeatedPtrField< ::protos::AData >&
      data_list() const;
  inline ::google::protobuf::RepeatedPtrField< ::protos::AData >*
      mutable_data_list();

  // repeated .protos.common.Prize prizes = 2;
  inline int prizes_size() const;
  inline void clear_prizes();
  static const int kPrizesFieldNumber = 2;
  inline const ::protos::common::Prize& prizes(int index) const;
  inline ::protos::common::Prize* mutable_prizes(int index);
  inline ::protos::common::Prize* add_prizes();
  inline const ::google::protobuf::RepeatedPtrField< ::protos::common::Prize >&
      prizes() const;
  inline ::google::protobuf::RepeatedPtrField< ::protos::common::Prize >*
      mutable_prizes();

  // optional int32 get_prize = 3;
  inline bool has_get_prize() const;
  inline void clear_get_prize();
  static const int kGetPrizeFieldNumber = 3;
  inline ::google::protobuf::int32 get_prize() const;
  inline void set_get_prize(::google::protobuf::int32 value);

  // optional int32 pro_hp = 4;
  inline bool has_pro_hp() const;
  inline void clear_pro_hp();
  static const int kProHpFieldNumber = 4;
  inline ::google::protobuf::int32 pro_hp() const;
  inline void set_pro_hp(::google::protobuf::int32 value);

  // optional int32 pro_atk = 5;
  inline bool has_pro_atk() const;
  inline void clear_pro_atk();
  static const int kProAtkFieldNumber = 5;
  inline ::google::protobuf::int32 pro_atk() const;
  inline void set_pro_atk(::google::protobuf::int32 value);

  // optional int32 pro_def = 6;
  inline bool has_pro_def() const;
  inline void clear_pro_def();
  static const int kProDefFieldNumber = 6;
  inline ::google::protobuf::int32 pro_def() const;
  inline void set_pro_def(::google::protobuf::int32 value);

  // optional int32 pro_dex = 7;
  inline bool has_pro_dex() const;
  inline void clear_pro_dex();
  static const int kProDexFieldNumber = 7;
  inline ::google::protobuf::int32 pro_dex() const;
  inline void set_pro_dex(::google::protobuf::int32 value);

  // optional int32 pro_hit = 8;
  inline bool has_pro_hit() const;
  inline void clear_pro_hit();
  static const int kProHitFieldNumber = 8;
  inline ::google::protobuf::int32 pro_hit() const;
  inline void set_pro_hit(::google::protobuf::int32 value);

  // optional int32 pro_crit = 9;
  inline bool has_pro_crit() const;
  inline void clear_pro_crit();
  static const int kProCritFieldNumber = 9;
  inline ::google::protobuf::int32 pro_crit() const;
  inline void set_pro_crit(::google::protobuf::int32 value);

  // optional int32 pro_renew = 10;
  inline bool has_pro_renew() const;
  inline void clear_pro_renew();
  static const int kProRenewFieldNumber = 10;
  inline ::google::protobuf::int32 pro_renew() const;
  inline void set_pro_renew(::google::protobuf::int32 value);

  // optional int32 pro_dodge = 11;
  inline bool has_pro_dodge() const;
  inline void clear_pro_dodge();
  static const int kProDodgeFieldNumber = 11;
  inline ::google::protobuf::int32 pro_dodge() const;
  inline void set_pro_dodge(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.AtlasIns)
 private:
  inline void set_has_get_prize();
  inline void clear_has_get_prize();
  inline void set_has_pro_hp();
  inline void clear_has_pro_hp();
  inline void set_has_pro_atk();
  inline void clear_has_pro_atk();
  inline void set_has_pro_def();
  inline void clear_has_pro_def();
  inline void set_has_pro_dex();
  inline void clear_has_pro_dex();
  inline void set_has_pro_hit();
  inline void clear_has_pro_hit();
  inline void set_has_pro_crit();
  inline void clear_has_pro_crit();
  inline void set_has_pro_renew();
  inline void clear_has_pro_renew();
  inline void set_has_pro_dodge();
  inline void clear_has_pro_dodge();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::protos::AData > data_list_;
  ::google::protobuf::RepeatedPtrField< ::protos::common::Prize > prizes_;
  ::google::protobuf::int32 get_prize_;
  ::google::protobuf::int32 pro_hp_;
  ::google::protobuf::int32 pro_atk_;
  ::google::protobuf::int32 pro_def_;
  ::google::protobuf::int32 pro_dex_;
  ::google::protobuf::int32 pro_hit_;
  ::google::protobuf::int32 pro_crit_;
  ::google::protobuf::int32 pro_renew_;
  ::google::protobuf::int32 pro_dodge_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(11 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AtlasIns* default_instance_;
};
// -------------------------------------------------------------------

class AData : public ::google::protobuf::Message {
 public:
  AData();
  virtual ~AData();

  AData(const AData& from);

  inline AData& operator=(const AData& from) {
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
  static const AData& default_instance();

  void Swap(AData* other);

  // implements Message ----------------------------------------------

  AData* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const AData& from);
  void MergeFrom(const AData& from);
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

  // optional int32 type = 1;
  inline bool has_type() const;
  inline void clear_type();
  static const int kTypeFieldNumber = 1;
  inline ::google::protobuf::int32 type() const;
  inline void set_type(::google::protobuf::int32 value);

  // optional int32 thumb = 2;
  inline bool has_thumb() const;
  inline void clear_thumb();
  static const int kThumbFieldNumber = 2;
  inline ::google::protobuf::int32 thumb() const;
  inline void set_thumb(::google::protobuf::int32 value);

  // optional int32 color = 3;
  inline bool has_color() const;
  inline void clear_color();
  static const int kColorFieldNumber = 3;
  inline ::google::protobuf::int32 color() const;
  inline void set_color(::google::protobuf::int32 value);

  // optional int32 process = 4;
  inline bool has_process() const;
  inline void clear_process();
  static const int kProcessFieldNumber = 4;
  inline ::google::protobuf::int32 process() const;
  inline void set_process(::google::protobuf::int32 value);

  // optional int32 max_process = 5;
  inline bool has_max_process() const;
  inline void clear_max_process();
  static const int kMaxProcessFieldNumber = 5;
  inline ::google::protobuf::int32 max_process() const;
  inline void set_max_process(::google::protobuf::int32 value);

  // optional int32 param = 6;
  inline bool has_param() const;
  inline void clear_param();
  static const int kParamFieldNumber = 6;
  inline ::google::protobuf::int32 param() const;
  inline void set_param(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:protos.AData)
 private:
  inline void set_has_type();
  inline void clear_has_type();
  inline void set_has_thumb();
  inline void clear_has_thumb();
  inline void set_has_color();
  inline void clear_has_color();
  inline void set_has_process();
  inline void clear_has_process();
  inline void set_has_max_process();
  inline void clear_has_max_process();
  inline void set_has_param();
  inline void clear_has_param();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 type_;
  ::google::protobuf::int32 thumb_;
  ::google::protobuf::int32 color_;
  ::google::protobuf::int32 process_;
  ::google::protobuf::int32 max_process_;
  ::google::protobuf::int32 param_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(6 + 31) / 32];

  friend void  protobuf_AddDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_AssignDesc_atlas_5fprotocol_2eproto();
  friend void protobuf_ShutdownFile_atlas_5fprotocol_2eproto();

  void InitAsDefaultInstance();
  static AData* default_instance_;
};
// ===================================================================


// ===================================================================

// AtlasRequest

// required int32 atlas_type = 1;
inline bool AtlasRequest::has_atlas_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AtlasRequest::set_has_atlas_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AtlasRequest::clear_has_atlas_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AtlasRequest::clear_atlas_type() {
  atlas_type_ = 0;
  clear_has_atlas_type();
}
inline ::google::protobuf::int32 AtlasRequest::atlas_type() const {
  return atlas_type_;
}
inline void AtlasRequest::set_atlas_type(::google::protobuf::int32 value) {
  set_has_atlas_type();
  atlas_type_ = value;
}

// -------------------------------------------------------------------

// AtlasResponse

// repeated .protos.AtlasIns atlas_list = 1;
inline int AtlasResponse::atlas_list_size() const {
  return atlas_list_.size();
}
inline void AtlasResponse::clear_atlas_list() {
  atlas_list_.Clear();
}
inline const ::protos::AtlasIns& AtlasResponse::atlas_list(int index) const {
  return atlas_list_.Get(index);
}
inline ::protos::AtlasIns* AtlasResponse::mutable_atlas_list(int index) {
  return atlas_list_.Mutable(index);
}
inline ::protos::AtlasIns* AtlasResponse::add_atlas_list() {
  return atlas_list_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::protos::AtlasIns >&
AtlasResponse::atlas_list() const {
  return atlas_list_;
}
inline ::google::protobuf::RepeatedPtrField< ::protos::AtlasIns >*
AtlasResponse::mutable_atlas_list() {
  return &atlas_list_;
}

// -------------------------------------------------------------------

// AtlasPrizeReq

// required int32 atlas_type = 1;
inline bool AtlasPrizeReq::has_atlas_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AtlasPrizeReq::set_has_atlas_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AtlasPrizeReq::clear_has_atlas_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AtlasPrizeReq::clear_atlas_type() {
  atlas_type_ = 0;
  clear_has_atlas_type();
}
inline ::google::protobuf::int32 AtlasPrizeReq::atlas_type() const {
  return atlas_type_;
}
inline void AtlasPrizeReq::set_atlas_type(::google::protobuf::int32 value) {
  set_has_atlas_type();
  atlas_type_ = value;
}

// required int32 atlas_id = 2;
inline bool AtlasPrizeReq::has_atlas_id() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void AtlasPrizeReq::set_has_atlas_id() {
  _has_bits_[0] |= 0x00000002u;
}
inline void AtlasPrizeReq::clear_has_atlas_id() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void AtlasPrizeReq::clear_atlas_id() {
  atlas_id_ = 0;
  clear_has_atlas_id();
}
inline ::google::protobuf::int32 AtlasPrizeReq::atlas_id() const {
  return atlas_id_;
}
inline void AtlasPrizeReq::set_atlas_id(::google::protobuf::int32 value) {
  set_has_atlas_id();
  atlas_id_ = value;
}

// -------------------------------------------------------------------

// AtlasPrizeRes

// required int32 result = 1;
inline bool AtlasPrizeRes::has_result() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AtlasPrizeRes::set_has_result() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AtlasPrizeRes::clear_has_result() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AtlasPrizeRes::clear_result() {
  result_ = 0;
  clear_has_result();
}
inline ::google::protobuf::int32 AtlasPrizeRes::result() const {
  return result_;
}
inline void AtlasPrizeRes::set_result(::google::protobuf::int32 value) {
  set_has_result();
  result_ = value;
}

// -------------------------------------------------------------------

// AtlasIns

// repeated .protos.AData data_list = 1;
inline int AtlasIns::data_list_size() const {
  return data_list_.size();
}
inline void AtlasIns::clear_data_list() {
  data_list_.Clear();
}
inline const ::protos::AData& AtlasIns::data_list(int index) const {
  return data_list_.Get(index);
}
inline ::protos::AData* AtlasIns::mutable_data_list(int index) {
  return data_list_.Mutable(index);
}
inline ::protos::AData* AtlasIns::add_data_list() {
  return data_list_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::protos::AData >&
AtlasIns::data_list() const {
  return data_list_;
}
inline ::google::protobuf::RepeatedPtrField< ::protos::AData >*
AtlasIns::mutable_data_list() {
  return &data_list_;
}

// repeated .protos.common.Prize prizes = 2;
inline int AtlasIns::prizes_size() const {
  return prizes_.size();
}
inline void AtlasIns::clear_prizes() {
  prizes_.Clear();
}
inline const ::protos::common::Prize& AtlasIns::prizes(int index) const {
  return prizes_.Get(index);
}
inline ::protos::common::Prize* AtlasIns::mutable_prizes(int index) {
  return prizes_.Mutable(index);
}
inline ::protos::common::Prize* AtlasIns::add_prizes() {
  return prizes_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::protos::common::Prize >&
AtlasIns::prizes() const {
  return prizes_;
}
inline ::google::protobuf::RepeatedPtrField< ::protos::common::Prize >*
AtlasIns::mutable_prizes() {
  return &prizes_;
}

// optional int32 get_prize = 3;
inline bool AtlasIns::has_get_prize() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void AtlasIns::set_has_get_prize() {
  _has_bits_[0] |= 0x00000004u;
}
inline void AtlasIns::clear_has_get_prize() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void AtlasIns::clear_get_prize() {
  get_prize_ = 0;
  clear_has_get_prize();
}
inline ::google::protobuf::int32 AtlasIns::get_prize() const {
  return get_prize_;
}
inline void AtlasIns::set_get_prize(::google::protobuf::int32 value) {
  set_has_get_prize();
  get_prize_ = value;
}

// optional int32 pro_hp = 4;
inline bool AtlasIns::has_pro_hp() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void AtlasIns::set_has_pro_hp() {
  _has_bits_[0] |= 0x00000008u;
}
inline void AtlasIns::clear_has_pro_hp() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void AtlasIns::clear_pro_hp() {
  pro_hp_ = 0;
  clear_has_pro_hp();
}
inline ::google::protobuf::int32 AtlasIns::pro_hp() const {
  return pro_hp_;
}
inline void AtlasIns::set_pro_hp(::google::protobuf::int32 value) {
  set_has_pro_hp();
  pro_hp_ = value;
}

// optional int32 pro_atk = 5;
inline bool AtlasIns::has_pro_atk() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void AtlasIns::set_has_pro_atk() {
  _has_bits_[0] |= 0x00000010u;
}
inline void AtlasIns::clear_has_pro_atk() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void AtlasIns::clear_pro_atk() {
  pro_atk_ = 0;
  clear_has_pro_atk();
}
inline ::google::protobuf::int32 AtlasIns::pro_atk() const {
  return pro_atk_;
}
inline void AtlasIns::set_pro_atk(::google::protobuf::int32 value) {
  set_has_pro_atk();
  pro_atk_ = value;
}

// optional int32 pro_def = 6;
inline bool AtlasIns::has_pro_def() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void AtlasIns::set_has_pro_def() {
  _has_bits_[0] |= 0x00000020u;
}
inline void AtlasIns::clear_has_pro_def() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void AtlasIns::clear_pro_def() {
  pro_def_ = 0;
  clear_has_pro_def();
}
inline ::google::protobuf::int32 AtlasIns::pro_def() const {
  return pro_def_;
}
inline void AtlasIns::set_pro_def(::google::protobuf::int32 value) {
  set_has_pro_def();
  pro_def_ = value;
}

// optional int32 pro_dex = 7;
inline bool AtlasIns::has_pro_dex() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void AtlasIns::set_has_pro_dex() {
  _has_bits_[0] |= 0x00000040u;
}
inline void AtlasIns::clear_has_pro_dex() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void AtlasIns::clear_pro_dex() {
  pro_dex_ = 0;
  clear_has_pro_dex();
}
inline ::google::protobuf::int32 AtlasIns::pro_dex() const {
  return pro_dex_;
}
inline void AtlasIns::set_pro_dex(::google::protobuf::int32 value) {
  set_has_pro_dex();
  pro_dex_ = value;
}

// optional int32 pro_hit = 8;
inline bool AtlasIns::has_pro_hit() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void AtlasIns::set_has_pro_hit() {
  _has_bits_[0] |= 0x00000080u;
}
inline void AtlasIns::clear_has_pro_hit() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void AtlasIns::clear_pro_hit() {
  pro_hit_ = 0;
  clear_has_pro_hit();
}
inline ::google::protobuf::int32 AtlasIns::pro_hit() const {
  return pro_hit_;
}
inline void AtlasIns::set_pro_hit(::google::protobuf::int32 value) {
  set_has_pro_hit();
  pro_hit_ = value;
}

// optional int32 pro_crit = 9;
inline bool AtlasIns::has_pro_crit() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void AtlasIns::set_has_pro_crit() {
  _has_bits_[0] |= 0x00000100u;
}
inline void AtlasIns::clear_has_pro_crit() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void AtlasIns::clear_pro_crit() {
  pro_crit_ = 0;
  clear_has_pro_crit();
}
inline ::google::protobuf::int32 AtlasIns::pro_crit() const {
  return pro_crit_;
}
inline void AtlasIns::set_pro_crit(::google::protobuf::int32 value) {
  set_has_pro_crit();
  pro_crit_ = value;
}

// optional int32 pro_renew = 10;
inline bool AtlasIns::has_pro_renew() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void AtlasIns::set_has_pro_renew() {
  _has_bits_[0] |= 0x00000200u;
}
inline void AtlasIns::clear_has_pro_renew() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void AtlasIns::clear_pro_renew() {
  pro_renew_ = 0;
  clear_has_pro_renew();
}
inline ::google::protobuf::int32 AtlasIns::pro_renew() const {
  return pro_renew_;
}
inline void AtlasIns::set_pro_renew(::google::protobuf::int32 value) {
  set_has_pro_renew();
  pro_renew_ = value;
}

// optional int32 pro_dodge = 11;
inline bool AtlasIns::has_pro_dodge() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void AtlasIns::set_has_pro_dodge() {
  _has_bits_[0] |= 0x00000400u;
}
inline void AtlasIns::clear_has_pro_dodge() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void AtlasIns::clear_pro_dodge() {
  pro_dodge_ = 0;
  clear_has_pro_dodge();
}
inline ::google::protobuf::int32 AtlasIns::pro_dodge() const {
  return pro_dodge_;
}
inline void AtlasIns::set_pro_dodge(::google::protobuf::int32 value) {
  set_has_pro_dodge();
  pro_dodge_ = value;
}

// -------------------------------------------------------------------

// AData

// optional int32 type = 1;
inline bool AData::has_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void AData::set_has_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void AData::clear_has_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void AData::clear_type() {
  type_ = 0;
  clear_has_type();
}
inline ::google::protobuf::int32 AData::type() const {
  return type_;
}
inline void AData::set_type(::google::protobuf::int32 value) {
  set_has_type();
  type_ = value;
}

// optional int32 thumb = 2;
inline bool AData::has_thumb() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void AData::set_has_thumb() {
  _has_bits_[0] |= 0x00000002u;
}
inline void AData::clear_has_thumb() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void AData::clear_thumb() {
  thumb_ = 0;
  clear_has_thumb();
}
inline ::google::protobuf::int32 AData::thumb() const {
  return thumb_;
}
inline void AData::set_thumb(::google::protobuf::int32 value) {
  set_has_thumb();
  thumb_ = value;
}

// optional int32 color = 3;
inline bool AData::has_color() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void AData::set_has_color() {
  _has_bits_[0] |= 0x00000004u;
}
inline void AData::clear_has_color() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void AData::clear_color() {
  color_ = 0;
  clear_has_color();
}
inline ::google::protobuf::int32 AData::color() const {
  return color_;
}
inline void AData::set_color(::google::protobuf::int32 value) {
  set_has_color();
  color_ = value;
}

// optional int32 process = 4;
inline bool AData::has_process() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void AData::set_has_process() {
  _has_bits_[0] |= 0x00000008u;
}
inline void AData::clear_has_process() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void AData::clear_process() {
  process_ = 0;
  clear_has_process();
}
inline ::google::protobuf::int32 AData::process() const {
  return process_;
}
inline void AData::set_process(::google::protobuf::int32 value) {
  set_has_process();
  process_ = value;
}

// optional int32 max_process = 5;
inline bool AData::has_max_process() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void AData::set_has_max_process() {
  _has_bits_[0] |= 0x00000010u;
}
inline void AData::clear_has_max_process() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void AData::clear_max_process() {
  max_process_ = 0;
  clear_has_max_process();
}
inline ::google::protobuf::int32 AData::max_process() const {
  return max_process_;
}
inline void AData::set_max_process(::google::protobuf::int32 value) {
  set_has_max_process();
  max_process_ = value;
}

// optional int32 param = 6;
inline bool AData::has_param() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void AData::set_has_param() {
  _has_bits_[0] |= 0x00000020u;
}
inline void AData::clear_has_param() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void AData::clear_param() {
  param_ = 0;
  clear_has_param();
}
inline ::google::protobuf::int32 AData::param() const {
  return param_;
}
inline void AData::set_param(::google::protobuf::int32 value) {
  set_has_param();
  param_ = value;
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

#endif  // PROTOBUF_atlas_5fprotocol_2eproto__INCLUDED