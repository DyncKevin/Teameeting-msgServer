// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: sys_msg.proto

#import "GPBProtocolBuffers.h"

#if GOOGLE_PROTOBUF_OBJC_GEN_VERSION != 30001
#error This file was generated by a different version of protoc which is incompatible with your Protocol Buffer library sources.
#endif

// @@protoc_insertion_point(imports)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

CF_EXTERN_C_BEGIN

@class ToUser;
GPB_ENUM_FWD_DECLARE(EConnTag);
GPB_ENUM_FWD_DECLARE(EModuleType);
GPB_ENUM_FWD_DECLARE(EServerCmd);
GPB_ENUM_FWD_DECLARE(ETransferFlag);
GPB_ENUM_FWD_DECLARE(ETransferModule);
GPB_ENUM_FWD_DECLARE(ETransferPriority);
GPB_ENUM_FWD_DECLARE(ETransferType);

NS_ASSUME_NONNULL_BEGIN

#pragma mark - SysMsgRoot

/// Exposes the extension registry for this file.
///
/// The base class provides:
/// @code
///   + (GPBExtensionRegistry *)extensionRegistry;
/// @endcode
/// which is a @c GPBExtensionRegistry that includes all the extensions defined by
/// this file and all files that it depends on.
@interface SysMsgRoot : GPBRootObject
@end

#pragma mark - ConnMsg

typedef GPB_ENUM(ConnMsg_FieldNumber) {
  ConnMsg_FieldNumber_ConnTag = 1,
  ConnMsg_FieldNumber_TrModule = 2,
  ConnMsg_FieldNumber_Moduleid = 3,
  ConnMsg_FieldNumber_Transferid = 4,
};

/// this is for server module connecting connection
@interface ConnMsg : GPBMessage

@property(nonatomic, readwrite) enum EConnTag connTag;

/// the server module
@property(nonatomic, readwrite) enum ETransferModule trModule;

@property(nonatomic, readwrite, copy, null_resettable) NSString *moduleid;

@property(nonatomic, readwrite, copy, null_resettable) NSString *transferid;

@end

/// Fetches the raw value of a @c ConnMsg's @c connTag property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t ConnMsg_ConnTag_RawValue(ConnMsg *message);
/// Sets the raw value of an @c ConnMsg's @c connTag property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetConnMsg_ConnTag_RawValue(ConnMsg *message, int32_t value);

/// Fetches the raw value of a @c ConnMsg's @c trModule property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t ConnMsg_TrModule_RawValue(ConnMsg *message);
/// Sets the raw value of an @c ConnMsg's @c trModule property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetConnMsg_TrModule_RawValue(ConnMsg *message, int32_t value);

#pragma mark - RelayMsg

typedef GPB_ENUM(RelayMsg_FieldNumber) {
  RelayMsg_FieldNumber_SvrCmds = 1,
  RelayMsg_FieldNumber_TrModule = 2,
  RelayMsg_FieldNumber_ContModule = 3,
  RelayMsg_FieldNumber_Connector = 4,
  RelayMsg_FieldNumber_HandleCmd = 5,
  RelayMsg_FieldNumber_HandleMtype = 6,
  RelayMsg_FieldNumber_HandleData = 7,
  RelayMsg_FieldNumber_Content = 8,
  RelayMsg_FieldNumber_Touser = 9,
};

/// this is for msg relaying between server
/// this is used by TransferMsg's content field
@interface RelayMsg : GPBMessage

@property(nonatomic, readwrite) enum EServerCmd svrCmds;

/// the server module
@property(nonatomic, readwrite) enum ETransferModule trModule;

/// the msg content's module
@property(nonatomic, readwrite) enum EModuleType contModule;

@property(nonatomic, readwrite, copy, null_resettable) NSString *connector;

@property(nonatomic, readwrite, copy, null_resettable) NSString *handleCmd;

@property(nonatomic, readwrite, copy, null_resettable) NSString *handleMtype;

@property(nonatomic, readwrite, copy, null_resettable) NSString *handleData;

/// this is the application or module msg
@property(nonatomic, readwrite, copy, null_resettable) NSData *content;

@property(nonatomic, readwrite, strong, null_resettable) ToUser *touser;
/// Test to see if @c touser has been set.
@property(nonatomic, readwrite) BOOL hasTouser;

@end

/// Fetches the raw value of a @c RelayMsg's @c svrCmds property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t RelayMsg_SvrCmds_RawValue(RelayMsg *message);
/// Sets the raw value of an @c RelayMsg's @c svrCmds property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetRelayMsg_SvrCmds_RawValue(RelayMsg *message, int32_t value);

/// Fetches the raw value of a @c RelayMsg's @c trModule property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t RelayMsg_TrModule_RawValue(RelayMsg *message);
/// Sets the raw value of an @c RelayMsg's @c trModule property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetRelayMsg_TrModule_RawValue(RelayMsg *message, int32_t value);

/// Fetches the raw value of a @c RelayMsg's @c contModule property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t RelayMsg_ContModule_RawValue(RelayMsg *message);
/// Sets the raw value of an @c RelayMsg's @c contModule property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetRelayMsg_ContModule_RawValue(RelayMsg *message, int32_t value);

#pragma mark - TransferMsg

typedef GPB_ENUM(TransferMsg_FieldNumber) {
  TransferMsg_FieldNumber_Type = 1,
  TransferMsg_FieldNumber_Flag = 2,
  TransferMsg_FieldNumber_Priority = 3,
  TransferMsg_FieldNumber_Content = 4,
};

/// this is for msg relaying between server
@interface TransferMsg : GPBMessage

/// the type of transfer
@property(nonatomic, readwrite) enum ETransferType type;

/// the flag
@property(nonatomic, readwrite) enum ETransferFlag flag;

@property(nonatomic, readwrite) enum ETransferPriority priority;

/// the message to transfer
@property(nonatomic, readwrite, copy, null_resettable) NSData *content;

@end

/// Fetches the raw value of a @c TransferMsg's @c type property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t TransferMsg_Type_RawValue(TransferMsg *message);
/// Sets the raw value of an @c TransferMsg's @c type property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetTransferMsg_Type_RawValue(TransferMsg *message, int32_t value);

/// Fetches the raw value of a @c TransferMsg's @c flag property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t TransferMsg_Flag_RawValue(TransferMsg *message);
/// Sets the raw value of an @c TransferMsg's @c flag property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetTransferMsg_Flag_RawValue(TransferMsg *message, int32_t value);

/// Fetches the raw value of a @c TransferMsg's @c priority property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t TransferMsg_Priority_RawValue(TransferMsg *message);
/// Sets the raw value of an @c TransferMsg's @c priority property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetTransferMsg_Priority_RawValue(TransferMsg *message, int32_t value);

#pragma mark - ErrorMsg

typedef GPB_ENUM(ErrorMsg_FieldNumber) {
  ErrorMsg_FieldNumber_Emodule = 1,
  ErrorMsg_FieldNumber_Userid = 2,
  ErrorMsg_FieldNumber_Reason = 3,
  ErrorMsg_FieldNumber_Errcode = 4,
};

/// this is for system error msg, report to higher level
@interface ErrorMsg : GPBMessage

/// which module has error
@property(nonatomic, readwrite) enum ETransferModule emodule;

/// which user meet this error
@property(nonatomic, readwrite, copy, null_resettable) NSString *userid;

/// what reason for error
@property(nonatomic, readwrite, copy, null_resettable) NSString *reason;

/// err code
@property(nonatomic, readwrite) int32_t errcode;

@end

/// Fetches the raw value of a @c ErrorMsg's @c emodule property, even
/// if the value was not defined by the enum at the time the code was generated.
int32_t ErrorMsg_Emodule_RawValue(ErrorMsg *message);
/// Sets the raw value of an @c ErrorMsg's @c emodule property, allowing
/// it to be set to a value that was not defined by the enum at the time the code
/// was generated.
void SetErrorMsg_Emodule_RawValue(ErrorMsg *message, int32_t value);

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

#pragma clang diagnostic pop

// @@protoc_insertion_point(global_scope)
