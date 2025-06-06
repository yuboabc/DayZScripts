
//!	Car's sound controller list. (native, do not change or extend)
enum CarSoundCtrl
{
	// simulation
	ENGINE, //!< indicates if engine is ON
	RPM,    //!< engine's RPM
	SPEED,  //!< speed of the car in km/h

	// miscellaneous
	DOORS, //!< indicates if doors are open
	PLAYER //!< indicates if driver is controlled by player
};



//!	Type of vehicle's fluid. (native, do not change or extend)
enum CarFluid
{
	FUEL,
	OIL,
	BRAKE,
	COOLANT,

	USER1, //!< reserved for user / modding support
	USER2, //!< reserved for user / modding support
	USER3, //!< reserved for user / modding support
	USER4  //!< reserved for user / modding support
};



//!	Enumerated gearbox types. (native, do not change or extend)
enum CarGearboxType
{
	MANUAL,   //!< classic manual transmission with friction plates between engine and gearbox
	AUTOMATIC //!< automatic transmission with torque converter between engine and gearbox
}



//!	Enumerated vehicle's gears. (native, do not change or extend)
enum CarGear
{
	REVERSE,
	NEUTRAL,
	FIRST,
	SECOND,
	THIRD,
	FOURTH,
	FIFTH,
	SIXTH,
	SEVENTH,
	EIGTH,
	NINTH,
	TENTH,
	ELEVENTH,
	TWELFTH,
	THIRTEENTH,
	FOURTEENTH,
	FIFTEENTH,
	SIXTEENTH
};



//!	Enumerated automatic gearbox modes. (native, do not change or extend)
enum CarAutomaticGearboxMode
{
	P, //!< park
	R, //!< reverse
	N, //!< neutral
	D  //!< drive
};


//!	Enumerated car wheel water state. (native, do not change or extend)
enum CarWheelWaterState
{
	ON_LAND,		//!< if the wheel is on or above land 
	IN_WATER,		//!< if the wheel is partially within some water plane
	UNDER_WATER		//!< if the wheel is under a water plane
};

class CarOwnerState : TransportOwnerState
{
};

class CarMove : TransportMove
{
};

//!	Native class for cars - handles physics simulation
class Car extends Transport
{
	//!
	protected override event typename GetOwnerStateType()
	{
		return CarOwnerState;
	}

	//!
	protected override event typename GetMoveType()
	{
		return CarMove;
	}
	
	//!	Returns the current speed of the vehicle in km/h.
	proto native float GetSpeedometer();
	
	//! Returns the current speed of the vehicle in km/h. Value is absolute
	float GetSpeedometerAbsolute()
	{
		return Math.AbsFloat(GetSpeedometer());
	}

	override bool IsAreaAtDoorFree( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		vector transform[4];
		
		vector extents;
		
		extents[0] = horizontalExtents;
		extents[1] = playerHeight;
		extents[2] = horizontalExtents;

		float speed = GetSpeedometerAbsolute();
		if (speed > 8)
			extents[2] = extents[2] * 6;
		if (speed > 8)
			extents[0] = 2;
		
		return IsAreaAtDoorFree( currentSeat, maxAllowedObjHeight, extents, transform );
	}
	
	override Shape DebugFreeAreaAtDoor( int currentSeat, float maxAllowedObjHeight = 0.5, float horizontalExtents = 0.5, float playerHeight = 1.7 )
	{
		int color = ARGB(20, 0, 255, 0);
		
		vector transform[4];
		
		vector extents;
		
		extents[0] = horizontalExtents;
		extents[1] = playerHeight;
		extents[2] = horizontalExtents;

		float speed = GetSpeedometerAbsolute();
		if (speed > 8)
			extents[2] = extents[2] * 6;
		if (speed > 8)
			extents[0] = 2;
		
		if (!IsAreaAtDoorFree( currentSeat, maxAllowedObjHeight, extents, transform ))
		{
			color = ARGB(20, 255, 0, 0);
		}
		
		Shape shape = Debug.DrawBox(-extents * 0.5, extents * 0.5, color);
		shape.SetMatrix(transform);
		return shape;
	}
	
	protected bool DetectFlippedUsingWheels(VehicleFlippedContext ctx, bool disallowSide)
	{
		if (disallowSide && (vector.Dot(GetDirectionUp(), vector.Up) < 0.7))
		{
			// return as "flipped", vehicle isn't pointing enough up to be reasonably certain
			return true;
		}
		
		int wheelCount = WheelCount();
		
		for (int wheelIdx = 0; wheelIdx < wheelCount; wheelIdx++)
		{
			if (!WheelHasContact(wheelIdx))
			{
				// wheel not in contact, then we could be flipped, we assume there exist other predicates
				return true;
			}
		}
		
		// all wheels in contact (or zero registered wheels), then we are in contact
		return false;
	}
	

	//!	Returns the current steering value in range <-1, 1>.
	proto native float GetSteering();

	//! Sets the future steering value.
	proto native void SetSteering(float value, bool unused0 = false);

	//!	Returns the actual throttle value in range <0, 1>.
	proto native float GetThrottle();
	
	//! Sets the future throttle value.
	proto native void SetThrottle(float value);

	//! Returns the value of how much the clutch is disengaged.
	proto native int GetClutch();

	//! Sets the future clutch value.
	proto native void SetClutch(float value);

	//! Returns the current brake value in range <0, 1>.
	proto native float GetBrake();

	//! Sets the future brake value
	proto native void SetBrake(float value, float unused0 = 0, bool unused1 = false);
	
	//! Returns the current handbrake value in range <0, 1>.
	proto native float GetHandbrake();

	//! Sets the future handbrake value
	proto native void SetHandbrake(float value);
	
	//! Sets if brakes should activate without a driver present
	proto native void SetBrakesActivateWithoutDriver(bool activate = true);
	
	//! Returns engine's min operating rpm
	proto native float EngineGetRPMMin();
	
	//! Returns engine's idle rpm before engine stalls.
	proto native float EngineGetRPMIdle();
	
	//! Returns engine's max rpm before engine blows up.
	proto native float EngineGetRPMMax();

	//! Returns engine's maximal working rpm without damaging the engine.
	proto native float EngineGetRPMRedline();

	//! Returns engine's rpm value.
	proto native float EngineGetRPM();

	//! Returns true when engine is running, false otherwise.
	proto native bool EngineIsOn();

	//! Starts the engine.
	proto native void EngineStart();

	//! Stops the engine.
	proto native void EngineStop();

	//! Get actual position of engine (model space)
	proto native vector GetEnginePos();
	
	//! Override the position of engine (model space)
	proto native void SetEnginePos(vector pos);

	//! Returns the index of the current gear, -1 if there is no engine.
	proto native int GetCurrentGear();

	//! Returns the index of the future gear, -1 if there is no engine.
	proto native int GetGear();

	//! Returns the index of the neutral gear.
	proto native int GetNeutralGear();

	//! Returns the number of gears.
	proto native int GetGearCount();

	//! Shifts the future gear up, triggering gearbox simulation.
	proto native void ShiftUp();

	//! Shifts the future gear to selected gear, triggering gearbox simulation.
	proto native void ShiftTo(int gear);

	//! Shifts the future gear down, triggering gearbox simulation.
	proto native void ShiftDown();

	//! Returns gearbox type. See CarGearboxType enum for more info.
	proto native CarGearboxType GearboxGetType();

	//! Returns gearbox mode. This is useful when car has automatic gearbox.
	proto native CarAutomaticGearboxMode GearboxGetMode();

	//! Returns true if any of the wheels are locked in terms of its movement.
	proto native bool WheelIsAnyLocked();

	/*!
		Returns the raw angular velocity of the wheel, unstable value

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native float WheelGetAngularVelocity( int wheelIdx );

	/*!
		Returns true if given wheel is making any contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native bool WheelHasContact( int wheelIdx );

	/*!
		Returns the position of contact in world space, only valid if there was an actual contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetContactPosition( int wheelIdx );

	/*!
		Returns the normal of contact in world space, only valid if there was an actual contact

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetContactNormal( int wheelIdx );

	/*!
		Returns the direction pointing forwards that the wheel is facing

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native vector WheelGetDirection( int wheelIdx );

	/*!
		Returns the surface that the wheel is nearby

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native SurfaceInfo WheelGetSurface( int wheelIdx );

	/*!
		Returns the state that the wheel is in with water

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native CarWheelWaterState WheelGetWaterState( int wheelIdx );

	/*!
		Returns the entity attached that represents the wheel

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native EntityAI WheelGetEntity( int wheelIdx );

	/*!
		Returns true if given wheel is locked in terms of its movement.

		\param[in] wheelIdx index of the wheel, they are counted from left-front to rear-right
	*/
	proto native bool WheelIsLocked( int wheelIdx );
	
	//! How many wheel can be attached to a car (hubs only)
	proto native int WheelCount();
	
	//! Number of actually attached wheels (hubs only)
	proto native int WheelCountPresent();

	/*!
		Returns tank capacity for the specified vehicle's fluid.

		\param fluid the specified fluid type
	*/
	proto native float GetFluidCapacity(CarFluid fluid);

	/*!
		Returns fraction value (in range <0, 1>)
		of the current state of the specified vehicle's fluid.

		\param[in] fluid the specified fluid type
	*/
	proto native float GetFluidFraction(CarFluid fluid);

	//! Removes from the specified fluid the specified amount.
	proto native void Leak(CarFluid fluid, float amount);

	//! Removes all the specified fluid from vehicle.
	proto native void LeakAll(CarFluid fluid);

	//! Adds to the specified fluid the specified amount.
	proto native void Fill(CarFluid fluid, float amount);

	/*!
		Is called every time the game wants to start the engine.

		\return true if the engine can start, false otherwise.
	*/
	bool OnBeforeEngineStart()
	{
		// engine can start by default
		return true;
	}

	/*!
		Is called every time the engine starts.
	*/
	void OnEngineStart() {}

	/*!
		Is called every time the engine stops.
	*/
	void OnEngineStop() {}

	/*!
		Is called every time when the simulation changed gear.

		\param[in] newGear new gear level
		\param[in] oldGear previous gear level before gear shift
	*/
	void OnGearChanged(int newGear, int oldGear)
	{
	}

	/*!
		Is called every time when the specified vehicle's fluid level changes. 
		This callback is called on owner only.

		\param[in] fluid fluid identifier, \see CarFluid
		\param[in] newValue new fluid level
		\param[in] oldValue previous fluid level before change
	*/
	void OnFluidChanged(CarFluid fluid, float newValue, float oldValue) {}

	/*!
		Is called every sound simulation step.
		In this callback, user can modify behaviour of sound controllers.

		\param[in] ctrl sound controller identifier, \see CarSoundCtrl
		\param[in] oldValue already computed value by the game code
		\return new value of the specified sound controller.
	*/
	float OnSound(CarSoundCtrl ctrl, float oldValue)
	{
		// just use the computed value by the game code
		return oldValue;
	}

	[Obsolete("no replacement")]
	proto native void ForcePosition(vector pos);
	
	[Obsolete("no replacement")]
	proto native void ForceDirection(vector dir);

	[Obsolete("Use methods directly on Car")]
	proto native CarController GetController();

	[Obsolete("Use Car.IsTurbo")]
	proto native float GetThrustTurbo();

	[Obsolete("Use Car.IsGentle")]
	proto native float GetThrustGentle();

	[Obsolete("Use Car.GetThrottle")]
	proto native float GetThrust();
	
	[Obsolete("Use Car.SetThrottle/Car.SetTurbo/Car.SetGentle")]
	proto native void SetThrust(float in, float gentle = 0, float turbo = 0);

	[Obsolete("no replacement")];
	proto native void SetClutchState(bool in);

	[Obsolete("Use Car.GetGearCount")]
	proto native int GetGearsCount();

};



//! DEPRECATED class left for backwards compatibility, methods are available on car itself now
class CarController
{
	private void CarController() {}
	private void ~CarController() {}
	
	//!	Returns the current steering value in range <-1, 1>.
	proto float GetSteering();
	/*!
		Sets the steering value.

		\param in     should be in range <-1, 1>
		\param analog indicates if the input value was taken from analog controller
	*/
	proto void SetSteering( float in, bool analog = false );

	//!	Returns the current thrust turbo modifier value in range <0, 1>.
	proto float GetThrustTurbo();
	//!	Returns the current thrust gentle modifier value in range <0, 1>.
	proto float GetThrustGentle();
	//!	Returns the current thrust value in range <0, 1>.
	proto float GetThrust();
	/*!
		Sets the thrust value.

		\param in     should be in range <0, 1>
		\param gentle should be in range <0, 1>, thrust modifier
		\param turbo  should be in range <0, 1>, thrust modifier
	*/
	proto void SetThrust( float in, float gentle = 0, float turbo = 0 );

	//! Returns the current brake value in range <0, 1>.
	proto float GetBrake();
	/*!
		Sets the brake value.

		\param in should be in range <0, 1>
		\param panic should be in range <0, 1>
	*/
	proto void SetBrake( float in, float panic = 0 );

	//!	Returns index of the current gear.
	proto int GetGear();

	proto void ShiftUp();
	proto void ShiftTo( CarGear gear );
	proto void ShiftDown();
};