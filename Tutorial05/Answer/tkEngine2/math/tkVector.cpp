/*!
 * @brief	ベクトルクラス。
 */

#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/math/tkVector.h"

namespace tkEngine2{
	const CVector3 CVector3::Zero	= {  0.0f,  0.0f,  0.0f };
	const CVector3 CVector3::Right	= {  1.0f,  0.0f,  0.0f };
	const CVector3 CVector3::Left	= { -1.0f,  0.0f,  0.0f };
	const CVector3 CVector3::Up		= {  0.0f,  1.0f,  0.0f };
	const CVector3 CVector3::Down	= {  0.0f, -1.0f,  0.0f };
	const CVector3 CVector3::Front	= { 0.0f,   0.0f,  1.0f };
	const CVector3 CVector3::Back	= { 0.0f,   0.0f, -1.0f };

	const CVector3 CVector3::AxisX	= { 1.0f,  0.0f,  0.0f };
	const CVector3 CVector3::AxisY	= { 0.0f,  1.0f,  0.0f };
	const CVector3 CVector3::AxisZ	= { 0.0f,  0.0f,  1.0f };
	const CVector3 CVector3::One = { 1.0f, 1.0f, 1.0f };
	const CQuaternion CQuaternion::Identity = { 0.0f,  0.0f, 0.0f, 1.0f };

	/*!
	*@brief	行列からクォータニオンを作成。
	*/
	void CQuaternion::SetRotation(const CMatrix& m)
	{
		DirectX::XMStoreFloat4(&vec, DirectX::XMQuaternionRotationMatrix(m));
	}

}