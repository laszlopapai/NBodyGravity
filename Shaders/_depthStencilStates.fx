DepthStencilState defaultCompositor
{
};

DepthStencilState noDepthTestCompositor
{
	DepthEnable = false;
	DepthWriteMask = zero;
};

DepthStencilState noDepthWriteCompositor
{
	DepthEnable = true;
	DepthWriteMask = zero;
};