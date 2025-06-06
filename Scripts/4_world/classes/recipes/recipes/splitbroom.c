class SplitBroom extends RecipeBase
{	
	override void Init()
	{
		m_Name = "#STR_split0";
		m_IsInstaRecipe = false;//should this recipe be performed instantly without animation
		m_AnimationLength = 1;//animation length in relative time units
		m_Specialty = 0.01;// value > 0 for roughness, value < 0 for precision
		
		
		//conditions
		m_MinDamageIngredient[0] = -1;//-1 = disable check
		m_MaxDamageIngredient[0] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[0] = -1;//-1 = disable check
		m_MaxQuantityIngredient[0] = -1;//-1 = disable check
		
		m_MinDamageIngredient[1] = -1;//-1 = disable check
		m_MaxDamageIngredient[1] = 3;//-1 = disable check
		
		m_MinQuantityIngredient[1] = -1;//-1 = disable check
		m_MaxQuantityIngredient[1] = -1;//-1 = disable check
		//----------------------------------------------------------------------------------------------------------------------
		
		//INGREDIENTS
		//ingredient 1
		InsertIngredient(0,"Broom");//you can insert multiple ingredients this way
		InsertIngredient(0,"Broom_Birch");//you can insert multiple ingredients this way
		
		m_IngredientAddHealth[0] = 0;// 0 = do nothing
		m_IngredientSetHealth[0] = -1; // -1 = do nothing
		m_IngredientAddQuantity[0] = 0;// 0 = do nothing
		m_IngredientDestroy[0] = true;//true = destroy, false = do nothing
		m_IngredientUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//ingredient 2
		InsertIngredient(1,"Sickle", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);//you can insert multiple ingredients this way
		InsertIngredient(1,"KukriKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"FangeKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Hacksaw", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"HandSaw", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"KitchenKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"SteakKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"HayHook", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"StoneKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Cleaver", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"CombatKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"HuntingKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Machete", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"CrudeMachete", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"OrientalMachete", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Crowbar", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Pickaxe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"WoodAxe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Hatchet", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"FirefighterAxe", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Sword", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"AK_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"M9A1_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"Mosin_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"SKS_Bayonet", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);
		InsertIngredient(1,"BoneKnife", DayZPlayerConstants.CMD_ACTIONFB_SPLITTING_FIREWOOD, true);	
		
		m_IngredientAddHealth[1] = -4;// 0 = do nothing
		m_IngredientSetHealth[1] = -1; // -1 = do nothing
		m_IngredientAddQuantity[1] = 0;// 0 = do nothing
		m_IngredientDestroy[1] = false;// false = do nothing
		m_IngredientUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this ingredient
		
		//----------------------------------------------------------------------------------------------------------------------
		
		//result1
		AddResult("WoodenStick");//add results here

		m_ResultSetFullQuantity[0] = false;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[0] = 3;//-1 = do nothing
		m_ResultSetHealth[0] = -1;//-1 = do nothing
		m_ResultInheritsHealth[0] = 0;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[0] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[0] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[0] = false;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[0] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		
		
		//result2
		AddResult("LongWoodenStick");//add results here
		
		m_ResultSetFullQuantity[1] = true;//true = set full quantity, false = do nothing
		m_ResultSetQuantity[1] = -1;//-1 = do nothing
		m_ResultSetHealth[1] = -1;//-1 = do nothing
		m_ResultInheritsHealth[1] = 0;// (value) == -1 means do nothing; a (value) >= 0 means this result will inherit health from ingredient number (value);(value) == -2 means this result will inherit health from all ingredients averaged(result_health = combined_health_of_ingredients / number_of_ingredients)
		m_ResultInheritsColor[1] = -1;// (value) == -1 means do nothing; a (value) >= 0 means this result classname will be a composite of the name provided in AddResult method and config value "color" of ingredient (value)
		m_ResultToInventory[1] = -2;//(value) == -2 spawn result on the ground;(value) == -1 place anywhere in the players inventory, (value) >= 0 means switch position with ingredient number(value)
		m_ResultUseSoftSkills[1] = true;// set 'true' to allow modification of the values by softskills on this result
		m_ResultReplacesIngredient[1] = -1;// value == -1 means do nothing; a value >= 0 means this result will transfer item propertiesvariables, attachments etc.. from an ingredient value
		//----------------------------------------------------------------------------------------------------------------------
	}

	override bool CanDo(ItemBase ingredients[], PlayerBase player)//final check for recipe's validity
	{
		ItemBase broom = ingredients[0];
		
		if (broom.GetCompEM().IsWorking())
		{
			return false;
		}
		
		if ( broom.IsEmpty() )
		{
			return true;
		}
		return false;
	}

	override void Do(ItemBase ingredients[], PlayerBase player,array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
	{
		ItemBase broom = ingredients[0];
	
		for (int i=0; i < results.Count(); i++)
		{
			ItemBase item_result;
			Class.CastTo(item_result, results.Get(i));
			
			MiscGameplayFunctions.TransferItemProperties(broom, item_result);
		}
	}
};
