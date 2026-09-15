class CfgPatches
{
    class RathGG
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class RathGG
    {
        dir = "rath-gg";
        name = "rath-gg";
        type = "mod";
        author = "Ry";
        version = "1.1.0";
        dependencies[] = {"Mission"};

        class defs
        {
            class missionScriptModule
            {
                value = "";
                files[] = {"rath-gg/Scripts/5_Mission"};
            };
        };
    };
};
