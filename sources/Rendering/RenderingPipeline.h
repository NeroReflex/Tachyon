#pragma once

#include "Core/Camera.h"
#include "Core/HDR.h"
#include "GeometryPrimitive.h"

namespace Tachyon {
	namespace Rendering {

		class RenderingPipeline {
		public:
			RenderingPipeline(GLFWwindow* window) noexcept;

			RenderingPipeline(const RenderingPipeline&) = delete;

			RenderingPipeline(RenderingPipeline&&) = delete;

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;

			virtual ~RenderingPipeline() = default;

			virtual void enqueueModel(std::vector<GeometryPrimitive>&& primitive, glm::uint32 location, glm::mat4 modelMatrix) noexcept = 0;

			void reset() noexcept;

			void render(const Core::HDR& hdr, const Core::Camera& camera) noexcept;

		protected:
			virtual void onResize(glm::uint32 oldWidth, glm::uint32 oldHeight, glm::uint32 newWidth, glm::uint32 newHeight) noexcept;

			virtual void onRender(const Core::HDR& hdr, const Core::Camera& camera) noexcept = 0;

			virtual void onReset() noexcept = 0;

			glm::uint32 getWidth() const noexcept;

			glm::uint32 getHeight() const noexcept;

			const struct RaytracerInfo {
				const glm::uint32 expOfTwo_numberOfModels;
				const glm::uint32 expOfTwo_numberOfGeometryCollectionOnBLAS;
				const glm::uint32 expOfTwo_numberOfGeometryOnCollection;

				// Geometry is stored as consecutive vec4 in a texture, on the x axis.
				const glm::uint32 expOfTwo_numberOfTesselsForGeometryTexturazation; // This number is the number of texture texels used to store the texture

				inline RaytracerInfo(glm::uint32 expOfTwo_numberOfModels,
					glm::uint32 expOfTwo_numberOfGeometryCollectionOnBLAS,
					glm::uint32 expOfTwo_numberOfGeometryOnCollection,
					glm::uint32 expOfTwo_numberOfTesselsForGeometryTexturazation) noexcept :
					expOfTwo_numberOfModels(std::move(expOfTwo_numberOfModels)),
					expOfTwo_numberOfGeometryCollectionOnBLAS(std::move(expOfTwo_numberOfGeometryCollectionOnBLAS)),
					expOfTwo_numberOfGeometryOnCollection(std::move(expOfTwo_numberOfGeometryOnCollection)),
					expOfTwo_numberOfTesselsForGeometryTexturazation(std::move(expOfTwo_numberOfTesselsForGeometryTexturazation)) {}
			} mRaytracerInfo;

			const struct RaytracingGPUDataRequirements{
				const glm::uint32 mTLASTexels_Width;
				const glm::uint32 mBLASCollectionTexels_Width;
				const glm::uint32 mBLASCollectionTexels_Height;
				const glm::uint32 mModelMatrixCollection_Width;
				const glm::uint32 mModelMatrixCollection_Height;
				const glm::uint32 mGeometryCollectionTexels_Width;
				const glm::uint32 mGeometryCollectionTexels_Height;
				const glm::uint32 mGeometryCollectionTexels_Depth;

				RaytracingGPUDataRequirements() = delete;

				inline RaytracingGPUDataRequirements(
					glm::uint32 TLASTexels_Width,
					glm::uint32 BLASCollectionTexels_Width, glm::uint32 BLASCollectionTexels_Height,
					glm::uint32 ModelMatrixCollection_Width, glm::uint32 ModelMatrixCollection_Height,
					glm::uint32 geometryCollectionTexels_Width, glm::uint32 geometryCollectionTexels_Height, glm::uint32 geometryCollectionTexels_Depth) noexcept :
					mTLASTexels_Width(std::move(TLASTexels_Width)), 
					mBLASCollectionTexels_Width(std::move(BLASCollectionTexels_Width)),
					mBLASCollectionTexels_Height(std::move(BLASCollectionTexels_Height)),
					mModelMatrixCollection_Width(std::move(ModelMatrixCollection_Width)),
					mModelMatrixCollection_Height(std::move(ModelMatrixCollection_Height)),
					mGeometryCollectionTexels_Width(std::move(geometryCollectionTexels_Width)),
					mGeometryCollectionTexels_Height(std::move(geometryCollectionTexels_Height)),
					mGeometryCollectionTexels_Depth(std::move(geometryCollectionTexels_Depth))  {};
			} mRaytracerRequirements;

		private:
			void resize(glm::uint32 width, glm::uint32 height) noexcept;

			GLFWwindow* mWindow;

			glm::uint32 mWindowWidth, mWindowHeight;
		};
		
	}
}