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

void BLAS::linearize(Tachyon::Rendering::BLAS* blas, glm::mat4* blasModelMatrix) const noexcept {
	*blasModelMatrix = getTransform();

	size_t maxNumberOfObjectsToSerialize = std::max<size_t>(maxNumberOfTreeElements, maxNumberOfElements);

	for (size_t i = 0; i < maxNumberOfObjectsToSerialize; ++i) {
		// tree elements are 2*N - 1 .....
		if (i < maxNumberOfTreeElements) getNodeIndex(i).linearize(blas->tree[i]);

		//.... but content is just N so a check is requred
		if (i < maxNumberOfElements) getElementAtIndex(i).linearize(blas->collection[i]);
	}
}