RasterizerState defaultRasterizer
{
};

RasterizerState noCullRasterizer
{
	CullMode = none;
	FillMode = solid;
};

RasterizerState backfaceRasterizer
{
	CullMode = front;
	FillMode = solid;
};

RasterizerState wireframeRasterizer
{
	CullMode = none;
	FillMode = wireFrame;
};