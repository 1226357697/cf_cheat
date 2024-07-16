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

	// ռ��
	float Proportion = CurrentHealth / MaxHealth;
	// Ѫ�������
	float Width = RectSize.x * Proportion;
	// Ѫ������ɫ
	ImColor Color;

	// ����
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		BackGroundColor, 5, 15);

	// ��ɫ�л�
	float Color_Lerp_t = pow(Proportion, 2.5);
	if (InRange(Proportion, 0.5, 1))
		Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
	else
		Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

	// �����������Ѫ��
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
			// ��ʱ��ֹͣ��ʾ����Ѫ�������Ҹ����������Ѫ��
			LastestBackupHealth = CurrentHealth;
			InShowBackupHealth = false;
		}

		if (InShowBackupHealth)
		{
			// ����Ѫ�����ƿ��
			float BackupHealthWidth = LastestBackupHealth / MaxHealth * RectSize.x;
			// ����Ѫ��alpha����
			float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			ImColor BackupHealthColorTemp = BackupHealthColor;
			BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
			// ����Ѫ����Ȼ���
			float BackupHealthWidth_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			BackupHealthWidth_Lerp *= (BackupHealthWidth - Width);
			BackupHealthWidth -= BackupHealthWidth_Lerp;
			// ����Ѫ��
			DrawList->AddRectFilled(RectPos,
				{ RectPos.x + BackupHealthWidth,RectPos.y + RectSize.y },
				BackupHealthColorTemp, 5);
		}
	}

	// Ѫ��
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + Width,RectPos.y + RectSize.y },
		Color, 5);

	// �߿�
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

	// ռ��
	float Proportion = CurrentHealth / MaxHealth;
	// Ѫ�����߶�
	float Height = RectSize.y * Proportion;
	// Ѫ������ɫ
	ImColor Color;

	// ����
	DrawList->AddRectFilled(RectPos,
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		BackGroundColor, 5, 15);

	// ��ɫ�л�
	float Color_Lerp_t = pow(Proportion, 2.5);
	if (InRange(Proportion, 0.5, 1))
		Color = Mix(FirstStageColor, SecondStageColor, Color_Lerp_t * 3 - 1);
	else
		Color = Mix(SecondStageColor, ThirdStageColor, Color_Lerp_t * 4);

	// �����������Ѫ��
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
			// ��ʱ��ֹͣ��ʾ����Ѫ�������Ҹ����������Ѫ��
			LastestBackupHealth = CurrentHealth;
			InShowBackupHealth = false;
		}

		if (InShowBackupHealth)
		{
			// ����Ѫ�����Ƹ߶�
			float BackupHealthHeight = LastestBackupHealth / MaxHealth * RectSize.y;
			// ����Ѫ��alpha����
			float BackupHealthColorAlpha = 1 - 0.95 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			ImColor BackupHealthColorTemp = BackupHealthColor;
			BackupHealthColorTemp.Value.w = BackupHealthColorAlpha;
			// ����Ѫ���߶Ȼ���
			float BackupHealthHeight_Lerp = 1 * (std::chrono::duration_cast<std::chrono::milliseconds>(CurrentTime - BackupHealthTimePoint).count() / (float)ShowBackUpHealthDuration);
			BackupHealthHeight_Lerp *= (BackupHealthHeight - Height);
			BackupHealthHeight -= BackupHealthHeight_Lerp;
			// ����Ѫ��
			DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - BackupHealthHeight },
				{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
				BackupHealthColorTemp, 5);
		}
	}

	// Ѫ��
	DrawList->AddRectFilled({ RectPos.x,RectPos.y + RectSize.y - Height },
		{ RectPos.x + RectSize.x,RectPos.y + RectSize.y },
		Color, 5);

	// �߿�
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