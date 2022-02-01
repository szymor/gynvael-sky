// .S.k.y by Gynvael Coldwind of Vexillium
// http://gynvael.coldwind.pl
// http://vexillium.org
// mailto: gynvael@vexillium.org
//
// Note:
//  This is just a simple vector lib
//
// License (BSD):
//  Copyright (c) 2008, Gynvael Coldwind of Vexillium
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//      * Neither the name of the Gynvael Coldwind nor Vexillium nor the
//        names of its contributors may be used to endorse or promote products
//        derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY Gynvael Coldwind ''AS IS'' AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL Gynvael Coldwind BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include <cstdio>
#include <cmath>
#include "Vector3D.h"

using namespace std;

Vector3D::Vector3D()
{
  this->x = this->y = this->z = 0.0;
}

Vector3D::Vector3D(float x, float y, float z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

void
Vector3D::PrintVector()
{
  printf("[%f, %f, %f]", this->x, this->y, this->z);
}

 Vector3D&
Vector3D::operator=  (Vector3D v)
{
  this->x = v.x;
  this->y = v.y;
  this->z = v.z;
  return *this;
}

 Vector3D&
Vector3D::operator-= (Vector3D v)
{
  this->x -= v.x;
  this->y -= v.y;
  this->z -= v.z;
  return *this;
}

 Vector3D&
Vector3D::operator+= (Vector3D v)
{
  this->x += v.x;
  this->y += v.y;
  this->z += v.z;
  return *this;
}

 Vector3D&
Vector3D::Set(float x, float y, float z )
{
  this->x = x;
  this->y = y;
  this->z = z;
  return *this;
}

 Vector3D
Vector3D::operator+  (Vector3D v)
{
  Vector3D t;
  t.x = this->x + v.x;
  t.y = this->y + v.y;
  t.z = this->z + v.z;
  return t;
}

 Vector3D
Vector3D::operator-  (Vector3D v)
{
  Vector3D t;
  t.x = this->x - v.x;
  t.y = this->y - v.y;
  t.z = this->z - v.z;
  return t;
}

 Vector3D
Vector3D::operator*  (Vector3D v)
{
  Vector3D t;
  t.x = this->x * v.x;
  t.y = this->y * v.y;
  t.z = this->z * v.z;
  return t;
}

 Vector3D
Vector3D::operator*= (Vector3D v)
{
  this->x *= v.x;
  this->y *= v.y;
  this->z *= v.z;
  return *this;
}

 Vector3D
Vector3D::operator*  (float r)
{
  Vector3D t;
  t.x = this->x * r;
  t.y = this->y * r;
  t.z = this->z * r;
  return t;
}

 Vector3D&
Vector3D::operator*= (float r)
{
  this->x *= r;
  this->y *= r;
  this->z *= r;
  return *this;
}

 Vector3D
Vector3D::Cross(Vector3D v)
{
  Vector3D temp(
    this->y * v.z - this->z * v.y,
    this->z * v.x - this->x * v.z,
    this->x * v.y - this->y * v.x);
  return temp;
}

 float
Vector3D::Dot(Vector3D v)
{
  return this->x * v.x + this->y * v.y + this->z * v.z;
}


 float
Vector3D::Length()
{
  return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

 float
Vector3D::SqrLength()
{
  return this->x * this->x + this->y * this->y + this->z * this->z;
}

 void
Vector3D::Norm()
{
  float l = 1.0 / this->Length();
  this->x *= l;
  this->y *= l;
  this->z *= l;
}

