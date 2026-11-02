# The Graphics Pipeline (The Big Picture)

**Course section:** Lesson 1 (theory foundations, Ben Cook's Modern OpenGL course)
**Date:** 2026-10-24

## Summary

The **graphics pipeline** is the series of steps the GPU goes through to turn your data (a list of numbers describing shapes) into colored pixels on the screen. Your C++ program runs on the CPU. It does not draw pixels itself. It sends vertex data to the GPU and says "draw this." The GPU then runs that data through the pipeline, one stage after another. Some stages are fixed and run automatically. Others are programmable with small GPU programs called **shaders**. Knowing the pipeline is the basis of everything in OpenGL, and it is also your main debugging tool: when the screen is black, you walk through the pipeline to find which stage broke.

## Key concepts

### Basic terms
- **GPU (Graphics Processing Unit):** the chip on the graphics card. It is built to do thousands of small, simple math jobs at the same time.
- **CPU:** the computer's main processor. Your C++ code runs here.
- **Vertex:** one corner point of a shape. A triangle has 3 vertices. Each one stores at least a position, and can also store things like a color.
- **Pixel:** one tiny colored dot on the screen.
- **Shader:** a small program you write (in GLSL) that runs on the GPU at a programmable stage of the pipeline.

### Analogy: a car factory assembly line 🏭

| Factory | Graphics pipeline |
|---|---|
| Raw materials arrive at the loading dock | Vertex data arrives at the GPU |
| Station 1 puts the frame parts in the right spots | Vertex shader places each vertex on screen |
| Station 2 bolts parts into a car body | Primitive assembly connects vertices into triangles |
| Station 3 works out which panels need paint | Rasterization works out which pixels the triangle covers |
| Station 4 paints every panel | Fragment shader picks a color for each pixel |
| Finished car rolls out the door | Finished image appears on screen |

Three things this analogy gets right:
1. **Order matters.** Each stage depends on the one before it.
2. **Every stage works at the same time** on different pieces of work, which is a big part of why the GPU is fast.
3. **Some stations are reprogrammable** (shaders) and others are fixed machines.

### Why the GPU and not the CPU? (parallelism)
The CPU is not too weak. The real difference is **parallelism**: doing many jobs at the same time.
- A **CPU** has a few (4–16) very smart **cores** (a core is one worker inside a chip). They are great at complicated work with lots of decisions.
- A **GPU** has **thousands** of simpler cores, all running at once.

**Analogy:** grading 10,000 one-line quizzes. 8 brilliant professors (CPU) vs 5,000 students who can each check one simple answer (GPU). The students win, because each job is simple and the jobs don't depend on each other. Pixel colors work the same way.

**The numbers:**
```
1920 × 1080        = 2,073,600 pixels per frame
× 60 frames/second = 124,416,000 pixel colors per second   (1080p)
4K at 144 fps      ≈ 1.2 billion per second
```

### The pipeline stages
✏️ = you program it with a shader, ⚙️ = fixed, the GPU does it on its own.

```
   YOUR C++ PROGRAM (CPU)
          │  "Here are my vertices, please draw them"
          ▼
 1. Vertex Specification      Pack vertex data, send it to the GPU, say how to read it
          ▼
 2. Vertex Shader        ✏️   Runs once PER VERTEX: outputs its final position
          ▼
 3. Tessellation         ✏️   (optional) split shapes into smaller pieces
 4. Geometry Shader      ✏️   (optional) add or change whole shapes
          ▼
 5. Vertex Post-Processing    Clipping: cut away anything outside the screen
 6. Primitive Assembly   ⚙️   Connect vertices into primitives (triangles)
          ▼
 7. Rasterization        ⚙️   Work out which pixels each triangle covers → fragments
          ▼
 8. Fragment Shader      ✏️   Runs once PER FRAGMENT: outputs its color
          ▼
 9. Per-Sample Operations ⚙️  Depth test (is it hidden?), blending (transparency)
          ▼
   FRAMEBUFFER → 🖥️ SCREEN
```

- **Vertex Specification:** happens on the **C++ side**. Your code puts vertex data into GPU memory and tells the GPU how to read it (where to find the vertices, and how many numbers make up one vertex). VBOs and VAOs come in here (later lessons).
- **Vertex Shader:** runs once for every vertex. It does **not** decide which vertices exist or where they live in memory. That was already set up in Vertex Specification. It receives **one vertex at a time** and its main job is to output that vertex's final position (`gl_Position`).
- **Primitive:** a basic shape the GPU can draw: a point, a line, or a triangle. Primitive assembly groups vertices into primitives according to the **draw mode** you asked for (e.g. triangles → every 3 vertices make 1 triangle).
- **Clipping:** keeps what's inside the screen and throws away what's outside (details below).
- **Rasterization:** lays the triangle over the pixel grid and marks every covered pixel as a **fragment**.
- **Fragment:** a **candidate pixel**: all the information needed to color one pixel, *not yet approved*. It can still be thrown away in Per-Sample Operations (e.g. the depth test finds it is behind something already drawn). **Every pixel on screen came from a fragment, but not every fragment becomes a pixel.**
- **Fragment Shader:** runs once for every fragment and outputs its color. A big triangle can cover thousands of fragments, so this shader usually runs far more times than the vertex shader.
- **Per-Sample Operations:** final quality control. The **depth test** throws away fragments hidden behind closer ones. **Blending** mixes colors for see-through things like glass. Survivors are written to the framebuffer.

### The two shaders to remember

| Shader | Runs once per... | Main job |
|---|---|---|
| **Vertex shader** | vertex (corner point) | *Where* is it? (position) |
| **Fragment shader** | fragment (candidate pixel) | *What color* is it? |

### NDC (Normalized Device Coordinates)
OpenGL's standard coordinate space. The visible screen goes from **-1 to +1** on both x and y, whatever the window size. (0, 0) is the center. Before you learn transformations and cameras, whatever you put in `gl_Position` must land inside this range to be visible.

The GPU converts NDC to window pixels (the **viewport transform**):
```
pixel_x = (ndc_x + 1) / 2 × width
pixel_y = (ndc_y + 1) / 2 × height
```
In OpenGL, pixel (0, 0) is the **bottom-left** corner of the window.

### Clipping: scissors, not squishing ✂️
Clipping does **not** move or clamp vertices to the screen edge. It **cuts** the shape along the edge, keeps the inside part exactly where it was, and throws away the rest.

| Triangle is... | What clipping does | Result on screen |
|---|---|---|
| Completely inside | Nothing, it passes through unchanged | Whole triangle drawn |
| Partly outside | Cuts it along the screen edge, adding new vertices where edges cross it | Only the inside part drawn |
| Completely outside | Throws it away | Nothing drawn, 0 fragment shader runs |

**Camera analogy:** a friend half in the photo frame shows up cut off at the edge. A friend completely outside the frame isn't in the photo at all. They aren't squeezed into a strip along the edge.

Clipping comes **after** the vertex shader (the GPU only learns where a vertex ends up once the vertex shader has run), so off-screen vertices still cost vertex shader runs. They do save the expensive fragment shader work.

### Framebuffers and double buffering
- A **framebuffer** is the block of memory that holds an image, the "canvas" the pipeline paints onto.
- When GLFW creates a window, it also creates a **default framebuffer**: the window itself.
- It is **double-buffered**:
  ```
  FRONT buffer → what's on the screen right now
  BACK buffer  → where the GPU draws the next frame
  ```
- **Flip-book analogy:** the artist draws the next page on a fresh sheet while you look at the current one, then swaps them. You never see a half-finished drawing.
- `glClear(...)` wipes the back buffer before drawing a new frame.
- `glfwSwapBuffers(window)` swaps back and front so the finished frame appears.
- Later in the course you can create **your own framebuffers** that draw into a texture, not the screen. That's how shadows, mirrors and post-processing (blur, black-and-white) work. Probably covered in the shadow mapping sections near the end of the course.

## Code examples

### Worked example: one triangle's journey
Setup: an **800 × 600** window, three vertices in NDC:
```
A: (-0.5, -0.5)  bottom-left
B: ( 0.5, -0.5)  bottom-right
C: ( 0.0,  0.5)  top
```

**Stage 1, Vertex Specification:** C++ uploads `-0.5, -0.5, 0.5, -0.5, 0.0, 0.5` to the GPU, says "every 2 numbers = one vertex position," and asks to draw **triangles**.

**Stage 2, Vertex Shader (runs 3 times):**
```glsl
#version 330 core
layout (location = 0) in vec2 aPos;   // input: ONE vertex's (x, y), read from your data

void main()
{
    gl_Position = vec4(aPos, 0.0, 1.0);   // built-in output: final position (x, y, z, w)
                                          // z = 0 (flat), w = 1 (explained with transformations)
}
```

**Stages 5–6, Clipping + Assembly:** all vertices are inside -1..1, so nothing is cut. The 3 vertices become 1 triangle. The viewport transform gives pixel positions:
```
A: x = (-0.5 + 1)/2 × 800 = 200   y = (-0.5 + 1)/2 × 600 = 150
B: x = ( 0.5 + 1)/2 × 800 = 600   y = 150
C: x = ( 0.0 + 1)/2 × 800 = 400   y = ( 0.5 + 1)/2 × 600 = 450
```

**Stage 7, Rasterization:** base 400 px, height 300 px →
`area = ½ × 400 × 300 = 60,000 fragments`

**Stage 8, Fragment Shader (runs ~60,000 times):**
```glsl
#version 330 core
out vec4 FragColor;   // output: this fragment's color

void main()
{
    FragColor = vec4(1.0, 0.5, 0.2, 1.0);   // orange: (red, green, blue, alpha)
                                            // colors go 0.0–1.0, NOT 0–255
                                            // alpha 1.0 = fully solid
}
```

**Stage 9, Per-Sample Ops:** nothing else on screen, so every fragment survives and is written to the back buffer.

**Screen:** `glfwSwapBuffers` swaps it to the front → orange triangle.

```
6 numbers → 3 vertex shader runs → 1 triangle → ~60,000 fragments
          → ~60,000 fragment shader runs → orange triangle on screen
```

### Worked example: clipping math
Change C to `(0.0, 1.5)`, above the top of the screen. Clipping cuts along y = 1.0:

Edge A → C goes from (-0.5, -0.5) to (0.0, 1.5):
```
y climbs 2.0 total. To reach y = 1.0 it climbs 1.5 → 1.5 / 2.0 = 0.75 (75% along the edge)
x at 75%: -0.5 + 0.75 × (0.0 - (-0.5)) = -0.125
→ new vertex D = (-0.125, 1.0)
```
Edge B → C by symmetry → **E = (0.125, 1.0)**.

```
y=1.0 ─────D═══E─────   ← flat top along the screen edge
          /     \
         /       \
        A─────────B
```
The visible shape has **4 corners** (A, B, E, D). The GPU can only rasterize triangles, so it splits this into **2 triangles** behind the scenes (e.g. A-B-E and A-E-D).

## My questions and the answers

**Q: Why does the GPU do the drawing instead of the CPU?**
My first answer was that the CPU "doesn't have the power." Close, but the real reason is **parallelism**, not power. Each CPU core is actually stronger than a GPU core. But pixel colors are millions of small, *independent* jobs, so thousands of GPU cores working at once beat a handful of CPU cores working one pixel at a time (the professors vs students analogy).

**Q: When will we go over framebuffers?**
I'm already using one: the **default framebuffer** that GLFW makes with the window. It is double-buffered (front = shown, back = being drawn), which is why my render loop calls `glClear` and `glfwSwapBuffers`. Custom framebuffers (rendering into a texture for shadows, mirrors, post-processing) come much later, probably in the course's shadow mapping sections.

**Q: If a vertex is at y = 1.5 (off the top), does clipping clamp it to 1.0?**
No. That was my first guess, but clipping works like **scissors**: it cuts the triangle along the screen edge and creates **new vertices** where the edges cross it. The tip is chopped off, leaving a flat-topped 4-cornered shape. It is not a shorter pointy triangle. (Also: "floored" means rounding down to a whole number. Forcing a value into a range is "clamping.")

**Q: What if all three vertices are off-screen on the same side (e.g. all x > 1)?**
I first guessed it would become a rectangle along the edge. Wrong: clipping **keeps only what's inside**, and here nothing is inside, so the whole triangle is **thrown away**. 0 fragments, 0 fragment shader runs, nothing drawn. (The vertex shader still runs 3 times, since clipping happens after it.)

**Q: Friend set `vec4(0.0, 200.0, 0.0, 1.0)` and says the triangle is white. What's going on?**
The math says it should be **bright green**: 200.0 is clamped to 1.0 → `(0, 1, 0)`. For the intended darker green, divide by 255: `200/255 ≈ 0.78`. But if it truly shows **white**, the fragment shader probably **isn't running at all** (compile failure, e.g. missing `out vec4`, or the shader program was never made active before drawing). Many drivers draw white when no working shader is active.
> If the color is *wrong*, check the numbers. If changing the color does *nothing*, check that the shader is actually running.

**Correction from my summary: what does the vertex shader do?**
I described the vertex shader as "where we tell the GPU what vertices we want and where to find them in memory." That's actually **Vertex Specification**, done in C++ *before* the vertex shader. The vertex shader receives **one already-uploaded vertex at a time** and outputs its **final position**. Factory analogy: the delivery truck (C++) brings the parts and labels them. The vertex shader is the station that puts each part in its spot.

## Common mistakes

Most of these cause **a black screen with no error message**. OpenGL usually fails silently.

1. **Vertices outside -1 to +1.** Thinking in pixels (`100.0f, 400.0f`) instead of NDC (`-0.5f, 0.5f`). The shape gets clipped away. *Spot it:* nothing shows, or only a corner does.
2. **Colors 0–255 instead of 0.0–1.0.** Values above 1.0 are clamped, so `(255, 128, 51)` becomes white. *Fix:* divide by 255.
3. **A shader not writing its required output.** No `gl_Position` in the vertex shader, or no `out vec4` color in the fragment shader. *Ask:* "Does my vertex shader set a position? Does my fragment shader set a color?"
4. **Shader compile errors you never see.** Shaders are compiled by the driver **while the program runs**. C++ compiles fine, the shader fails, nobody tells you. *Fix:* check `glGetShaderiv(..., GL_COMPILE_STATUS, ...)` and print `glGetShaderInfoLog`.
5. **Forgetting to clear or swap.** No `glfwSwapBuffers` → black or frozen window. No `glClear` → moving objects leave smeared trails.

### Black-screen checklist: walk the assembly line 🔍
```
1. Vertex data     → Are my coordinates between -1 and 1? Did I tell the GPU where/how to read them?
2. Vertex shader   → Did it compile? Does it write gl_Position?
3. Clipping        → Is my shape actually on screen?
4. Fragment shader → Did it compile? Does it output a color (0.0–1.0)?
5. Framebuffer     → Am I clearing AND swapping every frame?
```

## Quick review

1. Why does the GPU handle pixel work instead of the CPU? Use the word "parallel."
2. Put in order: Fragment Shader, Rasterization, Vertex Shader, Per-Sample Operations, Primitive Assembly.
3. 2 separate triangles (no shared vertices) cover 1,000 pixels. How many vertex shader runs? Fragment shader runs?
4. What is a fragment, and how is it different from a screen pixel?
5. A triangle's vertex sits at x = 1.5 while the other two are on screen. Does clipping move that vertex to 1.0? What actually happens?
6. What's the difference between Vertex Specification and the vertex shader?

<details>
<summary>Answers</summary>

1. The GPU has thousands of simple cores that work in **parallel**, and every pixel's color is a small independent calculation, so they can all be done at once. A CPU has only a few (powerful) cores.
2. Vertex Shader → Primitive Assembly → Rasterization → Fragment Shader → Per-Sample Operations.
3. 6 vertex shader runs (2 × 3), 1,000 fragment shader runs.
4. A fragment is a candidate pixel: one covered spot produced by rasterization, carrying what's needed to color it. It is not yet approved and can be thrown away (e.g. by the depth test). Every screen pixel came from a fragment, but not every fragment becomes a pixel.
5. No. Clipping cuts the triangle along x = 1.0, creating new vertices where the edges cross. The part past the edge is removed and the result is a 4-cornered shape (split into 2 triangles).
6. Vertex Specification (C++ side) uploads the vertex data and tells the GPU where it is and how to read it. The vertex shader (GPU side) runs once per vertex and outputs its final position.

</details>
