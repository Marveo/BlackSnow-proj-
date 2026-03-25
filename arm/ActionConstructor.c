modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        
        actions.Insert(ActionCraftAmmo);
        actions.Insert(ActionPour);
        actions.Insert(ActionPourBullets_9x19);
    }
}
