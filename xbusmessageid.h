#ifndef XBUSMESSAGEID_H
#define XBUSMESSAGEID_H


//Xbus message IDs
enum XsMessageId
{
	XMID_Wakeup              = 0x3E,
	XMID_WakeupAck           = 0x3F,
	XMID_ReqDid              = 0x00,
	XMID_DeviceId            = 0x01,
	XMID_ReqProductCode             = 0x1C,
	XMID_ProductCode                = 0x1D,
	XMID_GotoConfig          = 0x30,
	XMID_GotoConfigAck       = 0x31,
	XMID_GotoMeasurement     = 0x10,
	XMID_GotoMeasurementAck  = 0x11,
	XMID_MtData2             = 0x36,
	XMID_ReqOutputConfig     = 0xC0,
	XMID_SetOutputConfiguration     = 0xC0,
	XMID_SetOutputConfigurationAck        = 0xC1,
	XMID_Reset               = 0x40,
	XMID_ResetAck            = 0x41,
	XMID_Error               = 0x42,
	XMID_Warning                    = 0x43,
	XMID_ToggleIoPins        = 0xBE,
	XMID_ToggleIoPinsAck     = 0xBF,
	XMID_FirmwareUpdate      = 0xF2,
	XMID_GotoBootLoader      = 0xF0,
	XMID_GotoBootLoaderAck   = 0xF1,
	XMID_ReqFirmwareRevision = 0x12,
	XMID_FirmwareRevision    = 0x13,
	XMID_ReqConfiguration           = 0x0C,
	XMID_Configuration              = 0x0D,
	XMID_RestoreFactoryDef          = 0x0E,
	XMID_RestoreFactoryDefAck       = 0x0F,
	XMID_SetNoRotation              = 0x22,
	XMID_SetNoRotationAck           = 0x23,
	XMID_RunSelfTest                = 0x24,
	XMID_SelfTestResults            = 0x25,
	XMID_ReqOptionFlags             = 0x48,
	XMID_ReqOptionFlagsAck          = 0x49,
	XMID_SetOptionFlags             = 0x48,
	XMID_SetOptionFlagsAck          = 0x49,

	XMID_ReqAvailableFilterProfiles = 0x62,        //!< Request the available filter profiles
	XMID_AvailableFilterProfiles    = 0x63,        //!< Message contains the available filter profiles

	XMID_ReqFilterProfile           = 0x64,        //!< Request the current filter profile
	XMID_ReqFilterProfileAck        = 0x65,        //!< Message contains the current filter profile
	XMID_SetFilterProfile           = 0x64,        //!< Set the current filter profile
	XMID_SetFilterProfileAck        = 0x65,        //!< Acknowledge of setting the current filter profile

	XMID_ReqGnssLeverArm            = 0x68,
	XMID_ReqGnssLeverArmAck         = 0x69,
	XMID_SetGnssLeverArm            = 0x68,
	XMID_SetGnssLeverArmAck         = 0x69,

	XMID_IccCommand                 = 0x74,
	XMID_IccCommandAck              = 0x75,

	XMID_ReqGnssPlatform            = 0x76,
	XMID_ReqGnssPlatformAck         = 0x77,
	XMID_SetGnssPlatform            = 0x76,
	XMID_SetGnssPlatformAck         = 0x77,

	XMID_SetInitialHeading          = 0xD6,
	XMID_SetInitialHeadingAck       = 0xD7,


};



#endif
