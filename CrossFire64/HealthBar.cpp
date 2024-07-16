#include "HealthBar.h"

void HealthBar::DrawHealthBar_Horizontal(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size)
{
	auto InRange = [&](float value, float min, float max) -> bool
		{
			return value > min && value <= max;
		};

	ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

	this->MaxHealth = MaxHealth;
	this->CurrentHealth = CurrentHealth;
	this->RectPos = Pos;
	this->RectSize = Size;

	// 占比
	float Proportion = CurrentHealth / MaxHealth;
	// 血量条宽度
	float Width = RectSize.x * Proportion;
	// 血量条颜色
	ImColor Color;

	// 背景
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		BackGroundColor, 5, 15);

	// 颜色切换
	float Color_Lerp_t = pow(Proportion, 2.5);
	if (InRange(Proportion, 0.5, 1))
		Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
	else
		Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

	// 更新最近备份血量
	if (LastestBackupHealth == 0
		|| LastestBackupHealth < CurrentHealth)
		LastestBackupHealth = CurrentHealth;

	if (LastestBackupHealth != CurrentHealth)
	{
		if (!InShowBackupHealth)
		{
			BackupHealthTimePoint = std::chrono::steady_clock::now();
			InShowBackupHealth = true;
		}

		std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
		if (CurrentTime - BackupHealthTimePoint > std::chrono::milliseconds(ShowBackUpHealthDuration))
		{
			// 超时就停止显示备份血量，并且更新最近备份血量
			LastestBackupHealth = CurrentHealth;
			InShowBackupHealth = false;
		}

		if (InShowBackupHealth)
		{
			// 备份血量绘制宽度
			float BackupHealthWidth = LastestBackupHealth / MaxHealth * RectSize.x;
			// 备份血量alpha渐变
			float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			ImColor BackupHealthColorTemp = BackupHealthColor;
			BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
			// 备份血量宽度缓动
			float BackupHealthWidth_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			BackupHealthWidth_Lerp *= (BackupHealthWidth - Width);
			BackupHealthWidth -= BackupHealthWidth_Lerp;
			// 备份血条
			DrawList->AddRectFilled(RectPos,
				{ RectPos.x + BackupHealthWidth,RectPos.y + RectSize.y },
				BackupHealthColorTemp, 5);
		}
	}

	// 血条
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + Width,RectPos.y + RectSize.y },
		Color, 5);

	// 边框
	DrawList->AddRect(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		FrameColor, 5, 15, 1);
}

void HealthBar::DrawHealthBar_Vertical(float MaxHealth, float CurrentHealth, ImVec2 Pos, ImVec2 Size)
{
	auto InRange = [&](float value, float min, float max) -> bool
		{
			return value > min && value <= max;
		};

	ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

	this->MaxHealth = MaxHealth;
	this->CurrentHealth = CurrentHealth;
	this->RectPos = Pos;
	this->RectSize = Size;

	// 占比
	float Proportion = CurrentHealth / MaxHealth;
	// 血量条高度
	float Height = RectSize.y * Proportion;
	// 血量条颜色
	ImColor Color;

	// 背景
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		BackGroundColor, 5, 15);

	// 颜色切换
	float Color_Lerp_t = pow(Proportion, 2.5);
	if (InRange(Proportion, 0.5, 1))
		Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
	else
		Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

	// 更新最近备份血量
	if (LastestBackupHealth == 0
		|| LastestBackupHealth < CurrentHealth)
		LastestBackupHealth = CurrentHealth;

	if (LastestBackupHealth != CurrentHealth)
	{
		if (!InShowBackupHealth)
		{
			BackupHealthTimePoint = std::chrono::steady_clock::now();
			InShowBackupHealth = true;
		}

		std::chrono::steady_clock::time_point CurrentTime = std::chrono::steady_clock::now();
		if (CurrentTime - BackupHealthTimePoint > std::chrono::milliseconds(ShowBackUpHealthDuration))
		{
			// 超时就停止显示备份血量，并且更新最近备份血量
			LastestBackupHealth = CurrentHealth;
			InShowBackupHealth = false;
		}

		if (InShowBackupHealth)
		{
			// 备份血量绘制高度
			float BackupHealthHeight = LastestBackupHealth / MaxHealth * RectSize.y;
			// 备份血量alpha渐变
			float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			ImColor BackupHealthColorTemp = BackupHealthColor;
			BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
			// 备份血量高度缓动
			float BackupHealthHeight_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			BackupHealthHeight_Lerp *= (BackupHealthHeight - Height);
			BackupHealthHeight -= BackupHealthHeight_Lerp;
			// 备份血条
			DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - BackupHealthHeight },
				{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
				BackupHealthColorTemp, 5);
		}
	}

	// 血条
	DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - Height },
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		Color, 5);

	// 边框
	DrawList->AddRect(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		FrameColor, 5, 15, 1);
}

ImColor HealthBar::Mix(ImColor Col_1, ImColor Col_2, float t)
{
	ImColor Col;
	Col.Value.x = t * Col_1.Value.x + (1 - t) * Col_2.Value.x;
	Col.Value.y = t * Col_1.Value.y + (1 - t) * Col_2.Value.y;
	Col.Value.z = t * Col_1.Value.z + (1 - t) * Col_2.Value.z;
	Col.Value.w = Col_1.Value.w;
	return Col;
}