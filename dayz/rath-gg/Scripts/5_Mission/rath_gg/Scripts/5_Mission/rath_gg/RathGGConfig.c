// ============================================================================
// rath-gg - JSON configuration
//
// Runtime config location:
//     $profile:rath-gg/config.json
//
// If config.json does not exist, rath-gg creates one with defaults.
// ============================================================================

class RathGGConfig
{
    bool Enabled = true;

    int CheckIntervalMS = 30000;

    float NotificationTime = 6.0;
    string NotificationTitle = "rath-gg";

    bool MonitorClothing = true;
    bool MonitorHands = true;

    bool WarnOnBadlyDamaged = true;
    bool WarnOnRuined = true;

    // If false, logging in with already-damaged gear will only establish the
    // baseline and will not immediately notify the player.
    bool NotifyOnFirstScan = false;

    ref array<string> MonitoredSlots;

    void RathGGConfig()
    {
        MonitoredSlots = new array<string>;

        MonitoredSlots.Insert("Headgear");
        MonitoredSlots.Insert("Mask");
        MonitoredSlots.Insert("Eyewear");
        MonitoredSlots.Insert("Body");
        MonitoredSlots.Insert("Vest");
        MonitoredSlots.Insert("Back");
        MonitoredSlots.Insert("Hips");
        MonitoredSlots.Insert("Legs");
        MonitoredSlots.Insert("Feet");
        MonitoredSlots.Insert("Gloves");
    }

    void Validate()
    {
        if (CheckIntervalMS < 5000)
        {
            Print("[rath-gg] CheckIntervalMS was below 5000. Resetting to 30000.");
            CheckIntervalMS = 30000;
        }

        if (NotificationTime < 1.0)
            NotificationTime = 1.0;

        if (NotificationTime > 30.0)
            NotificationTime = 30.0;

        if (NotificationTitle == "")
            NotificationTitle = "rath-gg";

        if (!MonitoredSlots)
            MonitoredSlots = new array<string>;
    }
};

class RathGGConfigManager
{
    static const string CONFIG_DIR = "$profile:rath-gg";
    static const string CONFIG_PATH = "$profile:rath-gg/config.json";

    static RathGGConfig Load()
    {
        RathGGConfig config = new RathGGConfig();

        if (!FileExist(CONFIG_DIR))
            MakeDirectory(CONFIG_DIR);

        if (!FileExist(CONFIG_PATH))
        {
            Save(config);
            Print("[rath-gg] No config found. Created default config.");
            return config;
        }

        string errorMessage;
        if (!JsonFileLoader<RathGGConfig>.LoadFile(CONFIG_PATH, config, errorMessage))
        {
            Print("[rath-gg] Config load failed: " + errorMessage);
            Print("[rath-gg] Falling back to defaults.");
            config = new RathGGConfig();
            Save(config);
            return config;
        }

        config.Validate();

        // Save after validation so corrected values are written back to disk.
        Save(config);

        Print("[rath-gg] Config loaded.");
        return config;
    }

    static void Save(RathGGConfig config)
    {
        if (!FileExist(CONFIG_DIR))
            MakeDirectory(CONFIG_DIR);

        string errorMessage;
        if (!JsonFileLoader<RathGGConfig>.SaveFile(CONFIG_PATH, config, errorMessage))
            Print("[rath-gg] Config save failed: " + errorMessage);
    }
};
