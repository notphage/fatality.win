#include "../include_cheat.h"

void __fastcall hooks::emit_sound( void* pThis, int edx, void* filter, int iEntIndex, int iChannel,
								   const char *pSoundEntry, unsigned int nSoundEntryHash, const char *pSample,
								   float flVolume, float flAttenuation, int nSeed, int iFlags, int iPitch,
								   const void *pOrigin, const void *pDirection, void * pUtlVecOrigins,
								   bool bUpdatePositions, float soundtime, int speakerentity )
{
	/*if ( strstr( pSample, "null" ) )
	{
		iFlags = ( 1 << 2 ) | ( 1 << 5 );
		iPitch = -1;
		flVolume = -1;
		pSample = "";
	}*/

	orig_emit_sound( pThis, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, flAttenuation, nSeed, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity );
}