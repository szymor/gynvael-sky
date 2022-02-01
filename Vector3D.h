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
#pragma once
class Vector3D
{
  public:
    union
    {
      struct { float x, y, z; };
      struct { float r, g, b; };
      struct { float arr[3];  };
    };

    Vector3D();
    Vector3D(float x, float y, float z);
    void PrintVector();
     Vector3D& operator=  (Vector3D v);
     Vector3D& operator-= (Vector3D v);
     Vector3D& operator+= (Vector3D v);
     Vector3D& Set(float x, float y, float z );
     Vector3D  operator+  (Vector3D v);
     Vector3D  operator-  (Vector3D v);
     Vector3D  operator*  (Vector3D v);
     Vector3D  operator*= (Vector3D v);
     Vector3D  operator*  (float r);
     Vector3D& operator*= (float r);
     Vector3D  Cross(Vector3D v);
     float    Dot(Vector3D v);
     float    Length();
     float    SqrLength();
     void      Norm();
};

