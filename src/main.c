#include <complex.h>
#include <omp.h>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdbool.h>
#include <stdio.h>

#define SCREEN_WIDTH 800      // Screen width
#define SCREEN_HEIGHT 800     // Screen height
#define MANDELBROT_WIDTH 400  // Mandelbrot computation width
#define MANDELBROT_HEIGHT 400 // Mandelbrot computation height
#define MAX_ITER 255          // Maximum iterations for Mandelbrot

int MandelBrotColor(float real, float imag) {
  float complex z = 0.0f + 0.0f * I;
  float complex c = real + imag * I;

  int iterations = 0;
  while (iterations < MAX_ITER) {
    z = z * z + c;

    if (crealf(z) * crealf(z) + cimagf(z) * cimagf(z) > 4.0f)
      break;

    iterations++;
  }

  if (iterations == MAX_ITER) {
    return iterations;
  }

  return iterations;
}

void ComputeMandebrotSet(Color *pixels, Color *gradient, float startX,
                         float startY, float scaleX, float scaleY) {

#pragma omp parallel for collapse(2)
  for (int y = 0; y < MANDELBROT_HEIGHT; y++) {
    for (int x = 0; x < MANDELBROT_WIDTH; x++) {
      float real = startX + x * scaleX;
      float imag = startY + y * scaleY;

      int colorIndex = MandelBrotColor(real, imag);
      pixels[y * MANDELBROT_WIDTH + x] = gradient[colorIndex];
    }
  }
}

int main() {
  const int screenWidht = 1920;
  const int screenHeight = 1080;

  const int centerX = screenWidht / 2;
  const int centerY = screenHeight / 2;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mandebrot set - render");

  Camera2D camera = {0};
  camera.zoom = 1.0f;
  camera.target = (Vector2){0.0f, 0.0f};

  SetTargetFPS(30);

  Image image = GenImageColor(MANDELBROT_WIDTH, MANDELBROT_HEIGHT, BLACK);
  Texture2D texture = LoadTextureFromImage(image);

  Color gradient[MAX_ITER];
  for (int i = 0; i < MAX_ITER; i++) {
    gradient[i] = (Color){i * 5 % 255, i * 2 % 255, i % 255, 255};
  }

  float scaleX = 4.0f / (MANDELBROT_WIDTH * camera.zoom);
  float scaleY = 4.0f / (MANDELBROT_HEIGHT * camera.zoom);
  float startX = camera.target.x - (2.0f / camera.zoom);
  float startY = camera.target.y - (2.0f / camera.zoom);

  Color *pixels = LoadImageColors(image);

  ComputeMandebrotSet(pixels, gradient, startX, startY, scaleX, scaleY);
  UpdateTexture(texture, pixels);
  UnloadImageColors(pixels);

  float lastZoom = camera.zoom;
  Vector2 lastTarget = camera.target;

  while (!WindowShouldClose()) {

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      float panSpeed = 0.01f;
      Vector2 delta = GetMouseDelta();
      delta = Vector2Scale(delta, -panSpeed / camera.zoom);
      camera.target = Vector2Add(camera.target, delta);
    }
    if (IsKeyPressed(KEY_UP)) {
      camera.zoom *= 1.1f;
    }
    if (IsKeyPressed(KEY_DOWN)) {
      camera.zoom /= 1.1f;
    }
    if (camera.zoom < 0.1f) {
      camera.zoom = 0.1f;
    }

    if (camera.zoom != lastZoom || !Vector2Equals(camera.target, lastTarget)) {
      lastZoom = camera.zoom;
      lastTarget = camera.target;

      // Recompute the scale factors and starting points based on camera zoom
      scaleX = 4.0f / (MANDELBROT_WIDTH * camera.zoom);
      scaleY = 4.0f / (MANDELBROT_HEIGHT * camera.zoom);
      startX = camera.target.x - (2.0f / camera.zoom);
      startY = camera.target.y - (2.0f / camera.zoom);

      // Recompute Mandelbrot for the current view
      pixels = LoadImageColors(image);
      ComputeMandebrotSet(pixels, gradient, startX, startY, scaleX, scaleY);
      UpdateTexture(texture, pixels);
      UnloadImageColors(pixels);
    }

    double startTime = GetTime();

    double endTime = GetTime();

    BeginDrawing();
    ClearBackground(BLACK);

    DrawTexturePro(texture,
                   (Rectangle){0, 0, MANDELBROT_WIDTH, -MANDELBROT_HEIGHT},
                   (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                   (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();

    int FPS = GetFPS();
    float frameTime = GetFrameTime();

    printf("Render Time: %f seconds\n", endTime - startTime);
    printf("Frame Time: %f second\n", frameTime);
    printf("Current FPS: %d FPS\n", FPS);
  }
  UnloadTexture(texture);
  CloseWindow();

  return 0;
}
