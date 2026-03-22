class PPERequester_RadGrain extends PPERequester_GameplayBase
{
    protected float m_grainSize;
    
    override protected void OnStart(Param par = null)
    {
        super.OnStart();
        
        if (par && par.IsInherited(Param1) )
        {
            Param1<float> param = Param1<float>.Cast(par);
            m_grainSize = param.param1;
        }

        SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_GRAINSIZE, false, m_grainSize, PPEFilmGrain.L_2_NVG, PPOperators.SET);
        SetTargetValueFloat(PostProcessEffectType.FilmGrain, PPEFilmGrain.PARAM_SHARPNESS, false, 10, PPEFilmGrain.L_1_NVG, PPOperators.SET);
        SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS, PPELightIntensityParamsNative.PARAM_NOISE_MULT, false, 2.0, PPELightIntensityParamsNative.L_1_NVG, PPOperators.HIGHEST);
        SetTargetValueFloat(PPEExceptions.NVLIGHTPARAMS, PPELightIntensityParamsNative.PARAM_LIGHT_MULT, false, 3.0, PPELightIntensityParamsNative.L_0_NVG, PPOperators.HIGHEST);
    }
}