#include "BLAS.h"

using namespace Tachyon;
using namespace Tachyon::Core;

AABB BLAS::bvBase() const noexcept
{
	AABB globalBaseAABB;
	bool first = true;

	traverse([this, &first, &globalBaseAABB](const GeometryCollection& collection) {
		globalBaseAABB = (first) ? collection.bvBase() : AABB(globalBaseAABB, collection.bvBase());

		// The following won't be the first geometric collection
		first = false;
	});

	DBG_ASSERT( (!first) );

	return globalBaseAABB;
}

void BLAS::linearize(Tachyon::Rendering::BLAS* blas) const noexcept {
	blas->ModelMatrix = getTransform();

	for (size_t i = 0; i < maxNumberOfTreeElements; ++i) {
		// tree elements are 2*N - 1 .....
		getNodeIndex(i).linearize(blas->tree[i]);

		//.... but content is just N so a check is requred
		if (i < maxNumberOfElements) getElementAtIndex(i).linearize(blas->collection[i]);
	}
}