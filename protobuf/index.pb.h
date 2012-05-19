// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: index.proto

#ifndef PROTOBUF_index_2eproto__INCLUDED
#define PROTOBUF_index_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

namespace idxfile {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_index_2eproto();
void protobuf_AssignDesc_index_2eproto();
void protobuf_ShutdownFile_index_2eproto();

class SigUnit;
class Entry;
class EntryList;

// ===================================================================

class SigUnit : public ::google::protobuf::Message {
 public:
  SigUnit();
  virtual ~SigUnit();
  
  SigUnit(const SigUnit& from);
  
  inline SigUnit& operator=(const SigUnit& from) {
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
  static const SigUnit& default_instance();
  
  void Swap(SigUnit* other);
  
  // implements Message ----------------------------------------------
  
  SigUnit* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SigUnit& from);
  void MergeFrom(const SigUnit& from);
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
  
  // required int64 init = 1;
  inline bool has_init() const;
  inline void clear_init();
  static const int kInitFieldNumber = 1;
  inline ::google::protobuf::int64 init() const;
  inline void set_init(::google::protobuf::int64 value);
  
  // repeated int64 deltas = 2;
  inline int deltas_size() const;
  inline void clear_deltas();
  static const int kDeltasFieldNumber = 2;
  inline ::google::protobuf::int64 deltas(int index) const;
  inline void set_deltas(int index, ::google::protobuf::int64 value);
  inline void add_deltas(::google::protobuf::int64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      deltas() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_deltas();
  
  // required int64 cnt = 3;
  inline bool has_cnt() const;
  inline void clear_cnt();
  static const int kCntFieldNumber = 3;
  inline ::google::protobuf::int64 cnt() const;
  inline void set_cnt(::google::protobuf::int64 value);
  
  // @@protoc_insertion_point(class_scope:idxfile.SigUnit)
 private:
  inline void set_has_init();
  inline void clear_has_init();
  inline void set_has_cnt();
  inline void clear_has_cnt();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int64 init_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > deltas_;
  ::google::protobuf::int64 cnt_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];
  
  friend void  protobuf_AddDesc_index_2eproto();
  friend void protobuf_AssignDesc_index_2eproto();
  friend void protobuf_ShutdownFile_index_2eproto();
  
  void InitAsDefaultInstance();
  static SigUnit* default_instance_;
};
// -------------------------------------------------------------------

class Entry : public ::google::protobuf::Message {
 public:
  Entry();
  virtual ~Entry();
  
  Entry(const Entry& from);
  
  inline Entry& operator=(const Entry& from) {
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
  static const Entry& default_instance();
  
  void Swap(Entry* other);
  
  // implements Message ----------------------------------------------
  
  Entry* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Entry& from);
  void MergeFrom(const Entry& from);
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
  
  // required int64 proc = 1;
  inline bool has_proc() const;
  inline void clear_proc();
  static const int kProcFieldNumber = 1;
  inline ::google::protobuf::int64 proc() const;
  inline void set_proc(::google::protobuf::int64 value);
  
  // required .idxfile.SigUnit logical_offset = 2;
  inline bool has_logical_offset() const;
  inline void clear_logical_offset();
  static const int kLogicalOffsetFieldNumber = 2;
  inline const ::idxfile::SigUnit& logical_offset() const;
  inline ::idxfile::SigUnit* mutable_logical_offset();
  inline ::idxfile::SigUnit* release_logical_offset();
  
  // repeated .idxfile.SigUnit length = 3;
  inline int length_size() const;
  inline void clear_length();
  static const int kLengthFieldNumber = 3;
  inline const ::idxfile::SigUnit& length(int index) const;
  inline ::idxfile::SigUnit* mutable_length(int index);
  inline ::idxfile::SigUnit* add_length();
  inline const ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >&
      length() const;
  inline ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >*
      mutable_length();
  
  // repeated .idxfile.SigUnit physical_offset = 4;
  inline int physical_offset_size() const;
  inline void clear_physical_offset();
  static const int kPhysicalOffsetFieldNumber = 4;
  inline const ::idxfile::SigUnit& physical_offset(int index) const;
  inline ::idxfile::SigUnit* mutable_physical_offset(int index);
  inline ::idxfile::SigUnit* add_physical_offset();
  inline const ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >&
      physical_offset() const;
  inline ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >*
      mutable_physical_offset();
  
  // @@protoc_insertion_point(class_scope:idxfile.Entry)
 private:
  inline void set_has_proc();
  inline void clear_has_proc();
  inline void set_has_logical_offset();
  inline void clear_has_logical_offset();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::int64 proc_;
  ::idxfile::SigUnit* logical_offset_;
  ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit > length_;
  ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit > physical_offset_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];
  
  friend void  protobuf_AddDesc_index_2eproto();
  friend void protobuf_AssignDesc_index_2eproto();
  friend void protobuf_ShutdownFile_index_2eproto();
  
  void InitAsDefaultInstance();
  static Entry* default_instance_;
};
// -------------------------------------------------------------------

class EntryList : public ::google::protobuf::Message {
 public:
  EntryList();
  virtual ~EntryList();
  
  EntryList(const EntryList& from);
  
  inline EntryList& operator=(const EntryList& from) {
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
  static const EntryList& default_instance();
  
  void Swap(EntryList* other);
  
  // implements Message ----------------------------------------------
  
  EntryList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const EntryList& from);
  void MergeFrom(const EntryList& from);
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
  
  // repeated .idxfile.Entry entry = 1;
  inline int entry_size() const;
  inline void clear_entry();
  static const int kEntryFieldNumber = 1;
  inline const ::idxfile::Entry& entry(int index) const;
  inline ::idxfile::Entry* mutable_entry(int index);
  inline ::idxfile::Entry* add_entry();
  inline const ::google::protobuf::RepeatedPtrField< ::idxfile::Entry >&
      entry() const;
  inline ::google::protobuf::RepeatedPtrField< ::idxfile::Entry >*
      mutable_entry();
  
  // @@protoc_insertion_point(class_scope:idxfile.EntryList)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::idxfile::Entry > entry_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_index_2eproto();
  friend void protobuf_AssignDesc_index_2eproto();
  friend void protobuf_ShutdownFile_index_2eproto();
  
  void InitAsDefaultInstance();
  static EntryList* default_instance_;
};
// ===================================================================


// ===================================================================

// SigUnit

// required int64 init = 1;
inline bool SigUnit::has_init() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SigUnit::set_has_init() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SigUnit::clear_has_init() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SigUnit::clear_init() {
  init_ = GOOGLE_LONGLONG(0);
  clear_has_init();
}
inline ::google::protobuf::int64 SigUnit::init() const {
  return init_;
}
inline void SigUnit::set_init(::google::protobuf::int64 value) {
  set_has_init();
  init_ = value;
}

// repeated int64 deltas = 2;
inline int SigUnit::deltas_size() const {
  return deltas_.size();
}
inline void SigUnit::clear_deltas() {
  deltas_.Clear();
}
inline ::google::protobuf::int64 SigUnit::deltas(int index) const {
  return deltas_.Get(index);
}
inline void SigUnit::set_deltas(int index, ::google::protobuf::int64 value) {
  deltas_.Set(index, value);
}
inline void SigUnit::add_deltas(::google::protobuf::int64 value) {
  deltas_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
SigUnit::deltas() const {
  return deltas_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
SigUnit::mutable_deltas() {
  return &deltas_;
}

// required int64 cnt = 3;
inline bool SigUnit::has_cnt() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void SigUnit::set_has_cnt() {
  _has_bits_[0] |= 0x00000004u;
}
inline void SigUnit::clear_has_cnt() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void SigUnit::clear_cnt() {
  cnt_ = GOOGLE_LONGLONG(0);
  clear_has_cnt();
}
inline ::google::protobuf::int64 SigUnit::cnt() const {
  return cnt_;
}
inline void SigUnit::set_cnt(::google::protobuf::int64 value) {
  set_has_cnt();
  cnt_ = value;
}

// -------------------------------------------------------------------

// Entry

// required int64 proc = 1;
inline bool Entry::has_proc() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Entry::set_has_proc() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Entry::clear_has_proc() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Entry::clear_proc() {
  proc_ = GOOGLE_LONGLONG(0);
  clear_has_proc();
}
inline ::google::protobuf::int64 Entry::proc() const {
  return proc_;
}
inline void Entry::set_proc(::google::protobuf::int64 value) {
  set_has_proc();
  proc_ = value;
}

// required .idxfile.SigUnit logical_offset = 2;
inline bool Entry::has_logical_offset() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Entry::set_has_logical_offset() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Entry::clear_has_logical_offset() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Entry::clear_logical_offset() {
  if (logical_offset_ != NULL) logical_offset_->::idxfile::SigUnit::Clear();
  clear_has_logical_offset();
}
inline const ::idxfile::SigUnit& Entry::logical_offset() const {
  return logical_offset_ != NULL ? *logical_offset_ : *default_instance_->logical_offset_;
}
inline ::idxfile::SigUnit* Entry::mutable_logical_offset() {
  set_has_logical_offset();
  if (logical_offset_ == NULL) logical_offset_ = new ::idxfile::SigUnit;
  return logical_offset_;
}
inline ::idxfile::SigUnit* Entry::release_logical_offset() {
  clear_has_logical_offset();
  ::idxfile::SigUnit* temp = logical_offset_;
  logical_offset_ = NULL;
  return temp;
}

// repeated .idxfile.SigUnit length = 3;
inline int Entry::length_size() const {
  return length_.size();
}
inline void Entry::clear_length() {
  length_.Clear();
}
inline const ::idxfile::SigUnit& Entry::length(int index) const {
  return length_.Get(index);
}
inline ::idxfile::SigUnit* Entry::mutable_length(int index) {
  return length_.Mutable(index);
}
inline ::idxfile::SigUnit* Entry::add_length() {
  return length_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >&
Entry::length() const {
  return length_;
}
inline ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >*
Entry::mutable_length() {
  return &length_;
}

// repeated .idxfile.SigUnit physical_offset = 4;
inline int Entry::physical_offset_size() const {
  return physical_offset_.size();
}
inline void Entry::clear_physical_offset() {
  physical_offset_.Clear();
}
inline const ::idxfile::SigUnit& Entry::physical_offset(int index) const {
  return physical_offset_.Get(index);
}
inline ::idxfile::SigUnit* Entry::mutable_physical_offset(int index) {
  return physical_offset_.Mutable(index);
}
inline ::idxfile::SigUnit* Entry::add_physical_offset() {
  return physical_offset_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >&
Entry::physical_offset() const {
  return physical_offset_;
}
inline ::google::protobuf::RepeatedPtrField< ::idxfile::SigUnit >*
Entry::mutable_physical_offset() {
  return &physical_offset_;
}

// -------------------------------------------------------------------

// EntryList

// repeated .idxfile.Entry entry = 1;
inline int EntryList::entry_size() const {
  return entry_.size();
}
inline void EntryList::clear_entry() {
  entry_.Clear();
}
inline const ::idxfile::Entry& EntryList::entry(int index) const {
  return entry_.Get(index);
}
inline ::idxfile::Entry* EntryList::mutable_entry(int index) {
  return entry_.Mutable(index);
}
inline ::idxfile::Entry* EntryList::add_entry() {
  return entry_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::idxfile::Entry >&
EntryList::entry() const {
  return entry_;
}
inline ::google::protobuf::RepeatedPtrField< ::idxfile::Entry >*
EntryList::mutable_entry() {
  return &entry_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace idxfile

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_index_2eproto__INCLUDED