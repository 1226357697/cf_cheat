#include "Radar.h"

Vec2 RevolveCoordinatesSystem(float RevolveAngle, Vec2 OriginPos, Vec2 DestPos)
{
	Vec2 ResultPos;
	if (RevolveAngle == 0)
		return DestPos;
	ResultPos.x = OriginPos.x + (DestPos.x - OriginPos.x) * cos(RevolveAngle) + (DestPos.y - OriginPos.y) * sin(RevolveAngle);
	ResultPos.y = OriginPos.y - (DestPos.x - OriginPos.x) * sin(RevolveAngle) + (DestPos.y - OriginPos.y) * cos(RevolveAngle);
	return ResultPos;
}


void Radar::SetRange(const float& Range)
{
	this->RenderRange = Range;
}

void Radar::SetCrossColor(const ImColor& Color)
{
	this->CrossColor = Color;
}

void Radar::SetPos(const Vec2& Pos)
{
	this->Pos = Pos;
}

void Radar::SetSize(const float& Size)
{
	this->Width = Size;
}

float Radar::GetSize()
{
	return this->Width;
}

Vec2 Radar::GetPos()
{
	return this->Pos;
}

void Radar::SetProportion(const float& Proportion)
{
	this->Proportion = Proportion;
}

void Radar::AddPoint(const Vec3& LocalPos, const float& LocalYaw, const Vec3& Pos, ImColor Color, int Type, float Yaw)
{
	Vec2 PointPos;
	float Distance;
	float Angle;

	this->LocalYaw = LocalYaw;

	Angle = atan2f(LocalPos.z - Pos.z, LocalPos.x - Pos.x);
	Angle = this->LocalYaw - Angle;

	Distance = sqrt(pow(LocalPos.x - Pos.x, 2) + pow(LocalPos.z - Pos.z, 2));
	Distance/= this->Proportion;
	Distance = min(Distance, this->RenderRange);


	PointPos.x = this->Pos.x - Distance * sin(Angle);
	PointPos.y = this->Pos.y + Distance * cos(Angle);

	std::tuple<Vec2, ImColor, int, float> Data(PointPos, Color, Type, Yaw);
	this->Points.push_back(Data);
}

void DrawTriangle(Vec2 Center, ImColor Color, float Width, float Height, float Yaw)
{
	Vec2 a, b, c;
	Vec2 Re_a, Re_b, Re_c;
	a = Vec2{ Center.x - Width / 2,Center.y };
	b = Vec2{ Center.x + Width / 2,Center.y };
	c = Vec2{ Center.x,Center.y - Height };
	a = RevolveCoordinatesSystem(-Yaw, Center, a);
	b = RevolveCoordinatesSystem(-Yaw, Center, b);
	c = RevolveCoordinatesSystem(-Yaw, Center, c);
	ImGui::GetForegroundDrawList()->AddTriangleFilled(
		ImVec2(a.x, a.y),
		ImVec2(b.x, b.y),
		ImVec2(c.x, c.y),
		Color);
}

void Radar::Render()
{
	if (Width <= 0)
		return;

	// Cross
	std::pair<Vec2, Vec2> Line1;
	std::pair<Vec2, Vec2> Line2;

	Line1.first = Vec2(this->Pos.x - this->Width / 2, this->Pos.y);
	Line1.second = Vec2(this->Pos.x + this->Width / 2, this->Pos.y);
	Line2.first = Vec2(this->Pos.x, this->Pos.y - this->Width / 2);
	Line2.second = Vec2(this->Pos.x, this->Pos.y + this->Width / 2);

	if (this->Opened)
	{
		if (this->ShowCrossLine)
		{
			Gui.Line(Line1.first, Line1.second, this->CrossColor, 1);
			Gui.Line(Line2.first, Line2.second, this->CrossColor, 1);
		}

		for (auto PointSingle : this->Points)
		{
			Vec2	PointPos = std::get<0>(PointSingle);
			ImColor PointColor = std::get<1>(PointSingle);
			int		PointType = std::get<2>(PointSingle);
			float	PointYaw = std::get<3>(PointSingle);
			if (PointType == 0)
			{
				// 圆形样式
				Gui.CircleFilled(PointPos, this->PointRadius, PointColor);
				Gui.Circle(PointPos, this->PointRadius, ImColor(0, 0, 0), 1);
			}
			else if (PointType == 1)
			{
				// 圆弧箭头
				Vec2 TrianglePoint;
				float Angle = (this->LocalYaw - PointYaw) - 90;
				Gui.Arc(PointPos.ToImVec2(), this->ArcArrowSize, ImColor(220, 220, 220, 170), 0.05,
					(Angle - 0.2 * M_PI), (Angle + 0.2 * M_PI));

				Gui.Arc(PointPos.ToImVec2(), this->ArcArrowSize, ImColor(220, 220, 220, 200), 1.5,
					(Angle - 0.125 * M_PI), (Angle + 0.125 * M_PI));

				Gui.CircleFilled(PointPos, 0.85 * this->ArcArrowSize, PointColor, 30);
				Gui.Circle(PointPos, 0.95 * this->ArcArrowSize, ImColor(0, 0, 0, 150), 0.1);
				TrianglePoint.x = PointPos.x + this->ArcArrowSize * cos(-Angle);
				TrianglePoint.y = PointPos.y - this->ArcArrowSize * sin(-Angle);
				DrawTriangle(TrianglePoint, ImColor(255, 255, 255, 220), 0.7 * this->ArcArrowSize, 0.5 * this->ArcArrowSize, Angle + 90);
			}
		}
	}
	if (this->Points.size() > 0)
		this->Points.clear();
}