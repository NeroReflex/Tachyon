#include "TLAS.h"

using namespace Tachyon;
using namespace Tachyon::Core;

void TLAS::linearize(Tachyon::Rendering::TLAS& tlas) const noexcept {
	tlas.ViewMatrix = getTransform();

	for (size_t i = 0; i < maxNumberOfTreeElements; ++i) {
		// tree elements are 2*N - 1 .....
		getNodeIndex(i).linearize(tlas.tree[i]);

		//.... but content is just N so a check is requred
		if (i < maxNumberOfElements)
			getElementAtIndex(i).linearize(tlas.blas[i]);
	}
}
