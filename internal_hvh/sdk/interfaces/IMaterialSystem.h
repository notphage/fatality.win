#pragma once

typedef unsigned short MaterialHandle_t;

struct MaterialVideoMode_t
{
	int m_Width;
	int m_Height;
	int m_Format;
	int m_RefreshRate;
};

struct MaterialSystem_Config_t
{
	MaterialVideoMode_t m_VideoMode;
	float m_fMonitorGamma;
	float m_fGammaTVRangeMin;
	float m_fGammaTVRangeMax;
	float m_fGammaTVExponent;
	bool m_bGammaTVEnabled;
	bool m_bTripleBuffered;
	int m_nAASamples;
	int m_nForceAnisotropicLevel;
	int m_nSkipMipLevels;
	int m_nDxSupportLevel;
	int m_nFlags;
	bool m_bEditMode;
	char m_nProxiesTestMode;
	bool m_bCompressedTextures;
	bool m_bFilterLightmaps;
	bool m_bFilterTextures;
	bool m_bReverseDepth;
	bool m_bBufferPrimitives;
	bool m_bDrawFlat;
	bool m_bMeasureFillRate;
	bool m_bVisualizeFillRate;
	bool m_bNoTransparency;
	bool m_bSoftwareLighting;
	bool m_bAllowCheats;
	char m_nShowMipLevels;
	bool m_bShowLowResImage;
	bool m_bShowNormalMap;
	bool m_bMipMapTextures;
	char m_nFullbright;
	bool m_bFastNoBump;
	bool m_bSuppressRendering;
	bool m_bDrawGray;
	bool m_bShowSpecular;
	bool m_bShowDiffuse;
	int m_nWindowedSizeLimitWidth;
	int m_nWindowedSizeLimitHeight;
	int m_nAAQuality;
	bool m_bShadowDepthTexture;
	bool m_bMotionBlur;
	bool m_bSupportFlashlight;
	bool m_bPaintEnabled;
	char pad[ 0xC ];
};

class IMatRenderContext;
class ITexture;
class IMaterialSystem
{
public:
	VFUNC( 21, OverrideConfig( const MaterialSystem_Config_t& cfg, bool b ), bool( __thiscall* )( void*, const MaterialSystem_Config_t&, bool ) )( cfg, b )
		VFUNC( 36, GetBackBufferFormat(), ImageFormat( __thiscall* )( void* ) )( )
		VFUNC( 84, FindMaterial( const char* name, const char *texgroup, bool complain = true, const char *complainprefix = nullptr ), IMaterial*( __thiscall* )( void*, const char*, const char*, bool, const char* ) )( name, texgroup, complain, complainprefix )
		VFUNC( 86, FirstMaterial(), MaterialHandle_t( __thiscall* )( void* ) )( )
		VFUNC( 87, NextMaterial( MaterialHandle_t h ), MaterialHandle_t( __thiscall* )( void*, MaterialHandle_t ) )( h )
		VFUNC( 88, InvalidMaterial(), MaterialHandle_t( __thiscall* )( void* ) )( )
		VFUNC( 89, GetMaterial( MaterialHandle_t h ), IMaterial* ( __thiscall* )( void*, MaterialHandle_t ) )( h )
		VFUNC( 90, GetNumMaterials(), int( __thiscall* )( void* ) )( )
		VFUNC( 91, FindTexture( const char* name, const char *groupname, bool complain ), ITexture*( __thiscall* )( void*, const char*, const char*, bool ) )( name, groupname, complain )
		VFUNC( 94, BeginRenderTargetAllocation(), void( __thiscall* )( void* ) )( )
		VFUNC( 95, EndRenderTargetAllocation(), void( __thiscall* )( void* ) )( )
		VFUNC( 115, GetRenderContext(), IMatRenderContext*( __thiscall* )( void* ) )( )
};

extern IMaterialSystem* g_pMaterialSystem;
