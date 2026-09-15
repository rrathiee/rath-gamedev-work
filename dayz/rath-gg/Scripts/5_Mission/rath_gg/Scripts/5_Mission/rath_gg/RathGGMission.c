// ============================================================================
// rath-gg
// Configurable client-side quality-of-life gear condition monitor for DayZ.
// ============================================================================

modded class MissionGameplay
{
    protected ref map<string, string> m_RathGG_LastSlotStates;
    protected ref RathGGConfig m_RathGG_Config;

    override void OnInit()
    {
        super.OnInit();

        m_RathGG_LastSlotStates = new map<string, string>;
        m_RathGG_Config = RathGGConfigManager.Load();

        if (!m_RathGG_Config || !m_RathGG_Config.Enabled)
        {
            Print("[rath-gg] Disabled by config.");
            return;
        }

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(
            this.RathGG_CheckGear,
            m_RathGG_Config.CheckIntervalMS,
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

        m_RathGG_Config = NULL;

        Print("[rath-gg] Shut down.");
        super.OnMissionFinish();
    }

    void RathGG_CheckGear()
    {
        if (!m_RathGG_Config || !m_RathGG_Config.Enabled)
            return;

        PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

        if (!player || !player.IsAlive())
            return;

        if (m_RathGG_Config.MonitorClothing && m_RathGG_Config.MonitoredSlots)
        {
            foreach (string slotName : m_RathGG_Config.MonitoredSlots)
            {
                EntityAI entity = player.FindAttachmentBySlotName(slotName);
                RathGG_EvaluateItem(slotName, ItemBase.Cast(entity));
            }
        }

        if (m_RathGG_Config.MonitorHands)
        {
            EntityAI handsEntity = player.GetHumanInventory().GetEntityInHands();
            RathGG_EvaluateItem("Hands", ItemBase.Cast(handsEntity));
        }
        else
        {
            m_RathGG_LastSlotStates.Remove("Hands");
        }
    }

    protected void RathGG_EvaluateItem(string slotName, ItemBase item)
    {
        if (!m_RathGG_Config || !m_RathGG_LastSlotStates)
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
        
        if (!hadPrevious && !m_RathGG_Config.NotifyOnFirstScan)
            return;

        if (hadPrevious && previousSignature == signature)
            return;

        if (!RathGG_ShouldWarn(healthLevel))
            return;

        string stateText = RathGG_GetStateText(healthLevel);
        string itemName = item.GetDisplayName();

        if (itemName == "")
            itemName = item.GetType();

        NotificationSystem.AddNotificationExtended(
            m_RathGG_Config.NotificationTime,
            m_RathGG_Config.NotificationTitle,
            "You notice that your " + itemName + " has become " + stateText + "."
        );
    }

    protected bool RathGG_ShouldWarn(int healthLevel)
    {
        if (healthLevel == GameConstants.STATE_BADLY_DAMAGED)
            return m_RathGG_Config.WarnOnBadlyDamaged;

        if (healthLevel == GameConstants.STATE_RUINED)
            return m_RathGG_Config.WarnOnRuined;

        return false;
    }

    protected string RathGG_GetStateText(int healthLevel)
    {
        switch (healthLevel)
        {
            case GameConstants.STATE_RUINED:
                return "ruined";

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
