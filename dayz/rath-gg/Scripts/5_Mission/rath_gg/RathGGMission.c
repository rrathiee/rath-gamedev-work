// ============================================================================
// rath-gg
// Client-side quality-of-life gear condition monitor for DayZ.
//
// What it does:
// - Checks equipped gear on a timed interval.
// - Warns the player when an equipped item becomes Badly Damaged or Ruined.
// - Tracks the last observed item + state per slot so it does not spam the same
//   warning every time the check runs.
// - Does not repair or modify clothing.
// ============================================================================

class RathGGSettings
{
    static const int CHECK_INTERVAL_MS = 30000; 

    static const int WARNING_LEVEL = GameConstants.STATE_BADLY_DAMAGED;

    static const float NOTIFICATION_TIME = 6.0;
};

modded class MissionGameplay
{
    protected ref map<string, string> m_RathGG_LastSlotStates;

    override void OnInit()
    {
        super.OnInit();

        m_RathGG_LastSlotStates = new map<string, string>;
        
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(
            this.RathGG_CheckGear,
            RathGGSettings.CHECK_INTERVAL_MS,
            true
        );

        Print("[rath-gg] Initialized.");
    }

    override void OnMissionFinish()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(this.RathGG_CheckGear);

        if (m_RathGG_LastSlotStates)
        {
            m_RathGG_LastSlotStates.Clear();
            m_RathGG_LastSlotStates = NULL;
        }

        Print("[rath-gg] Shut down.");

        super.OnMissionFinish();
    }


    void RathGG_CheckGear()
    {
        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (!player || !player.IsAlive())
            return;

        TStringArray slots =
        {
            "Headgear",
            "Mask",
            "Eyewear",
            "Body",
            "Vest",
            "Back",
            "Hips",
            "Legs",
            "Feet",
            "Gloves"
        };

        foreach (string slotName : slots)
        {
            EntityAI entity = player.FindAttachmentBySlotName(slotName);
            RathGG_EvaluateItem(slotName, ItemBase.Cast(entity));
        }

        EntityAI handsEntity = player.GetHumanInventory().GetEntityInHands();
        RathGG_EvaluateItem("Hands", ItemBase.Cast(handsEntity));
    }

    protected void RathGG_EvaluateItem(string slotName, ItemBase item)
    {
        if (!m_RathGG_LastSlotStates)
            return;

        if (!item)
        {
            m_RathGG_LastSlotStates.Remove(slotName);
            return;
        }

        int healthLevel = item.GetHealthLevel();
        string signature = item.GetType() + ":" + healthLevel.ToString();

        string previousSignature;
        bool hadPrevious = m_RathGG_LastSlotStates.Find(slotName, previousSignature);

        if (hadPrevious)
            m_RathGG_LastSlotStates.Set(slotName, signature);
        else
            m_RathGG_LastSlotStates.Insert(slotName, signature);

        if (healthLevel < RathGGSettings.WARNING_LEVEL)
            return;

        if (hadPrevious && previousSignature == signature)
            return;

        string stateText = RathGG_GetStateText(healthLevel);
        string itemName = item.GetDisplayName();

        if (itemName == "")
            itemName = item.GetType();

        NotificationSystem.AddNotificationExtended(
            RathGGSettings.NOTIFICATION_TIME,
            "rath-gg",
            "You notice that your " + itemName + " has become " + stateText + "."
        );
    }

    protected string RathGG_GetStateText(int healthLevel)
    {
        switch (healthLevel)
        {
            case GameConstants.STATE_RUINED:
                return "RUINED";

            case GameConstants.STATE_BADLY_DAMAGED:
                return "badly damaged";

            case GameConstants.STATE_DAMAGED:
                return "damaged";

            case GameConstants.STATE_WORN:
                return "worn";
        }

        return "pristine";
    }
};
