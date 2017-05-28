/*!
 * @brief	�x�N�g��
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
		* @brief	���`��ԁB
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
	 * @brief	�x�N�g���B
	 */
	class CVector3{
	public:
		union{
			DirectX::XMFLOAT3 vec;
			float v[4];
			struct { float x, y, z; };
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
		//XMVECTOR�ւ̈Öق̕ϊ��B
		operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat3(&vec);
		}
		//operator D3DXVECTOR3(void) { return s_cast<D3DXVECTOR3>(*this); }
		/*!
		*@brief	������Z�q�B
		*/
		CVector3& operator=(const CVector3& _v)
		{
			vec = _v.vec;
			return *this;
		}
		CVector3() {}
		/*!
		* @brief	�R���X�g���N�^�B
		*/
		CVector3(float x, float y, float z)
		{
			Set(x, y, z);
		}
		/*!
		* @brief	���`��ԁB
		*@details
		* this = v0 + (v1-v0) * t;
		*/
		void Lerp(float t, const CVector3& v0, const CVector3& v1)
		{
			DirectX::XMVECTOR _v = DirectX::XMVectorLerp(
				DirectX::XMLoadFloat3(&v0.vec),
				DirectX::XMLoadFloat3(&v1.vec),
				t);
			DirectX::XMStoreFloat3(&vec, _v);
		}
		template<class TVector>
		void CopyTo(TVector& dst) const
		{
			dst.x = x;
			dst.y = y;
			dst.z = z;
		}
		/*!
		* @brief	�x�N�g���̊e�v�f��ݒ�B
		*/
		void Set(float _x, float _y, float _z)
		{
			vec.x = _x;
			vec.y = _y;
			vec.z = _z;
		}
		template<class TVector>
		void Set(TVector& v)
		{
			Set(v.x, v.y, v.z);
		}
		/*void Set(btVector3& v)
		{
			this->x = v.x();
			this->y = v.y();
			this->z = v.z();
		}
		*/
		/*!
		 * @brief	�x�N�g�������Z�B
		 */
		void Add( const CVector3& _v) 
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		void Add( const CVector3& v0, const CVector3& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/*!
		 * @brief	�x�N�g�������Z�B
		 */
		void Subtract( const CVector3& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		void Subtract( const CVector3& v0, const CVector3& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/*!
		 * @brief	���ρB
		 */
		float Dot( const CVector3& _v ) const
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			return DirectX::XMVector3Dot(xmv0, xmv1).m128_f32[0];
		}
		/*!
		 * @brief	�O�ρB
		 */
		void Cross(const CVector3& _v)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		void Cross(const CVector3& v0, const CVector3& v1)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVector3Cross(xmv0, xmv1);
			DirectX::XMStoreFloat3(&vec, xmvr);
		}
		/*!
		 * @brief	�������擾
		 */
		float Length() const
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			return DirectX::XMVector3Length(xmv).m128_f32[0];
		}
		/*!
		 * @brief	�����̓����擾
		 */
		float LengthSq() const
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			return DirectX::XMVector3LengthSq(xmv).m128_f32[0];
		}
		/*!
		* @brief	�g��B
		*/
		void Scale(float s)
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat3(&vec, xmv);
		}
		/*!
		* @brief	�@���𐳋K���B
		*/
		void Normalize()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat3(&vec);
			xmv = DirectX::XMVector3Normalize(xmv);
			DirectX::XMStoreFloat3(&vec, xmv);
		}
		/*!
		* @brief	���Z�B
		*/
		void Div(float d)
		{
			float scale = 1.0f / d;
			Scale(scale);
		}
		/*!
		* @brief	�ő�l��ݒ�B
		*/
		void Max(const CVector3& vMax)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&vMax.vec);
			DirectX::XMStoreFloat3(&vec,  DirectX::XMVectorMax(xmv0, xmv1));
		}
		/*!
		* @brief	�ŏ��l��ݒ�B
		*/
		void Min(const CVector3& vMin)
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat3(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat3(&vMin.vec);
			DirectX::XMStoreFloat3(&vec, DirectX::XMVectorMin(xmv0, xmv1));
		}
	};
	/*!
	 *@brief	4�v�f�̃x�N�g���N���X�B
	 */
	class CVector4{
	public:
		union{
			DirectX::XMFLOAT4 vec;
			struct { float x, y, z, w; };
			float v[4];
		};
	public:
		operator DirectX::XMVECTOR() const
		{
			return DirectX::XMLoadFloat4(&vec);
		}
		CVector4(){}
		/*!
		*@brief	������Z�q�B
		*/
		CVector4& operator=(const CVector4& _v)
		{
			vec = _v.vec;
			return *this;
		}
		/*!
		 *@brief	�R���X�g���N�^
		 */
		CVector4(float x, float y, float z, float w)
		{
			Set(x, y, z, w);
		}
		/*!
		*@brief	�R���X�g���N�^
		*@details
		* w�ɂ�1.0���i�[����܂��B
		*/
		CVector4(const CVector3& v)
		{
			Set(v);
		}
		/*!
		 *@brief	�x�N�g���̊e�v�f��ݒ�B
		 */
		void Set(float _x, float _y, float _z, float _w)
		{
			this->x = _x;
			this->y = _y;
			this->z = _z;
			this->w = _w;
		}
		/*!
		*@brief	�x�N�g����ݒ�B
		*/
		void Set(const CVector4& _v)
		{
			*this = _v;
		}
		/*!
		*@brief	�x�N�g����ݒ�B
		*@details
		* w�ɂ�1.0���i�[����܂��B
		*/
		
		void Set(const CVector3& _v)
		{
			this->x = _v.x;
			this->y = _v.y;
			this->z = _v.z;
			this->w = 1.0f;
		}
		/*!
		 *@brief	�x�N�g�������Z�B
		 */
		void Add( const CVector4& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		void Add( const CVector4& v0, const CVector4& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorAdd(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		/*!
		 *@brief	�x�N�g�������Z�B
		 */
		void Subtract( const CVector4& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		void Subtract( const CVector4& v0, const CVector4& v1 )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&v0.vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&v1.vec);
			DirectX::XMVECTOR xmvr = DirectX::XMVectorSubtract(xmv0, xmv1);
			DirectX::XMStoreFloat4(&vec, xmvr);
		}
		/*!
		 *@brief	����
		 */
		float Dot( const CVector4& _v )
		{
			DirectX::XMVECTOR xmv0 = DirectX::XMLoadFloat4(&vec);
			DirectX::XMVECTOR xmv1 = DirectX::XMLoadFloat4(&_v.vec);
			return DirectX::XMVector4Dot(xmv0, xmv1).m128_f32[0];
		}
		/*!
		 * @brief	�������擾
		 */
		float Length()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			return DirectX::XMVector4Length(xmv).m128_f32[0];
		}
		/*!
		 * @brief	�����̓����擾
		 */
		float LengthSq()
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			return DirectX::XMVector4LengthSq(xmv).m128_f32[0];
		}
		/*!
		* @brief	�g��B
		*/
		void Scale(float s)
		{
			DirectX::XMVECTOR xmv = DirectX::XMLoadFloat4(&vec);
			xmv = DirectX::XMVectorScale(xmv, s);
			DirectX::XMStoreFloat4(&vec, xmv);
		}
	};
	
	class CQuaternion : public CVector4{
	public:
		static const CQuaternion Identity;		//!<�P�ʃN�H�[�^�j�I���B
		CQuaternion() {}
		CQuaternion(float x, float y, float z, float w) :
			CVector4(x, y, z, w)
		{
		}
		
		/*!
		 *@brief	�C�ӂ̎�����̉�]�N�H�[�^�j�I�����쐬�B
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
		*@brief	�s�񂩂�N�H�[�^�j�I�����쐬�B
		*/
		void SetRotation(const CMatrix& m);
		
		/*!
		*@brief	�N�H�[�^�j�I�����m�̐ρB
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
		 *@brief	�N�H�[�^�j�I�����m�̏�Z�B
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
	//�����^�̃x�N�g���N���X�B
	__declspec(align(16)) class CVector4i {
	public:
		union {
			struct { int x, y, z, w; };
			int v[4];
		};
	};
}


#endif // _TKVECTOR_H_