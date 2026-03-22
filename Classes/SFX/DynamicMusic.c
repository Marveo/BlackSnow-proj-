class DynamicMusic
{
	protected PlayerBase m_Player;
	protected vector m_PlayerPos;
	protected string m_PlayerPosSurface;
	protected float m_PrimaryTimer;
	protected float m_LastEventPlayed;
	protected float m_LastLocationPlayed;
	protected float m_LastTimePlayed;
	protected AbstractWave m_ActiveMusic;
	protected AbstractWave m_LantiaMusic;
	protected string m_ActiveLocation;
	protected string m_ActiveTime;
	protected string m_ActiveEvent;
	protected bool m_Night;

	protected float m_Namalsk_Sunrise_Jan = 9.16;
	protected float m_Namalsk_Sunset_Jan = 14.85;
	protected float m_Namalsk_Sunrise_Jul = 2.85;
	protected float m_Namalsk_Sunset_Jul = 21.15;

	void DynamicMusic(PlayerBase pPlayer)
	{
		m_Player = pPlayer;
		m_PlayerPos = vector.Zero;
		m_PlayerPosSurface = "";
		m_ActiveLocation = "none";
		m_ActiveTime = "none";
		m_ActiveEvent = "none";
		m_PrimaryTimer = 0.0;
		m_LastEventPlayed = 0.0;
		m_LastLocationPlayed = Math.RandomInt(30,60);
		m_LastTimePlayed = m_LastLocationPlayed + Math.RandomInt(60,80);
		m_Night = false;
	}

	// this method is called from PlayerBase
	void TimeUpdate(float pDt)
	{
		m_PrimaryTimer += pDt;
		MusicUpdate();
	}

	private void MusicUpdate()
	{
		// do necessary checks every second
		if (m_PrimaryTimer > 1.0)
		{
			m_PrimaryTimer = 0.0;
			m_PlayerPos = m_Player.GetPosition();
			m_PlayerPosSurface = m_Player.GetSurfaceType();
			m_ActiveEvent = EventTrigger();
			m_ActiveLocation = PositionTriggers();
			m_ActiveTime = DayNightTrigger();

			if ( !m_ActiveMusic )
			{
				if ( m_ActiveEvent != "none" )
				{
					if ( m_LastEventPlayed <= 0.0 )
					{
						PlayMusic( m_ActiveEvent );
						m_LastEventPlayed = Math.RandomInt( 240, 300 );
					}
					else
					{
						m_LastEventPlayed += -1.0;
					}
				}
				if ( m_ActiveLocation != "none" )
				{
					if ( m_LastLocationPlayed <= 0.0 )
					{
						PlayMusic( m_ActiveLocation );
						m_LastLocationPlayed = Math.RandomInt( 240, 300 );
					}
					else
					{
						m_LastLocationPlayed += -1.0;
					}
				}
				if ( m_ActiveTime != "none" )
				{
					if ( m_LastTimePlayed <= 0.0 )
					{
						PlayMusic( m_ActiveTime );
						m_LastTimePlayed = Math.RandomInt( 240, 300 );
					}
					else
					{
						m_LastTimePlayed += -1.0;
					}
				}
			}

			/*DbgUI.BeginCleanupScope();
			DbgUI.Begin("DynamicMusic debug - general", 300, 100);
			DbgUI.Text("pos: " + m_PlayerPos.ToString());
			DbgUI.Text("surface: " + m_PlayerPosSurface);
			DbgUI.Text("activeLoc: " + m_ActiveLocation);
			DbgUI.Text("activeTime: " + m_ActiveTime);
			DbgUI.Text("activeEvent: " + m_ActiveEvent);
			DbgUI.Text("lastEventPlayed: " + m_LastEventPlayed.ToString());
			DbgUI.Text("lastLocationPlayed: " + m_LastLocationPlayed.ToString());
			DbgUI.Text("lastTimePlayed: " + m_LastTimePlayed.ToString());
			DbgUI.End();
			DbgUI.EndCleanupScope();*/
		}
	}

	protected string EventTrigger()
	{
		// get event manager, check if Aurora or Blizzard is running
		if ( NamEventManager.GetInstance() )
		{
			NamEventBase event_check = NamEventManager.GetInstance().GetEvent( Blizzard );
			if ( event_check )
				return "nam_event_blizzard";

			event_check = NamEventManager.GetInstance().GetEvent( Aurora );
			if ( event_check && m_Night )
				return "nam_event_aurora";
		}
		return "none";
	}

	protected string PositionTriggers()
	{
		// Check Lantia first
		if ( m_Player.IsAtLantia() )
		{
			AdjustLantiaMusicVolume( 1.0 );
			AdjustMusicVolume( 0.0 );
		}
		else
		{
			AdjustLantiaMusicVolume( 0.0 );
			AdjustMusicVolume( 1.0 );
		}
		
		// a1
		if (((m_PlayerPos[0]) > 3586) && ((m_PlayerPos[0]) < 3931))
		{
			if (((m_PlayerPos[2]) > 8098) && ((m_PlayerPos[2]) < 8409))
			{
				return "nam_loc_a1";
			}
		}

		// ari
		if (((m_PlayerPos[0]) > 4079) && ((m_PlayerPos[0]) < 4552))
		{
			if (((m_PlayerPos[2]) > 7977) && ((m_PlayerPos[2]) < 8271))
			{
				if (!IsNightTime())
					return "nam_loc_athena_day_set";
				else
					return "nam_loc_sebjan_night_set";
			}
		}

		// sebjan
		if (((m_PlayerPos[0]) > 4589) && ((m_PlayerPos[0]) < 5166))
		{
			if (((m_PlayerPos[2]) > 7890) && ((m_PlayerPos[2]) < 8280))
			{
				if (!IsNightTime())
					return "nam_loc_sebjan_day_set";
				else
					return "nam_loc_sebjan_night_set";
			}
		}

		// vorkuta
		if (((m_PlayerPos[0]) > 6467) && ((m_PlayerPos[0]) < 7042))
		{
			if (((m_PlayerPos[2]) > 10886) && ((m_PlayerPos[2]) < 11553))
			{
				if (!IsNightTime())
					return "nam_loc_vorkuta_day_set";
				else
					return "nam_loc_vorkuta_night_set";
			}
		}

		// skat
		if (((m_PlayerPos[0]) > 11711) && ((m_PlayerPos[0]) < 12119))
		{
			if (((m_PlayerPos[2]) > 11763) && ((m_PlayerPos[2]) < 12255))
			{
				if (!IsNightTime())
					return "nam_loc_sub_day_set";
				else
					return "nam_loc_sub_night_set";
			}
		}

		// polar base
		if (((m_PlayerPos[0]) > 1137) && ((m_PlayerPos[0]) < 1349))
		{
			if (((m_PlayerPos[2]) > 11737) && ((m_PlayerPos[2]) < 11951))
			{
				if (!IsNightTime())
					return "nam_loc_sub_day_set";
				else
					return "nam_loc_sub_night_set";
			}
		}

		// phoenix
		if (((m_PlayerPos[0]) > 11563) && ((m_PlayerPos[0]) < 12208))
		{
			if (((m_PlayerPos[2]) > 7181) && ((m_PlayerPos[2]) < 7851))
			{
				return "nam_loc_phoenix_set";
			}
		}

		// a2
		if (((m_PlayerPos[0]) > 4944) && ((m_PlayerPos[0]) < 5030))
		{
			if (((m_PlayerPos[2]) > 6580) && ((m_PlayerPos[2]) < 6653))
			{
				if ((m_PlayerPos[1]) < 44)
				{
					return "nam_loc_underground_set";
				}
			}
		}
		
		// a3
		if ( ( ( m_PlayerPos[0] ) > 3563 ) && ( ( m_PlayerPos[0] ) < 3649 ) )
		{
			if ( ( ( m_PlayerPos[2] ) > 6451 ) && ( ( m_PlayerPos[2] ) < 6759 ) )
			{
				if ( ( ( m_PlayerPos[1] ) < 200 ) && ( ( m_PlayerPos[1] ) > 38 ) )
				{
					return "nam_loc_underground_set";
				}
			}
		}

		// rail tunnel
		if (((m_PlayerPos[0]) > 3571) && ((m_PlayerPos[0]) < 3741))
		{
			if (((m_PlayerPos[2]) > 6195) && ((m_PlayerPos[2]) < 7175))
			{
				if (((m_PlayerPos[1]) < 55) && (m_PlayerPosSurface == "gravel_small_int"))
				{
					return "nam_loc_underground_set";
				}
			}
		}

		return "none";
	}

	protected string DayNightTrigger()
	{
		int year, month, day, hour, minute;
		GetGame().GetWorld().GetDate(year, month, day, hour, minute);
		float sunriseTime = GetApproxNamalskSunriseTime(month + (day / 32.0));
		float sunsetTime = GetApproxNamalskSunsetTime(month + (day / 32.0));
		float hourminute = hour + (minute / 60.0);

		/*DbgUI.BeginCleanupScope();
		DbgUI.Begin("DynamicMusic debug - time", 300, 100);
		DbgUI.Text("hourminute: " + hourminute.ToString());
		DbgUI.Text("sunriseTime: " + sunriseTime.ToString());
		DbgUI.Text("sunsetTime: " + sunsetTime.ToString());
		DbgUI.End();
		DbgUI.EndCleanupScope();*/

		m_Night = false;

		// order here matters!
		if ( m_Player.IsAtLantia() || IsAtA3() )
		{
			return "none";
		}

		if ((hourminute < sunriseTime) && (hourminute > Math.Max((sunriseTime - 1.5), 0.0)))
		{
			return "nam_time_dawn_set";
		}
		if ((hourminute > sunriseTime) && (hourminute < Math.Min((sunriseTime + 4.5), 12.0)))
		{
			return "nam_time_morning_set";
		}
		if ((hourminute > 10) && (hourminute < 14))
		{
			return "nam_time_noon_set";
		}
		if ((hourminute < sunsetTime) && (hourminute > Math.Min((sunsetTime + 2.0), 12.0)))
		{
			return "nam_time_afternoon_set";
		}
		if ((hourminute > sunsetTime) && (hourminute < Math.Min((sunsetTime + 2.0), 22.0)))
		{
			return "nam_time_dusk_set";
		}
		if ((hourminute > sunsetTime) || (hourminute < sunriseTime))
		{
			m_Night = true;
			return "nam_time_night_set";
		}
		return "none";
	}

	void PlayMusic( string soundSetName )
	{
		if ( m_ActiveMusic )
		{
			return;
		}
		
		SoundParams soundParams			= new SoundParams( soundSetName );
		SoundObjectBuilder soundBuilder	= new SoundObjectBuilder( soundParams );
		SoundObject soundObject			= soundBuilder.BuildSoundObject();
		soundObject.SetKind( WaveKind.WAVEMUSIC );
		m_ActiveMusic = GetGame().GetSoundScene().Play2D( soundObject, soundBuilder );
		m_ActiveMusic.Play();
	}
	protected void AdjustMusicVolume( float volume )
	{
		if ( m_ActiveMusic )
		{
			m_ActiveMusic.SetVolumeRelative( volume );
		}
	}
	void StopMusic()
	{
		if ( m_ActiveMusic )
		{
			m_ActiveMusic.Stop();
		}
	}

	// stopping and playing music right after does not work, so give it a small delay
	void PlayA3JumpMusic()
	{
		StopMusic();
		GetGame().GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater( PlayA3JumpMusicDelayed, 2000, false, this);
	}
	protected void PlayA3JumpMusicDelayed()
	{
		PlayMusic( "nam_loc_a3" );
	}

	void PlayLantiaMusic()
	{
		if ( m_LantiaMusic )
		{
			return;
		}

		SoundParams soundParams			= new SoundParams( "nam_loc_lantia_set" );
		SoundObjectBuilder soundBuilder	= new SoundObjectBuilder( soundParams );
		SoundObject soundObject			= soundBuilder.BuildSoundObject();
		soundObject.SetKind( WaveKind.WAVEMUSIC );
		m_LantiaMusic = GetGame().GetSoundScene().Play2D( soundObject, soundBuilder );
		m_LantiaMusic.Play();
	}
	void StopLantiaMusic()
	{
		if ( m_LantiaMusic )
		{
			m_LantiaMusic.Stop();
		}
	}
	protected void AdjustLantiaMusicVolume( float volume )
	{
		if ( m_LantiaMusic )
		{
			m_LantiaMusic.SetVolumeRelative( volume );
		}
	}

	protected bool IsNightTime()
	{
		return m_Night;
	}
	
	float GetApproxNamalskSunriseTime(float monthday)
	{
		if (monthday <= 8.0)
			return ((m_Namalsk_Sunrise_Jan - m_Namalsk_Sunrise_Jul)/(8 - 1)) * (1 - monthday) + m_Namalsk_Sunrise_Jan;
		else
			return (((monthday - 8) * (m_Namalsk_Sunrise_Jan - m_Namalsk_Sunrise_Jul)) / (13 - 8)) + m_Namalsk_Sunrise_Jul;
	}
	
	float GetApproxNamalskSunsetTime(float monthday)
	{
		if (monthday <= 8.0)
			return ((m_Namalsk_Sunset_Jan - m_Namalsk_Sunset_Jul)/(8 - 1)) * (1 - monthday) + m_Namalsk_Sunset_Jan;
		else
			return (((monthday - 8) * (m_Namalsk_Sunset_Jan - m_Namalsk_Sunset_Jul)) / (13 - 8)) + m_Namalsk_Sunset_Jul;
	}

	bool IsAtA3()
	{
		if ( ( ( m_PlayerPos[0] ) > 3563 ) && ( ( m_PlayerPos[0] ) < 3649 ) )
		{
			if ( ( ( m_PlayerPos[2] ) > 6451 ) && ( ( m_PlayerPos[2] ) < 6759 ) )
			{
				if ( ( ( m_PlayerPos[1] ) < 200 ) && ( ( m_PlayerPos[1] ) > 38 ) )
				{
					return true;
				}
			}
		}

		return false;
	}
}