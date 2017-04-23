#pragma once

#pragma warning (disable  : 4201)

#define BUILD_LEVEL_DEBUG		0		//デバッグビルド
#define BUILD_LEVEL_PREVIEW		1		//プレビュービルド
#define BUILD_LEVEL_MASTER		2		//マスタービルド。

/*!
* @brief	ビルドレベル。
* @details
*  BUILD_LEVELはBUILD_LEVEL_DEBUG、BUILD_LEVEL_PREVIEW、BUILD_LEVEL_MASTER</br>
*	の３つの値をとります。この３つの値は下記の意味を持ちます。
*		BUILD_LEVEL_DEBUG		完全なデバッグビルド。
*		BUILD_LEVEL_PREVIEW		コードは最適化されているが、デバッグ機能は残っている。
*		BUILD_LEVEL_MASTER		マスタービルド。デバッグ機能は完全に削除されている。
* デバッグコードを埋め込む時は、BUILD_LEVEL_MASTERでは無効になるように実装してください。
*/
#ifdef _DEBUG
#define BUILD_LEVEL		BUILD_LEVEL_DEBUG
#elif defined(TK_MASTER)
#define BUILD_LEVEL		BUILD_LEVEL_MASTER
#else
#define BUILD_LEVEL		BUILD_LEVEL_PREVIEW
#endif

#include <windows.h>
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include <list>
#include <map>
#include "tkEngine2/policy/tkNoncopyable.h"
#include "tkEngine2/policy/tkScopedResource.h"
#include "tkEngine2/debug/tkLog.h"
#include "tkEngine2/debug/tkAssert.h"
#include "tkEngine2/math/tkVector.h"
#include "tkEngine2/math/tkMatrix.h"
#include "graphics/tkRenderContext.h"

#include "gameObject/tkGameObjectManager.h"

#include "tkEngine2\graphics\tkShader.h"
#include "tkEngine2/graphics/GPUBuffer/tkVertexBuffer.h"
#include "tkEngine2/graphics/GPUBuffer/tkStructuredBuffer.h"
#include "tkEngine2/graphics/GPUView/tkShaderResourceView.h"
#include "tkEngine2/graphics/GPUView/tkUnorderedAccessView.h"
#include "tkEngine2/graphics/tkMapper.h"
#include "tkEngine2/graphics/GPUBuffer/tkConstantBuffer.h"
