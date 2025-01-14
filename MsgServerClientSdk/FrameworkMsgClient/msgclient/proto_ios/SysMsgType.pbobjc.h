// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sys_msg_type.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30001
#error This file was generated by a different version of protoc which is incompatible with your Protocol Buffer library sources.
#endif

// @@protoc_insertion_point(imports)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

CF_EXTERN_C_BEGIN

NS_ASSUME_NONNULL_BEGIN

#pragma mark - Enum ETransferModule

/// the sever logic module name
/// it userd between servers, not used in client
typedef GPB_ENUM(ETransferModule) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  ETransferModule_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  ETransferModule_Mconnector = 0,
  ETransferModule_Mmsgqueue = 1,
  ETransferModule_Mmeeting = 2,
  ETransferModule_Mp2P = 3,
  ETransferModule_Mlive = 4,
  ETransferModule_Mcallcenter = 5,
  ETransferModule_Mlogical = 6,
  ETransferModule_Msequence = 7,
  ETransferModule_Mstorage = 8,
  ETransferModule_Mgrpnotify = 9,
  ETransferModule_Mpusher = 10,
};

GPBEnumDescriptor *ETransferModule_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL ETransferModule_IsValidValue(int32_t value);

#pragma mark - Enum ETransferType

/// the type of server transfering msg
typedef GPB_ENUM(ETransferType) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  ETransferType_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  ETransferType_Tconn = 0,
  ETransferType_Ttrans = 1,
  ETransferType_Tqueue = 2,
  ETransferType_Tdispatch = 3,
  ETransferType_Tpush = 4,
  ETransferType_Tlogin = 5,
  ETransferType_Tlogout = 6,
  ETransferType_TwriteRequest = 7,
  ETransferType_TwriteResponse = 8,
  ETransferType_TreadRequest = 9,
  ETransferType_TreadResponse = 10,
  ETransferType_Terror = 11,
};

GPBEnumDescriptor *ETransferType_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL ETransferType_IsValidValue(int32_t value);

#pragma mark - Enum ETransferPriority

/// the priority of transfering msg
/// default is normal
typedef GPB_ENUM(ETransferPriority) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  ETransferPriority_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  ETransferPriority_Plow = 0,
  ETransferPriority_Pnormal = 1,
  ETransferPriority_Phigh = 2,
};

GPBEnumDescriptor *ETransferPriority_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL ETransferPriority_IsValidValue(int32_t value);

#pragma mark - Enum ETransferFlag

/// just the flag
typedef GPB_ENUM(ETransferFlag) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  ETransferFlag_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  ETransferFlag_Fnoack = 0,
  ETransferFlag_Fneedack = 1,
  ETransferFlag_Fack = 2,
};

GPBEnumDescriptor *ETransferFlag_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL ETransferFlag_IsValidValue(int32_t value);

#pragma mark - Enum ETransferTag

typedef GPB_ENUM(ETransferTag) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  ETransferTag_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  ETransferTag_Seqn = 0,
  ETransferTag_Data = 1,
};

GPBEnumDescriptor *ETransferTag_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL ETransferTag_IsValidValue(int32_t value);

#pragma mark - Enum EConnTag

/// the tag used for connecting of module
typedef GPB_ENUM(EConnTag) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  EConnTag_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  EConnTag_Thi = 0,
  EConnTag_Thello = 1,
  EConnTag_Thellohi = 2,
  EConnTag_Tkeepalive = 3,
};

GPBEnumDescriptor *EConnTag_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EConnTag_IsValidValue(int32_t value);

#pragma mark - Enum EConnType

/// the type of connection
typedef GPB_ENUM(EConnType) {
  /// Value used if any message's field encounters a value that is not defined
  /// by this enum. The message will also have C functions to get/set the rawValue
  /// of the field.
  EConnType_GPBUnrecognizedEnumeratorValue = kGPBUnrecognizedEnumeratorValue,
  EConnType_Ttcp = 0,
  EConnType_Thttp = 1,
};

GPBEnumDescriptor *EConnType_EnumDescriptor(void);

/// Checks to see if the given value is defined by the enum or was not known at
/// the time this source was generated.
BOOL EConnType_IsValidValue(int32_t value);

#pragma mark - SysMsgTypeRoot

/// Exposes the extension registry for this file.
///
/// The base class provides:
/// @code
///   + (GPBExtensionRegistry *)extensionRegistry;
/// @endcode
/// which is a @c GPBExtensionRegistry that includes all the extensions defined by
/// this file and all files that it depends on.
@interface SysMsgTypeRoot : GPBRootObject
@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

#pragma clang diagnostic pop

// @@protoc_insertion_point(global_scope)
