void hndDebugPrint (string s)
{
#ifdef INV_DEBUG
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void hndDebugSpam (string s)
{
#ifdef INV_DEBUG_SPAM
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}
void hndDebugSpamALot (string s)
{
#ifdef INV_DEBUG_SPAM_FREQ
	PrintToRPT("" + s); // comment/uncomment to hide/see debug logs
#else
	//Print("" + s); // comment/uncomment to hide/see debug logs
#endif
}



typedef FSMTransition<HandStateBase, HandEventBase, HandActionBase, HandGuardBase> HandTransition;


/**@class		HandFSM
 * @brief		Hand finite state machine
 **/
class HandFSM extends HFSMBase<HandStateBase, HandEventBase, HandActionBase, HandGuardBase>
{
	int GetCurrentStateID ()
	{
		HandStableState hState = HandStableState.Cast(m_State);
		if(hState)
			return hState.GetCurrentStateID();

		return HandStateID.UNKNOWN;
	}

	/**@fn			SyncStateFromID
	 * @brief		load from database - reverse lookup for state from saved id
	 * @param[in]	id			the id stored in database during save
	 **/
	protected bool SyncStateFromID (int id)
	{
		/*if (id == 0)
			return false;

		int s0 = id & 0x000000ff;
		int s1 = id & 0x0000ff00;

		int count = m_Transitions.Count();
		bool set0 = false;
		bool set1 = false;
		for (int i = 0; i < count; ++i)
		{
			HandTransition t = m_Transitions.Get(i);
			if (!set0 && t.m_srcState && s0 == t.m_srcState.GetCurrentStateID())
			{
				m_States[0] = t.m_srcState;
				set0 = true;
			}
			if (!set1 && t.m_srcState && s1 == t.m_srcState.GetCurrentStateID())
			{
				m_States[1] = t.m_srcState;
				set1 = true;
			}
			if (set0 && set1)
				return true;
		}*/
		return false;
	}

	/**@fn			OnStoreLoad
	 * @brief		load state of fsm
	 **/
	bool OnStoreLoad (ParamsReadContext ctx, int version)
	{
		/*int id = 0;
		ctx.Read(id);
		if (SyncStateFromID(id))
			if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[hndfsm] OnStoreLoad - loaded current state from id=" + id);
		else
			Print("[hndfsm] Warning! OnStoreLoad - cannot load curent hand state, id=" + id);*/
		return true;
	}

	/**@fn			OnStoreSave
	 * @brief		save state of fsm
	 **/
	void OnStoreSave (ParamsWriteContext ctx)
	{
		/*int id = GetCurrentStateID();
		ctx.Write(id);
		if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[hndfsm] OnStoreSave - saving current state=" + GetCurrentState() + " id=" + id);*/
	}

	/**@fn			NetSyncCurrentStateID
	 * @brief		Engine callback - network synchronization of FSM's state. not intended to direct use.
	 **/
	void NetSyncCurrentStateID (int id)
	{
		/*if (SyncStateFromID(id))
			if (LogManager.IsInventoryHFSMLogEnable()) hndDebugPrint("[hndfsm] NetSyncCurrentStateID - loaded current state from id=" + id);
		else
			Print("[hndfsm] NetSyncCurrentStateID called with null, ignoring request to set current fsm state.");*/
	}
};

