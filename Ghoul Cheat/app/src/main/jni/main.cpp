#include <jni.h>
#include <android/log.h>
#include <cmath>
#include <vector>
#include "ImGui/imgui.h"
#include "Includes/dobby.h"
#include "KittyMemory/MemoryPatch.h"

#define LOG_TAG "WormShadow"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

struct Vector3 {
    float x, y, z;
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
};

struct Vector2 {
    float x, y;
};

struct EntityData {
    char name[32];
    Vector3 position;
    ImVec2 screenPosition;
    float distance;
    bool isAlive;
    bool isEnemy;
    bool isVisible;
};

std::vector<EntityData> entityList;

Vector2 CalculateAngles(const Vector3& localPosition, const Vector3& targetPosition) {
    Vector3 dir = targetPosition - localPosition;
    float hyp = sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
    Vector2 angles;
    angles.x = -asin(dir.y / hyp) * (180.0f / M_PI);
    angles.y = atan2(dir.x, dir.z) * (180.0f / M_PI);
    return angles;
}

void ApplyAimbot(Vector2& currentAngles, const Vector2& targetAngles, float smoothing) {
    if (smoothing < 1.0f) smoothing = 1.0f;
    currentAngles.x += (targetAngles.x - currentAngles.x) / smoothing;
    currentAngles.y += (targetAngles.y - currentAngles.y) / smoothing;
}

void RenderESP(ImDrawList* drawList, const std::vector<EntityData>& entities, bool showBox, bool showLine, bool showDistance) {
    for (const auto& entity : entities) {
        if (!entity.isAlive) continue;

        ImU32 col = entity.isEnemy ? IM_COL32(255, 50, 50, 255) : IM_COL32(50, 255, 50, 255);

        if (showLine) {
            ImVec2 screenCenter(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y);
            drawList->AddLine(screenCenter, entity.screenPosition, col, 1.5f);
        }

        if (showBox) {
            ImVec2 boxMin(entity.screenPosition.x - 20.0f, entity.screenPosition.y - 50.0f);
            ImVec2 boxMax(entity.screenPosition.x + 20.0f, entity.screenPosition.y + 10.0f);
            drawList->AddRect(boxMin, boxMax, col, 0.0f, 0, 1.5f);
        }

        if (showDistance) {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%s [%.1fm]", entity.name, entity.distance);
            drawList->AddText(ImVec2(entity.screenPosition.x - 30.0f, entity.screenPosition.y - 65.0f), col, buffer);
        }
    }
}

void* (*old_PlayerUpdate)(void* instance);
void* hooked_PlayerUpdate(void* instance) {
    if (instance != nullptr) {}
    return old_PlayerUpdate(instance);
}

void init_hooks() {
    LOGD("Initializing hooks & modules for Oxide on BlueStacks...");
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    init_hooks();
    return JNI_VERSION_1_6;
}
