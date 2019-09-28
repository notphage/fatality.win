#pragma once

class QAngle;
class Vector;

class NoticeText_t
{
public:
	wchar_t m_nString[ 512 ]; //0x0000 
	char pad_0x0400[ 0xC ]; //0x0400
	float set; //0x040C
	float m_flStartTime; //0x0410 
	float m_flStartTime2; //0x0414 
	float m_flLifeTimeModifier; //0x0418 
	char pad_0x041C[ 0x4 ]; //0x041C
}; //Size=0x420

class SFHudDeathNoticeAndBotStatus
{
public:
	char pad_0x00[ 0x68 ];
	CUtlVector<NoticeText_t>m_nDeathNotices;
};

extern SFHudDeathNoticeAndBotStatus* g_pDeathNotice;

class CUserCmd
{
public:
	virtual ~CUserCmd() {};
	int		command_number;
	int		tick_count;
	QAngle	viewangles;
	Vector	aimdirection;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	byte    impulse;
	int		weaponselect;
	int		weaponsubtype;
	int		random_seed;
	short	mousedx;
	short	mousedy;
	bool	hasbeenpredicted;
	char	pad_0x4C[ 0x18 ];
};

class CCSWeaponData
{
public:

	virtual ~CCSWeaponData() {};

	char*		consoleName;				// 0x0004
	char		pad_0008[ 12 ];				// 0x0008
	int			iMaxClip1;					// 0x0014
	int			iMaxClip2;					// 0x0018
	int			iDefaultClip1;				// 0x001C
	int			iDefaultClip2;				// 0x0020
	char		pad_0024[ 8 ];				// 0x0024
	char*		szWorldModel;				// 0x002C
	char*		szViewModel;				// 0x0030
	char*		szDroppedModel;				// 0x0034
	char		pad_0038[ 4 ];				// 0x0038
	char*		N0000023E;					// 0x003C
	char		pad_0040[ 56 ];				// 0x0040
	char*		szEmptySound;				// 0x0078
	char		pad_007C[ 4 ];				// 0x007C
	char*		szBulletType;				// 0x0080
	char		pad_0084[ 4 ];				// 0x0084
	char*		szHudName;					// 0x0088
	char*		szWeaponName;				// 0x008C
	char		pad_0090[ 56 ];				// 0x0090
	int 		WeaponType;					// 0x00C8
	int			iWeaponPrice;				// 0x00CC
	int			iKillAward;					// 0x00D0
	char*		szAnimationPrefix;			// 0x00D4
	float		flCycleTime;				// 0x00D8
	float		flCycleTimeAlt;				// 0x00DC
	float		flTimeToIdle;				// 0x00E0
	float		flIdleInterval;				// 0x00E4
	bool		bFullAuto;					// 0x00E8
	char		pad_0x00E5[ 3 ];			// 0x00E9
	int			iDamage;					// 0x00EC
	float		flArmorRatio;				// 0x00F0
	int			iBullets;					// 0x00F4
	float		flPenetration;				// 0x00F8
	float		flFlinchVelocityModifierLarge;	// 0x00FC
	float		flFlinchVelocityModifierSmall;	// 0x0100
	float		flRange;					// 0x0104
	float		flRangeModifier;			// 0x0108
	float		flThrowVelocity;			// 0x010C
	char		pad_0x010C[ 12 ];			// 0x0110
	bool		bHasSilencer;				// 0x011C
	char		pad_0x0119[ 3 ];			// 0x011D
	char*		pSilencerModel;				// 0x0120
	int			iCrosshairMinDistance;		// 0x0124
	int			iCrosshairDeltaDistance;	// 0x0128 - iTeam?
	float		flMaxPlayerSpeed;			// 0x012C
	float		flMaxPlayerSpeedAlt;		// 0x0130
	float		flSpread;					// 0x0134
	float		flSpreadAlt;				// 0x0138
	float		flInaccuracyCrouch;			// 0x013C
	float		flInaccuracyCrouchAlt;		// 0x0140
	float		flInaccuracyStand;			// 0x0144
	float		flInaccuracyStandAlt;		// 0x0148
	float		flInaccuracyJumpInitial;	// 0x014C
	float		flInaccuracyJump;			// 0x0150
	float		flInaccuracyJumpAlt;		// 0x0154
	float		flInaccuracyLand;			// 0x0158
	float		flInaccuracyLandAlt;		// 0x015C
	float		flInaccuracyLadder;			// 0x0160
	float		flInaccuracyLadderAlt;		// 0x0164
	float		flInaccuracyFire;			// 0x0168
	float		flInaccuracyFireAlt;		// 0x016C
	float		flInaccuracyMove;			// 0x0170
	float		flInaccuracyMoveAlt;		// 0x0174
	float		flInaccuracyReload;			// 0x0178
	int			iRecoilSeed;				// 0x017C
	float		flRecoilAngle;				// 0x0180
	float		flRecoilAngleAlt;			// 0x0184
	float		flRecoilAngleVariance;		// 0x0188
	float		flRecoilAngleVarianceAlt;	// 0x018C
	float		flRecoilMagnitude;			// 0x0190
	float		flRecoilMagnitudeAlt;		// 0x0194
	float		flRecoilMagnitudeVariance;	// 0x0198
	float		flRecoilMagnitudeVarianceAlt;	// 0x019C
	float		flRecoveryTimeCrouch;		// 0x01A0
	float		flRecoveryTimeStand;		// 0x01A4
	float		flRecoveryTimeCrouchFinal;	// 0x01A8
	float		flRecoveryTimeStandFinal;	// 0x01AC
	int			iRecoveryTransitionStartBullet;	// 0x01B0 
	int			iRecoveryTransitionEndBullet;	// 0x01B4
	bool		bUnzoomAfterShot;			// 0x01B8
	bool		bHideViewModelZoomed;		// 0x01B9
	char		pad_0x01B5[ 2 ];			// 0x01BA
	char		iZoomLevels[ 3 ];			// 0x01BC
	int			iZoomFOV[ 2 ];				// 0x01C0
	float		fZoomTime[ 3 ];				// 0x01C4
	char*		szWeaponClass;				// 0x01D4
	float		flAddonScale;				// 0x01D8
	char		pad_0x01DC[ 4 ];			// 0x01DC
	char*		szEjectBrassEffect;			// 0x01E0
	char*		szTracerEffect;				// 0x01E4
	int			iTracerFrequency;			// 0x01E8
	int			iTracerFrequencyAlt;		// 0x01EC
	char*		szMuzzleFlashEffect_1stPerson;	// 0x01F0
	char		pad_0x01F4[ 4 ];				// 0x01F4
	char*		szMuzzleFlashEffect_3rdPerson;	// 0x01F8
	char		pad_0x01FC[ 4 ];			// 0x01FC
	char*		szMuzzleSmokeEffect;		// 0x0200
	float		flHeatPerShot;				// 0x0204
	char*		szZoomInSound;				// 0x0208
	char*		szZoomOutSound;				// 0x020C
	float		flInaccuracyPitchShift;		// 0x0210
	float		flInaccuracySoundThreshold;	// 0x0214
	float		flBotAudibleRange;			// 0x0218
	char		pad_0x0218[ 8 ];			// 0x0220
	char*		pWrongTeamMsg;				// 0x0224
	bool		bHasBurstMode;				// 0x0228
	char		pad_0x0225[ 3 ];			// 0x0229
	bool		bIsRevolver;				// 0x022C
	bool		bCannotShootUnderwater;		// 0x0230
};

class CViewSetup
{
public:
	int			x, x_old;
	int			y, y_old;
	int			width, width_old;
	int			height, height_old;
	bool		m_bOrtho;
	float		m_OrthoLeft;
	float		m_OrthoTop;
	float		m_OrthoRight;
	float		m_OrthoBottom;
	bool		m_bCustomViewMatrix;
	matrix3x4_t	m_matCustomViewMatrix;
	char		pad_0x68[ 0x48 ];
	float		fov;
	float		fovViewmodel;
	Vector		origin;
	QAngle		angles;
	float		zNear;
	float		zFar;
	float		zNearViewmodel;
	float		zFarViewmodel;
	float		m_flAspectRatio;
	float		m_flNearBlurDepth;
	float		m_flNearFocusDepth;
	float		m_flFarFocusDepth;
	float		m_flFarBlurDepth;
	float		m_flNearBlurRadius;
	float		m_flFarBlurRadius;
	int			m_nDoFQuality;
	int			m_nMotionBlurMode;
	float		m_flShutterTime;
	Vector		m_vShutterOpenPosition;
	QAngle		m_shutterOpenAngles;
	Vector		m_vShutterClosePosition;
	QAngle		m_shutterCloseAngles;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	int			m_EdgeBlur;
};

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate;	// Set to false when this var doesn't
											// need Interpolate() called on it anymore.
	void *data;
	void *watcher;
};

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a float[4]
public:
	inline Quaternion( void ) {}
	inline Quaternion( float ix, float iy, float iz, float iw ) : x( ix ), y( iy ), z( iz ), w( iw ) {}

	inline void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f ) { x = ix; y = iy; z = iz; w = iw; }

	float* Base() { return ( float* )this; }
	const float* Base() const { return ( float* )this; }

	float x, y, z, w;
};

class ALIGN16 QuaternionAligned : public Quaternion
{
public:
	inline QuaternionAligned( void ) {};
	inline QuaternionAligned( float X, float Y, float Z, float W )
	{
		Init( X, Y, Z, W );
	}
public:
	explicit QuaternionAligned( const Quaternion &vOther )
	{
		Init( vOther.x, vOther.y, vOther.z, vOther.w );
	}

	QuaternionAligned& operator=( const Quaternion &vOther )
	{
		Init( vOther.x, vOther.y, vOther.z, vOther.w );
		return *this;
	}
};

class C_AnimationLayer
{
public:
	char  pad_0000[ 20 ];
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[ 4 ]; //0x0034
};

class ClientClass;
class CEventInfo
{
public:
	uint16_t classID; //0x0000 0 implies not in use
	char pad_0002[ 2 ]; //0x0002 
	float fire_delay; //0x0004 If non-zero, the delay time when the event should be fired ( fixed up on the client )
	char pad_0008[ 4 ]; //0x0008
	ClientClass *pClientClass; //0x000C
	void *pData; //0x0010 Raw event data
	char pad_0014[ 48 ]; //0x0014
}; //Size: 0x0044

class C_TEFireBullets
{
public:
	char pad[ 12 ];
	int		m_iPlayer; //12
	int _m_iItemDefinitionIndex;
	Vector	_m_vecOrigin;
	QAngle	m_vecAngles;
	int		_m_iWeaponID;
	int		m_iMode;
	int		m_iSeed;
	float	m_flSpread;
};

struct Item_t
{
	Item_t( std::string model, std::string icon = "" ) :
		model( model ),
		icon( icon )
	{
	}

	std::string model;
	std::string icon;
};

struct WeaponName_t
{
	WeaponName_t( int definition_index, std::string name ) :
		definition_index( definition_index ),
		name( name )
	{
	}

	int definition_index = 0;
	std::string name = nullptr;
};

struct QualityName_t
{
	QualityName_t( int index, std::string name ) :
		index( index ),
		name( name )
	{
	}

	int index = 0;
	std::string name;
};