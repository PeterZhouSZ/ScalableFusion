#ifndef FILE_INFORMATION_RENDERER_H
#define FILE_INFORMATION_RENDERER_H

//TODO: rename this to InformationRenderer (better Name, as i think)

#include <map>
#include <mutex>
#include <thread>
#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <opencv2/core.hpp>

#include <gpu/gpu_tex.h>
#include <gpu/shader.h>
#include <gpu/gpu_buffer.h>

using namespace std;
using namespace Eigen;

class MeshReconstruction;
class Meshlet;
class MeshTexture;
class ActiveSet;
class LowDetailRenderer;
class GpuStorage;
class GLFWwindow;
class MeshletGPU;
class TexAtlasPatch;
/**
 * @brief The RenderMapInformations class
 * does some of the rendering of the map to the current view (current camera pose).
 * Rendered information should include:
 * 1) depth: only new depth measurements that are similar to the current map get incorporated inteo
 *    the existing geometry. If new depth measurements are bigger than the projected map depth,
 *    we could should remove geometry obviously.
 *    If the new geometry is closer, it should be added to the map.
 *    If the new geometry is within range of the existing one.
 * 2) The geometry density. (low density means we can use the rendered indices to change geometry)
 *    High density in geometry means that in the index images for vertices does not show all the geometry.
 *    The geometry itself has to be iterated to see if it is in range of the depth buffer and see if it
 *    is fit to be altered trough the new data.
 * 3) The indices of the vertices (triangles have 3 values for the 3 vertices it is made out of).
 * 4) Borders of existing geometry.
 * 5) All of that magic we need for ICP or Dense Visual Odometry
 *
 * To do this, it mostly manages a bunch of shaders and buffers.
 * MAYBE LAYERED RENDERING MIGHT BE USEFUL WHEN NEEDING MULTIPLE IMAGES (only performance stuff)
 * or this: (who knows!)
 * http://stackoverflow.com/questions/31768783/get-element-id-in-vertex-shader-in-opengl
 */
class InformationRenderer { //TODO: maybe split this up
	friend MeshReconstruction;

public:

	InformationRenderer(int width = 640, int height = 480);

	~InformationRenderer();

	void initInContext(int width, int height, GarbageCollector* garbage_collector);

	void initInContext(GarbageCollector* garbage_collector);

	shared_ptr<gfx::GpuTex2D> getDepthTexture();

	shared_ptr<gfx::GpuTex2D> getStdTexture();

	void renderDepth(ActiveSet *active_set,
					 GpuStorage* gpu_storage,
					 Matrix4f projection, Matrix4f pose);

	///TODO: create a renderer for which patch is used in which image!!!
	//L.G. Debugging tool

	void bindRenderTriangleReferenceProgram(GpuStorage* gpu_storage);

	//old (REMOVE)
	void renderTriangleReferencesForPatch(ActiveSet *active_set,
	                                      GpuStorage* gpu_storage,
	                                      shared_ptr<Meshlet> &patch,
	                                      shared_ptr <MeshTexture> &target_texture);

	//new
	void renderReference(MeshletGPU* meshlet_gpu,
						 shared_ptr<TexCoordBufConnector> coords,
						 shared_ptr<TexAtlasPatch> ref_tex );

	void renderTriangleReferencesAndDepth(ActiveSet *active_set,
	                                      GpuStorage* gpu_storage,
	                                      Matrix4f projection,
	                                      Matrix4f pose);

	void render(ActiveSet *active_set, GpuStorage* gpu_storage, Matrix4f projection, Matrix4f pose,
	            cv::Mat *depth = nullptr, cv::Mat *normals = nullptr, 
	            cv::Mat *color = nullptr, cv::Mat *labels = nullptr);

	Vector4f renderAndExtractInfo(MeshReconstruction* reconstruction,
			                      ActiveSet* active_set, //TODO: make this a whole list of active sets
			                      GpuStorage* gpu_storage,
			                      Matrix4f view, Matrix4f proj,
								  LowDetailRenderer* low_detail_renderer,
	                              bool render_visible_from_cam,
	                              GLFWwindow *root_context, 
	                              int width, int height,
	                              int x, int y,
	                              int *patch_ind = nullptr,
	                              int *triangle_ind = nullptr);

	//TODO: remove, just for debug purposes
	shared_ptr<gfx::GpuTex2D> m_debug_texture;
	//debug
	shared_ptr<gfx::GLSLProgram> depth_program;

private:

	struct PerThread_ {
	   GLuint index_FBO;
	   shared_ptr<gfx::GpuTex2D> index_texture;

	   GLuint depth_VAO = 0;
	   GLuint depth_FBO = 0;
	   GLuint depth_buffer_tex;
	   shared_ptr<gfx::GpuTex2D> depth_texture;
	   shared_ptr<gfx::GpuTex2D> std_texture;

		//TODO: these are the new textures
	   GLuint combined_VAO = 0;
	   GLuint combined_FBO = 0;
	   shared_ptr<gfx::GpuTex2D> z_texture;
	   shared_ptr<gfx::GpuTex2D> color_texture;
	   shared_ptr<gfx::GpuTex2D> label_texture;
	   shared_ptr<gfx::GpuTex2D> normal_texture;

	};

	int width_;
	int height_;

	//maybe thread_local variable
	map<thread::id, PerThread_> per_thread_gl_objects_;

	static mutex shader_mutex_;
	static weak_ptr<gfx::GLSLProgram> s_depth_program_;

	//MeshReconstruction *map_;

	//TODO: rename since it is more about vertex references now!!!!!
	static weak_ptr<gfx::GLSLProgram> s_triangle_reference_program_;
	shared_ptr<gfx::GLSLProgram> triangle_reference_program_;

	static weak_ptr<gfx::GLSLProgram> s_triangle_ref_depth_program_;
	shared_ptr<gfx::GLSLProgram> triangle_ref_depth_prog_;

	//combined rendering of labels,depth and normals. (TODO)
	shared_ptr<gfx::GLSLProgram> unified_info_prog_;

};

#endif // FILE_INFORMATION_RENDERER_H
