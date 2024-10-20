#pragma once

#include "OS-ImGui/OS-ImGui.h"

class Radar
{
public:
	// 设置雷达数据
	void SetSize(const float& Size);
	void SetPos(const Vec2& Pos);
	void SetRange(const float& Range);
	void SetCrossColor(const ImColor& Color);
	void SetProportion(const float& Proportion);
	// 获取雷达数据
	float GetSize();
	Vec2 GetPos();
	// 添加绘制点
	void AddPoint(const Vec3& LocalPos,const float& LocalYaw, const Vec3& Pos, ImColor Color, int Type = 0, float Yaw = 0.0f);
	// 渲染
	void Render();
public:
	// 十字显示
	bool ShowCrossLine = true;
	// 十字颜色
	ImColor CrossColor = ImColor(255, 255, 255, 255);
	// 比例
	float Proportion = 2680;
	// 圆点半径
	float PointRadius = 3;
	// 圆弧箭头尺寸
	float ArcArrowSize = 3;
	// 雷达范围
	float RenderRange = 250;
	// 本地Yaw数据
	float LocalYaw = 0.0f;
	// 状态
	bool Opened = true;
	// 雷达绘制类型 0:圆形 1:圆弧箭头
	int  PointType = 0;
private:
	Vec2  Pos{ 0,0 };
	float Width = 200;
	std::vector<std::tuple<Vec2, ImColor, int, float>> Points;
};