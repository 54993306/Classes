// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: common/prop_common.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "prop_common.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace protos {
namespace common {

namespace {

const ::google::protobuf::Descriptor* Prop_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Prop_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_common_2fprop_5fcommon_2eproto() {
  protobuf_AddDesc_common_2fprop_5fcommon_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "common/prop_common.proto");
  GOOGLE_CHECK(file != NULL);
  Prop_descriptor_ = file->message_type(0);
  static const int Prop_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Prop, addtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Prop, addval_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Prop, param_),
  };
  Prop_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      Prop_descriptor_,
      Prop::default_instance_,
      Prop_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Prop, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Prop, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(Prop));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_common_2fprop_5fcommon_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    Prop_descriptor_, &Prop::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_common_2fprop_5fcommon_2eproto() {
  delete Prop::default_instance_;
  delete Prop_reflection_;
}

void protobuf_AddDesc_common_2fprop_5fcommon_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\030common/prop_common.proto\022\rprotos.commo"
    "n\"6\n\004Prop\022\017\n\007AddType\030\001 \001(\005\022\016\n\006AddVal\030\002 \001"
    "(\005\022\r\n\005param\030\003 \001(\005B5\n%anvin.server.gamese"
    "rver.protos.commonB\nPropCommonH\001", 152);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "common/prop_common.proto", &protobuf_RegisterTypes);
  Prop::default_instance_ = new Prop();
  Prop::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_common_2fprop_5fcommon_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_common_2fprop_5fcommon_2eproto {
  StaticDescriptorInitializer_common_2fprop_5fcommon_2eproto() {
    protobuf_AddDesc_common_2fprop_5fcommon_2eproto();
  }
} static_descriptor_initializer_common_2fprop_5fcommon_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int Prop::kAddTypeFieldNumber;
const int Prop::kAddValFieldNumber;
const int Prop::kParamFieldNumber;
#endif  // !_MSC_VER

Prop::Prop()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void Prop::InitAsDefaultInstance() {
}

Prop::Prop(const Prop& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void Prop::SharedCtor() {
  _cached_size_ = 0;
  addtype_ = 0;
  addval_ = 0;
  param_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Prop::~Prop() {
  SharedDtor();
}

void Prop::SharedDtor() {
  if (this != default_instance_) {
  }
}

void Prop::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Prop::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Prop_descriptor_;
}

const Prop& Prop::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_common_2fprop_5fcommon_2eproto();
  return *default_instance_;
}

Prop* Prop::default_instance_ = NULL;

Prop* Prop::New() const {
  return new Prop;
}

void Prop::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    addtype_ = 0;
    addval_ = 0;
    param_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool Prop::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional int32 AddType = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &addtype_)));
          set_has_addtype();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_AddVal;
        break;
      }

      // optional int32 AddVal = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_AddVal:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &addval_)));
          set_has_addval();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_param;
        break;
      }

      // optional int32 param = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_param:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &param_)));
          set_has_param();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void Prop::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional int32 AddType = 1;
  if (has_addtype()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->addtype(), output);
  }

  // optional int32 AddVal = 2;
  if (has_addval()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->addval(), output);
  }

  // optional int32 param = 3;
  if (has_param()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->param(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* Prop::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional int32 AddType = 1;
  if (has_addtype()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->addtype(), target);
  }

  // optional int32 AddVal = 2;
  if (has_addval()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->addval(), target);
  }

  // optional int32 param = 3;
  if (has_param()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->param(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int Prop::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional int32 AddType = 1;
    if (has_addtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->addtype());
    }

    // optional int32 AddVal = 2;
    if (has_addval()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->addval());
    }

    // optional int32 param = 3;
    if (has_param()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->param());
    }

  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Prop::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const Prop* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Prop*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Prop::MergeFrom(const Prop& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_addtype()) {
      set_addtype(from.addtype());
    }
    if (from.has_addval()) {
      set_addval(from.addval());
    }
    if (from.has_param()) {
      set_param(from.param());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void Prop::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Prop::CopyFrom(const Prop& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Prop::IsInitialized() const {

  return true;
}

void Prop::Swap(Prop* other) {
  if (other != this) {
    std::swap(addtype_, other->addtype_);
    std::swap(addval_, other->addval_);
    std::swap(param_, other->param_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata Prop::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Prop_descriptor_;
  metadata.reflection = Prop_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace common
}  // namespace protos

// @@protoc_insertion_point(global_scope)