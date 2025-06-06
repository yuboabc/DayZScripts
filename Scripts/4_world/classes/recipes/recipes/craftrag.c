class CraftRag extends RecipeBase	
{	
	override void Init()
	{
		m_Name = "#STR_CraftRag0";
		m_IsInstaRecipe = false;						// should this recipe be performed instantly without animation
		m_AnimationLength = 1;							// animation length in relative time units

		//conditions
		m_MinDamageIngredient[0]		= -1;			// -1 = disable check
		m_MaxDamageIngredient[0]		= 3;			// -1 = disable check
		m_MinQuantityIngredient[0]		= -1;			// -1 = disable check
		m_MaxQuantityIngredient[0]		= -1;			// -1 = disable check
		
		m_MinDamageIngredient[1]		= -1;
		m_MaxDamageIngredient[1]		= 3;
		m_MinQuantityIngredient[1]		= -1;
		m_MaxQuantityIngredient[1]		= -1;

		//----------------------------------------------------------------------------------------------------------------------
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"Sickle");					// you can insert multiple ingredients this way
		InsertIngredient(0,"Hacksaw");
		InsertIngredient(0,"HandSaw");
		InsertIngredient(0,"KitchenKnife");
		InsertIngredient(0,"SteakKnife");
		InsertIngredient(0,"HayHook");
		InsertIngredient(0,"StoneKnife");
		InsertIngredient(0,"Cleaver");
		InsertIngredient(0,"CombatKnife");
		InsertIngredient(0,"HuntingKnife");
		InsertIngredient(0,"KukriKnife");
		InsertIngredient(0,"FangeKnife");
		InsertIngredient(0,"Machete");
		InsertIngredient(0,"CrudeMachete");
		InsertIngredient(0,"OrientalMachete");
		InsertIngredient(0,"Screwdriver");
		InsertIngredient(0,"Crowbar");
		InsertIngredient(0,"Pickaxe");
		InsertIngredient(0,"WoodAxe");
		InsertIngredient(0,"Hatchet");
		InsertIngredient(0,"FirefighterAxe");
		InsertIngredient(0,"Sword");
		InsertIngredient(0,"AK_Bayonet");
		InsertIngredient(0,"M9A1_Bayonet");
		InsertIngredient(0,"Mosin_Bayonet");
		InsertIngredient(0,"SKS_Bayonet");	
		InsertIngredient(0,"FarmingHoe");
		InsertIngredient(0,"Shovel");
		InsertIngredient(0,"FieldShovel");
		InsertIngredient(0,"BoneKnife");
		InsertIngredient(0,"Iceaxe");	
	
		m_IngredientAddHealth[0]			= -2;				// 0 = do nothing
		m_IngredientSetHealth[0]			= -1;				// -1 = do nothing
		m_IngredientAddQuantity[0]			= 0;				// 0 = do nothing
		m_IngredientDestroy[0]				= false;			// true = destroy, false = do nothing

		
		//ingredient 2
		InsertIngredientEx(1,"Inventory_Base","Rags_craft");	//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[1]			= 0;
		m_IngredientSetHealth[1]			= -1;
		m_IngredientAddQuantity[1]			= 0;
		m_IngredientDestroy[1]				= true;

		//----------------------------------------------------------------------------------------------------------------------
		//RESULTS
		//result1
		AddResult("Rag");//add results here

		m_ResultSetFullQuantity[0]			= false;			// true = set full quantity, false = do nothing
		m_ResultSetQuantity[0]				= -1;				// -1 = do nothing
		m_ResultSetHealth[0]				= -1;				// -1 = do nothing
		m_ResultInheritsHealth[0]			= 1;				// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0]			= -1;				// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0]				= -2;				// (value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultReplacesIngredient[0]		= -1;				// (value) == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
	}

	//final check for recipe's validity
	override bool CanDo(ItemBase ingredients[], PlayerBase player)
	{
		ItemBase ingredient2 = ingredients[1];
		if( ingredient2.ConfigGetFloat("ragQuantity") > 0 && ingredient2.IsEmpty())
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	//gets called upon recipe's completion
	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)
	{
		int quantity = ingredients[1].ConfigGetFloat("ragQuantity");
		results.Get(0).SetQuantity(quantity);
	}
};
