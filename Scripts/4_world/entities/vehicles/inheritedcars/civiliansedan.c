class CivilianSedan extends CarScript
{
	protected ref UniversalTemperatureSource m_UTSource;
	protected ref UniversalTemperatureSourceSettings m_UTSSettings;
	protected ref UniversalTemperatureSourceLambdaEngine m_UTSLEngine;

	void CivilianSedan()
	{
		//m_dmgContactCoef		= 0.065;
		
		m_EngineStartOK 		= "CivilianSedan_engine_start_SoundSet";
		m_EngineStartBattery	= "CivilianSedan_engine_failed_start_battery_SoundSet";
		m_EngineStartPlug		= "CivilianSedan_engine_failed_start_sparkplugs_SoundSet";
		m_EngineStartFuel		= "CivilianSedan_engine_failed_start_fuel_SoundSet";
		m_EngineStop 			= "CivilianSedan_engine_stop_SoundSet";
		m_EngineStopFuel		= "CivilianSedan_engine_stop_fuel_SoundSet";

		m_CarDoorOpenSound		= "offroad_door_open_SoundSet";
		m_CarDoorCloseSound		= "offroad_door_close_SoundSet";
		
		m_CarHornShortSoundName = "CivilianSedan_Horn_Short_SoundSet";
		m_CarHornLongSoundName	= "CivilianSedan_Horn_SoundSet";
		
		SetEnginePos("0 0.7 1.6");
	}
	
	override void EEInit()
	{		
		super.EEInit();
		
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
 			m_UTSSettings 						= new UniversalTemperatureSourceSettings();
			m_UTSSettings.m_ManualUpdate 		= true;
			m_UTSSettings.m_TemperatureItemCap 	= GameConstants.ITEM_TEMPERATURE_NEUTRAL_ZONE_MIDDLE;
			m_UTSSettings.m_TemperatureCap		= 0;
			m_UTSSettings.m_RangeFull			= 0.5;
			m_UTSSettings.m_RangeMax			= 2;
			
			m_UTSLEngine						= new UniversalTemperatureSourceLambdaEngine();
			m_UTSource							= new UniversalTemperatureSource(this, m_UTSSettings, m_UTSLEngine);
		}		
	}
	
	override void OnEngineStart()
	{
		super.OnEngineStart();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(true, 20.0);
		}
	}
	
	override void OnEngineStop()
	{
		super.OnEngineStop();

		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			m_UTSource.SetDefferedActive(false, 10.0);
		}
	}
	
	override void EOnPostSimulate(IEntity other, float timeSlice)
	{
		if (GetGame().IsServer() || !GetGame().IsMultiplayer())
		{
			if (m_UTSource.IsActive())
			{
				m_UTSource.Update(m_UTSSettings, m_UTSLEngine);
			}
		}
	}
	
	override int GetAnimInstance()
	{
		return VehicleAnimInstances.SEDAN;
	}
	
	override float GetTransportCameraDistance()
	{
		return 4.5;
	}

	override int GetSeatAnimationType(int posIdx)
	{
		switch (posIdx)
		{
		case 0:
			return DayZPlayerConstants.VEHICLESEAT_DRIVER;
		case 1:
			return DayZPlayerConstants.VEHICLESEAT_CODRIVER;
		case 2:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_L;
		case 3:
			return DayZPlayerConstants.VEHICLESEAT_PASSENGER_R;
		}

		return 0;
	}
	
	// Override for car-specific light type
	override CarLightBase CreateFrontLight()
	{
		return CarLightBase.Cast( ScriptedLightBase.CreateLight(CivilianSedanFrontLight) );
	}
	
	// Override for car-specific light type
	override CarRearLightBase CreateRearLight()
	{
		return CarRearLightBase.Cast( ScriptedLightBase.CreateLight(CivilianSedanRearLight) );
	}
	
	override bool CanReleaseAttachment( EntityAI attachment )
	{
		if( !super.CanReleaseAttachment( attachment ) )
			return false;
		
		string attType = attachment.GetType();
		
		if ( EngineIsOn() || GetCarDoorsState("CivSedanHood") == CarDoorState.DOORS_CLOSED )
		{
			if ( attType == "CarRadiator" || attType == "CarBattery" || attType == "SparkPlug")
				return false;
		}

		return true;
	}

	override protected bool CanManipulateSpareWheel(string slotSelectionName)
	{
		return GetCarDoorsState("CivSedanTrunk") != CarDoorState.DOORS_CLOSED;
	}
	
	override bool CanDisplayAttachmentCategory(string category_name)
	{
		//super
		if (!super.CanDisplayAttachmentCategory(category_name))
			return false;
		//
	
		category_name.ToLower();		
		if (category_name.Contains("engine"))
		{
			if (GetCarDoorsState("CivSedanHood") == CarDoorState.DOORS_CLOSED)
				return false;
		}
				
		return true;
	}
	
	override bool CanDisplayCargo()
	{
		if ( !super.CanDisplayCargo() )
			return false;
		
		if ( GetCarDoorsState("CivSedanTrunk") == CarDoorState.DOORS_CLOSED )
			return false;
		
		return true;
	}
	
	override int GetCarDoorsState( string slotType )
	{
		CarDoor carDoor;

		Class.CastTo( carDoor, FindAttachmentBySlotName( slotType ) );
		if (!carDoor)
		{
			return CarDoorState.DOORS_MISSING;
		}
	
		switch (slotType)
		{
		case "CivSedanDriverDoors":
			return TranslateAnimationPhaseToCarDoorState("DoorsDriver");
			break;
		
		case "CivSedanCoDriverDoors":
			return TranslateAnimationPhaseToCarDoorState("DoorsCoDriver");
			break;

		case "CivSedanCargo1Doors":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo1");
			break;

		case "CivSedanCargo2Doors":
			return TranslateAnimationPhaseToCarDoorState("DoorsCargo2");
			break;
		
		case "CivSedanTrunk":
			return TranslateAnimationPhaseToCarDoorState("DoorsTrunk");
			break;
		
		case "CivSedanHood":
			return TranslateAnimationPhaseToCarDoorState("DoorsHood");
			break;
		}

		return CarDoorState.DOORS_MISSING;
	}
	

	override bool CrewCanGetThrough( int posIdx )
	{
		switch( posIdx )
		{
			case 0:
				if ( GetCarDoorsState("CivSedanDriverDoors") == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
			
			case 1:
				if ( GetCarDoorsState("CivSedanCoDriverDoors") == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 2:
				if ( GetCarDoorsState("CivSedanCargo1Doors") == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;

			case 3:
				if ( GetCarDoorsState("CivSedanCargo2Doors") == CarDoorState.DOORS_CLOSED )
					return false;

				return true;
			break;
		}

		return false;
	}

	override string GetDoorSelectionNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "doors_driver";
		break;
		case 1:
			return "doors_codriver";
		break;
		case 2:
			return "doors_cargo1";
		break;
		case 3:
			return "doors_cargo2";
		break;
		}
		
		return super.GetDoorSelectionNameFromSeatPos(posIdx);
	}

	override string GetDoorInvSlotNameFromSeatPos(int posIdx)
	{
		switch( posIdx )
		{
		case 0:
			return "CivSedanDriverDoors";
		break;
		case 1:
			return "CivSedanCoDriverDoors";
		break;
		case 2:
			return "CivSedanCargo1Doors";
		break;
		case 3:
			return "CivSedanCargo2Doors";
		break;
		}
		
		return super.GetDoorInvSlotNameFromSeatPos(posIdx);
	}

	// 0 full ambient and engine sound
	// 1 zero ambient and engine sound
	override float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		switch (ctrl)
		{
		case CarSoundCtrl.DOORS:
			float newValue = 0;
			if (GetCarDoorsState("CivSedanDriverDoors") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("CivSedanCoDriverDoors") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}
		
			if (GetCarDoorsState("CivSedanCargo1Doors") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}

			if (GetCarDoorsState("CivSedanCargo2Doors") == CarDoorState.DOORS_CLOSED)
			{
				newValue += 0.25;
			}
		
			if (GetHealthLevel("WindowFront") == GameConstants.STATE_RUINED)
			{
				newValue -= 0.6;
			}

			if (GetHealthLevel("WindowBack") == GameConstants.STATE_RUINED)
			{
				newValue -= 0.6;
			}
		
			return Math.Clamp(newValue, 0, 1);
		break;
		}

		return super.OnSound(ctrl, oldValue);
	}

	override string GetAnimSourceFromSelection(string selection)
	{
		switch (selection)
		{
			case "doors_driver":
				return "DoorsDriver";
			case "doors_codriver":
				return "DoorsCoDriver";
			case "doors_cargo1":
				return "DoorsCargo1";
			case "doors_cargo2":
				return "DoorsCargo2";
			case "doors_hood":
				return "DoorsHood";
			case "doors_trunk":
				return "DoorsTrunk";
		}

		return "";
	}

	override bool IsVitalTruckBattery()
	{
		return false;
	}

	override bool IsVitalGlowPlug()
	{
		return false;
	}
	
	override bool CanReachSeatFromSeat(int currentSeat, int nextSeat)
	{
		switch (currentSeat)
		{
			case 0:
				return nextSeat == 1;
	
			case 1:
				return nextSeat == 0;
	
			case 2:
				return nextSeat == 3;
	
			case 3:
				return nextSeat == 2;
		}
		
		return false;
	}

	override bool CanReachDoorsFromSeat( string pDoorsSelection, int pCurrentSeat )
	{
		switch (pCurrentSeat)
		{
			case 0:
				return pDoorsSelection == "DoorsDriver";
	
			case 1:
	 			return pDoorsSelection == "DoorsCoDriver";
	
			case 2:
				return pDoorsSelection == "DoorsCargo1";
	
			case 3:
				return pDoorsSelection == "DoorsCargo2";
		}
		
		return false;		
	}

	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");

		GetInventory().CreateInInventory("CivSedanDoors_Driver");
		GetInventory().CreateInInventory("CivSedanDoors_CoDriver");
		GetInventory().CreateInInventory("CivSedanDoors_BackLeft");
		GetInventory().CreateInInventory("CivSedanDoors_BackRight");
		GetInventory().CreateInInventory("CivSedanHood");
		GetInventory().CreateInInventory("CivSedanTrunk");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
	}
	
	override float GetPushForceCoefficientMultiplier()
	{
		return 1.5;
	}
}

class CivilianSedan_Wine extends CivilianSedan
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");

		GetInventory().CreateInInventory("CivSedanDoors_Driver_Wine");
		GetInventory().CreateInInventory("CivSedanDoors_CoDriver_Wine");
		GetInventory().CreateInInventory("CivSedanDoors_BackLeft_Wine");
		GetInventory().CreateInInventory("CivSedanDoors_BackRight_Wine");
		GetInventory().CreateInInventory("CivSedanHood_Wine");
		GetInventory().CreateInInventory("CivSedanTrunk_Wine");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
	}
}

class CivilianSedan_Black extends CivilianSedan
{
	override void OnDebugSpawn()
	{
		SpawnUniversalParts();
		SpawnAdditionalItems();
		FillUpCarFluids();

		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");

		GetInventory().CreateInInventory("CivSedanDoors_Driver_Black");
		GetInventory().CreateInInventory("CivSedanDoors_CoDriver_Black");
		GetInventory().CreateInInventory("CivSedanDoors_BackLeft_Black");
		GetInventory().CreateInInventory("CivSedanDoors_BackRight_Black");
		GetInventory().CreateInInventory("CivSedanHood_Black");
		GetInventory().CreateInInventory("CivSedanTrunk_Black");

		//-----IN CAR CARGO
		GetInventory().CreateInInventory("CivSedanWheel");
		GetInventory().CreateInInventory("CivSedanWheel");
	}
}