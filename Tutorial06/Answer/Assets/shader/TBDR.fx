/*!
 * @brief	ポイントライトの影響範囲をタイルベースで計算するコンピュートシェーダー。
 */



//一度に実行されるスレッド数。
#define TILE_WIDTH			16
//タイルの総数。
#define TILE_SIZE			(TILE_WIDTH * TILE_WIDTH)
//ポイントライトの最大数。
#define MAX_LIGHT 			1024

//ポイントライト。
struct SPointLight {
	float3	position;		//位置。
	float3	positionInView;	//カメラ座標系でのポイントライトの位置。
	float4	color;			//カラー。
	float4  attn;			//減衰定数。(xを小さくするとライトの明るさが増す、yとzが大きくなるとライトが遠くまで届くようになる。)
};

//! コンスタントバッファ
cbuffer cbCameraParam : register( b0 )
{
	float4x4	mtxProj				: packoffset( c0 );		// 投影行列
	float4x4 	mtxProjInv			: packoffset( c4 );		// 投影行列の逆行列。
	float4x4	mtxViewRot			: packoffset( c8 );
	float4		screenParam			: packoffset( c12 );	// スクリーンパラメータ(near, far, screenWidth, screenHeight)
	
};

//入力
//ディフューズテクスチャ。
Texture2D diffuseTexture : register(t0);
//法線テクスチャ。
Texture2D normalTexture : register(t1);
//深度テクスチャ。
Texture2D depthTexture : register(t2);
//ポイントライト。
StructuredBuffer<SPointLight> pointLightList : register(t3);

//出力用のバッファ。
RWTexture2D<float4>		rwFinal		: register( u0 );	// ライティングの結果の出力先。

//共有メモリ。
groupshared uint sMinZ;		//タイルの最小深度。
groupshared uint sMaxZ;		//タイルの最大深度。
groupshared uint sTileLightIndices[MAX_LIGHT];	//タイルに接触しているポイントライトのインデックス。
groupshared uint sTileNumLights;				//タイルに接触しているポイントライトの数。
groupshared uint sPerSamplePixels[TILE_SIZE];
groupshared uint sNumPerSamplePixels;

/*!
 * @brief	タイルごとの視推台平面を求める。
 */
void GetTileFrustumPlane( out float4 frustumPlanes[6], uint3 groupId )
{
	// タイルの最大・最小深度を浮動小数点に変換
	float minTileZ = asfloat(sMinZ);
	float maxTileZ = asfloat(sMaxZ);

	// Intelのサンプルと微妙に違うのは右手系でやっているから
	// あと、Intelのサンプルは微妙に間違ってると思う
	float2 tileScale = screenParam.zw * rcp( float(2 * TILE_WIDTH) );
	float2 tileBias = tileScale - float2(groupId.xy);

	float4 c1 = float4(mtxProj._11 * tileScale.x, 0.0, tileBias.x, 0.0);
	float4 c2 = float4(0.0, -mtxProj._22 * tileScale.y, tileBias.y, 0.0);
	float4 c4 = float4(0.0, 0.0, 1.0, 0.0);

	frustumPlanes[0] = c4 - c1;		// Right
	frustumPlanes[1] = c4 + c1;		// Left
	frustumPlanes[2] = c4 - c2;		// Top
	frustumPlanes[3] = c4 + c2;			// Bottom
	frustumPlanes[4] = float4(0.0, 0.0, 1.0, -minTileZ);
	frustumPlanes[5] = float4(0.0, 0.0, -1.0, maxTileZ);

	// 法線が正規化されていない4面についてだけ正規化する
	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		frustumPlanes[i] *= rcp( length( frustumPlanes[i].xyz ) );
	}
}
/*!
 * @brief	カメラ空間での座標を計算する。
 */
float3 ComputePositionInCamera( uint2 globalCoords )
{
	float2	st = ((float2)globalCoords + 0.5) * rcp( screenParam.zw );
	st = st * float2( 2.0, -2.0 ) - float2( 1.0, -1.0 );
	float3 screenPos;
	screenPos.xy = st.xy;
	screenPos.z = depthTexture.Load(uint3(globalCoords, 0.0f));
	float4 cameraPos = mul(mtxProjInv, float4(screenPos, 1.0f));
	
	return cameraPos.xyz / cameraPos.w;
}

/*!
 * @brief	CSMain
 */
[numthreads(TILE_WIDTH, TILE_WIDTH, 1)]
void CSMain(
	uint3 groupId          : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId    : SV_GroupThreadID
)
{
	// タイル内でのインデックスを求める
    uint groupIndex = groupThreadId.y * TILE_WIDTH + groupThreadId.x;
    // ライトの数を取得する
    uint numLights, dummy;
    pointLightList.GetDimensions( numLights, dummy );
    //共有メモリを初期化する。
    if(groupIndex == 0)
    {
		sTileNumLights = 0;
		sNumPerSamplePixels = 0;
		sMinZ = 0x7F7FFFFF;		// floatの最大値
		sMaxZ = 0;
	}
	
	//ビュー空間での座標を計算する。
	float3 posInView = ComputePositionInCamera(dispatchThreadId.xy);
	// ここで同期を取る
    GroupMemoryBarrierWithGroupSync();
    
    // タイルの最大・最小深度を求める
	// この処理は並列するスレッド全てで排他的に処理される
	InterlockedMin( sMinZ, asuint(posInView.z) );
	InterlockedMax( sMaxZ, asuint(posInView.z) );
	
	// ここで同期を取ることでタイルの最大・最小深度を正しいものにする
    GroupMemoryBarrierWithGroupSync();
    
    // タイルの錘台を求める
	float4 frustumPlanes[6];
	GetTileFrustumPlane( frustumPlanes, groupId );
	
	// タイルとポイントライトの衝突判定
	for (uint lightIndex = groupIndex; lightIndex < numLights; lightIndex += TILE_SIZE)
	{
		SPointLight light = pointLightList[lightIndex];

		// タイルとの判定
		bool inFrustum = true;
		for (uint i = 0; i < 4; ++i)
		{
			float4 lp = float4(light.positionInView, 1.0f);
			float d = dot( frustumPlanes[i], lp );
			inFrustum = inFrustum && (d >= -light.attn.x);
		}

		// タイルと衝突している場合
		if (inFrustum)
		{
			uint listIndex;
			InterlockedAdd( sTileNumLights, 1, listIndex );
			sTileLightIndices[listIndex] = lightIndex;
		}
	}

	// ここで同期を取ると、sTileLightIndicesにタイルと衝突しているライトのインデックスが積まれている
    GroupMemoryBarrierWithGroupSync();
    
    float3 normal = normalTexture[dispatchThreadId.xy].xyz;
    //カメラ座標系でライティングの計算を行うので法線を回す回す。
    normal = mul(mtxViewRot, float4(normal, 1.0f)).xyz;

    //ライティングを行っていく。
    //ポイントライトを計算。
    float3 lig = 0.0f;
    for (uint i = 0; i < sTileNumLights; ++i){
		uint lightIndex = sTileLightIndices[i];
		SPointLight light = pointLightList[lightIndex];
		float3 lightDir = posInView - light.positionInView;
		float len = length(lightDir);
		lightDir = normalize(lightDir);	//正規化。
		float3 pointLightColor = saturate(-dot(normal, lightDir)) * light.color.xyz;
		//減衰を計算する。
		float	litRate = len / light.attn.x;
		float	attn = max(1.0 - litRate * litRate, 0.0);
		pointLightColor *= attn;
		lig += pointLightColor;
	}
	float3 diffuseColor = diffuseTexture[dispatchThreadId.xy] ;
	rwFinal[dispatchThreadId.xy] = float4(diffuseColor * lig, 1.0f);
}

