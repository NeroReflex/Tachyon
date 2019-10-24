#include "BLAS.h"

using namespace Tachyon;
using namespace Tachyon::Core;

AABB BLAS::bvBase() const noexcept
{
	AABB globalBaseAABB;
	bool first = true;

	BVHLinearTree<GeometryCollection, 3>::traverse([this, &first, &globalBaseAABB](const GeometryCollection& collection) {
		globalBaseAABB = (first) ? collection.bvBase() : AABB(globalBaseAABB, collection.bvBase());

		// The following won't be the first geometric collection
		first = false;
	});

	DBG_ASSERT( (!first) );

	return globalBaseAABB;
}
