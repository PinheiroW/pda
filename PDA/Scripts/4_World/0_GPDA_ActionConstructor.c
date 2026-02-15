// ═══════════════════════════════════════════════════════════════════════════════
// GRESHNIK PDA LITE - ActionConstructor для глобальной регистрации actions
// ═══════════════════════════════════════════════════════════════════════════════

modded class ActionConstructor
{
	override void RegisterActions(TTypenameArray actions)
	{
		super.RegisterActions(actions);
		actions.Insert(ActionOpenGPDA);
		actions.Insert(ActionOpenCaptureMenu);
	}
}
