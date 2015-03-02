
Gremlin.defineStep('instrToBasicBlock', [Vertex,Pipe], {
	_().in().filter{ it.type == TYPE_BASIC_BLOCK}
})
