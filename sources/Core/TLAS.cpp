#include "TLAS.h"

using namespace Tachyon;
using namespace Tachyon::Core;

void TLAS::linearize(Tachyon::Rendering::BLAS* blas, Tachyon::Rendering::NodeData* tree) const noexcept {
	size_t maxNumberOfObjectsToSerialize = std::max<size_t>(maxNumberOfTreeElements, maxNumberOfElements);

	for (size_t i = 0; i < maxNumberOfObjectsToSerialize; ++i) {
		// tree elements are 2*N - 1 .....
		if (i < maxNumberOfTreeElements)
			getNodeIndex(i).linearize(tree[i]);

		//.... but content is just N so a check is requred
		if (i < maxNumberOfElements)
			getElementAtIndex(i).linearize(&blas[i]);
	}
}

