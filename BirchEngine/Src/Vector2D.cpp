#include "Vector2D.h"

Vector2D::Vector2D()
{
	x = 0.0f;
	y = 0.0f;
}

Vector2D::Vector2D(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector2D& Vector2D::Add(const Vector2D& vec)
{
	this->x += vec.x;
	this->y += vec.y;

	return *this;
}


Vector2D& Vector2D::Subtract(const Vector2D& vec)
{
	Vector2D subtract;
	subtract.x = this->x - vec.x;
	subtract.y = this->y - vec.y;

	return subtract;
}


Vector2D& Vector2D::Multiply(const Vector2D& vec)
{
	this->x *= vec.x;
	this->y *= vec.y;

	return *this;
}

Vector2D& Vector2D::Divide(const Vector2D& vec)
{
	this->x /= vec.x;
	this->y /= vec.y;

	return *this;
}


Vector2D&  operator+(Vector2D& v1, const Vector2D& v2)
{
	return v1.Add(v2);
}

Vector2D& operator-(Vector2D& v1, const Vector2D& v2)
{
	return v1.Subtract(v2);
}

Vector2D&  operator*(Vector2D& v1, const Vector2D& v2)
{
	return v1.Multiply(v2);
}

Vector2D& operator/(Vector2D& v1, const Vector2D& v2)
{
	return v1.Divide(v2);
}

Vector2D&  Vector2D::operator+=(const Vector2D& vec)
{
	return this->Add(vec);
}

Vector2D&  Vector2D::operator-=(const Vector2D& vec)
{
	return this->Subtract(vec);
}

Vector2D&  Vector2D::operator*=(const Vector2D& vec)
{
	return this->Multiply(vec);
}

Vector2D&  Vector2D::operator/=(const Vector2D& vec)
{
	return this->Divide(vec);
}
Vector2D& Vector2D::operator*(const int& i)
{
	this->x *= i;
	this->y *= i;

	return *this;

}

Vector2D Vector2D::operator*(const float& i)
{
	Vector2D mult;
	mult.x = this->x * i;
	mult.y = this->y * i;

	return mult;
}


Vector2D& Vector2D::Zero()
{
	this->x *= 0;
	this->y *= 0;

	return *this;
}
std::ostream& operator<<(std::ostream& stream, const Vector2D& vec)
{
	stream << "(" << vec.x << "," << vec.y << ")";
	return stream;
}
Vector2D & Vector2D::Normal() { 
	float x = this->x;
	this->x = this->y * -1;
	this->y = this->x;
	return *this; }
Vector2D & Vector2D::Normalize()
{
	if (magnitude() != 0)
	{
		float32 length = magnitude();
		x /= length; y /= length;
		return *this;
	}
}
 float Vector2D::magnitude() { 

	
	return sqrt(this->x *this->x + this->y * this->y);
}