#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <D3Dcompiler.h>
#include <Windows.h>
#include <d3d11.h>

const int NUM_THREAD = 2;
extern ID3D11Device* g_pd3dDevice ;								//D3D11デバイス。
extern ID3D11DeviceContext* g_pd3dDiferredContext[NUM_THREAD];	//DiferredContextコンテキスト。
extern ID3D11DeviceContext* g_pd3dImmidiateContext;				//即時コンテキスト。

