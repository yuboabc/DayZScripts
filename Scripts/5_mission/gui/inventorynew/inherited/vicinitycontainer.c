class VicinityContainer: CollapsibleContainer
{
	ref VicinitySlotsContainer			m_VicinityIconsContainer;
	ref map<EntityAI, ref Container>	m_ShowedItems				= new map<EntityAI, ref Container>;
	ref map<int, ref Container>			m_ShowedItemsIDs			= new map<int, ref Container>;
	ref array<EntityAI>					m_ShowedItemIcons			= new array<EntityAI>;
	ref map<CargoBase, ref Container>	m_ShowedCargos				= new map<CargoBase, ref Container>;
	protected bool						m_IsProcessing = false; // Prevents refreshing every time a child is added while it is still processing
	
	const float DISTANCE_TO_ENTITIES 	= 1.0;
	const float DISTANCE_TO_THE_REST 	= 0.5;
	
	void VicinityContainer( LayoutHolder parent, int sort = -1 )
	{
		m_VicinityIconsContainer = new VicinitySlotsContainer( this );
		m_Body.Insert( m_VicinityIconsContainer );
		m_VicinityIconsContainer.GetRootWidget().SetColor(166 << 24 | 120 << 16 | 120 << 8 | 120);
		
		#ifndef PLATFORM_CONSOLE
		LeftArea leftArea = LeftArea.Cast(GetParent());
		if (leftArea)
		{
			leftArea.GetSlotsHeader().AddChild(GetHeader().GetRootWidget());
			leftArea.GetSlotsArea().AddChild(m_VicinityIconsContainer.GetRootWidget());
		}
		#endif
		
		m_MainWidget = m_RootWidget.FindAnyWidget( "body" );
		WidgetEventHandler.GetInstance().RegisterOnChildAdd( m_MainWidget, this, "OnChildAdd" );
		WidgetEventHandler.GetInstance().RegisterOnChildRemove( m_MainWidget, this, "OnChildRemove" );
		
		RecomputeOpenedContainers();
		m_CollapsibleHeader.SetName("#container_vicinity");
		LoadDefaultState();
	}
	
	bool IsVicinityContainerIconsActive()
	{
		return m_VicinityIconsContainer.IsActive();
	}
	
	VicinitySlotsContainer GetVicinityIconsContainer()
	{
		return m_VicinityIconsContainer;
	}
	
	bool IsContainerWithCargoActive()
	{
		return ( ContainerWithCargo.Cast( GetFocusedContainer() ) != null );
	}
	
	bool IsItemWithAttachmentsActive()
	{
		return ( ContainerWithCargoAndAttachments.Cast( GetFocusedContainer() ) != null );
	}
	
	bool IsItemWithCategoriesActive()
	{
		return ( AttachmentCategoriesContainer.Cast( GetFocusedContainer() ) != null );
	}

	void TraverseShowedItems()
	{
		map<string, bool> serialized_types = new map<string, bool>();
		for ( int i = 0; i < m_ShowedItems.Count(); i++ )
		{
			EntityAI entity = m_ShowedItems.GetKey( i );
			Container container = m_ShowedItems.GetElement( i );
			string type = entity.GetType();
			if ( container.IsInherited( ContainerWithCargo ) )
			{
				if ( !serialized_types.Contains( type ) )
				{
					ContainerWithCargo item_with_cargo = ContainerWithCargo.Cast( container );
					serialized_types.Insert( type, item_with_cargo.IsOpened() );
				}
			}
		}

		if ( serialized_types.Count() > 0 )
		{
			FileSerializer file = new FileSerializer();
			if ( file.Open( "inventory_state.save", FileMode.APPEND ) )
			{
				file.Write( serialized_types.Count() );

				for ( i = 0; i < serialized_types.Count(); i++ )
				{
					file.Write( serialized_types.GetKey( i ) );
					file.Write( serialized_types.GetElement( i ) );
				}
			}
			file.Close();
		}
	}

	override void DraggingOverHeader( Widget w, int x, int y, Widget receiver )
	{
		if ( w == null )
		{
			return;
		}
		
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if ( !ipw )
		{
			string name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if ( !ipw && ItemPreviewWidget.Cast( w ) )
		{
			ipw = ItemPreviewWidget.Cast( w );
		}

		if ( !ipw || !ipw.GetItem() )
		{
			return;
		}

		if ( ipw.GetItem() && GetGame().GetPlayer().CanDropEntity( ipw.GetItem() ) && ipw.GetItem().GetInventory().CanRemoveEntity() && m_ShowedItemIcons.Find( ipw.GetItem() ) == -1 )
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
			ItemManager.GetInstance().HideDropzones();
			ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
			
			#ifndef PLATFORM_CONSOLE
			bool ignorePointer = ItemManager.GetInstance().GetLeftSlotsScroller().GetFlags() & WidgetFlags.IGNOREPOINTER;
			if (!ignorePointer)
			{
				ItemManager.GetInstance().GetLeftSlotsScroller().SetFlags(WidgetFlags.IGNOREPOINTER);
			}
			#endif
		}
		else
		{
			ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
			ItemManager.GetInstance().ShowSourceDropzone( ipw.GetItem() );
		}
	}

	void DraggingOverIcon( Widget w, int x, int y, Widget receiver )
	{
		string name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		EntityAI receiver_item = receiver_iw.GetItem();

		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if ( !ipw )
		{
			name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}
		
		if ( !ipw && ItemPreviewWidget.Cast( w ) )
		{
			ipw = ItemPreviewWidget.Cast( w );
		}
		
		if (!ipw)
		{
			return;
		}

		if ( !ItemBase.Cast( receiver_item ) || !ipw.GetItem() )
		{
			return;
		}

		if ( ipw.GetItem().GetInventory().CanRemoveEntity() || m_ShowedItemIcons.Find( ipw.GetItem() ) > -1 )
		{
			if ( ( ItemBase.Cast( receiver_item ) ).CanBeCombined( ItemBase.Cast( ipw.GetItem() ) ) )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.COMBINE_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				return;
			}
			else if ( GameInventory.CanSwapEntitiesEx( receiver_item, ipw.GetItem() ) )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.SWAP_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				return;
			}
			/*else if( GetGame().GetPlayer().CanDropEntity( ipw.GetItem() ) )
			{
				ColorManager.GetInstance().SetColor( w, ColorManager.GREEN_COLOR );
				ItemManager.GetInstance().HideDropzones();
				ItemManager.GetInstance().GetLeftDropzone().SetAlpha( 1 );
				return;
			}*/
		}
			
		ColorManager.GetInstance().SetColor( w, ColorManager.RED_COLOR );
		ItemManager.GetInstance().ShowSourceDropzone( ipw.GetItem() );
	}

	void OnDropReceivedFromIcon( Widget w, int x, int y, Widget receiver )
	{
		string name = receiver.GetName();
		name.Replace("PanelWidget", "Render");
		ItemPreviewWidget receiver_iw = ItemPreviewWidget.Cast( receiver.FindAnyWidget(name) );
		EntityAI receiver_item = receiver_iw.GetItem();

		ItemPreviewWidget ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( "Render" ) );
		if ( !ipw )
		{
			name = w.GetName();
			name.Replace( "PanelWidget", "Render" );
			ipw = ItemPreviewWidget.Cast( w.FindAnyWidget( name ) );
		}

		if ( !ItemBase.Cast( receiver_item ) || !ipw.GetItem() )
		{
			return;
		}
		
		EntityAI item = ipw.GetItem();
		bool equal_typed = item.GetType() == receiver_item.GetType();
		if ( !receiver_item.IsInherited( ItemBase ) || item == null )
		{
			return;
		}
		
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );

		if ( !item.GetInventory().CanRemoveEntity() )
			return;
		
		if ( ( ItemBase.Cast( receiver_item ) ).CanBeCombined( ItemBase.Cast( item ) ) )
		{
			( ItemBase.Cast( receiver_item ) ).CombineItemsClient( ItemBase.Cast( item ) );
		}
		else if( GameInventory.CanSwapEntitiesEx( receiver_item, item ) )
		{
			InventoryLocation il1 = new InventoryLocation;
			InventoryLocation il2 = new InventoryLocation;
			receiver_item.GetInventory().GetCurrentInventoryLocation( il1 );
			item.GetInventory().GetCurrentInventoryLocation( il2 );
			
			if( !receiver_item.GetInventory().CanRemoveEntity() || ( il1.GetType() == InventoryLocationType.GROUND && il2.GetType() == InventoryLocationType.GROUND ) )
				return;
			player.PredictiveSwapEntities( item, receiver_item );
		}
		
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging( false );
		PrepareOwnedTooltip(item);

		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
		if ( menu )
		{
			menu.RefreshQuickbar();
		}
	}
	
	void OnLeftPanelDropReceived( Widget w, int x, int y, Widget receiver )
	{
		OnDropReceivedFromHeader( w, x, y, receiver );
	}
	
	void OnDropReceivedFromEmptySpace( Widget w, int x, int y, Widget receiver )
	{
		OnDropReceivedFromHeader( w, x, y, receiver );
	}

	override void OnDropReceivedFromHeader(Widget w, int x, int y, Widget receiver)
	{
		if (!w)
			return;
		
		ItemManager.GetInstance().HideDropzones();
		ItemManager.GetInstance().SetIsDragging(false);
		
		ItemPreviewWidget ipw = ItemPreviewWidget.Cast(w.FindAnyWidget("Render"));
		
		if (!ipw)
		{
			string name = w.GetName();
			name.Replace("PanelWidget", "Render");
			ipw = ItemPreviewWidget.Cast(w.FindAnyWidget(name));
		}
		
		if (!ipw)
		{
			ipw = ItemPreviewWidget.Cast(w);
			if (!ipw)
				return;
		}
		
		EntityAI item = ipw.GetItem();
		if (!ipw.IsInherited(ItemPreviewWidget) || !item)
			return;
		
		if (!item.GetInventory().CanRemoveEntity() || m_ShowedItemIcons.Find(item) > -1)
			return;
		
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());		
		if (player.CanDropEntity(item))
		{
			ItemBase itemBase = ItemBase.Cast(item);
			if (itemBase)
			{
				if (itemBase.GetTargetQuantityMax() < itemBase.GetQuantity())
					itemBase.SplitIntoStackMaxClient(null, -1);
				else
					player.PhysicalPredictiveDropItem(itemBase);
			}
		}

		InventoryMenu menu = InventoryMenu.Cast( GetGame().GetUIManager().FindMenu( MENU_INVENTORY ) );
		if ( menu )
		{
			menu.RefreshQuickbar();
		}
	}

	override void UpdateInterval()
	{
		PlayerBase player = PlayerBase.Cast( GetGame().GetPlayer() );
		
		if (!player)
			return;
		
		super.UpdateInterval();
		
		EntityAI eai;
		vector pos = player.GetPosition();
		array<EntityAI> objects;
		
		VicinityItemManager.GetInstance().Update( player.GetDeltaT() );
		objects = VicinityItemManager.GetInstance().GetVicinityItems();
		
		//GetItemsShowableInInventory
		array<EntityAI> showable_items = new array<EntityAI>;
		bool isUsedMicromanagment = false;
		EntityAI selectesItem = ItemManager.GetInstance().GetSelectedItem();
		int m_OldShowedItemIconsCount = m_ShowedItemIcons.Count();

		if (selectesItem && ItemManager.GetInstance().IsMicromanagmentMode())
		{
			if (m_ShowedItemIcons.Find(selectesItem) != -1)
			{
				isUsedMicromanagment = true;
			}
		}
		m_ShowedItemIcons.Clear();
		
		GameInventory game_inventory = player.GetInventory();
		for ( int i = 0; i < objects.Count(); i++ )
		{
			eai = objects.Get( i );			
			if ( eai == null || game_inventory.IsPlaceholderEntity( eai ) )
				continue; // noproxy: ignore body placeholder
			
			BaseBuildingBase base_building = BaseBuildingBase.Cast(eai);
			if (base_building && !base_building.IsPlayerInside(player,""))
				continue;
			
			if (eai.IsInventoryVisible())
			{
				showable_items.Insert(eai);
				
				if (!eai.DisableVicinityIcon())
				{
					m_ShowedItemIcons.Insert(eai);
				}
			}
		}
		
		if (isUsedMicromanagment && m_ShowedItemIcons.Find(selectesItem) == -1 )
		{
			//ItemManager.GetInstance().SetItemMicromanagmentMode(false);
			ItemManager.GetInstance().SetSelectedItemEx(null, null, null);
			isUsedMicromanagment = false;
		}
		
		//MW HOTFIX - old containers must be removed before new one are added (part of initialize are refreshed whole structure - need proper fix to avoid it)
		map<EntityAI, ref Container> new_showed_items = new map<EntityAI, ref Container>;
		Container con;
		
		for ( i = 0; i < showable_items.Count(); i++ )
		{
			new_showed_items.Insert(showable_items[i],null);
		}
		
		
		bool need_update_focus = false;
		for ( i = 0; i < m_ShowedItems.Count(); i++ )
		{
			EntityAI ent = m_ShowedItems.GetKey( i );
			m_ShowedItems.GetElement( i ).UpdateInterval();
			con = m_ShowedItems.GetElement( i );
			if ( !new_showed_items.Contains( ent ) )
			{
				if ( selectesItem && selectesItem.GetHierarchyRoot() == ent)
				{
					//ItemManager.GetInstance().SetItemMicromanagmentMode(false);
					ItemManager.GetInstance().SetSelectedItemEx(null, null, null);
				}
				GetMainWidget().Update();
				if ( con.IsActive() )
					need_update_focus = true;
				Container.Cast( GetParent() ).Remove( con );
				Remove( con );
			}
			else
			{
				UpdateHeader(ent,con,player); //squeezed here, the map is iterated over enough already..
			}
		}
		//////////
		
		bool accessInventory = PlayerBase.DEBUG_INVENTORY_ACCESS;
		accessInventory |= FreeDebugCamera.GetInstance() && FreeDebugCamera.GetInstance().IsActive();


		//map<EntityAI, ref Container> new_showed_items = new map<EntityAI, ref Container>;
		new_showed_items.Clear();
		map<int, ref Container> showed_items_IDs = new map<int, ref Container>;
		for ( i = 0; i < showable_items.Count(); i++ )
		{
			EntityAI entity = showable_items.Get( i );
			if ( entity )
			{
				if ( game_inventory && !m_ShowedItems.Contains( entity ) )
				{
					string config = "CfgVehicles " + entity.GetType() + " GUIInventoryAttachmentsProps";

					if ( GetGame().ConfigIsExisting( config ) )
					{
						AttachmentCategoriesContainer ac = new AttachmentCategoriesContainer( m_Parent, -1 );
						ac.SetEntity( entity );
						new_showed_items.Insert( entity, ac );
						showed_items_IDs.Insert( entity.GetID(), ac);
					}
					else if ( entity.GetSlotsCountCorrect() > 0 && entity.GetInventory().GetCargo() )
					{
						if ( entity.IsInherited( DayZInfected ) )
						{
							if ( entity.IsAlive() )
							{
								continue;
							}
							ZombieContainer zmb_cnt = new ZombieContainer( m_Parent, -1 );
							zmb_cnt.SetEntity( entity );
							new_showed_items.Insert( entity, zmb_cnt );
							showed_items_IDs.Insert( entity.GetID(), zmb_cnt );
							Container.Cast( GetParent() ).Insert( zmb_cnt );
						}
						else
						{
							ContainerWithCargoAndAttachments iwca = new ContainerWithCargoAndAttachments( this, -1 );
							iwca.SetEntity( entity, false );
							new_showed_items.Insert( entity, iwca );
							showed_items_IDs.Insert( entity.GetID(), iwca );
						}
					}
					else if ( entity.GetInventory().GetCargo() )
					{
						ContainerWithCargo iwc = new ContainerWithCargo( this, -1 );
						iwc.SetEntity( entity, 0, false );
						new_showed_items.Insert( entity, iwc );
						showed_items_IDs.Insert( entity.GetID(), iwc );
						iwc.UpdateInterval();
					}
					else if ( entity.GetSlotsCountCorrect() > 0 )
					{
						if ( entity.HasEnergyManager() )
						{
							ContainerWithElectricManager iwem = new ContainerWithElectricManager( this, -1 );
							iwem.SetEntity( entity, false );
							new_showed_items.Insert( entity, iwem );
							showed_items_IDs.Insert( entity.GetID(), iwem );
						}
						else
						{
							if ( entity.IsInherited( PlayerBase ) )
							{
								//! Allow view access of inventory if in free camera. Write access is elsewhere 
								if (accessInventory == false)
								{
									if ( entity.IsAlive() && ( !PlayerBase.Cast( entity ).IsUnconscious() && !PlayerBase.Cast( entity ).IsRestrained() ) )
									{
										continue;
									}
								}
								
								PlayerContainer plyr_cnt = new PlayerContainer( m_Parent, false );
								plyr_cnt.SetPlayer( PlayerBase.Cast( entity ) );
								Container.Cast( GetParent() ).Insert( plyr_cnt );
								( Container.Cast( m_Parent ) ).Refresh();
								new_showed_items.Insert( entity, plyr_cnt );
								showed_items_IDs.Insert( entity.GetID(), plyr_cnt);
							}
							else
							{
								ContainerWithCargoAndAttachments iwcas = new ContainerWithCargoAndAttachments( this, -1 );
								iwcas.SetEntity( entity, false );
								new_showed_items.Insert( entity, iwcas );
								showed_items_IDs.Insert( entity.GetID(), iwcas );
								iwcas.UpdateInterval();
							}
						}
					}
				}
				else
				{
					//TODO MW: remove old containers on one spot (hotfix)
					con = m_ShowedItems.Get( entity );
					if ( con )
					{
						if ( entity.IsInherited( PlayerBase ) )
						{
							//! Allow view access of inventory if in free camera. Write access is elsewhere
							if ( !accessInventory && entity.IsAlive() && ( !PlayerBase.Cast( entity ).IsUnconscious() && !PlayerBase.Cast( entity ).IsRestrained() ) )
							{
								GetMainWidget().Update();
								if ( con.IsActive() )
									need_update_focus = true;
								Container.Cast( GetParent() ).Remove( con );
								Remove( con );
								continue;
							}
						}
						
						if ( entity.IsInherited( BaseBuildingBase ) )
						{	
							if ( !BaseBuildingBase.Cast(entity).IsPlayerInside(player,""))
							{
								GetMainWidget().Update();
								if ( con.IsActive() )
									need_update_focus = true;
								Container.Cast( GetParent() ).Remove( con );
								Remove( con );
								continue;
							}
						}
						
						new_showed_items.Insert( entity, m_ShowedItems.Get( entity ) );
						showed_items_IDs.Insert( entity.GetID(), m_ShowedItemsIDs.Get( entity.GetID() ) );
					}
				}
			}
		}
		
		map<CargoBase, ref Container> new_showed_cargos = new map<CargoBase, ref Container>;
		auto cargoes = VicinityItemManager.GetInstance().GetVicinityCargos();
		for ( i = 0; i < cargoes.Count(); i++ )
		{
			CargoBase cgo = cargoes.Get( i );
			if ( cgo )
			{
				if (game_inventory && !m_ShowedCargos.Contains( cgo ) )
				{
					ContainerWithCargo pxc = new ContainerWithCargo( this, -1 );
					pxc.SetEntity( cgo.GetCargoOwner(), cgo.GetOwnerCargoIndex(), false );
					new_showed_cargos.Insert( cgo, pxc );
					pxc.UpdateInterval();
				}
				else
				{
					if ( m_ShowedCargos.Get( cgo ) )
					{
						new_showed_cargos.Insert( cgo, m_ShowedCargos.Get( cgo ) );
					}
				}
			}
		}
		
		for ( int ic = 0; ic < m_ShowedCargos.Count(); ic++ )
		{
			CargoBase cgo2 = m_ShowedCargos.GetKey( ic );
			m_ShowedCargos.GetElement( ic ).UpdateInterval();
			if ( !new_showed_cargos.Contains( cgo2 ) )
			{
				Container con2 = m_ShowedCargos.GetElement( ic );
				if( con2.IsActive() )
					need_update_focus = true;
				GetMainWidget().Update();
				Container.Cast( GetParent() ).Remove( con2 );
				Remove( con2 );
			}
		}
		
		m_ShowedItems = new_showed_items;
		m_ShowedItemsIDs = showed_items_IDs;
		m_ShowedCargos = new_showed_cargos;
		//Because WTF (zombie container)
		Container.Cast( GetParent() ).RecomputeOpenedContainers();
		if (need_update_focus)
			SetFirstActive();
		//RecomputeOpenedContainers();
		UpdateCollapseButtons();
		m_VicinityIconsContainer.ShowItemsInContainers( m_ShowedItemIcons );
		
		if ( m_ShowedItemIcons.Count() != m_OldShowedItemIconsCount )
		{
			Inventory in = Inventory.Cast( GetRoot() );
			if ( in )
				in.UpdateConsoleToolbar();	
		}
	}
	
	void ToggleContainer( Widget w, EntityAI item )
	{
		SlotsIcon slots_icon;
		Container c;
		w.GetUserData(slots_icon);
				
		if ( item )
		{
			c = Container.Cast( m_ShowedItems.Get( item ) );
		}
		
		if (c)
		{
			ToggleContainer(c);
		}
	}
	
	override bool CanOpenCloseContainerEx(EntityAI focusedEntity)
	{
		if (focusedEntity)
		{
			ClosableContainer c = ClosableContainer.Cast( m_ShowedItems.Get( focusedEntity ) );
			if (c && c.IsDisplayable())
			{	
				return true;
			}
		}
		return false;
	}

	//Call from ExpandCollapseContainer - not call
	void ToggleContainer( notnull Container conta )
	{
		conta.Toggle();
	}
	
	override void ExpandCollapseContainer()
	{
		EntityAI item = GetFocusedItem();
		Container conta;
			
		if (m_ShowedItems && item)
		{
			conta = m_ShowedItems.Get( item );
			if (conta)
			{		
				ToggleContainer( conta );
			}
		}
	}
	
	override bool OnChildRemove( Widget w, Widget child )
	{
		w.Update();
		float x, y;
		w.GetScreenSize( x, y );
		if ( w == GetMainWidget() )
		{
			GetMainWidget().Update();
			m_Parent.OnChildRemove( w, child );
		}
		return true;
	}
	
	override bool OnChildAdd( Widget w, Widget child )
	{
		if ( w == GetMainWidget() )
		{
			GetMainWidget().Update();
			m_Parent.OnChildAdd( w, child );
		}
		return true;
	}

	override void CollapseButtonOnMouseButtonDown(Widget w)
	{
		super.CollapseButtonOnMouseButtonDown(w);
		ItemManager.GetInstance().SetDefaultHeaderOpenState( "VICINITY", !m_Hidden );
	}
	
	//! Updates header dragability to be consistent with current 'icon' behaviour
	void UpdateHeader(EntityAI entity, Container cont, PlayerBase player)
	{
		bool draggable = ItemManager.GetInstance().EvaluateContainerDragabilityDefault(entity);
		
		if (cont.GetHeader())
		{
			if (GetDragWidget() == cont.GetHeader())
			{
				CancelWidgetDragging();
			}
			ItemManager.GetInstance().SetWidgetDraggable(cont.GetHeader().GetMainWidget(),draggable);
		}
	}
}
