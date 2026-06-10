#include "tge/game.h"
#include "tge/graphics.h"
#include "tge/input.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <vector>

class Cube : public tge::ComponentBase {
public:
    void Init() override { resize(tge::Terminal::Size()); }

    void Update() override {
        auto ts = tge::Terminal::Size();
        if (ts != termSize) resize(ts);

        if (rotationTick.Await()) {
            // degrees-per-second per axis × seconds-per-tick.
            const tge::Vector3f rate{40.0f, 63.0f, 23.0f};
            angles = angles + rate * TICK_SECONDS;
        }
    }

    void Render() override {
        clearBuffers();

        // Unit cube vertices (side = 2, centered at origin).
        static const std::array<tge::Vector3f, 8> verts = {{
            {-1, -1, -1},
            {1, -1, -1},
            {1, 1, -1},
            {-1, 1, -1},
            {-1, -1, 1},
            {1, -1, 1},
            {1, 1, 1},
            {-1, 1, 1},
        }};
        // Faces wound so cross(b-a, c-a) yields the outward normal.
        static const std::array<std::array<int, 4>, 6> faces = {{
            {{0, 3, 2, 1}}, // -Z front
            {{4, 5, 6, 7}}, // +Z back
            {{0, 4, 7, 3}}, // -X left
            {{1, 2, 6, 5}}, // +X right
            {{0, 1, 5, 4}}, // -Y bottom
            {{3, 7, 6, 2}}, // +Y top
        }};
        static const std::array<tge::Color, 6> faceColors = {{
            tge::Color::BrightRed,     // front
            tge::Color::BrightCyan,    // back
            tge::Color::BrightGreen,   // left
            tge::Color::BrightMagenta, // right
            tge::Color::BrightBlue,    // bottom
            tge::Color::BrightYellow,  // top
        }};

        // Rotate every vertex once, reuse across all faces.
        const tge::Vector3f xAxis{1, 0, 0}, yAxis{0, 1, 0}, zAxis{0, 0, 1};
        const tge::Vector3f camOffset{0, 0, 5.0f}; // push cube in front of camera at origin

        std::array<tge::Vector3f, 8> world;
        for (int i = 0; i < 8; ++i) {
            world[i] = verts[i].Rotate(xAxis, angles.x).Rotate(yAxis, angles.y).Rotate(zAxis, angles.z) + camOffset;
        }

        // Project once, reuse across both triangles of each face.
        // Projected verts reuse Vector3f: xy = screen pixel coords, z = camera-space depth.
        std::array<tge::Vector3f, 8> proj;
        for (int i = 0; i < 8; ++i) proj[i] = project(world[i]);

        for (int f = 0; f < 6; ++f) {
            const auto& face = faces[f];
            const tge::Vector3f& a = world[face[0]];
            const tge::Vector3f& b = world[face[1]];
            const tge::Vector3f& c = world[face[2]];

            // Backface cull in camera space: face hidden when its outward
            // normal points away from camera (origin).
            tge::Vector3f n = tge::Vector3f::Cross(b - a, c - a);
            if (tge::Vector3f::Dot(n, a) >= 0) continue;

            tge::Color col = faceColors[f];
            drawTriangle(proj[face[0]], proj[face[1]], proj[face[2]], col);
            drawTriangle(proj[face[0]], proj[face[2]], proj[face[3]], col);
        }

        blit();
    }

private:
    void resize(const tge::Vector2i& ts) {
        termSize = ts;
        pixels = {ts.x, ts.y * 2}; // half-block doubles vertical resolution
        size_t total = static_cast<size_t>(pixels.x) * pixels.y;
        colorBuf.assign(total, tge::Color::None);
        depthBuf.assign(total, INF);
    }

    void clearBuffers() {
        std::fill(colorBuf.begin(), colorBuf.end(), tge::Color::None);
        std::fill(depthBuf.begin(), depthBuf.end(), INF);
    }

    tge::Vector3f project(const tge::Vector3f& v) const {
        // Half-block framebuffer: 1 pixel wide = 1 cell wide; 1 pixel tall = half
        // a cell tall. Cells are ~2x taller than wide, so pixels are ~square and
        // X and Y share the same focal length. Flip Y because screen Y grows down.
        const float focal = static_cast<float>(pixels.y) * 0.9f;
        const float invZ = 1.0f / v.z;
        return tge::Vector3f{v.x, -v.y, 0} * (focal * invZ) + tge::Vector3f{pixels.x * 0.5f, pixels.y * 0.5f, v.z};
    }

    // 2D cross product / edge function: positive when p is to the left of edge a -> b.
    static inline float edge(const tge::Vector2f& a, const tge::Vector2f& b, const tge::Vector2f& p) {
        tge::Vector2f ab = b - a;
        tge::Vector2f ap = p - a;
        return ab.x * ap.y - ab.y * ap.x;
    }

    void drawTriangle(const tge::Vector3f& a, const tge::Vector3f& b, const tge::Vector3f& c, tge::Color color) {
        const tge::Vector2f a2 = a.XY(), b2 = b.XY(), c2 = c.XY();

        float area = edge(a2, b2, c2);
        if (std::fabs(area) < 1e-6f) return; // degenerate
        const float invArea = 1.0f / area;

        tge::Vector2i bbMin{
            static_cast<int>(std::floor(std::min({a2.x, b2.x, c2.x}))),
            static_cast<int>(std::floor(std::min({a2.y, b2.y, c2.y}))),
        };
        tge::Vector2i bbMax{
            static_cast<int>(std::ceil(std::max({a2.x, b2.x, c2.x}))),
            static_cast<int>(std::ceil(std::max({a2.y, b2.y, c2.y}))),
        };
        bbMin.x = tge::Math::Clamp(0, pixels.x - 1, bbMin.x);
        bbMin.y = tge::Math::Clamp(0, pixels.y - 1, bbMin.y);
        bbMax.x = tge::Math::Clamp(0, pixels.x - 1, bbMax.x);
        bbMax.y = tge::Math::Clamp(0, pixels.y - 1, bbMax.y);
        if (bbMin.x > bbMax.x || bbMin.y > bbMax.y) return;

        // Edge functions are linear in p, so step them incrementally. For edge AB,
        // the gradient ∇w_AB is the 90° CCW perpendicular of (b-a): (-Δy, Δx).
        const tge::Vector2f ab = b2 - a2, bc = c2 - b2, ca = a2 - c2;
        const tge::Vector2f stepAB{-ab.y, ab.x};
        const tge::Vector2f stepBC{-bc.y, bc.x};
        const tge::Vector2f stepCA{-ca.y, ca.x};

        const tge::Vector2f start{bbMin.x + 0.5f, bbMin.y + 0.5f};
        float w0_row = edge(b2, c2, start);
        float w1_row = edge(c2, a2, start);
        float w2_row = edge(a2, b2, start);

        for (int y = bbMin.y; y <= bbMax.y; ++y) {
            float w0 = w0_row, w1 = w1_row, w2 = w2_row;
            int rowOff = y * pixels.x;
            for (int x = bbMin.x; x <= bbMax.x; ++x) {
                // Sign-agnostic inside test: weights must agree in sign with area.
                if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                    float z = (w0 * a.z + w1 * b.z + w2 * c.z) * invArea;
                    int idx = rowOff + x;
                    if (z < depthBuf[idx]) {
                        depthBuf[idx] = z;
                        colorBuf[idx] = color;
                    }
                }
                w0 += stepBC.x;
                w1 += stepCA.x;
                w2 += stepAB.x;
            }
            w0_row += stepBC.y;
            w1_row += stepCA.y;
            w2_row += stepAB.y;
        }
    }

    // Pack two stacked pixels into one terminal cell using the upper-half block.
    // ScreenBuffer auto-clears its next buffer and diffs against the prior frame,
    // so cells we skip will be erased for us if they were drawn last frame.
    void blit() {
        for (int cy = 0; cy < termSize.y; ++cy) {
            int topRow = (cy * 2) * pixels.x;
            int botRow = (cy * 2 + 1) * pixels.x;
            bool hasBot = (cy * 2 + 1) < pixels.y;
            for (int x = 0; x < pixels.x; ++x) {
                tge::Color top = colorBuf[topRow + x];
                tge::Color bot = hasBot ? colorBuf[botRow + x] : tge::Color::None;
                if (top == tge::Color::None && bot == tge::Color::None) continue;
                render.DrawAtXY({x, cy}, {L'▀', top, bot});
            }
        }
    }

    static constexpr float INF = 1e30f;
    static constexpr long long TICK_MS = 16; // ~60 Hz
    static constexpr float TICK_SECONDS = TICK_MS / 1000.0f;

    tge::Vector2i termSize{0, 0};
    tge::Vector2i pixels{0, 0};
    std::vector<tge::Color> colorBuf;
    std::vector<float> depthBuf;
    tge::Vector3f angles{0, 0, 0}; // current rotation in degrees per axis
    tge::Timer<std::chrono::milliseconds> rotationTick = TICK_MS;
};

class CubeDemo : public tge::GameManager {
public:
    CubeDemo() : tge::GameManager() {
        this->SetFPS(60);
        this->SetTicksPerSecond(60);
    }

    void Start() override { Component<Cube>("cube")(); }

    void Update() override {
        if (tge::Keyboard::GetKeyDown(tge::Key::Escape) || tge::Keyboard::GetKeyDown('q')) {
            this->Quit();
            return;
        }
        Get("cube")->Update();
    }

    void Render() override { Get("cube")->Render(); }
};

int main() {
    CubeDemo game;
    game.Run();
}
