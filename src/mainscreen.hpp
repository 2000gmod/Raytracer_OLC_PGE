#include "hittable.hpp"
#include "olc/olcPGE.hpp"

#include "camera.hpp"
#include "vec3.hpp"
#include "util.hpp"
#include <thread>
#include <vector>

#include "threadpool/ThreadPool.hpp"

struct RenderRegion {
    int x0, x1, y0, y1;
};

class MainScreen : public olc::PixelGameEngine {
    private:
        MemoryManager manager;
        Camera cam;
        std::shared_ptr<Hittable> world;
        double processedSamples;
        int maxDepth;

        Vec3 camPos;
        double theta = 0, phi = 0;
        double focusDist = 1.0, aperture = 0, fov = 60.0;

        Vec3 *colorArray;
        bool running;
        uint32_t tileSize;
        bool visualizeRenderRegions = false;
        bool drawInfo = true;
        std::string sceneName;


        std::vector<RenderRegion> rRegions;
        ll::ThreadPool pool;

        void DrawScene();

        void drawToColorArray(int x, int y, const Vec3 &col);
        Vec3& getColorArray(int x, int y) const;
        void clearColorArray();


        void drawColorArray();

        void processKeys(float dTime);

        void randomScene(int limit);
        void createRenderRegions();
        void reloadScene();

    public:
        MainScreen(uint32_t tileSize, int numThreads, const std::string &sceneName);
        ~MainScreen();

        bool OnUserCreate() override;
        bool OnUserUpdate(float) override;
        bool OnUserDestroy() override;
};