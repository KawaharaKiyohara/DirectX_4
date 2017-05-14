/*!
 * @brief	ベクトル
 */

#ifndef _TKVECTOR_H_
#define _TKVECTOR_H_


namespace tkEngine2{
	class CMatrix;
	
	class CVector2 {
	public:
		CVector2()
		{

		}
		CVector2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}
		union {
			struct { float x, y; };
			float v[2];
		};
		/*!
		* @brief	線形補間。
		*@details
		* this = v0 + (v1-v0) * t;
		*/
		void Lerp(float t, const CVector2& v0, const CVector2& v1)
		{
			x = v0.x + (v1.x - v0.x) * t;
			y = v0.y + (v1.y - v0.y) * t;
		}
	};
	/*!
	 * @brief	ベクトル。
	 */
	class CVector3{
	public:
		union{
			struct { float x, y, z; };
			float v[3];
		};
		static const CVector3 Zero;
		static const CVector3 Right;
		static const CVector3 Left;
		static const CVector3 Up;
		static const CVector3 Down;
		static const CVector3 Front;
		static const CVector3 Back;
		static const CVector3 AxisX;
		static const CVector3 AxisY;
		static const CVector3 AxisZ;
		static const CVector3 One;
	public:
		//operator D3DXVECTOR3(void) { return s_cast<D3DXVECTOR3>(*this); }
		CVector3() {}
		/*!
		* @brief	コンストラクタ。
		*/
		CVector3(float x, float y, float z)
		{
			Set(x, y, z);
		}
		/*!
		* @brief	線形補間。
		*@details
		* this = v0 + (v1-v0) * t;
		*/
		void Lerp(float t, const CVector3& v0, const CVector3& v1)
		{
			x = v0.x + (v1.x - v0.x) * t;
			y = v0.y + (v1.y - v0.y) * t;
			z = v0.z + (v1.z - v0.z) * t;
		}
		template<class TVector>
		void CopyTo(TVector& dst) const
		{
			dst.x = x;
			dst.y = y;
			dst.z = z;
		}
		/*!
		* @brief	ベクトルの各要素を設定。
		*/
		void Set(float _x, float _y, float _z)
		{
			this->x = _x;
			this->y = _y;
			this->z = _z;
		}
		template<class TVector>
		void Set(TVector& v)
		{
			this->x = v.x;
			this->y = v.y;
			this->z = v.z;
		}
		/*void Set(btVector3& v)
		{
			this->x = v.x();
			this->y = v.y();
			this->z = v.z();
		}
		*/
		/*!
		 * @brief	ベクトルを加算。
		 */
		void Add(const CVector3& _v) 
		{
			x += _v.x;
			y += _v.y;
			z += _v.z;
		}
		void Add( const CVector3& v0, const CVector3& v1 )
		{
			x = v0.x + v1.x;
			y = v0.y + v1.y;
			z = v0.z + v1.z;
		}
		/*!
		 * @brief	ベクトルを減算。
		 */
		void Subtract( const CVector3& _v )
		{
			x -= _v.x;
			y -= _v.y;
			z -= _v.z;
		}
		void Subtract( const CVector3& v0, const CVector3& v1 )
		{
			x = v0.x - v1.x;
			y = v0.y - v1.y;
			z = v0.z - v1.z;
		}
		/*!
		 * @brief	内積。
		 */
		float Dot( const CVector3& _v ) const
		{
			return x * _v.x + y * _v.y + z * _v.z;
		}
		/*!
		 * @brief	外積。
		 */
		void Cross(const CVector3& _v)
		{
			float _x = ( x * _v.z ) - ( _v.y * z );
			float _y = ( z * _v.x ) - ( _v.z * x );
			float _z = ( x * _v.y ) - ( _v.x * y );
			x = _x;
			y = _y;
			z = _z;
		}
		void Cross(const CVector3& v0, const CVector3& v1)
		{
			float _x = ( v0.y * v1.z ) - ( v1.y * v0.z );
			float _y = ( v0.z * v1.x ) - ( v1.z * v0.x );
			float _z = ( v0.x * v1.y ) - ( v1.x * v0.y );
			x = _x;
			y = _y;
			z = _z;
		}
		/*!
		 * @brief	長さを取得
		 */
		float Length() const
		{
			return sqrt(LengthSq());
		}
		/*!
		 * @brief	長さの二乗を取得
		 */
		float LengthSq() const
		{
			return x * x + y * y + z * z;
		}
		/*!
		* @brief	拡大。
		*/
		void Scale(float s)
		{
			x *= s;
			y *= s;
			z *= s;
		}
		/*!
		* @brief	法線を正規化。
		*/
		void Normalize()
		{
			float len = Length();
			if (len > 0.0f) {
				x /= len;
				y /= len;
				z /= len;
			}
			else {
				x = 0.0f;
				y = 0.0f;
				z = 0.0f;
			}
		}
		/*!
		* @brief	除算。
		*/
		void Div(float d)
		{
			x /= d;
			y /= d;
			z /= d;
		}
		/*!
		* @brief	最大値を設定。
		*/
		void Max(const CVector3& vMax)
		{
			x = max(x, vMax.x);
			y = max(y, vMax.y);
			z = max(z, vMax.z);
		}
		/*!
		* @brief	最小値を設定。
		*/
		void Min(const CVector3& vMin)
		{
			x = min(x, vMin.x);
			y = min(y, vMin.y);
			z = min(z, vMin.z);
		}
	};
	/*!
	 *@brief	4要素のベクトルクラス。
	 */
	class CVector4{
	public:
		union{
			struct { float x, y, z, w; };
			float v[4];
		};
	public:
		CVector4(){}
		/*!
		 *@brief	コンストラクタ
		 */
		CVector4(float x, float y, float z, float w)
		{
			Set(x, y, z, w);
		}
		/*!
		*@brief	コンストラクタ
		*@details
		* wには1.0が格納されます。
		*/
		CVector4(const CVector3& v)
		{
			Set(v);
		}
		/*!
		 *@brief	ベクトルの各要素を設定。
		 */
		void Set(float _x, float _y, float _z, float _w)
		{
			this->x = _x;
			this->y = _y;
			this->z = _z;
			this->w = _w;
		}
		/*!
		*@brief	ベクトルを設定。
		*/
		void Set(const CVector4& _v)
		{
			*this = _v;
		}
		/*!
		*@brief	ベクトルを設定。
		*@details
		* wには1.0が格納されます。
		*/
		
		void Set(const CVector3& _v)
		{
			this->x = _v.x;
			this->y = _v.y;
			this->z = _v.z;
			this->w = 1.0f;
		}
		/*!
		 *@brief	ベクトルを加算。
		 */
		void Add( const CVector4& _v )
		{
			x += _v.x;
			y += _v.y;
			z += _v.z;
			w += _v.w;
		}
		void Add( const CVector4& v0, const CVector4& v1 )
		{
			x = v0.x + v1.x;
			y = v0.y + v1.y;
			z = v0.z + v1.z;
			w = v0.w + v1.w;
		}
		/*!
		 *@brief	ベクトルを減算。
		 */
		void Subtract( const CVector4& _v )
		{
			x -= _v.x;
			y -= _v.y;
			z -= _v.z;
		}
		void Subtract( const CVector4& v0, const CVector4& v1 )
		{
			x = v0.x - v1.x;
			y = v0.y - v1.y;
			z = v0.z - v1.z;
			w = v0.w - v1.w;
		}
		/*!
		 *@brief	内積
		 */
		float Dot( const CVector4& _v )
		{
			return x * _v.x + y * _v.y + z * _v.z + w * _v.w;
		}
		/*!
		 * @brief	長さを取得
		 */
		float Length()
		{
			return sqrt(LengthSq());
		}
		/*!
		 * @brief	長さの二乗を取得
		 */
		float LengthSq()
		{
			return x * x + y * y + z * z + w * w;
		}
		/*!
		* @brief	拡大。
		*/
		void Scale(float s)
		{
			x *= s;
			y *= s;
			z *= s;
			w *= s;
		}
	};
	
	class CQuaternion : public CVector4{
	public:
		static const CQuaternion Identity;		//!<単位クォータニオン。
		CQuaternion() {}
		CQuaternion(float x, float y, float z, float w) :
			CVector4(x, y, z, w)
		{
		}
		
		/*!
		 *@brief	任意の軸周りの回転クォータニオンを作成。
		 */
		void SetRotation( const CVector3& axis, float angle )
		{
			float s;
			float halfAngle = angle * 0.5f;
			s = sin(halfAngle);
			w = cos(halfAngle);
			x = axis.x * s;
			y = axis.y * s;
			z = axis.z * s;
		}
		/*!
		*@brief	行列からクォータニオンを作成。
		*/
		void SetRotation(const CMatrix& m);
		
		/*!
		*@brief	クォータニオン同士の積。
		*/
		void Multiply(const CQuaternion& rot)
		{
			float pw, px, py, pz;
			float qw, qx, qy, qz;

			pw = w; px = x; py = y; pz = z;
			qw = rot.w; qx = rot.x; qy = rot.y; qz = rot.z;

			w = pw * qw - px * qx - py * qy - pz * qz;
			x = pw * qx + px * qw + py * qz - pz * qy;
			y = pw * qy - px * qz + py * qw + pz * qx;
			z = pw * qz + px * qy - py * qx + pz * qw;

		}
		/*!
		 *@brief	クォータニオン同士の乗算。
		 *@details
		 * this = rot0 * rot1;
		 */
		void Multiply(const CQuaternion& rot0, const CQuaternion& rot1)
		{
			float pw, px, py, pz;
			float qw, qx, qy, qz;

			pw = rot0.w; px = rot0.x; py = rot0.y; pz = rot0.z;
			qw = rot1.w; qx = rot1.x; qy = rot1.y; qz = rot1.z;

			w = pw * qw - px * qx - py * qy - pz * qz;
			x = pw * qx + px * qw + py * qz - pz * qy;
			y = pw * qy - px * qz + py * qw + pz * qx;
			z = pw * qz + px * qy - py * qx + pz * qw;
		}
	};
	//整数型のベクトルクラス。
	class CVector4i {
	public:
		union {
			struct { int x, y, z, w; };
			int v[4];
		};
	};
}


#endif // _TKVECTOR_H_