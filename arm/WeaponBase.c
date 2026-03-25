modded class Weapon_Base
{
    ref map<string, string> m_DropList = new map<string, string>();

    void Weapon_Base()
    {
        m_DropList["Bullet_9x19"] = "9mm_shell";
        m_DropList["Bullet_762x39"] = "762x39_shell";
        m_DropList["Bullet_357"] = "357_shell";
        m_DropList["Bullet_22"] = "22mm_shell";
        m_DropList["Bullet_308Win"] = "308win_shell";
        m_DropList["Bullet_12GaugePellets"] = "12_shell";
        m_DropList["Bullet_45ACP"] = "45ACP_shell";
        m_DropList["Bullet_380"] = "380acp_shell";
        m_DropList["Bullet_762x54"] = "762x54_shell";
        m_DropList["Bullet_545x39"] = "545x39_shell";
        m_DropList["Bullet_556x45"] = "556x45_shell";
    }

    override void EEFired(int muzzleType, int mode, string ammoType)
    {
        super.EEFired(muzzleType, mode, ammoType);
        PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
		if ( player )
        {
            if (m_DropList.Get(ammoType))
            {
                if (Math.RandomFloat01() < 0.4)
                {
                    ItemBase shell = ItemBase.Cast(GetGame().CreateObjectEx(m_DropList.Get(ammoType), player.GetPosition(), ECE_PLACE_ON_SURFACE));
                    if (shell)
                    {
                        shell.SetQuantity(1);
                    }
                }
            }
        }
    }
}