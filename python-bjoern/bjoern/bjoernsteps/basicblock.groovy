
Gremlin.defineStep('basicBlockToFunctionDef', [Vertex, Pipe], {
	_().transform{ g.v(it.functionId) }
})
