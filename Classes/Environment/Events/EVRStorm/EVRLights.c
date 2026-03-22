class BlowoutLightning: ScriptedLightBase
{    
    void BlowoutLightning()
    {
		SetLightType(LightType.AMBIENT);
        SetVisibleDuringDaylight(true);
        SetRadiusTo(100);
        SetBrightnessTo(20);
		SetLifetime(0.1);
		SetCastShadow(false);
		SetFlareVisible(false);
    }
}

class BlowoutLightFlash: ScriptedLightBase
{
	void BlowoutLightFlash()
	{
		SetLightType(LightType.AMBIENT);
		SetVisibleDuringDaylight(true);
		SetRadiusTo(100);
		SetBrightnessTo(80);
		SetLifetime(0.07);
		SetFadeOutTime(0.02);
		SetCastShadow(false);
		SetFlareVisible(false);
	}
}

class BlowoutTeleportLight: BlowoutLightFlash
{
	void BlowoutTeleportLight()
	{
		SetDiffuseColor(0.3, 0.3, 1);	
		SetAmbientColor(0.3, 0.3, 1);	
		SetBrightnessTo(25);
		SetRadiusTo(20);
		SetCastShadow(false);
		SetFlareVisible(true);
	}
}

class BlowoutLight: ScriptedLightBase
{
	void BlowoutLight()
	{
		SetLightType(LightSourceType.PointLight);
		SetVisibleDuringDaylight(true);
		SetRadiusTo(80);
		SetBrightnessTo(0.01);
		SetCastShadow(true);
		SetDiffuseColor(0.3, 0.3, 1);		
		SetFlickerSpeed(5);
		SetFlickerAmplitude(0.5);
		SetFlareVisible(false);
		
		EnableHeatHaze(false);
		SetHeatHazePower(1);
		SetHeatHazeRadius(0);
	}
}


class BlowoutStaticLight: ScriptedLightBase
{
	void BlowoutStaticLight()
	{
		SetLightType(LightSourceType.PointLight);
		SetVisibleDuringDaylight(true);
		SetRadiusTo(80);
		SetBrightnessTo(0.5);
		SetCastShadow(true);
		SetDiffuseColor(0.3, 0.3, 1);
		SetAmbientColor(0.3, 0.3, 1);
		SetFlickerSpeed(2.5);
		SetFlickerAmplitude(7.5);
		SetFlareVisible(false);
	}
}

class BlowoutReactorLight: ScriptedLightBase
{
	void BlowoutReactorLight()
	{
		SetLightType(LightSourceType.PointLight);
		SetVisibleDuringDaylight(true);
		SetDiffuseColor(0.05, 0.1, 1);
		SetRadiusTo(12);
		SetBrightnessTo(1);
		SetCastShadow(false);
	}
}
