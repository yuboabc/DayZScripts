class HeatComfortAnimHandler
{
	const float TICK_INTERVAL  = 2;
	float m_TimeSinceLastTick;
	float m_ProcessTimeAccuFreeze;
	float m_ProcessTimeAccuFreezeRattle;
	float m_ProcessTimeAccuHot;
	
	PlayerBase m_Player;
	float m_EventTimeFreeze = -1;
	float m_EventTimeFreezeRattle = -1;
	float m_EventTimeHot = -1;
	protected ref HumanMovementState m_MovementState = new HumanMovementState();
	
	const float TIME_INTERVAL_HC_MINUS_LOW_MIN 	= 12; 	const float TIME_INTERVAL_HC_MINUS_LOW_MAX 	= 20;
	const float TIME_INTERVAL_HC_MINUS_HIGH_MIN = 25; 	const float TIME_INTERVAL_HC_MINUS_HIGH_MAX = 40;
	
	const float TIME_INTERVAL_HC_PLUS_LOW_MIN 	= 5; 	const float TIME_INTERVAL_HC_PLUS_LOW_MAX 	= 12;
	const float TIME_INTERVAL_HC_PLUS_HIGH_MIN 	= 15;	const float TIME_INTERVAL_HC_PLUS_HIGH_MAX 	= 25;
	
	void HeatComfortAnimHandler(PlayerBase player)
	{
		m_Player = player;
	}

	void Update(float delta_time, HumanMovementState hms)
	{
		m_TimeSinceLastTick += delta_time;

		if( m_TimeSinceLastTick > TICK_INTERVAL )
		{
			Process(m_TimeSinceLastTick);
			m_TimeSinceLastTick = 0;
		}
	}
	
	float GetEventTime(float hc_value, float threshold_low, float threshold_high, float low_min, float high_min, float low_max, float high_max)
	{
		float inv_value = Math.InverseLerp(threshold_low, threshold_high, hc_value);
		float value_min = Math.Lerp(low_min, high_min, inv_value);
		float value_max = Math.Lerp(low_max, high_max, inv_value);

		return Math.RandomFloatInclusive(value_min, value_max);
	}
	
	
	void Process(float delta_time)
	{
		if( GetGame().IsServer() )
		{
			float hc = m_Player.GetStatHeatComfort().Get();
			
			if ( hc <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL )		// Deep blue zone, rattling sounds are layerd on top of freezing sounds
			{
				m_ProcessTimeAccuFreezeRattle++;
				
				if (m_EventTimeFreezeRattle < 0)
					m_EventTimeFreezeRattle = GetEventTime(hc, -1, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_CRITICAL, TIME_INTERVAL_HC_MINUS_LOW_MIN, TIME_INTERVAL_HC_MINUS_HIGH_MIN, TIME_INTERVAL_HC_MINUS_LOW_MAX, TIME_INTERVAL_HC_MINUS_HIGH_MAX);
				
				if( m_ProcessTimeAccuFreezeRattle > m_EventTimeFreezeRattle )
				{
					m_ProcessTimeAccuFreezeRattle = 0;
					m_EventTimeFreezeRattle = -1;
					m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_FREEZE_RATTLE);
				}
			}
			
			if ( hc <= PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING )		// Light blue zone
			{
				m_ProcessTimeAccuFreeze++;
				
				if(m_EventTimeFreeze < 0)	//if not set
				{
					m_EventTimeFreeze = GetEventTime(hc, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_EMPTY, PlayerConstants.THRESHOLD_HEAT_COMFORT_MINUS_WARNING, TIME_INTERVAL_HC_MINUS_LOW_MIN, TIME_INTERVAL_HC_MINUS_HIGH_MIN, TIME_INTERVAL_HC_MINUS_LOW_MAX, TIME_INTERVAL_HC_MINUS_HIGH_MAX);
				}
				
				if( m_ProcessTimeAccuFreeze > m_EventTimeFreeze )
				{
					m_ProcessTimeAccuFreeze = 0;
					m_EventTimeFreeze = -1;
					m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_FREEZE);
				}	
			}
			else if ( hc >= PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING )	// Yellow zone
			{
				m_ProcessTimeAccuHot++;
				
				if(m_EventTimeHot < 0)	//if not set
				{
					m_EventTimeHot = GetEventTime(hc, PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_EMPTY,PlayerConstants.THRESHOLD_HEAT_COMFORT_PLUS_WARNING, TIME_INTERVAL_HC_PLUS_LOW_MIN, TIME_INTERVAL_HC_PLUS_LOW_MIN, TIME_INTERVAL_HC_PLUS_LOW_MAX, TIME_INTERVAL_HC_PLUS_HIGH_MAX);
				}
				
				if( m_ProcessTimeAccuHot > m_EventTimeHot )
				{
					m_ProcessTimeAccuHot = 0;
					m_EventTimeHot = -1;
					m_Player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_HOT);					
				}	
			}
		}
	}
}