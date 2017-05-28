/*!
 *@brief	CGameObjectのマネージャ
 */
#include "tkEngine2/tkEnginePreCompile.h"
#include "tkEngine2/tkEngine.h"
#include "tkEngine2/gameObject/tkgameobjectmanager.h"
#include <future>

namespace tkEngine2{
	void CGameObjectManager::Execute(CRenderContext& renderContext)
	{
		ExecuteDeleteGameObjects();

		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->StartWrapper();
			}
		}
		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PreUpdateWrapper();
			}
		}
		//プリレンダリング。
		//@todo 未実装。 preRender.Update();
		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->UpdateWrapper();
			}
		}
		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PostUpdateWrapper();
			}
		}
		//シーングラフを更新。
		UpdateSceneGraph();

		//画面をクリア
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; //red,green,blue,alpha
		renderContext.ClearRenderTargetView(0, ClearColor);
		renderContext.RSSetViewport(0.0f, 0.0f, (float)Engine().GetFrameBufferWidth(), (float)Engine().GetFrameBufferHeight());
		
		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PreRenderWrapper(renderContext);
			}
		}

		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->RenderWrapper(renderContext);
			}
		}

		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PostRenderWrapper(renderContext);
			}
		}
		
#if 0
		{
			//ここからレンダリング部。未実装やで。
			//レンダリングステートの初期化。
			renderContext[0].Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
				D3DCOLOR_RGBA(0, 0, 255, 0), 1.0f, 0
				);
			tkEngine::SViewport vp = {
				0,
				0,
				(unsigned int)tkEngine::CEngine::Instance().GetFrameBufferWidth(),
				(unsigned int)tkEngine::CEngine::Instance().GetFrameBufferHeight(),
				0.0f,
				1.0f
			};
			renderContext[0].SetViewport(vp);
			renderContext[0].SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
			renderContext[0].SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			renderContext[0].SetRenderState(D3DRS_SRCBLEND, BLEND_ONE);
			renderContext[0].SetRenderState(D3DRS_DESTBLEND, BLEND_ZERO);
			renderContext[0].SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			renderContext[0].SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			renderContext[0].SetRenderState(D3DRS_ZENABLE, TRUE);
		}
		//プリレンダリング。
		preRender.Render(renderContext[0]);

		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PreRenderWrapper(renderContext[0]);
			}
		}
		
		if (numRenderContext == 1) {
			CPIXPerfTag tag(renderContext[0], L"Render MainRenderTarget");
			//シングルスレッド描画。
			//深度書き込み用のレンダリングターゲットを設定。
			renderContext[0].SetRenderTarget(1, Dof().GetDepthRenderTarget());
			//速度書き込み用のレンダリングターゲットを設定。
			renderContext[0].SetRenderTarget(2, MotionBlur().GetVelocityMapRenderTarget());
			for (GameObjectList objList : m_gameObjectListArray) {
				for (IGameObject* obj : objList) {
					obj->RenderWrapper(renderContext[0]);
				}
			}
		}
		else {
			//マルチスレッド描画。
			TK_ASSERT(0, "not implement!!");
			
		}
		//深度書き込み用のレンダリングターゲットを外す。
		renderContext[numRenderContext - 1].SetRenderTarget(1, NULL);
		postEffect.Render(renderContext[numRenderContext-1]);
		for (GameObjectList objList : m_gameObjectListArray) {
			for (IGameObject* obj : objList) {
				obj->PostRenderWrapper(renderContext[numRenderContext-1]);
			}
		}
#endif
	}
	void CGameObjectManager::UpdateSceneGraph()
	{
		//ワールド行列を更新。
		for (auto transform : m_childrenOfRootTransformList) {
			transform->UpdateWorldMatrixAll();
		}
	}
	void CGameObjectManager::ExecuteDeleteGameObjects()
	{
		int preBufferNo = m_currentDeleteObjectBufferNo;
		//バッファを切り替え。
		m_currentDeleteObjectBufferNo = 1 ^ m_currentDeleteObjectBufferNo;
		for(GameObjectList& goList : m_deleteObjectArray[preBufferNo]){
			for(IGameObject* go : goList){
				GameObjectPrio prio = go->GetPriority();
				GameObjectList& goExecList = m_gameObjectListArray.at(prio);
				auto it = std::find( goExecList.begin(),goExecList.end(),go );
				if (it != goExecList.end()) {
					//削除リストから除外された。
					(*it)->m_isRegistDeadList = false;
					if ((*it)->IsNewFromGameObjectManager()) {
						delete (*it);
					}
				}
				goExecList.erase(it);
			}
			goList.clear();
		}
	}
	void CGameObjectManager::Init( int gameObjectPrioMax )
	{
		TK_ASSERT( gameObjectPrioMax <= GAME_OBJECT_PRIO_MAX, "ゲームオブジェクトの優先度の最大数が大きすぎます。");
		m_gameObjectPriorityMax = static_cast<GameObjectPrio>(gameObjectPrioMax);
		m_gameObjectListArray.resize(gameObjectPrioMax);
		m_deleteObjectArray[0].resize(gameObjectPrioMax);
		m_deleteObjectArray[1].resize(gameObjectPrioMax);
	}
}