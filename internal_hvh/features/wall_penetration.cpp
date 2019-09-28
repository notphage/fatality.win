#include "../include_cheat.h"

bool apply_breakables_fix( IClientEntity* entity )
{
	ClientClass *pClass = entity->GetClientClass();

	if ( ( pClass->m_pNetworkName[ 1 ] == 'B' && pClass->m_pNetworkName[ 9 ] == 'e' && pClass->m_pNetworkName[ 10 ] == 'S' && pClass->m_pNetworkName[ 16 ] == 'e' )
		 || ( pClass->m_pNetworkName[ 1 ] != 'B' || pClass->m_pNetworkName[ 5 ] != 'D' ) )
		 ( ( C_CSPlayer* )entity )->get_take_damage() = DAMAGE_YES;

	return true;
}

bool is_cfuncbrush( IClientEntity* entity )
{
	if ( !entity || entity->EntIndex() == 0 )
		return false;


	ClientClass *pClass = entity->GetClientClass();

	if ( !pClass )
		return false;

	_( CFuncBrush, "CFuncBrush" );
	return strstr( pClass->m_pNetworkName, CFuncBrush ) != nullptr;
}

////////////////////////////////////// Misc Functions //////////////////////////////////////
bool penetration::is_breakable_entity( IClientEntity* entity )
{
	if ( !entity || entity->EntIndex() == 0 )
		return false;

	//m_takeDamage isn't properly set when using the signature.
	//Back it up, set it to true, then restore.
	int takeDamageBackup = ( ( C_CSPlayer* )entity )->get_take_damage();

	ClientClass *pClass = entity->GetClientClass();
	//				 '       ''     '      '   '
	//			    01234567890123456     012345678
	//Check against CBreakableSurface and CBaseDoor

	//Windows etc. are CBrekableSurface
	//Large garage door in Office is CBaseDoor and it get's reported as a breakable when it is not one
	//This is seperate from "CPropDoorRotating", which is a normal door.
	//Normally you would also check for "CFuncBrush" but it was acting oddly so I removed it. It's below if interested.
	//((clientClass->m_pNetworkName[1]) != 'F' || (clientClass->m_pNetworkName[4]) != 'c' || (clientClass->m_pNetworkName[5]) != 'B' || (clientClass->m_pNetworkName[9]) != 'h')

	if ( ( pClass->m_pNetworkName[ 1 ] == 'B' && pClass->m_pNetworkName[ 9 ] == 'e' && pClass->m_pNetworkName[ 10 ] == 'S' && pClass->m_pNetworkName[ 16 ] == 'e' )
		 || ( pClass->m_pNetworkName[ 1 ] != 'B' || pClass->m_pNetworkName[ 5 ] != 'D' ) )
		 ( ( C_CSPlayer* )entity )->get_take_damage() = DAMAGE_YES;

	bool breakable = ( ( C_CSPlayer* )entity )->is_breakable_entity();
	( ( C_CSPlayer* )entity )->get_take_damage() = takeDamageBackup;

	return breakable;
}

void penetration::scale_damage( C_CSPlayer* ent, CGameTrace &enterTrace, CCSWeaponData *weaponData, float& currentDamage, bool local )
{
	//Cred. to N0xius for reversing this.
	//TODO: _xAE^; look into reversing this yourself sometime

	bool hasHeavyArmor = ent->get_heavy_armor();
	int armorValue = ent->get_armor();
	int hitGroup = local ? 1 : enterTrace.hitgroup;

	//Fuck making a new function, lambda beste. ~ Does the person have armor on for the hitbox checked?
	auto IsArmored = [ & ]()->bool
	{
		switch ( hitGroup )
		{
			case HITGROUP_HEAD:
				return !!ent->get_helmet(); //Fuck compiler errors - force-convert it to a bool via (!!)
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				return true;
			default:
				return false;
		}
	};

	switch ( hitGroup )
	{
		case HITGROUP_HEAD:
			currentDamage *= hasHeavyArmor ? 2.f : 4.f; //Heavy Armor does 1/2 damage
			break;
		case HITGROUP_STOMACH:
			currentDamage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			currentDamage *= 0.75f;
			break;
		default:
			break;
	}

	if ( armorValue > 0 && IsArmored() )
	{
		float bonusValue = 1.f, armorBonusRatio = 0.5f, armorRatio = weaponData->flArmorRatio / 2.f;

		//Damage gets modified for heavy armor users
		if ( hasHeavyArmor )
		{
			armorBonusRatio = 0.33f;
			armorRatio *= 0.5f;
			bonusValue = 0.33f;
		}

		auto NewDamage = currentDamage * armorRatio;

		if ( hasHeavyArmor )
			NewDamage *= 0.85f;

		if ( ( ( currentDamage - ( currentDamage * armorRatio ) ) * ( bonusValue * armorBonusRatio ) ) > armorValue )
			NewDamage = currentDamage - ( armorValue / armorBonusRatio );

		currentDamage = NewDamage;
	}
}

////////////////////////////////////// Main Autowall Functions //////////////////////////////////////
bool penetration::trace_to_exit( CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, Vector direction, bool local )
{
	Vector start, end;
	float maxDistance = local ? 90.f : 90.f, rayExtension = local ? 4.f : 4.f, currentDistance = 0;
	int firstContents = 0;

	while ( currentDistance <= maxDistance )
	{
		//Add extra distance to our ray
		currentDistance += rayExtension;

		//Multiply the direction vector to the distance so we go outwards, add our position to it.
		start = startPosition + direction * currentDistance;

		if ( !firstContents )
			firstContents = g_pTrace->GetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr ); /*0x4600400B*/

		int pointContents = g_pTrace->GetPointContents( start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr );

		if ( !( pointContents & MASK_SHOT_HULL ) || pointContents & CONTENTS_HITBOX && pointContents != firstContents ) /*0x600400B, *0x40000000*/
		{
			//Let's setup our end position by deducting the direction by the extra added distance
			end = start - ( direction * rayExtension );

			//Let's cast a ray from our start pos to the end pos
			util_trace_line( start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace );

			//g_pDebugOverlay->AddLineOverlay( start, end, 255, 0, 0, true, 0.1 );

			//Let's check if a hitbox is in-front of our enemy and if they are behind a solid wall
			if ( exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX )
			{
				util_trace_line( start, startPosition, MASK_SHOT_HULL, exitTrace.m_pEnt, &exitTrace );

				if ( exitTrace.DidHit() && !exitTrace.startsolid )
				{
					start = exitTrace.endpos;
					return true;
				}

				continue;
			}

			//Can we hit? Is the wall solid?
			if ( exitTrace.DidHit() && !exitTrace.startsolid )
			{

				//Is the wall a breakable? If so, let's shoot through it.
				if ( is_breakable_entity( enterTrace.m_pEnt ) && is_breakable_entity( exitTrace.m_pEnt ) )
					return true;

				if ( enterTrace.surface.flags & SURF_NODRAW || !( exitTrace.surface.flags & SURF_NODRAW ) && ( exitTrace.plane.normal.Dot( direction ) <= 1.f ) )
				{
					float multAmount = exitTrace.fraction * 4.f;
					start -= direction * multAmount;
					return true;
				}

				continue;
			}

			if ( !exitTrace.DidHit() || exitTrace.startsolid )
			{
				if ( enterTrace.DidHitNonWorldEntity() && is_breakable_entity( enterTrace.m_pEnt ) )
				{
					exitTrace = enterTrace;
					exitTrace.endpos = start + direction;
					return true;
				}
			}
		}
	}
	return false;
}

bool penetration::handle_bullet_penetration( CCSWeaponData* weaponData, CGameTrace& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, bool sv_penetration_type, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, bool local, float dist, Vector start )
{
	CGameTrace exitTrace;
	C_CSPlayer* pEnemy = ( C_CSPlayer* )enterTrace.m_pEnt;
	surfacedata_t *enterSurfaceData = g_pSurfaceProps->GetSurfaceData( enterTrace.surface.surfaceProps );
	int enterMaterial = enterSurfaceData->game.material;

	float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;
	float enterDamageModifier = enterSurfaceData->game.damagemodifier;
	float thickness, modifier, lostDamage, finalDamageModifier, combinedPenetrationModifier;
	bool isSolidSurf = ( ( enterTrace.contents >> 3 ) & CONTENTS_SOLID );
	bool isLightSurf = ( ( enterTrace.surface.flags >> 7 ) & SURF_LIGHT );

	if ( possibleHitsRemaining <= 0
		 //Test for "DE_CACHE/DE_CACHE_TELA_03" as the entering surface and "CS_ITALY/CR_MISCWOOD2B" as the exiting surface.
		 //Fixes a wall in de_cache which seems to be broken in some way. Although bullet penetration is not recorded to go through this wall
		 //Decals can be seen of bullets within the glass behind of the enemy. Hacky method, but works.
		 //You might want to have a check for this to only be activated on de_cache.
		 || ( enterTrace.surface.name == ( const char* )0x2227c261 && exitTrace.surface.name == ( const char* )0x2227c868 )
		 || ( !possibleHitsRemaining && !isLightSurf && !isSolidSurf && enterMaterial != CHAR_TEX_GRATE && enterMaterial != CHAR_TEX_GLASS )
		 || weaponData->flPenetration <= 0.f
		 || !trace_to_exit( enterTrace, exitTrace, enterTrace.endpos, direction, local )
		 && !( g_pTrace->GetPointContents( enterTrace.endpos, MASK_SHOT_HULL, nullptr ) & MASK_SHOT_HULL ) )
		return false;

	surfacedata_t *exitSurfaceData = g_pSurfaceProps->GetSurfaceData( exitTrace.surface.surfaceProps );
	int exitMaterial = exitSurfaceData->game.material;
	float exitSurfPenetrationModifier = exitSurfaceData->game.penetrationmodifier;
	float exitDamageModifier = exitSurfaceData->game.damagemodifier;

	if ( enterMaterial == CHAR_TEX_GRATE || enterMaterial == CHAR_TEX_GLASS )
	{
		combinedPenetrationModifier = 3.f;
		finalDamageModifier = 0.05f;
	}
	else if ( isSolidSurf || isLightSurf )
	{
		combinedPenetrationModifier = 1.f;
		finalDamageModifier = 0.16f;
	}
	else if ( enterMaterial == CHAR_TEX_FLESH && ( !pEnemy->is_enemy() && ff_damage_reduction_bullets == 0.f ) ) //TODO: Team check config
	{
		//Look's like you aren't shooting through your teammate today
		if ( ff_damage_bullet_penetration == 0.f )
			return false;

		//Let's shoot through teammates and get kicked for teamdmg! Whatever, atleast we did damage to the enemy. I call that a win.
		combinedPenetrationModifier = ff_damage_bullet_penetration;
		finalDamageModifier = 0.16f;
	}
	else
	{
		combinedPenetrationModifier = ( enterSurfPenetrationModifier + exitSurfPenetrationModifier ) / 2.f;
		finalDamageModifier = 0.16f;
	}

	//Do our materials line up?
	if ( enterMaterial == exitMaterial )
	{
		if ( exitMaterial == CHAR_TEX_CARDBOARD || exitMaterial == CHAR_TEX_WOOD )
			combinedPenetrationModifier = 3.f;
		else if ( exitMaterial == CHAR_TEX_PLASTIC )
			combinedPenetrationModifier = 2.f;
	}

	//Calculate thickness of the wall by getting the length of the range of the trace and squaring
	thickness = ( exitTrace.endpos - enterTrace.endpos ).LengthSqr();
	modifier = fmaxf( 1.f / combinedPenetrationModifier, 0.f );

	//This calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier
	lostDamage = fmaxf(
		( ( modifier * thickness ) / 24.f )
		+ ( ( currentDamage * finalDamageModifier )
			+ ( fmaxf( 3.75f / penetrationPower, 0.f ) * 3.f * modifier ) ), 0.f );

	//Did we loose too much damage?
	if ( lostDamage > currentDamage )
		return false;

	//We can't use any of the damage that we've lost
	if ( lostDamage > 0.f )
		currentDamage -= lostDamage;

	//Do we still have enough damage to deal?
	if ( currentDamage < 1.f )
		return false;

	eyePosition = exitTrace.endpos;
	--possibleHitsRemaining;

	return true;
}

bool penetration::fire_bullet( C_BaseCombatWeapon* pWeapon, Vector& direction, float& currentDamage, C_CSPlayer* enemy, float length, const Vector pos )
{
	if ( !pWeapon )
		return false;

	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0.f, penetrationPower, penetrationDistance, maxRange, ff_damage_reduction_bullets, ff_damage_bullet_penetration, rayExtension = 40.f;
	Vector eyePosition = enemy ? enemy->get_eye_pos() : pos;

	__( ff_damage_reduction_bullets_s, "ff_damage_reduction_bullets" );
	static ConVar* damageReductionBullets = g_pCVar->FindVar( ff_damage_reduction_bullets_s );
	__( ff_damage_bullet_penetration_s, "ff_damage_bullet_penetration" );
	static ConVar* damageBulletPenetration = g_pCVar->FindVar( ff_damage_bullet_penetration_s );
	ff_damage_reduction_bullets = damageReductionBullets->get_float();
	ff_damage_bullet_penetration = damageBulletPenetration->get_float();

	CCSWeaponData* weaponData = pWeapon->get_wpn_data();
	CGameTrace enterTrace;

	//We should be skipping g_pLocalPlayer when casting a ray to players.
	CTraceFilter filter;
	filter.pSkip = enemy ? enemy : g_pLocalPlayer;

	if ( !weaponData )
		return false;

	maxRange = enemy ? 9999.f : weaponData->flRange;
	penetrationDistance = enemy ? 9999.f : 3000.0f;
	penetrationPower = enemy ? 5.f : weaponData->flPenetration;

//This gets set in FX_Firebullets to 4 as a pass-through value.
//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official
	int possibleHitsRemaining = enemy ? 4 : 4;

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = enemy ? 99999.f : weaponData->iDamage;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while ( possibleHitsRemaining > 0 && currentDamage >= 1.f )
	{
		//Calculate max bullet range
		maxRange -= currentDistance;

		//Create endpoint of bullet
		Vector end = eyePosition + direction * maxRange;


		//g_pDebugOverlay->AddLineOverlay( eyePosition, end, 0, 255, 0, true, 0.1 );
		util_trace_line( eyePosition, end, MASK_SHOT_HULL | CONTENTS_HITBOX, enemy ? enemy : g_pLocalPlayer, &enterTrace );
		util_clip_trace_to_players( eyePosition, end + direction * rayExtension, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &enterTrace );

		//We have to do this *after* tracing to the player.
		surfacedata_t *enterSurfaceData = g_pSurfaceProps->GetSurfaceData( enterTrace.surface.surfaceProps );
		float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;
		int enterMaterial = enterSurfaceData->game.material;

		//"Fraction == 1" means that we didn't hit anything. We don't want that- so let's break on it.
		if ( enterTrace.fraction == 1.f )
		{
			break;
		}

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * maxRange;

		if ( enemy && enterTrace.endpos.Dist( enemy->get_eye_pos() ) > length )
		{
			//scale_damage( g_pLocalPlayer, enterTrace, weaponData, currentDamage );
			return true;
		}

		//Let's make our damage drops off the further away the bullet is.
		if ( !enemy )
			currentDamage *= pow( weaponData->flRangeModifier, ( currentDistance / 500.f ) );

			//Sanity checking / Can we actually shoot through?
		if ( currentDistance > penetrationDistance && weaponData->flPenetration > 0.f || enterSurfPenetrationModifier < 0.1f )
			break;

		//This looks gay as fuck if we put it into 1 long line of code.
		bool canDoDamage = ( enterTrace.hitgroup != HITGROUP_GEAR && enterTrace.hitgroup != HITGROUP_GENERIC );
		bool isPlayer = ( ( enterTrace.m_pEnt )->GetClientClass()->m_ClassID == 35 );
		bool isEnemy = ( ( C_CSPlayer* )enterTrace.m_pEnt )->is_enemy();

		//TODO: Team check config
		if ( canDoDamage && isPlayer && isEnemy )
		{
			scale_damage( ( C_CSPlayer* )enterTrace.m_pEnt, enterTrace, weaponData, currentDamage );
			return true;
		}

		if ( isPlayer && ( C_CSPlayer* )enterTrace.m_pEnt == g_pLocalPlayer )
		{
			enterTrace.hitgroup = 1;
			//scale_damage( g_pLocalPlayer, enterTrace, weaponData, currentDamage );
			return true;
		}

		//auto pos = eyePosition;

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if ( !handle_bullet_penetration( weaponData, enterTrace, eyePosition, direction, possibleHitsRemaining, currentDamage, penetrationPower, true, ff_damage_reduction_bullets, ff_damage_bullet_penetration, enemy ? true : false ) )
			break;

	}
	return false;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
bool penetration::can_hit( C_CSPlayer* player, Vector &point, float& damage, const Vector pos, bool full )
{
	QAngle angles;
	Vector direction;
	Vector tmp = point - pos;

	math::get().vector_angles( tmp, angles );
	math::get().angle_vectors( angles, &direction );
	direction.NormalizeInPlace();

	const auto weapon = get_weapon( g_pLocalPlayer->get_active_weapon() );

	const auto ret = fire_bullet( weapon, direction, damage, full ? player : nullptr, full ? point.Dist( pos ) : 0, pos );

	if ( damage < get_config( weapon )->mindmg->get<float>() && damage < player->get_health() + 4.f )
		return false;

	return ret;
}

//////////////////////////////////////////////////////////////////////////////////////////////
bool penetration::get_damage( C_CSPlayer* shooter, C_CSPlayer* target, Vector end, float* damage, CCSWeaponData* custom_weapon, Vector* start_pos, matrix3x4_t* matrix )
{
	if ( !g_pLocalPlayer )
		return false;

	*damage = 0.f;

	if ( !shooter || !target )
		return false;

	auto weapon = get_weapon( shooter->get_active_weapon() );
	if ( !weapon )
		return false;

	auto weapon_data = weapon->get_wpn_data();
	if ( !weapon_data )
		return false;

	g_in_hbp = true;

	bool ultra_hit = !!custom_weapon;

	FireBulletData data;
	data.wpn_data = ultra_hit ? custom_weapon : weapon_data;
	if ( !data.wpn_data )
		return false;
	data.src = shooter->get_eye_pos();
	if ( start_pos )
	{
		data.src = *start_pos;
	}
	data.filter.pSkip = shooter;
	data.length_to_end = ( end - data.src ).Length();

	QAngle angles = math::get().calc_angle( data.src, end );
	math::get().angle_vectors( angles, &data.direction );

	data.direction.NormalizeInPlace();


	if ( simulate_bullet_fire_ultrahit( shooter, target, weapon, data, end ) )
	{
		*damage = data.current_damage;
		g_in_hbp = false;
		return true;
	}

	*damage = 0.f;
	g_in_hbp = false;
	return false;
}

bool penetration::simulate_bullet_fire_ultrahit( C_CSPlayer* shooter, C_CSPlayer* target, C_BaseCombatWeapon* weapon, FireBulletData& data,
												 const Vector& endpos )
{
	if ( !shooter || !target || !weapon )
		return false;


	data.penetrate_count = vars::aa.freestanding_fps.get<bool>() ? 1 : 4;
	data.trace_length = 0.0f;

	data.current_damage = static_cast< float >( data.wpn_data->iDamage );

	__( ff_damage_reduction_bullets_s, "ff_damage_reduction_bullets" );
	static ConVar* damageReductionBullets = g_pCVar->FindVar( ff_damage_reduction_bullets_s );
	__( ff_damage_bullet_penetration_s, "ff_damage_bullet_penetration" );
	static ConVar* damageBulletPenetration = g_pCVar->FindVar( ff_damage_bullet_penetration_s );
	auto ff_damage_reduction_bullets = damageReductionBullets->get_float();
	auto ff_damage_bullet_penetration = damageBulletPenetration->get_float();

	QAngle angles;
	Vector direction;
	Vector tmp = data.src - endpos;

	math::get().vector_angles( tmp, angles );
	math::get().angle_vectors( angles, &direction );
	direction.NormalizeInPlace();

	auto start = data.src;

	while ( data.penetrate_count > 0 && data.current_damage >= 1.f )
	{
		// 8192.f = max weapon range (awp and such)
		data.trace_length_remaining = 8192.f - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		util_trace_line( data.src, end, MASK_SHOT_HULL, shooter, &data.enter_trace );

		bool cond1 = data.enter_trace.fraction == 1.0f;
		// this is better than checking the fraction (its unreliable)
		bool cond2 = data.enter_trace.m_pEnt == target;

		bool cond3 = data.enter_trace.fraction * data.trace_length_remaining > data.length_to_end;

		if ( cond1 || cond2 || cond3 )
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf( data.wpn_data->flRangeModifier, data.trace_length * 0.002f );

			scale_damage( target, data.enter_trace, data.wpn_data, data.current_damage, target == g_pLocalPlayer );
			return true;
		}

		static DWORD dwOffset = 0;
		if ( !dwOffset )
		{
			dwOffset = sig( "client.dll", "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 78 8B 53 14" );
		}

		if ( dwOffset )
		{
			surfacedata_t* enter_surface = g_pSurfaceProps->GetSurfaceData( data.enter_trace.surface.surfaceProps );
			if ( !enter_surface )
				return false;

			int flag = ( data.enter_trace.contents >> 3 ) & 1;
			int enter_material = enter_surface->game.material;
			int mask = 0x1002;

			using HandleBulletPenetrationFn = bool( __thiscall* )( C_BaseEntity*, float&, int&, int*, trace_t*, Vector*, float, float, float, int, int, float, int*, Vector*, float, float, float* );
			static HandleBulletPenetrationFn handle_bullet_pen = reinterpret_cast< HandleBulletPenetrationFn >( dwOffset );

			if ( data.enter_trace.m_pEnt )
			{
				if ( is_cfuncbrush( data.enter_trace.m_pEnt ) )
				{
					/*if ( !did_trace_fuckery_once )
					{*/
					data.penetrate_count++;
				/*	did_trace_fuckery_once = true;
				}*/
				}
			}

			uint8_t* take_damage = reinterpret_cast< uint8_t* >( uintptr_t( data.enter_trace.m_pEnt ) + 0x27C );
			uint8_t old_dmg = *take_damage;
			if ( data.enter_trace.m_pEnt )
			{
				apply_breakables_fix( data.enter_trace.m_pEnt );
			}

			float pen = data.wpn_data->flPenetration;
			// handle_bullet_pen == false means our bullet is able to penetrate through sth
			bool bRetVal = handle_bullet_penetration( data.wpn_data, data.enter_trace, data.src, direction, data.penetrate_count, data.current_damage, data.wpn_data->flPenetration, true, ff_damage_reduction_bullets, ff_damage_bullet_penetration, true, data.length_to_end, start );

			/*bool bRetVal = !handle_bullet_pen( shooter, pen, enter_material,
											   &flag, &data.enter_trace, &data.direction, 0.f,
											   enter_surface->game.penetrationmodifier,
											   enter_surface->game.damagemodifier, 0, mask, pen,
											   &data.penetrate_count, &data.src, 0.f, 0.f, &data.current_damage );*/

			*take_damage = old_dmg;

			if ( !bRetVal )
				break;
		}
	}

	return false;
}

// regular one
bool penetration::simulate_bullet_fire( C_CSPlayer* shooter, C_CSPlayer* target, C_BaseCombatWeapon* weapon, FireBulletData& data )
{
	if ( !shooter || !target || !weapon )
		return false;

	data.penetrate_count = 4;
	data.trace_length = 0.0f;

	data.current_damage = static_cast< float >( data.wpn_data->iDamage );

	bool did_trace_fuckery_once = false;

	while ( data.penetrate_count > 0 && data.current_damage >= 1.f )
	{

		data.trace_length_remaining = data.wpn_data->flRange - data.trace_length;

		Vector end = data.src + data.direction * data.trace_length_remaining;

		util_trace_line( data.src, end, MASK_SHOT_HULL | CONTENTS_HITBOX, shooter, &data.enter_trace );
		util_clip_trace_to_players( data.src, end + data.direction * 40.f, MASK_SHOT_HULL | CONTENTS_HITBOX, &data.filter, &data.enter_trace );

		if ( data.enter_trace.fraction == 1.0f )
		{
			break;
		}

		if ( data.enter_trace.hitgroup <= 7 && data.enter_trace.hitgroup > 0 && data.enter_trace.m_pEnt && data.enter_trace.m_pEnt == target )
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf( data.wpn_data->flRangeModifier, data.trace_length * 0.002f );

			auto enemy = static_cast< C_CSPlayer* >( data.enter_trace.m_pEnt );
			if ( shooter->get_team() == enemy->get_team() )
				return false;

			ClientClass* enemy_class = enemy->GetClientClass();
			if ( enemy && enemy_class->m_ClassID != ClassId_CCSPlayer )
				return false;

			scale_damage( enemy, data.enter_trace, data.wpn_data, data.current_damage, enemy == g_pLocalPlayer );

			return true;
		}

		static DWORD dwOffset = 0;
		if ( !dwOffset )
		{
			dwOffset = sig( "client.dll", "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 78 8B 53 14" );
		}

		if ( dwOffset )
		{
			surfacedata_t* enter_surface = g_pSurfaceProps->GetSurfaceData( data.enter_trace.surface.surfaceProps );
			if ( !enter_surface )
				return false;

			int flag = ( data.enter_trace.contents >> 3 ) & 1;
			int enter_material = enter_surface->game.material;
			int mask = 0x1002;

			using HandleBulletPenetrationFn = bool( __thiscall* )( C_BaseEntity*, float&, int&, int*, trace_t*, Vector*, float, float, float, int, int, float, int*, Vector*, float, float, float* );
			static HandleBulletPenetrationFn handle_bullet_pen = reinterpret_cast< HandleBulletPenetrationFn >( dwOffset );

			if ( data.enter_trace.m_pEnt )
			{
				if ( is_cfuncbrush( data.enter_trace.m_pEnt ) )
				{
					if ( !did_trace_fuckery_once )
					{
						data.penetrate_count++;
						did_trace_fuckery_once = true;
					}
				}
			}

			uint8_t* take_damage = reinterpret_cast< uint8_t* >( uintptr_t( data.enter_trace.m_pEnt ) + 0x27C );
			uint8_t old_dmg = *take_damage;
			if ( data.enter_trace.m_pEnt )
			{
				apply_breakables_fix( data.enter_trace.m_pEnt );
			}

			float pen = data.wpn_data->flPenetration;
			// handle_bullet_pen == false means our bullet is able to penetrate through sth
			bool bRetVal = !handle_bullet_pen( shooter, pen, enter_material,
											   &flag, &data.enter_trace, &data.direction, 0.f,
											   enter_surface->game.penetrationmodifier,
											   enter_surface->game.damagemodifier, 0, mask, pen,
											   &data.penetrate_count, &data.src, 0.f, 0.f, &data.current_damage );

			*take_damage = old_dmg;

			if ( !bRetVal )
				break;
		}
		else
		{
			break;
		}
	}

	return false;
}


bool penetration::get_damage2( C_CSPlayer* shooter, C_CSPlayer* target, Vector end, float* damage, CCSWeaponData* custom_weapon, Vector* start_pos, matrix3x4_t* matrix )
{
	if ( !g_pLocalPlayer )
		return false;

	*damage = 0.f;

	if ( !shooter || !target )
		return false;

	auto weapon = get_weapon( shooter->get_active_weapon() );
	if ( !weapon )
		return false;

	auto weapon_data = weapon->get_wpn_data();
	if ( !weapon_data )
		return false;

	g_in_hbp = true;

	bool ultra_hit = !!custom_weapon;

	FireBulletData data;
	data.wpn_data = ultra_hit ? custom_weapon : weapon_data;
	if ( !data.wpn_data )
		return false;
	data.src = shooter->get_eye_pos();
	if ( start_pos )
	{
		data.src = *start_pos;
	}
	data.filter.pSkip = shooter;
	data.length_to_end = ( end - data.src ).Length();

	QAngle angles = math::get().calc_angle( data.src, end );
	math::get().angle_vectors( angles, &data.direction );

	data.direction.NormalizeInPlace();

	if ( simulate_bullet_fire( shooter, target, weapon, data ) )
	{
		*damage = data.current_damage;
		g_in_hbp = false;

		return !( *damage < get_config( weapon )->mindmg->get< float >() && *damage < target->get_health() + 4.f );
	}

	*damage = 0.f;
	g_in_hbp = false;
	return false;
}