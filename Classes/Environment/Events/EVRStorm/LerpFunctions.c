/*
*
*	Helper functions for EVR Storms
*
*	You are free to mod this as you please. 
*	Just credit @InclementDab for the scripting and @Sumrak for the modeling
*
*	That being said you are NOT free to redistribute / repack this into
*	your own mod. 
*
*/

vector RandomizeVector(vector in, float rand)
{
	for (int i = 0; i < 3; i++) {
		in[i] = Math.RandomFloat(in[i] - rand, in[i] + rand);
	}
	
	return in;
}

vector RandomizeVector(vector in, float min, float max)
{
	for (int i = 0; i < 3; i++) {
		in[i] = Math.RandomFloat(in[i] + Math.RandomFloat(-min, min), in[i] + Math.RandomFloat(-max, max));
	}
	
	return in;
}

EffectSound PlaySoundOnPlayer(BlowoutSound sound, float volume = 1, bool loop = false)
{
	EffectSound effect_sound;
	if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {
		effect_sound = SEffectManager.PlaySoundOnObject(typename.EnumToString(BlowoutSound, sound), GetGame().GetPlayer(), 0, 0, loop);
		if (!effect_sound) {
			PrintFormat("Sound failed to play %1", typename.EnumToString(ReactorSound, sound));
			return effect_sound;
		}
		
		effect_sound.SetParent(GetGame().GetPlayer());
		effect_sound.SetSoundAutodestroy(true);
		effect_sound.GetSoundObject().SetVolume(volume);	
	}
	
	return effect_sound;
}

EffectSound PlayEnvironmentSound(BlowoutSound sound, vector position, float volume, bool loop = false, Object parent = null)
{
	EffectSound effect_sound;
	if (GetGame().IsClient() || !GetGame().IsMultiplayer()) {	
			
		effect_sound = SEffectManager.PlaySound(typename.EnumToString(BlowoutSound, sound), position, 0, 0, loop);
		if (!effect_sound) {
			PrintFormat("Sound failed to create %1", typename.EnumToString(ReactorSound, sound));
			return effect_sound;
		}
		
		effect_sound.SetParent(parent);
		effect_sound.GetSoundObject().SetVolume(volume);
		effect_sound.SetSoundAutodestroy(true);

	}
	
	return effect_sound;
}

void LerpColorization(int index, float r, float g, float b, int time, float start_r = 0, float start_g = 0, float start_b = 0)
{	
	int td = 0;
	while (td < time) {
		float time_value = 1 / time * td;
		PPEffects.m_ColorizeEffects.Set(index, {SmoothLerp(start_r, r, time_value), SmoothLerp(start_g, g, time_value), SmoothLerp(start_b, b, time_value)});
		PPEffects.UpdateColorize();
		td += 10;
		Sleep(10);
	}

	// Some weird enfusion shit right here
	if (r == 1 && g == 1 && b == 1) {
		r = 0; g = 0; b = 0;
	}
	
	// Make sure we reach final value
	PPEffects.m_ColorizeEffects.Set(index, {r, g, b});
	PPEffects.UpdateColorize();
}

void LerpColorizationEx(PPERequester_NamEVRColours PPE_EVRColours, float r, float g, float b, int time, float start_r = 0, float start_g = 0, float start_b = 0)
{	
	if ( !PPE_EVRColours.IsRequesterRunning() )
		PPE_EVRColours.Start();
	
	int td = 0;
	while (td < time) {
		float time_value = 1 / time * td;
		PPE_EVRColours.SetEVRColorization( SmoothLerp(start_r, r, time_value), SmoothLerp(start_g, g, time_value), SmoothLerp(start_b, b, time_value) );
		td += 10;
		Sleep(10);
	}
}

void LerpTeleportEffect(float start, float finish, int time)
{
	Material rotation_blur = GetGame().GetWorld().GetMaterial("Graphics/Materials/postprocess/rotblur");
	Material radial_blur = GetGame().GetWorld().GetMaterial("Graphics/Materials/postprocess/radialblur");
	Material colors = GetGame().GetWorld().GetMaterial("Graphics/Materials/postprocess/glow");
	
	int td = 0;
	while (td < time) {
		float time_value = 1 / time * td;
		float lerp_value = SmoothLerp(start, finish, time_value);
		float hit_effect_color[4]; hit_effect_color[0] = 1; hit_effect_color[1] = 1; hit_effect_color[2] = 1; hit_effect_color[3] = 1;
		
		colors.SetParam("OverlayColor", hit_effect_color);
		colors.SetParam("OverlayFactor", 0.01 * lerp_value);
		rotation_blur.SetParam("Power", 0.1 * lerp_value);
		rotation_blur.SetParam("MaxAnglePerSec", 5 * lerp_value);
		radial_blur.SetParam("PowerX", 0.3 * lerp_value);
		radial_blur.SetParam("PowerY", 0.3 * lerp_value);
		td += 10;
		Sleep(10);
	}
}

static vector SmoothLerpVector(vector a, vector b, float t)
{
	vector ret;
	for (int i = 0; i < 3; i++) {
		ret[i] = SmoothLerp(a[i], b[i], t);
	}
	
	return ret;
}

static float SmoothLerp(float a, float b, float t)
{
	return (b - a) * SmoothTime(t) + a;
}

static float SmoothTime(float t)
{
	return (-Math.Cos(Math.PI * t) / 2) + 0.5;
}